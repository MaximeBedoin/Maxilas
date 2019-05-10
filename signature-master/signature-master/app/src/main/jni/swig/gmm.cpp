#include "stdafx.h"
#include "gmm.h"
#include "memalc.h"
#include "kmeans.h"
#include "memory.h"
#include "definitions.h"
#include "math.h"
#include "statistics.h"
#include "stdio.h"

GMM::GMM(int M_, int L_, int covMatrixType_,
					bool bRandomInit_,
					bool bParameterNormalization_, 
					double parameterStandardDeviation_,
					double endAccuracy_,
					int endIterationNumber_){
	// create the GMM and alocate the memory
	M = M_;		// number of gaussians
	L = L_;		// size of observation vector
	covMatrixType = covMatrixType_;
	bRandomInit = bRandomInit_;
	bParameterNormalization = bParameterNormalization_;
	parameterStandardDeviation = parameterStandardDeviation_;
	endAccuracy = endAccuracy_;
	endIterationNumber = endIterationNumber_;
	normalizationFactors = NULL;

	// mixture gain 
	c = memalc::dvector(M);
	// alocate the memory for the mean vector
	mu = memalc::dmatrix(L, M);
	// covariance matrix
	u = memalc::dmatrix3d(M, L, L);
	
	bInit = false;	// the GMM is not initialized yet
	logLikelihood = MIN_DOUBLE;

	// gama and bi_gaus are dependend of data, then could not initialize here
	gama = NULL;
	bi_gaus = NULL;
}

GMM::GMM(char *modelFile, bool binary)
{
	FILE *ifs;
	if(binary){
		ifs = fopen(modelFile, "rb");
	}else{
		ifs = fopen(modelFile, "rt");
	}

	if(ifs == NULL){
		printf("Template file doesn't exist!\n");
		bInit = false;
	}else{
		int  m, l, ligne, colonne;
		if(!binary){	// fichier text: pour déboguer
			// les paramètres de base 
			fscanf(ifs, "%d\n%d\n%lf\n", &M, &L, &logLikelihood);	// nombre d'observation
			// GMM configuration
			fscanf(ifs, "%d\n%d\n%d\n%lf\n%lf\n%d\n", &covMatrixType, &bRandomInit, 
				&bParameterNormalization, &parameterStandardDeviation, &endAccuracy, 
				&endIterationNumber);
			
			normalizationFactors = memalc::dvector(L);
			if(bParameterNormalization){
				for(l = 0; l < L; l++){
					fscanf(ifs, "%lf", normalizationFactors + l);
				}
			}

			// mixture gain
			c = memalc::dvector(M);
			for(m = 0; m < M; m++){
				fscanf(ifs, "%lf", c + m);
			}
			// mean
			mu = memalc::dmatrix(L, M);
			for(m = 0; m < M; m++){		
				for(l = 0; l < L; l++){
					fscanf(ifs, "%lf", mu[l] + m);
				}
			}

			// Covariance matrix
			u = memalc::dmatrix3d(M, L, L);
			for(m = 0; m < M; m++){
				for(ligne = 0; ligne < L; ligne++){
					for(colonne = 0; colonne < L; colonne++){
						fscanf(ifs, "%lf", u[m][ligne] + colonne);
					}
				}
			}
			gama = NULL;
			bi_gaus = NULL;
			bInit = true;
		}else{ //binary

		}
		fclose(ifs);
	}
}

