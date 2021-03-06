#include "stdafx.h"
#include "hmm.h"
#include "memalc.h"
#include "definitions.h"
#include "math.h"
#include "stdio.h"
#include "statistics.h"
#include "kmeans.h"

HMM::HMM(int N_, int M_, int transType_, int covMatrixType_,
					bool bRandomInit_,
					bool bParameterNormalization_, 
					double parameterStandardDeviation_,
					double endAccuracy_,
					int endIterationNumber_)
{
	N = N_;
	M = M_;
	transType = transType_;
	covMatrixType = covMatrixType_;
	bRandomInit = bRandomInit_;
	bParameterNormalization = bParameterNormalization_;
	parameterStandardDeviation = parameterStandardDeviation_;
	endAccuracy = endAccuracy_;
	endIterationNumber = endIterationNumber_;

	//les autres pointeurs sont NULL
	a = NULL;
	pi = NULL;
	obs = NULL;
	gmm = NULL;
	bi = NULL;
	bi_gaus = NULL;
	alpha = NULL;
	beta = NULL;
	scale = NULL;

	tbStSequenceLen = NULL;
	tbStSequence = NULL;
	normalizationFactors = NULL;

	biLen = 0;
	alphaLen = 0;
	betaLen = 0;
	bInit = false;
	K = 0;	// z�ro observation	
	L = 0;	// taille des vecteurs est z�ro
	// L et K concernent les observations: initialise plus tard
}

bool HMM::assignObs(int K_)
{
	if(K_ < 1) return false;
	// controle encore ici pour les bonnes observations
	obs = obsor;
	K = K_;
	L = obs[0]->getVecSize();
	
	// Normalize
	if(bParameterNormalization == PARAMETER_NORMALIZATION_YES){
		computeNormalizationFactor();
		normalizeTrainBaseParameters();
	}
	return true;
}

HMM::~HMM()
{
	destroyHmmVariable();
	// Cet variable ne doit pas etre d�truit dans destroyHmmVariable
	if(normalizationFactors != NULL){
		memalc::free_dvector(normalizationFactors);
		normalizationFactors = NULL;
	}
}

HMM::HMM(char *modelFile, bool binary)
{
	// charger le HMM � partir du fichier modelFile
	// il faut d�finir la forme du fichier de mod�le...	
	FILE *ifs;
	
	if(binary){
		ifs = fopen(modelFile, "rb");
	}else{
		ifs = fopen(modelFile, "rt");
	}

	if(ifs == NULL){
		printf("Can not open the template file!\n");
		//Comme le mod�le ne contient rien
		pi = NULL;
		a = NULL;
		obs = NULL;
		gmm = NULL;
		bi = NULL;
		bi_gaus = NULL;
		alpha = NULL;
		beta = NULL;
		scale = NULL;
		normalizationFactors = NULL;
		tbStSequenceLen = NULL;
		tbStSequence = NULL;

		biLen = 0;
		alphaLen = 0;
		betaLen = 0;
		bInit = false;
		K = 0;	// z�ro observation	
		L = 0;	// taille des vecteurs est z�ro
	}else{
		bInit = true;
		if(!binary){ // text file
			// les param�tres de base 
			fscanf(ifs, "%d%d%d", &N, &M, &L);
			fscanf(ifs, "%d", &K);
			fscanf(ifs, "%lf", &LLba);
			// HMM configuration
			fscanf(ifs, "%d%d%d%d", &transType, &covMatrixType, &bRandomInit, &bParameterNormalization);
			fscanf(ifs, "%lf", &parameterStandardDeviation);
			fscanf(ifs, "%lf", &endAccuracy);
			fscanf(ifs, "%d", &endIterationNumber);
			
			if(bParameterNormalization){
				int l;
				normalizationFactors = memalc::dvector(L);	// full fill zero
				for(l = 0; l < L; l++){
					fscanf(ifs, "%lf ", normalizationFactors + l);
				}
			}

			//K: nombre d'observation pour entrainer le HMM
			//les chemins Viterbi
			tbStSequenceLen = memalc::ivector(K);
			tbStSequence = new int * [K];
			int k, i, j;
			for(k = 0; k < K; k++){
				fscanf(ifs, "%d", tbStSequenceLen + k);
			}
			
			for(k = 0; k < K; k++){
				tbStSequence[k] = memalc::ivector(tbStSequenceLen[k]);
				for(i = 0; i < tbStSequenceLen[k]; i ++){
					fscanf(ifs, "%d", tbStSequence[k] + i);
				}
			}

			// Pas de m�moire allou�e avant, parce que c'est le Contructeur, GMM aussi
			pi = memalc::dvector(N);
			a = memalc::dmatrix(N, N);
			gmm = new GMM * [N];
			int n, m, l;
			for(n = 0; n < N; n++){
				gmm[n] = new GMM(M, L, covMatrixType, bRandomInit,
					bParameterNormalization, parameterStandardDeviation,
					endAccuracy, endIterationNumber);	// c, mu et u sont allou�es dans GMM: OKKK
			}
			// probabilit�s initiales
			for(i = 0; i < N; i ++){
				fscanf(ifs, "%lf", pi + i);
			}
			// matrice de transitions
			for(i = 0; i < N; i ++){
				for(j = 0; j < N; j ++){
					fscanf(ifs, "%lf", a[i] + j);
				}
			}
			// les GMMs: c
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					fscanf(ifs, "%lf", gmm[n]->c + m);
				}
			}
			// les GMMs: mu
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					for(l = 0; l < L; l++){
						fscanf(ifs, "%lf", gmm[n]->mu[l] + m);
					}
				}
			}
			// les GMMs: u
			int line, col;
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					for(line = 0; line < L; line++){
						for(col = 0; col < L; col ++){
							fscanf(ifs, "%lf", gmm[n]->u[m][line] + col);
						}
					}
				}
			}
		}else{// fichier binaire
			fread((void *)&N, sizeof(N), 1, ifs);
			fread((void *)&M, sizeof(M), 1, ifs);
			fread((void *)&L, sizeof(L), 1, ifs);
			fread((void *)&K, sizeof(K), 1, ifs);
			fread((void *)&LLba, sizeof(LLba), 1, ifs);
			// HMM configuration
			fread((void *)&transType, sizeof(transType), 1, ifs);
			fread((void *)&covMatrixType, sizeof(covMatrixType), 1, ifs);
			fread((void *)&bRandomInit, sizeof(bRandomInit), 1, ifs);
			fread((void *)&bParameterNormalization, sizeof(bParameterNormalization), 1, ifs);
			fread((void *)&parameterStandardDeviation, sizeof(parameterStandardDeviation), 1, ifs);
			fread((void *)&endAccuracy, sizeof(endAccuracy), 1, ifs);
			fread((void *)&endIterationNumber, sizeof(endIterationNumber), 1, ifs);

			if(bParameterNormalization){
				normalizationFactors = memalc::dvector(L);
				fread((void *)normalizationFactors, sizeof(normalizationFactors[0]), L, ifs);
			}

			//les chemins Viterbi
			tbStSequenceLen = memalc::ivector(K);
			fread((void *)tbStSequenceLen, sizeof(tbStSequenceLen[0]), K, ifs);

			tbStSequence = new int*[K];
			int k;
			for(k = 0; k < K; k++){
				tbStSequence[k] = memalc::ivector(tbStSequenceLen[k]);
				fread((void *)(tbStSequence[k]), sizeof(tbStSequence[0][0]), tbStSequenceLen[k], ifs);
			}

			// probabilit�s initiales
			pi = memalc::dvector(N);
			fread((void *)pi, sizeof(pi[0]), N, ifs);
			// transition matrix, read it later
			a = memalc::dmatrix(N, N);

			gmm = new GMM * [N];

			int n;
			// init the GMMs
			for(n = 0; n < N; n++){
				gmm[n] = new GMM(M, L, covMatrixType, bRandomInit,
					bParameterNormalization, parameterStandardDeviation,
					endAccuracy, endIterationNumber);	// c, mu et u sont allou�es dans GMM: OKKK
			}

			for(n = 0; n < N; n++){	//parcourir tous les �tats
				// transition
				fread((void *)a[n], sizeof(double), N, ifs);
				// mixture gain
				fread((void *)(gmm[n]->c), sizeof(gmm[n]->c[0]), M, ifs);
				// mean
				for(int l=0; l< L; l++){
					fread((void *)(gmm[n]->mu[l]), sizeof(double), M, ifs);
				}
				// covariance
				for(int m = 0; m < M; m++){
					for(int l = 0; l<L; l++){
						fread((void *)(gmm[n]->u[m][l]), sizeof(double), L, ifs);
					}
				}
			}
		}
		// these parameters were not read
		bi = NULL;
		bi_gaus = NULL;
		alpha = NULL;
		beta = NULL;
		scale = NULL;
		obs = NULL;
		fclose(ifs);
	}
}

