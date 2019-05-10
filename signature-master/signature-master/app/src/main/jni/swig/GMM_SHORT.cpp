//////////////////////////////////////////////////////////////////////
//  GMM_SHORT.cpp: implementation of the GMM_SHORT class.
//	This class takes a real GMM as input, then integerise all elements
//  of the normal GMM by saving them in SHORT. It performs also some
//  pre-computations to avoid them in future when constructing the object.
//
//  ATTENTION: The real GMM must be constructed with normalisation techniques.
//  
//  This class is created for the test with SHORT number. There exists
//  only the computation of log_likelihood functionality. When testing
//  only addition and multiplication operations are needed. NO LOG NO EXP required

//  This class does not cover all configuration of the GMM, only for 
//  diagonal covariance matrix.
// 
//  This study is specially for the SmartCard
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMM_SHORT.h"
#include "memalc.h"
#include "math.h"
#include "definitions.h"
#include "stdio.h"

/*
	Utility function: control the overflow and underflow problem when 
	computing on short number. Subject to be modified
*/
short GMM_SHORT::ToShort(double dl){
	if (dl <= MIN_SHORT){
		printf("==============================================");
		return MIN_SHORT;
	}else if (dl >= MAX_SHORT){
		printf("==============================================");
		return MAX_SHORT;
	}else{
		if (dl - short(dl) >= 0.5){ // positif
			return short(dl) + 1;
		}else if (dl - short(dl) <= -0.5){ // negatif
			return short(dl) - 1;
		}else{
			return short(dl);
		}
	}
}

/* Create the GMM_SHORT from a real GMM*/
GMM_SHORT::GMM_SHORT(GMM *gmm)
{
	// retrieve important information
	if (!gmm->isInit()){
		bInit = false;
		mu = NULL;
		u = NULL;
		log_gaus_coef = NULL;
		return;
	}

	bInit = true;
	L = gmm->getVectorSize();
	M = gmm->getGaussianNumber();
	LLBA = ToShort(LL_IMPROVEMENT * gmm->getTrainingLogLikelihood() * L);

	// alocate the memory
	// mean vector
	mu = memalc::smatrix(L, M);
	// covariance matrix
	u = memalc::smatrix(M, L);		
	// constant coefficience of each gaussian 
	log_gaus_coef = memalc::svector(M);

	// copy data from the GMM, knowing that the covariance matrix is diagonal
	short l, m;
	// copy mu
	for(l = 0; l < L; l++){
		for(m = 0; m < M; m++){
			mu[l][m] = ToShort(gmm->mu[l][m]);
		}
	}
	// copy u
	for(m = 0; m < M; m++){
		for(l = 0; l < L; l++){
			u[m][l] = ToShort(gmm->u[m][l][l]);
			if (u[m][l] < 1)
				u[m][l] = 1; //threshold for the variance, never 0.
		}
	}
	// compute the log_gaus_coef
	
	for(m = 0; m < M; m++){
		double det_u = 1;
		for(l = 0; l < L; l++){
			det_u *= u[m][l];
		}

		double cons = gmm->c[m] * pow(2 * PI, double(-L/2)) / sqrt(det_u);
		log_gaus_coef[m] = ToShort(log(cons));
	}
}

GMM_SHORT::~GMM_SHORT()
{
	if (bInit){
		// desalocate the memory:
		// mean vector of the gaussian
		memalc::free_smatrix(mu, L);
		// covariance matrix
		memalc::free_smatrix(u, M);		
		// constant coefficience of each gaussian 
		memalc::free_svector(log_gaus_coef);
	}
}

/* 
	Test a SHORT signature with only addition and multiplication operations.
*/
short GMM_SHORT::test(Obs *ob)
{
	// Control the signature
	if((ob->getVecSize() != L)){
		return false;
	}

	short T = ob->getObsSize();
	short m, l, t;

	short * max_log_probabilities;
	max_log_probabilities = memalc::svector(T);//1 x T: the log probabilities of O(t) given by the best gaussian
	for(t = 0; t < T; t++){
		max_log_probabilities[t] = MIN_SHORT;
	}

	//Compute the distance Malhanobis minimum from one observation to all gaussian centers,
	//Weighted by log_gaus_coef[m]
	for(t = 0; t < T; t++){	// For each singature point/observation
		for(m = 0; m < M; m++){		//for each gaussian

			short mal_distance = 0;			// that's the Malanobis distance
			
			for(l = 0; l < L; l++){
				mal_distance = ToShort(mal_distance + ToShort(ToShort((ob->o[l][t] - mu[l][m]) 
					* (ob->o[l][t] - mu[l][m])) / u[m][l]));
			}
			short tmp = ToShort(log_gaus_coef[m] - mal_distance/2);

			if(max_log_probabilities[t] < tmp ){
				max_log_probabilities[t] = ToShort(tmp);
			}
		}
		//printf("max_log_probabilities %d: %d\n", t, max_log_probabilities[t]);

	}

	// implementation of the SUM_BIG_SHORT to avoid the overflow problem
	// These code compute the average value of a short array of size T
	short sum_quotient = 0;
	short sum_remainder = 0;
	short quotient;
	for(t = 0; t < T; t++){
		quotient = max_log_probabilities[t] / T;
		sum_quotient += quotient;
		sum_remainder += (max_log_probabilities[t] - quotient * T);
		if(sum_remainder <= -T){  // they are all negatif
			quotient = sum_remainder / T;
			sum_quotient += quotient;
			sum_remainder = (sum_remainder - quotient * T);
		}
	}
	memalc::free_svector(max_log_probabilities);

	// multiple the log_likelihood LL_IMPROVEMENT time to improve the precision
	short LL;
	LL = sum_quotient * LL_IMPROVEMENT + (sum_remainder * LL_IMPROVEMENT) / T;
	return LL;
}

short GMM_SHORT::getTrainingLogLikelihood()
{
	return LLBA;
}

bool GMM_SHORT::save(char *modelFile, bool binary)
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
		if(!binary){
			fprintf(ofs, "%d\n%d\n%d\n", M, L, LLBA);

			short l, m;
			// log_gaus_coef
			for(m = 0; m < M; m++){
				fprintf(ofs, "%d ", log_gaus_coef[m]);
			}
			fprintf(ofs, "\n");
			
			// mu
			for(m = 0; m < M; m++){
				for(l = 0; l < L; l++){
					fprintf(ofs, "%d ", mu[l][m]);
				}
				fprintf(ofs, "\n");
			}

			// u
			for(m = 0; m < M; m++){
				for(l = 0; l < L; l++){
					fprintf(ofs, "%d ", u[m][l]);
				}
				fprintf(ofs, "\n");
			}
		}else{
			// reserve for future
		}
		fclose(ofs);
		return true;
	}
}