bool GMM::initGMM(Obs *obs_gmm){
	// initialiser le GMM par les observations, utiliser kmeans pour distribuer les vecteurs
	// à M groupes différents. Puis calculer la moyenne et covariance de chaque groupe
	KMEANS kms(obs_gmm, M, bRandomInit);
	// il faut contrôler si kmeans marche bien!!! avant l'affectation

	if(kms.isInit()){
		if(c!=NULL)		memalc::free_dvector(c);
		if(mu != NULL)	memalc::free_dmatrix(mu, L);
		if(u != NULL)	memalc::free_dmatrix3d(u, M, L);

		// gain de la mixture
		c = memalc::dvector(M);
		// alocation de la mémoire pour des vecteurs de moyenne
		mu = memalc::dmatrix(L, M);
		// alocation de la mémoire pour des matrices de covariance
		u = memalc::dmatrix3d(M, L, L);	

		// affecter les moyennes des M groupes aux M gaussiennes
		int l, m;
		for(l = 0; l < L; l++){
			for(m = 0; m < M; m++){
				mu[l][m] = kms.means[l][m];
			}
		}
		// et les covariances
		int ln, cl;
		for(m = 0; m < M; m++){
			for(ln = 0; ln < L; ln++){
				for(cl = 0; cl < L; cl++){
					u[m][ln][cl] = kms.covariances[m][ln][cl];
				}
			}
		}
		// éviter le problème variance zéro! Mais seulement pour les éléments diagonales
		for(m = 0; m < M; m++){
			for(l = 0; l < L; l++){
				if(u[m][l][l] < COV_MIN_VAL){
					u[m][l][l] = COV_MIN_VAL;
				}
			}
		}
		// gain de la mixture, tranférer de nombre à proportion (probabilité)
		int denominator = 0;
		int i;
		for(i = 0; i < M; i++){
			denominator = denominator + kms.gains[i];
		}
		for(i = 0; i < M; i++){
			c[i] = double(kms.gains[i]) / denominator;
		}
		bInit = true;
		return true;
	}else{	// kmeans ne marche pas, peut être peu de données
		printf("Could not initialize the gaussian in each state\n");
		bInit = false;
		return false;
	}
}

bool GMM::isInit()
{
	return bInit;
}

GMM::~GMM(){
	memalc::free_dvector(c);
	memalc::free_dmatrix(mu, L);
	memalc::free_dmatrix3d(u, M, L);
	memalc::free_dmatrix(gama, M);
	memalc::free_dmatrix(bi_gaus, M);
	if(bParameterNormalization == PARAMETER_NORMALIZATION_YES)
		memalc::free_dvector(normalizationFactors);

}

void GMM::dump()
{
	if(bInit){
		int  m, l, ligne, colonne;
		for(m = 0; m < M; m++){
			printf("\nGaussienne %d", m);
			printf("\nGain de mixture %f", c[m]);
			printf("\nMoyenne:\n");
		
			for(l = 0; l < L; l++){
				printf("%f ", mu[l][m]);
			}
			printf("\nCovariance:\n");
		
			for(ligne = 0; ligne < L; ligne++){
				for(colonne = 0; colonne < L; colonne++){
					printf("%f ", u[m][ligne][colonne]);
				}
				printf("\n");
			}
		}
	}	
}

bool GMM::train()
{

	// bRandomInit
	// compute bi_gaus et gama in function of ob
	int loop = 0;
	double oldLL; 
	if(!initGMM(obs)){
		return false;	// Number of gaussians is soo great
	}
	printf("Training the GMM with %d gaussians\n", M);
	do{
		loop++;
		//dump();
		oldLL = logLikelihood;
		initProObs(obs);	// recompute the gama matrix
		logLikelihood = restimateModel();

		printf("Iteration %d, LL = %f\n", loop, logLikelihood);

		if(endAccuracy != 0){
			// stop criteria
			if(fabs((exp(oldLL) - exp(logLikelihood))/exp(oldLL)) <= endAccuracy)
				break;
		}else{
			if(loop == endIterationNumber)
				break;
		}
	}while(true);
	return true;
}