void HMM::destroyHmmVariable()
// utiliser dans le cas o� on ne peut pas entrainer le HMM, et les variables
// sont d�j� alou�es
// il faut appeller cette fonction chaque fois �chec
{
	int n;
	if(gmm != NULL){
		for (n = 0; n < N; n++){
			if(gmm[n] != NULL) delete gmm[n];
		}
		delete gmm;
		gmm = NULL;
	}
	
	if(pi != NULL){
		memalc::free_dvector(pi);
		pi = NULL;
	}

	if(a != NULL){
		memalc::free_dmatrix(a, N);
		a = NULL;
	}

	if(alpha != NULL){
		memalc::free_dmatrix(alpha, N);
		alpha = NULL;
	}

	if(beta != NULL){
		memalc::free_dmatrix(beta, N);
		beta = NULL;
	}

	if(scale != NULL){
		memalc::free_dvector(scale);
		scale = NULL;
	}

	if(bi != NULL){
		memalc::free_dmatrix(bi, N);
		bi = NULL; // pour �viter le probl�me de DELETE deux fois un pointeur
	}

	if(bi_gaus != NULL){
		memalc::free_dmatrix3d(bi_gaus,	M, N);
		bi_gaus = NULL;
	}

	memalc::free_ivector(tbStSequenceLen);
	tbStSequenceLen = NULL;

	memalc::free_imatrix(tbStSequence, K);	// cela marche
	tbStSequence = NULL;
}

bool HMM::initHMM(){
	// il faut charger les observaltions avant appeler cette fonction
	// les observations sont charg�es par une autre fonction
	// parce que cette fonction utilise L, la taille d'un vecteur, qui d�pend de donn�es
	// Valeur retourn�e: true si tous les gaussiennes sont OK
	//		false si pas de donn�e, ou bien une gaussienne vide (pas de donn�e)
	if (obs != NULL){	// les observations sont charg�es
		// Attention: je voudrais initialiser les GMMs tout d'arbord, parce que parfois,
		// nous devons changer le nombre d'�tat de HMM
		// ****************************************************************
		destroyHmmVariable();
		// distribuer les observations aux �tats pour initialiser les gaussiennes
		Obs ** obs_etat = distributeObs(transType);
		if(obs_etat == NULL){
			// reason: in the case of ERGODIC, the K-means could not finalize
			// it will have no problem with the HMM Left-Right
			bInit = false;
			return false;
		}

		// cr�er les mixtures de gaussiennes pour N �tats, mais supprimer les anciennes
		gmm = new GMM * [N];
		bInit = true;
		// initialiser les gaussiennes
		int n;
		for (n = 0; n < N; n++){
			gmm[n] = new GMM(M, L, covMatrixType, bRandomInit);
			bInit = bInit & gmm[n]->initGMM(obs_etat[n]);
			delete obs_etat[n];
		}
		delete obs_etat;
		
		//then initialize the Transition Matrix and the Initial Probabilities
		//if the GMMs were well initialized
		if(bInit){
			initInitialProbabilities(transType);
			initTransitionMatrix(transType); 
		}else{ // si non, desallouer les GMMs
			printf("InitHMM: Trop d'�tats par rapport au nombre de vecteur d'observation\n");
			destroyHmmVariable();
		}
		return bInit;
	}else{
		printf("InitHMM: Ne pas pouvoir initialiser les gaussiennes sans donn�e d'apprentissage\n");
		bInit = false;
		return bInit;
	}
}

