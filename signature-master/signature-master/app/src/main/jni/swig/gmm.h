#if !defined(GMM_)
#define GMM_
#include "Obs.h"
#include "statistics.h"
#define _CRT_SECURE_NO_DEPRECATE
#define LOG_ZERO				-1e5	//ou MIN_DOUBLE

// Take attention to this value: the minimum probability
// In our program, we compute the logarithme of the probability,
// then we have to prevent some very low value of probability.

// To obtain good result when using the GMM_SHORT, set it to 1e-100
// when using the float HMM or GMM, pls set it to 1e-40: seat-of-the-pants experience
#define PRO_MIN_VAL				1e-40

#define COV_DIAG				1	
#define COV_FULL				2

//The default standard deviation when using the parameter normalization option
#define PARAMETER_STANDARD_VARIATION	2	

#define PARAMETER_NORMALIZATION_YES		true	
#define PARAMETER_NORMALIZATION_NO		false

class GMM{

	friend class HMM;
	friend class GMM_SHORT;

	bool bInit;			// the GMM has been initialized?
	int M, L;			// M: number of gaussians 
						// L: size of observation vector (number of parameters)

	int covMatrixType;		//covariance matrix type
	bool bRandomInit;		//init the HMM ramdomly or not each time run the prog: debug purpose
	bool bParameterNormalization;	// normalize observation parameters or not
	double parameterStandardDeviation;	// the parameter standard deviation after the normalization
	double * normalizationFactors;
	double endAccuracy;			// stop criteria: normally 1e-2
	int endIterationNumber;		// if 0, endAccuracy is used as stop creteria
								// if different of 0, it's the number of iteration while training with BaumWelch

	Obs * obs;			// Point to training base

	double ** bi_gaus;//M x T: compute the probabilities of O(t) given by each gaussian
	double ** gama;		//M x T: proportion of each gaussian by using also mixture weight
						//computed from bi_gaus. 
						//See "Assignment 3: GMM Speaker Identification 2E1400 Speech Signal Processing"

	double ** mu;		// L x M		:gaussian means
	double *** u;		// M x L x L	:covariance matrix
	double *c;			// 1 x M		:gaussian 
	double logLikelihood;
	
	void computeNormalizationFactor();

public:
	GMM(char* modelFile, bool binary);
	bool save(char * modelFile, bool binary);
	int getVectorSize(); 
	double getTrainingLogLikelihood();
	void computeGama();
	int getGaussianNumber();
	void setGaussianNumber(int M_);
	bool assignObs(Obs *ob);
	bool normalizeObsParameters(Obs *ob);
	double test(Obs *ob);
	double restimateModel();
	bool train();
	bool initProObs(Obs *ob);
	bool isInit();
	void dump();
	GMM(int M_, int L_, int covMatrixType_ = COV_DIAG,
					bool bRandomInit_ = false,
					bool bParameterNormalization_ = PARAMETER_NORMALIZATION_YES, 
					double parameterStandardDeviation_ = PARAMETER_STANDARD_VARIATION,
					double endAccuracy_ = 0,
					int endIterationNumber_ = 10);			// create the GMM
	virtual ~GMM();
	bool initGMM(Obs *obs_gmm);		// init the GMM with the Obs
};

#endif // #if !defined