bool GMM::initProObs(Obs *ob)
{
	// soit ob est incorrecte, soit bInit est fausse, ne fais rien
	if((ob->getVecSize() != L)){
		return false;
	}

	int T = ob->getObsSize();

	if(bi_gaus != NULL){
		memalc::free_dmatrix(bi_gaus, M);
		bi_gaus = NULL;
	}
	bi_gaus = memalc::dmatrix(M, T);//M x T: compute the probabilities of O(t) given by each gaussian
	
	int m, t;
	//Calculer les probabilités ici, loi gaussienne
	for(m = 0; m < M; m++){		//pour chaque gaussienne
		// det_u contient le déterminant de la matrice covariance
		double det_u = 1;
		int l;
		for(l = 0; l < L; l++){
			det_u *= u[m][l][l];	// diagonal only
		}

		if(det_u == 0){ 
			printf("matrice de covariance singulière\n"); return false;
		}

		////////// Calculer la matrice inverse ////////// 
		double ** inverse_u = memalc::dmatrix(L, L); // contient des zeros

		int i, j;
		for(i = 0; i < L; i++){
			double det_for_inv = 1;
			// pour une matrice diagonale, j'utilise quand même matrice 2D, pour l'avenir
			for(j = 0; j < L; j ++){
				if (j != i) det_for_inv *= u[m][j][j];
			}
			inverse_u[i][i] = det_for_inv / det_u;
		}
		////////// Terminer les calculs de la matrice inverse////////// 

		double con = pow(2 * PI, double(-L/2)) / sqrt(det_u);
		for(t = 0; t < T; t++){	// pour chaque vecteur d'observation
			double exp_com = 0;				// entre les parenthèses de exp
			int l;
			for(l = 0; l < L; l++){
				exp_com += (ob->o[l][t] - mu[l][m]) * inverse_u[l][l] * 
						   (ob->o[l][t] - mu[l][m]); // (o-mu)' * u^-1 * (o-mu)
			}				
			bi_gaus[m][t] = c[m] * con * exp(-exp_com/2);

			if(bi_gaus[m][t] < PRO_MIN_VAL){
				bi_gaus[m][t] = PRO_MIN_VAL;
			}
		}
		memalc::free_dmatrix(inverse_u, L);
	}//pour chaque gaussienne

	return true;
}

double GMM::restimateModel()
{
	int t, m, l;
	int T = obs->getObsSize();
	// Allocate memory for gama and compute gama
	if(gama != NULL){
		memalc::free_dmatrix(gama, M);
		gama = NULL;
	}
	gama = memalc::dmatrix(M, T);
	computeGama();

	// Reestimation GMM parameters
	for(m = 0; m < M; m++){
		double sum_gama = 0;
		for(t = 0; t < T; t++){ 
			sum_gama += gama[m][t];
		}

		// restimate
		c[m] = sum_gama / T;	//OK

		double *nominator_mu = memalc::dvector(L);
		for(l = 0; l < L; l++){
			for(t = 0; t < T; t++){ 
				nominator_mu[l] += gama[m][t] * obs->o[l][t];
			}
		}

		double *nominator_u = memalc::dvector(L); //diagonal matrix only
		for(t = 0; t < T; t++){ 
			for(l = 0; l < L; l++){
				nominator_u[l] += gama[m][t] * (obs->o[l][t] - mu[l][m]) * (obs->o[l][t] - mu[l][m]);
			}
		}

		for(l = 0; l < L; l++){
			mu[l][m] = nominator_mu[l] / sum_gama;
		}
		
		for(l = 0; l < L; l++){
			u[m][l][l] = nominator_u[l] / sum_gama;
			if(u[m][l][l] < COV_MIN_VAL){
				u[m][l][l] = COV_MIN_VAL;
			}
		}
		
		memalc::free_dvector(nominator_mu);
		memalc::free_dvector(nominator_u);
	}
	return test(obs);

}

double GMM::test(Obs *ob)	//normalize observation if needed
{
	initProObs(ob);
	double LL = 0;
	int T = ob->getObsSize();
	int t, m;
	for(t = 0; t < T; t++){
		double tmp = 0;
		for(m = 0; m < M; m++){
			tmp += bi_gaus[m][t];
		}
		LL += log(tmp);
	}
	return (LL / T / L);
}

void GMM::computeNormalizationFactor() // of traning data
// calculer les paramètres de normalisation personalisée
{
	// calculer les paramètres de normalisation des paras, l'écart-type moyen
	memalc::free_dvector(normalizationFactors);
	normalizationFactors = memalc::dvector(L);	// full fill zero
	double mean, var;
	int l;
	for(l = 0; l < L; l++){
		statistics::variance(obs->o[l], obs->getObsSize(), &mean, &var);
		normalizationFactors[l] = sqrt(var);
	}

	for(l = 0; l < L; l++){
		normalizationFactors[l] = normalizationFactors[l] / parameterStandardDeviation;
	}
}