double HMM::viterbi(Obs *ob, int ** statSeq, bool last_state_exit_oblige)
{
	// cette fonction retourne LL si OK z�ro sinon
	// pourtant, nous pouvons toujours controler bInit avant appeller cette fonction
	// retourner la log-vraisemblance _d'une_ observation: normalis������e
	// last_state_exit_oblige: il faut sortir par le dernier �tat dans le cas TRANS_LR
	// Quand cette variable == TRUE, la log_vraisemblance n'est pas obtimis�e. 
	double LL;
	double **phi;	// N x T
	int **psi;		// N x T
	double ** log_a;	// log de la matrice de transition N x N
	double ** log_bi;	// log de la matrice bi N * T;
	int * q;

	bool b = initProObs(ob);	//init les bi, bi_gaus
	// dans la fonction _initProObs_, on a d�j� contr�l� bInit, si HMM est bien init?
	if(b){						//	initProObs OK	
		int len = ob->getObsSize();
		phi = memalc::dmatrix(N, len);
		psi = memalc::imatrix(N, len);
		q = memalc::ivector(len);

		log_a = memalc::dmatrix(N, N);
		int i, j;
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				if(a[i][j] > 0){
					log_a[i][j] = log(a[i][j]);
				}else{
					log_a[i][j] = LOG_ZERO;
				}
			}
		}

		log_bi = memalc::dmatrix(N, len);
		int t, n;
		for(t = 0; t < len; t++){
			for(n = 0; n < N; n ++){
				if(bi[n][t] > 0){
					log_bi[n][t] = log(bi[n][t]);
				}else{
					log_bi[n][t] = LOG_ZERO;
				}
			}
		}

		// voir les �quations 105a, 105b, 105c RABINER
		for(i = 0; i < N; i++){
			if(pi[i] > 0){
				phi[i][0] = log(pi[i]) + log_bi[i][0];
			}else{
				phi[i][0] = LOG_ZERO + log_bi[i][0];
			}
		}

		for(t = 1; t < len; t++){
			// calculer phi[j][t]
			for(j = 0; j < N; j++){
				// chercher tout d'abord l'indice i de max(phi[i][t-1] + log_a
				double maxVal = MIN_DOUBLE;
				int indice;
				for(i = 0; i < N; i++){
					if((phi[i][t-1] + log_a[i][j]) >= maxVal){
						maxVal = phi[i][t-1] + log_a[i][j];
						indice = i;
					}
				}
				phi[j][t] = maxVal + log_bi[j][t];
				psi[j][t] = indice;
			}
		}

		// Terminaison
		if(last_state_exit_oblige && (transType == TRANS_LR)){ // n'applique que dans le cas Left-Right
			LL = phi[N-1][len-1];
			q[len-1] = N-1;
		}else{
			LL = MIN_DOUBLE; //la plus petite valeur, supposons...
			for(i = 0; i < N; i++){
				if(phi[i][len-1] >= LL){
					LL = phi[i][len-1];
					q[len-1] = i;
				}
			}
		}
					/*ofstream ofs;
					ofs.open("psi.txt", ios::out);
					if(ofs.is_open()){
						ofs << "\nn'importe quoi\nNombre d'�tats: " << N << "\n";
						for(int ipsi=0; ipsi < N; ipsi++){
							for(int jpsi=0; jpsi < len; jpsi++){
								ofs << phi[ipsi][jpsi] << "\t";
							}
							ofs << "\n";
						}
						ofs << "\n";
						ofs.close();
					}*/				
	
		// Back tracking
		for(t = len - 2; t >= 0; t--){
			q[t] = psi[q[t+1]][t+1];
		}		
		memalc::free_dmatrix(phi, N);
		memalc::free_imatrix(psi, N);
		memalc::free_dmatrix(log_a, N);
		memalc::free_dmatrix(log_bi, N);
		* statSeq = q;
		LL = LL / (len * L);
		return LL;
	}else{
		return 0;
	}
}

bool HMM::initProObs(Obs *ob)
{
	// soit ob est incorrecte, soit bInit est fausse, ne fais rien
	if((ob->getObsSize() < 1) | (!bInit)){
		return false;
	}

	// Enfait, la taille de bi d�pend de ob, plus pr�cise, sa longeur...
	// comment bien g�rer la m�moire pour bi
	// il vaut mieux avoir une variable qui tient sa longeur: biLen
	// nombre de lignes est N. bi: N x biLen

	if(bi != NULL){
		memalc::free_dmatrix(bi, N);
		bi = NULL; // pour �viter le probl�me de DELETE deux fois un pointeur
	}
	if(bi_gaus != NULL){
		memalc::free_dmatrix3d(bi_gaus,	M, N);
		bi_gaus = NULL;
	}

	// delete la m�moire avant de changer biLen
	biLen = ob->getObsSize();
	bi = memalc::dmatrix(N, biLen);

	bi_gaus = memalc::dmatrix3d(M, N, biLen);
	//Calculer les probabilit�s ici, loi gaussienne
	int m, n, l, i, j, t;
	for(m = 0; m < M; m++){		//pour chaque gaussienne
		for(n = 0; n < N; n++){	// pour chaque �tat
			// det_u contient le d�terminant de la matrice covariance
			double det_u = 1;
			for(l = 0; l < L; l++){
				// The next command is correct with only diagonal matrix
				// To implement with a full matrix, you have to modify the command below
				det_u *= gmm[n]->u[m][l][l];
				// Attention: gmm[n] parce qu'on calcule pour l'�tat n
			}
			if(det_u == 0){
				printf("matrice de covariance singuli�re\n"); return false;
			}
			
			////////// Calculer la matrice inverse////////// 
			double ** inverse_u = memalc::dmatrix(L, L); // contient des zeros

			for(i = 0; i < L; i++){
				double det_for_inv = 1;
				// pour une matrice diagonale, j'utilise quand m�me matrice 2D, pour l'avenir
				for(j = 0; j < L; j ++){
					if (j != i) det_for_inv *= gmm[n]->u[m][j][j];
				}
				inverse_u[i][i] = det_for_inv / det_u;
			}
			////////// Terminer les calculs de la matrice inverse////////// 

			double con = pow(2 * PI, double(-L/2)) / sqrt(det_u);
			for(t = 0; t < biLen; t++){	// pour chaque vecteur d'observation
				double exp_com = 0;				// entre les parenth�ses de exp
				for(l = 0; l < L; l++){
					exp_com += (ob->o[l][t] - gmm[n]->mu[l][m]) * inverse_u[l][l] * 
							   (ob->o[l][t] - gmm[n]->mu[l][m]); // (o-mu)' * u^-1 * (o-mu)
				}				
				bi_gaus[m][n][t] = gmm[n]->c[m] * con * exp(-exp_com/2);

				// Sometime, bi_gaus is very big since the con is very big. 
				// That can be explained by the fact that the covariance matrix 
				// contains a lot of mini-values. 
				if(bi_gaus[m][n][t] < PRO_MIN_VAL){
					bi_gaus[m][n][t] = PRO_MIN_VAL;
				}
			}
			memalc::free_dmatrix(inverse_u, L);
		} // pour chaque �tat
	}//pour chaque gaussienne

	for(n = 0; n < N; n++){
		for(t = 0; t < biLen; t ++){
			for(m = 0; m < M; m++){
				bi[n][t] += bi_gaus[m][n][t];	// les bi sont ini. � z�ro
			}
		}
	}// ok, bien calculer bi.


	// In fact, we have to control the value of bi, not bi_gaus


	/*printf("les_bi\n");
	for(n = 0; n < N; n++){
		for(int t = 0; t < biLen; t ++){
			printf("%f\t", bi[n][t]);	
		}
	}
	printf("\n");*/

	return true;
}

double HMM::forward(Obs *ob){
	// ce que nous avons, ce sont les alpha ch�peau et beta ch�peau. �quation 93b et 94
	// faire aussi le scaling, page 272, RABINER
	double LL = 0;

	bool b = initProObs(ob); //init les bi, bi_gaus
	if(b){//	initProObs OK
		// delete les anciennes variables alpha, scale si elles sont existe
		if(alpha != NULL){
			memalc::free_dmatrix(alpha, N);
			alpha = NULL;
		}
		if(scale != NULL){
			memalc::free_dvector(scale);
			scale = NULL;
		}

		alphaLen = ob->getObsSize();
		alpha = memalc::dmatrix(N, alphaLen);
		scale = memalc::dvector(alphaLen);

		// voir les �quations 19, 20, 21 de RABINER
		int i;
		for(i = 0; i < N; i ++){
			alpha[i][0] = pi[i] * bi[i][0];	// �quation 19
			scale[0] += alpha[i][0];
		}

		scale[0] = 1/scale[0];
		// scaling...
		for(i = 0; i < N; i ++){
			alpha[i][0] *= scale[0];
		}

		int t;
		for(t = 0; t < alphaLen-1; t++){
			int j;
			for(j = 0; j < N; j++){
				double tmp = 0;
				int i;
				for(i = 0; i < N; i++){
					tmp += alpha[i][t] * a[i][j];
				}
				alpha[j][t+1] = tmp * bi[j][t+1];	//�quation 20

				scale[t+1] += alpha[j][t+1];
			}
			// scaling...
			
			scale[t+1] = 1 / scale[t+1];
			for(j = 0; j < N; j++){
				alpha[j][t+1] *= scale[t+1];
			}// end scaling...
		}

		//debug
		/*cout << "scaling factor:\n";
		for(t = 0; t < alphaLen; t++){
			cout << scale[t] << "\t";
		}
		cout << "\n";

		cout << "alpha\n";
		for(int j = 0; j < N; j++){
			for(t = 0; t < alphaLen; t++){
				cout << alpha[j][t] << "\t";
			}
			cout << "\n";
		}*/

		for(t = 0; t < alphaLen; t++){
			LL -= log(scale[t]);
		}
		LL = LL / (alphaLen * L);
		return LL;
	}else{
		return 0;
	}
}

bool HMM::backward(Obs *ob)
// il faut toujour appeler forward avant d'appeler cet fonction
// si non, scale n'existe pas, ou il n'est pas le bon
{
	if(beta != NULL){
		memalc::free_dmatrix(beta, N);
		beta = NULL;
	}
	
	betaLen = ob->getObsSize();
	beta = memalc::dmatrix(N, betaLen);
	
	// calculer beta, vois les �quations 24, 25 de RABINER
	int i, t, j;
	for(i = 0; i < N; i ++){
		beta[i][betaLen-1] = scale[betaLen-1];	// �quation 24
	}

	for(t = betaLen - 2; t >= 0; t--){
		for(i = 0; i < N; i++){
			beta[i][t] = 0;
			for(j = 0; j < N; j++){
				beta[i][t] += a[i][j] * bi[j][t+1] * beta[j][t+1];	// �quation 25
			}

			beta[i][t] *= scale[t];

			// in linux, we must use another command to check beta
			if(beta[i][t] < 0){ // trop d'�tats ==> mauvais apprentissage
				printf("backward: Overflow problem when computing beta! beta[%d][%d] becomes negatif!\n", i, t);
				// 11/01/2005
				// here we have a big promblem, well understood.
				// When training data is not appropriated, the emission probabilities bi
				// are very small, ex. a lot become PRO_MIN_VAL...
				// scale[], the scaling factor, 
				// computed in forward__, become very big. When scaling 
				// of beta, and the later may exceed 1e308.
				// What we are sur is that the beta[i][T] are very big if the bi[j][T]
				// are small... (eq 24), then it may accumule to exceed 1e308!
				// Can not resolve by programming, since it depends on training data:(
				// Ex: the writer 46, when testing the Dolfing protocol.
				// In this case, it's better to reinitiate the HMM, randomly of course
				return false;
			}
		}
	}
	/*printf("beta\n");
	for(j = 0; j < N; j++){
		for(t = 0; t < betaLen; t++){
			printf("%f\t", beta[j][t]);	
		}
		printf("\n");
	}*/
	return true;
}