bool GMM::assignObs(Obs *ob_)
{
	obs = ob_;
	// Normalize the data if asked (specified in the caractéristics of GMM)
	if(bParameterNormalization == PARAMETER_NORMALIZATION_YES){
		computeNormalizationFactor();
		obs->normalizeParameters(normalizationFactors);
	}
	return true;
}

bool GMM::normalizeObsParameters(Obs * ob_)
// use to normalize an observation sequence
{
	if(bParameterNormalization == PARAMETER_NORMALIZATION_YES){ //control the GMM property
		if(!ob_->isInit())
			return false;	// not init
		if(ob_->getVecSize() != L)
			return false;	// not compatible
		ob_->normalizeParameters(normalizationFactors);
	}
	return true;
}

void GMM::setGaussianNumber(int M_)
{
	// take attention, the 3 variables u, mu ans c, their sizes change
	if(c!=NULL)		memalc::free_dvector(c);
	if(mu != NULL)	memalc::free_dmatrix(mu, L);
	if(u != NULL)	memalc::free_dmatrix3d(u, M, L);

	M = M_;

	c = memalc::dvector(M);
	mu = memalc::dmatrix(L, M);
	u = memalc::dmatrix3d(M, L, L);	
}

int GMM::getGaussianNumber()
{
	return M;
}

int GMM::getVectorSize()
{
	return L;
}

void GMM::computeGama()
{
	int T = obs->getObsSize();
	int t, m;
	double *denominator;
	denominator = memalc::dvector(T); //contains only zero

	for(t = 0; t < T; t++){
		for(m = 0; m < M; m++){
			denominator[t] += bi_gaus[m][t];
		}
	}

	for(t = 0; t < T; t++){
		for(m = 0; m < M; m++){
			gama[m][t] = bi_gaus[m][t] / denominator[t];
		}
	}
	memalc::free_dvector(denominator);
}

double GMM::getTrainingLogLikelihood(){
	return logLikelihood;
}

bool GMM::save(char *modelFile, bool binary)
{
	if(!bInit) return false;
	FILE *ofs;
	if(binary){
		ofs = fopen(modelFile, "wb");
	}else{
		ofs = fopen(modelFile, "wt");
	}

	if(ofs == NULL){
		printf("Can not save the template file!\n");
		return false;
	}else{
		int  m, l, ligne, colonne;
		if(!binary){	// fichier text: pour déboguer
			// les paramètres de base 
			fprintf(ofs, "%d\n%d\n%f\n", M, L, logLikelihood);	// nombre d'observation
			// GMM configuration
			fprintf(ofs, "%d\n%d\n%d\n%f\n%f\n%d\n", covMatrixType, bRandomInit, 
				bParameterNormalization, parameterStandardDeviation, endAccuracy, endIterationNumber);
			
			if(bParameterNormalization){
				int l;
				for(l = 0; l < L; l++){
					fprintf(ofs, "%f ", normalizationFactors[l]);
				}
				fprintf(ofs, "\n");
			}

			// mixture gain
			for(m = 0; m < M; m++){
				fprintf(ofs, "%f ", c[m]);
			}
			fprintf(ofs, "\n");
			// mean
			for(m = 0; m < M; m++){		
				for(l = 0; l < L; l++){
					fprintf(ofs, "%f ", mu[l][m]);
				}
				fprintf(ofs, "\n");
			}

			// Covariance matrix
			for(m = 0; m < M; m++){
				for(ligne = 0; ligne < L; ligne++){
					for(colonne = 0; colonne < L; colonne++){
						fprintf(ofs, "%f ", u[m][ligne][colonne]);
					}
					fprintf(ofs, "\n");
				}
			}
		}else{ //binary

		}
		fclose(ofs);
		return true;
	}
}