Obs ** HMM::distributeObs(int transType)
{
	// distribute the K observation sequences to N states to initialize 
	// the gaussians in each state

	// We need to specify the type of Transition Matrix to initialized 
	// cut the K observation sequences in N parts

	// return: if it return NULL, no successful
	Obs ** obs_etat;
	obs_etat = new Obs * [N];
	int n, k, t;
	for(n = 0; n < N; n++){
		// les racines pour raccorder les petites observations
		obs_etat[n] = new Obs(L, 0);
	}

	if(transType == TRANS_LR){
		for(k = 0; k < K; k++){
			int size = obs[k]->getObsSize() / N;
			// les N-1 premiers �tats
			for(n = 0; n < N-1; n++){
				obs_etat[n]->addObs(obs[k], n * size, size);
			}
			// dernier �tat
			obs_etat[N-1]->addObs(obs[k], (N - 1) * size, obs[k]->getObsSize() - (N - 1) * size);
		}
	}

	// In this case, use the K-means with K = N to assign the observations into state
	if(transType == TRANS_ERGODIC){
		Obs obs_all(L, 0); // This object is used to combine all the K sequences
		for(k = 0; k < K; k++){
			obs_all.addObs(obs[k], 0, obs[k]->getObsSize());
		}
		// Apply K-means on obs_all;
		KMEANS kms(&obs_all, N, bRandomInit);
		if(kms.isInit()){
			int * membership;
			membership = kms.getMemberShip();
			int obs_all_len = obs_all.getObsSize();
			for(t=0; t < obs_all_len; t++){
				obs_etat[membership[t]]->addObs(&obs_all, t, 1);
			}
		}else{
			// can't initialize the K-means, how to control this situation???
			// in this case, assign NULL to obs_etat
			for(n = 0; n < N; n++){
				delete obs_etat[n];
			}
			// delete the obs_etat
			delete obs_etat;
			obs_etat = NULL;
			printf("Could not initialize the HMM when assigning observation to state\n");
		}
	}
	return obs_etat;
}

void HMM::dump()
{
	printf("Transition matrix:\n");
	int i, j, n;
	if(a != NULL){
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				printf("%f ", a[i][j]);
			}
			printf("\n");
		}
	}else{
		printf("was not initialized");
	}

	printf("\nInitial state probability:\n");
	if(pi != NULL){
		for(j = 0; j < N; j++){
			printf("%f", pi[j]);
		}
		printf("\n");
	}else{
		printf("was not initialized");
	}

	if(gmm != NULL){
		for(n = 0; n < N; n++){
			printf("\nEtat %d\n", n);
			gmm[n]->dump();			
		}
	}
}

void HMM::initTransitionMatrix(int transType)
{
	a = memalc::dmatrix(N, N);
	int i, j;
	/**** Left-Right transition matrix ****/
	if(transType == TRANS_LR){
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				a[i][j] = 0;
			}
		}
		for(i = 0; i < N-1; i ++){
			a[i][i] = 0.5;
			a[i][i+1] = 0.5;
		}
		a[N-1][N-1] = 1;
	}
	/**** Full transition matrix ****/
	if(transType == TRANS_ERGODIC){
		for(i = 0; i < N; i ++){
			for(j = 0; j < N; j++){
				a[i][j] = double(1) / N;
			}
		}
	}
}

void HMM::initInitialProbabilities(int transType)
{
	pi = memalc::dvector(N);
	int i;
	/**** Left-Right transition matrix ****/
	if(transType == TRANS_LR){
		pi[0] = 1;	// les autres �l�ments sont z�ro... voir memalc
	}

	/**** Full transition matrix ****/
	if(transType == TRANS_ERGODIC){
		for(i = 0; i < N; i ++){
			pi[i] = double(1)/N;
		}
	}
}

// Declaration of baumwelch has to be changed (bool status, double LL)
bool HMM::baumwelch(double* LL)
// r�estimer les param�tres de HMM, une it�ration, utilise tous les observations de obs
// controler bInit avant toutes ces op�rations!!!
{
	double LLacc = 0;	// Log vraisemblance accumul�e sur K observation....
	// Toutes les matrices suivantes sont accumul�es via les K observations 
	// elles servent � r�estimer les param�tres de HMM APRES avoir parcouru 
	// toutes les K observations
	double ** nume_a;		// N x N	
	double ** denom_a;		// N x N
	double ** nume_c;		// N * M	pour chaque �tat, plusieurs gaussiennes
							// attention, nume_c est aussi d�num�rateur de 53, 54
							// c.a.d. nume_mu, nume_u
	double * denom_c;		// 1 x N
	double *** nume_mu;		// N x L x M : chaqe �tat une matrice de moyennes
	double **** nume_u;		// N x (M x L x L) : chaque �tat, plusieurs covariances
	// attention, les allocations de m�moire remplissent les z�ros partout
	nume_a = memalc::dmatrix(N, N);
	denom_a = memalc::dmatrix(N, N);
	nume_c = memalc::dmatrix(N, M);
	denom_c = memalc::dvector(N);
	nume_mu = memalc::dmatrix3d(N, L, M);
	nume_u = memalc::dmatrix4d(N, M, L, L);

	int k;
	for(k = 0; k < K; k++){	// parcourir toutes les observations
		int T;						// longeur de l'observation k obs[k]
		double *** gama;			// M x N x T, 
									//gama est recalcul� pour chaque observation
		double ll = forward(obs[k]);// cette fonction ex�cute _initProObs(obs[k])_

		bool backwardOk = backward(obs[k]);
		if(!backwardOk){
			// backward is not OK, the baumwelch has to return in this case a FALSE 
			// status
			memalc::free_dmatrix(nume_c, N);
			memalc::free_dvector(denom_c);
			memalc::free_dmatrix3d(nume_mu, N, L);
			memalc::free_dmatrix4d(nume_u, N, M, L);
			memalc::free_dmatrix(nume_a, N);
			memalc::free_dmatrix(denom_a, N);
			*LL = 0;
			return false;
		}

		T = obs[k]->getObsSize();
		LLacc += ll;
		gama = memalc::dmatrix3d(M, N, T);
		// calculer gama: page 267 RABINER
		int t, i, j, m;
		for(t = 0; t < T; t++){
			for(j = 0; j < N; j++){
				double denom_tmp = 0;
				for(i = 0; i < N; i++){
					denom_tmp += alpha[i][t] * beta[i][t];
				}
				// m est utilis� au lieu de k pour calculer gama, d'apr�s l'�quation 54, RA
				for(m = 0; m < M; m++){
					gama[m][j][t] = (alpha[j][t] * beta[j][t] / denom_tmp) * 
						(bi_gaus[m][j][t] / bi[j][t]);
				}
			}
		}// terminer gama: tr�s bien calcul�

		// Pour c, mu et u: les �quations (52), (53), (54) RABINER
		for(j = 0; j < N; j++){
			for(m = 0; m < M; m++){
				// num�rateur et d�num�rateur de c
				for(t = 0; t < T; t++){
					nume_c[j][m] += gama[m][j][t];
					denom_c[j] += gama[m][j][t];	
				}// OK pour les coef de c				
			}
		}

		int l;
		for(j = 0; j < N; j++){
			for(m = 0; m < M; m++){
				for(l=0; l < L; l++){
					for(t = 0; t< T; t++){
						nume_mu[j][l][m] += gama[m][j][t] * obs[k]->o[l][t];
						nume_u[j][m][l][l] += gama[m][j][t] * (obs[k]->o[l][t] - 
							gmm[j]->mu[l][m]) * (obs[k]->o[l][t] - gmm[j]->mu[l][m]);
					}
				}
			}
		}

		memalc::free_dmatrix3d(gama, M, N);
		// pour r�estimer la matrice de transition, OKKKK
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				for(t = 0; t < T-1; t++){
					// l'�quation 111 de RABINER marche pas, 
					// il faut diviser le denom_a par le facteur de normalisation
					nume_a[i][j] += alpha[i][t] * a[i][j] * bi[j][t+1] * beta[j][t+1];
					denom_a[i][j] += alpha[i][t] * beta[i][t] / scale[t];
				}
			}
		}
	} // d�j� parcouru toutes les observations!!!
	// en fait, quand nous contr�lons bien les matrices de covariance et les matrices 
	// de probabilit� bi et bi_gaus, le probl�me du d�num�ratuer z�ro n'existe pas!
	// J'ajoute quand m�me ces commandes pour tester le d�num�ratuer z�ro, s'il existe un jour

	int j, m, l, i; 


	/*printf("deno_a\n");
	for(i = 0; i < N; i++){
		for(int j = 0; j< N; j++){
			printf("%f\t", denom_a[i][j]);
		}
		printf("\n");
	}

	printf("deno_c\n");
	for(j = 0; j < N; j++){
		printf("%f\t", denom_c[j]);
	}
	printf("\n");

	printf("nume_c: denu_mu et denu_u\n");
	for(j = 0; j < N; j++){
		for(m = 0; m < M; m++){
			printf("%f\t", nume_c[j][m]);	//nume_c est d�num�rateur de mu et u
		}
		printf("\n");
	}*/


	// r�estimer les param�tres du HMM
	for(j = 0; j < N; j++){
		for(m = 0; m < M; m++){
			gmm[j]->c[m] = nume_c[j][m] / denom_c[j];
			for(l = 0; l < L; l++){
				gmm[j]->mu[l][m] = nume_mu[j][l][m] / nume_c[j][m] ;
				gmm[j]->u[m][l][l] = nume_u[j][m][l][l] / nume_c[j][m] ;
				// est ce que la variance est petite???- 
				// une bonne explication de la relation entre la valeur COV_MIN_VAL et la 
				// log vraisemblance, en changeant le nombre "�tats fois gaussiennes"
				// trouvons un bon compromis ... nombre de vecteur d'observation par gaussienne
				if(gmm[j]->u[m][l][l] < COV_MIN_VAL){
					gmm[j]->u[m][l][l] = COV_MIN_VAL;
				}
			}
		}
	}

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(denom_a[i][j] != 0){
				a[i][j] = nume_a[i][j] / denom_a[i][j];
			} // si non ne pas r�estimer a[i][j], un probl�me avec SUM(1 ligne) = 1
				// mais pas tr�s GRAVE,, quand SUM != 1
			else{
				printf("%f\t", denom_a[i][j]);
				if ( j == N)	printf("\n");
			}
		}
	}

	/* We do not reestimate the initial probabilities of HMM states */

	memalc::free_dmatrix(nume_c, N);
	memalc::free_dvector(denom_c);
	memalc::free_dmatrix3d(nume_mu, N, L);
	memalc::free_dmatrix4d(nume_u, N, M, L);
	memalc::free_dmatrix(nume_a, N);
	memalc::free_dmatrix(denom_a, N);
	*LL = (LLacc/K);
	return true;
}


bool HMM::save(char * modelFile, bool binary)
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
		if(!binary){	// fichier text: pour d�boguer
			// les param�tres de base 
			fprintf(ofs, "%d\n%d\n%d\n%d\n%f\n", N, M, L, K, LLba);	// nombre d'observation
			// HMM configuration
			fprintf(ofs, "%d\n%d\n%d\n%d\n%f\n%f\n%d\n", transType, covMatrixType, bRandomInit, 
				bParameterNormalization, parameterStandardDeviation, endAccuracy, endIterationNumber);
			
			if(bParameterNormalization){
				int l;
				for(l = 0; l < L; l++){
					fprintf(ofs, "%f ", normalizationFactors[l]);
				}
				fprintf(ofs, "\n");
			}

			//les chemins Viterbi
			int k, i, j;
			for(k = 0; k < K; k++){
				fprintf(ofs, "%d ", tbStSequenceLen[k]);
			}
			fprintf(ofs, "\n");

			for(k = 0; k < K; k++){
				for(i = 0; i < tbStSequenceLen[k]; i ++){
					fprintf(ofs, "%d ", tbStSequence[k][i]);
				}
				fprintf(ofs, "\n");
			}
			fprintf(ofs, "\n");

			// probabilit�s initiales
			for(i = 0; i < N; i ++){
				fprintf(ofs, "%f ", pi[i]);
			}
			fprintf(ofs, "\n");
			// matrice de transitions
			for(i = 0; i < N; i ++){
				for(j = 0; j < N; j ++){
					fprintf(ofs, "%f ", a[i][j]);
				}
				fprintf(ofs, "\n");
			}
			// les GMMs: c
			int n, m, l;
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					fprintf(ofs, "%f ", gmm[n]->c[m]);
				}
				fprintf(ofs, "\n");
			}
			// les GMMs: mu
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					for(l = 0; l < L; l++){
						fprintf(ofs, "%f ", gmm[n]->mu[l][m]);
					}
					fprintf(ofs, "\n");
				}
				fprintf(ofs, "\n");
			}
			// les GMMs: u
			int line, col;
			for(n = 0; n < N; n++){	//parcourir tous les GMMs
				for(m = 0; m < M; m++){
					for(line = 0; line < L; line++){
						for(col = 0; col < L; col ++){
							fprintf(ofs, "%f ", gmm[n]->u[m][line][col]);
						}
						fprintf(ofs, "\n");
					}
					fprintf(ofs, "\n");
				}
				fprintf(ofs, "\n");
			}
		}else{ //fichier binaire
			fwrite((void *)&N, sizeof(N), 1, ofs);
			fwrite((void *)&M, sizeof(M), 1, ofs);
			fwrite((void *)&L, sizeof(L), 1, ofs);
			fwrite((void *)&K, sizeof(K), 1, ofs);
			fwrite((void *)&LLba, sizeof(LLba), 1, ofs);
			// HMM configuration
			fwrite((void *)&transType, sizeof(transType), 1, ofs);
			fwrite((void *)&covMatrixType, sizeof(covMatrixType), 1, ofs);
			fwrite((void *)&bRandomInit, sizeof(bRandomInit), 1, ofs);
			fwrite((void *)&bParameterNormalization, sizeof(bParameterNormalization), 1, ofs);
			fwrite((void *)&parameterStandardDeviation, sizeof(parameterStandardDeviation), 1, ofs);
			fwrite((void *)&endAccuracy, sizeof(endAccuracy), 1, ofs);
			fwrite((void *)&endIterationNumber, sizeof(endIterationNumber), 1, ofs);

			if(bParameterNormalization){
				fwrite((void *)normalizationFactors, sizeof(normalizationFactors[0]), L, ofs);
			}

			//les chemins Viterbi
			fwrite((void *)tbStSequenceLen, sizeof(tbStSequenceLen[0]), K, ofs);

			int k;
			for(k = 0; k < K; k++){
				fwrite((void *)(tbStSequence[k]), sizeof(tbStSequence[0][0]), tbStSequenceLen[k], ofs);
			}

			// probabilit�s initiales
			fwrite((void *)pi, sizeof(pi[0]), N, ofs);

			for(int n = 0; n < N; n++){	//parcourir tous les �tats
				// transition
				fwrite((void *)a[n], sizeof(double), N, ofs);
				// mixture gain
				fwrite((void *)(gmm[n]->c), sizeof(gmm[n]->c[0]), M, ofs);
				// mean
				for(int l=0; l< L; l++){
					fwrite((void *)(gmm[n]->mu[l]), sizeof(double), M, ofs);
				}
				// covariance
				for(int m = 0; m < M; m++){
					for(int l = 0; l<L; l++){
						fwrite((void *)(gmm[n]->u[m][l]), sizeof(double), L, ofs);
					}
				}
			}
		}
		fclose(ofs);
		return true;
	}
}

bool HMM::train()
{
	initHMM();

//	save("model.txt", false);
//	dump();
	if(bInit){
		double LLbefore, LLafter;
		LLbefore = LOG_ZERO;
		printf("Nombre d'etats: %d\n", N);
		printf("Nombre de gaussians: %d\n", M);
		printf("Nombre de parametres: %d\n", L);
		int loop;
		bool success;
		for(loop = 0; loop < MAX_ITER_HMM; loop++){
			success = baumwelch(&LLafter);
			printf("Iteration %d: %f\n", loop + 1, LLafter);

			if(!success){	// cause: trop d'�tat pour entrainer le HMM
				destroyHmmVariable();
				return false;
			}

			if(endIterationNumber == 0){
				if(fabs((exp(LLafter) - exp(LLbefore))/exp(LLafter)) <= endAccuracy){
				//if(fabs((LLafter - LLbefore)/LLafter) <= endAccuracy){ non
					break;
				}
				LLbefore = LLafter;
			}else{
				if(loop == endIterationNumber - 1){
					break;
				}
			}
		}

		tbStSequenceLen = memalc::ivector(K);
		tbStSequence = new int * [K];
		double * llbas = memalc::dvector(K);
		int k;
		for(k = 0; k < K; k++){
			tbStSequenceLen[k] = obs[k]->getObsSize();
			llbas[k] = viterbi(obs[k], tbStSequence + k, false);
		}

		double VARba;
		statistics::variance(llbas, K, &LLba, &VARba);

		// Calculer meanSegVecDis et varSegVecDis: concernant les vecteurs de segmentation
		// Calculer le nombre de combinaison de 2 parmi K
/*		int ncombi = statistics::combination(2, K);
		double* disArray = memalc::dvector(ncombi);
		int i, j, counter;
		counter = 1;
		for(i = 0; i < K-1; i++){
			for(j = i+1; j < K; j ++){
				disArray[counter++] = computeViterbiHamDistance(tbStSequence[i], tbStSequenceLen[i], tbStSequence[j], tbStSequenceLen[j]);
			}
		}
		statistics::variance(disArray, ncombi,&meanSegVecDis, &varSegVecDis);

		counter = 1;
		for(i = 0; i < K-1; i++){
			for(j = i+1; j < K; j ++){
				disArray[counter++] = fabs(llbas[i] - llbas[j]);
			}
		}
		statistics::variance(disArray, ncombi,&meanLLDis, &varLLDis);
		memalc::free_dvector(disArray);
*/		memalc::free_dvector(llbas);


/*		int * statSeq;
		ofstream statfs;
		statfs.open("cheminBA.txt", ios::app);
		for(k = 0; k < K; k++){
			viterbi(obs[k], &statSeq);
			for(int i = 0; i < obs[k]->getObsSize(); i ++){
				statfs << statSeq[i] << " ";
				if(statSeq[i] != statSeq[i+1]){
					statfs << "\n";
				}
			}
			statfs << "\n";
			memalc::free_ivector(statSeq);
		}
		statfs.close();
*/	
		return true;
	}else{
		// le HMM n'est pas encore bien initialis�
		// pour quoi???: trop d'�tats et de gassiennes
		// diminue le nombre d'�tat et de gaussiennes puis r�entrainer
		return false;
	}
}

/********************/

double HMM::getLLba()
{
	return LLba;
}

bool HMM::isInit()
{
	return bInit;
}

int HMM::getObsNumber()
{	
	return K;
}

int HMM::getStateNumber()
{
	return N;
}

void HMM::setNStates(int N_)
{
	N = N_;
}


void HMM::setNGaussians(int M_)
{
	M = M_;
}

void HMM::computeNormalizationFactor()
// calculer les param�tres de normalisation personalis�e
{
	// calculer les param�tres de normalisation des paras, l'�cart-type moyen
	memalc::free_dvector(normalizationFactors);
	normalizationFactors = memalc::dvector(L);	// full fill zero
	double mean, var;
	int k, l;
	for(k = 0; k < K; k ++){	
		for(l = 0; l < L; l++){
			statistics::variance(obs[k]->o[l], obs[k]->getObsSize(), &mean, &var);
			normalizationFactors[l] += sqrt(var);
		}
	}
	// moyenner
	for(l = 0; l < L; l++){
		normalizationFactors[l] = normalizationFactors[l] / K / parameterStandardDeviation;
	}
}

bool HMM::normalizeTrainBaseParameters()
{
	if(normalizationFactors == NULL){
		return false;
	}else{
		int k;
		for(k = 0; k < K; k++){
			obs[k]->normalizeParameters(normalizationFactors);
		}
		return true;
	}
}

bool HMM::normalizeObsParameters(Obs * ob)
// use to normalize a 
{
	if(bParameterNormalization){ //control the HMM property
		if(!ob->isInit())
			return false;	// not init
		if(ob->getVecSize() != L)
			return false;	// not compatible
		ob->normalizeParameters(normalizationFactors);
	}
	return true;
}

double HMM::computeViterbiHamDistance(int *firstSeq, int firstSeqLen, 
									  int *secondSeq, int secondSeqLen)
// Cette fonction est utilis�e dans le cas de comparaison des chemins de Viterbi
// En fait, elle calcule la distance Hamming entre deux vecteurs de segmentation
// Attention: ces deux chemins doivent etre g�n�r�s par ce m�me HMM
{
	double * hisx;
	double * hisy;
	double dis = 0;
	hisx = memalc::dvector(N);
	hisy = memalc::dvector(N);
	int i, n;

	for(i = 0; i < firstSeqLen; i++){
		hisx[firstSeq[i]]++;
	}
	for(i = 0; i < secondSeqLen; i++){
		hisy[secondSeq[i]]++;
	}
	for(n = 0; n < N; n++){
		dis += fabs(hisx[n] - hisy[n]);
	}
	memalc::free_dvector(hisx);
	memalc::free_dvector(hisy);
	return dis/N;
}

void HMM::setInitializationProperty(bool bRandomInit_)
{
	bRandomInit = bRandomInit_;
}


int HMM::getSeqLenK(int k)
{
	return tbStSequenceLen[k];
}

int* HMM::getSeqK(int k)
{
	return tbStSequence[k];
}

void HMM::initKObs(int k, Obs* obs)
{
	obsor[k] = (Obs*) obs;
}

void HMM::getInitObs(int k)
{
	obsor = new Obs*[k];
}

double HMM::scomputeViterbiHamDistance(int ** firstSeq, int firstSeqLen, int * secondSeq, int secondSeqLen)
{
    return computeViterbiHamDistance(*firstSeq, firstSeqLen, secondSeq, secondSeqLen);
}
int** HMM::getInitInt(void)
{
	return (new int*[1]);
}

double HMM::entropySwig(Obs *ob,int**statSeq){
	return entropy(ob, *statSeq);
}

double HMM::entropy(Obs *ob,int*statSeq){
	
	double *ak = NULL;
	double info=0;
	bool b = initProObs(ob); //init les bi, bi_gaus
	if(b){

		ak=memalc::dvector(N);
		int ipsi=0;
				
		for(int n = 0; n < N; n++){
			for(int t = 0; t < biLen; t ++){

				if(statSeq[t]==n){
					if ((bi[n][t])<=1){						
						ak[n]+= (((bi[n][t])*(log(bi[n][t])))/((log(2))));							
					
					}
					else {
						ak[n]+= 0;
					}
				}
			}
		}
	}
	    		
			for(int n = 0; n < N; n++){
			info+= (ak[n]/N);
			}

		return (-info);
}				
			
