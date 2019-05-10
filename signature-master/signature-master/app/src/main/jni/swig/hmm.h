#if !defined(HMM_)
#define HMM_
#include "gmm.h"

/*The maximum number of iteration to train the HMM*/
#define MAX_ITER_HMM			30

/*end accuracy, stop criteria, training precision*/
#define END_ACCURACY			1e-2

/*if 0, use END_ACCURACY as stop criteria
if not, stop the training process after END_ITERATION_NUMBER*/
#define END_ITERATION_NUMBER		0	


/*Definition of some HMM's topologies
In this librery, we will install only the Left-Right and Full topologies */
#define TRANS_LR				1	//left-right
#define TRANS_BAKIS				2
#define TRANS_ERGODIC			3	//full connexion

class HMM{
	bool bInit;			// The HMM was initialized

	int N;				// number of states
	int M;				// number of gaussians
	int L;				// size of an obervation vector
	int K;				// number of observations, length of the observations sequence O
	double LLba;		// mean log_likelihood computed on the training base

	double kjbqksdfiqsdf;	// that is something very strange in Embedded visual C++ 4.0
							// if you supress this unuserful variable, Ginas doesn't work:))
							// in fact, you have to add something to have 8 bytes more,
							// if not, in the testModel function, when accessing the 
							// hmm.tbStSequenceLen[k], it's not the good adresse. 

public:
	int ** tbStSequence;	// Optimal states sequence, given by Viterbi, on the training bases "obs"
	int * tbStSequenceLen;	// training base state sequence length; length of the training base sequences

private:
	double * normalizationFactors; //1xL: normalisation factor of the personalized feature normalisation 

	double **a;			// N x N:	transition matrix;
	double *pi;			// 1 x N:	probability of the initial state

	int biLen;		// longeur de bi (voir en bas) actuel, peut être changée en fonction de la longeur
					// de l'observation utilisée pour calculer bi (avant appeler viterbi...)
	double ** bi;	// N x biLen:	proba de voir un vecteur à chaque état
		// il faut avoir une autre variable qui contient la probabilité à chaque gaussienne
		// bi_gaus contient M (nombre de gaussiennes) matrices comme bi. c'est la prob de voir
		// un vecteur à un état sachant la gaussinne, y compris le gain de mixture
	double *** bi_gaus; //M x N x biLen

	double ** alpha;	// même forme que bi, et la taille N x betaLen
	double ** beta;		// même forme que bi, et la taille N x alphaLen
	int alphaLen, betaLen;	// en fait, alphaLen == betaLen; pourtant, nous pouvons utiliser
							// forward pour calculer LL d'une observation, je m'intéresse à
							// deux valeur différentes
	double * scale;			// scale est utilisée par Backward, Fordward et baumwelch
	Obs **obs;				// 1 x K:	les K observations pour entraîner le modèle
	GMM ** gmm;				// 1 x N:	la distribution aux états, a chaque état, 
							// nous avons une GMM *

	/**** Caracteristics of the HMM ****/
	int transType;			//transition matrix type 
	int covMatrixType;		//covariance matrix type
	bool bRandomInit;		//init the HMM ramdomly or not each time run the prog: debug purpose
	bool bParameterNormalization;	// normalize observation parameters or not
	double parameterStandardDeviation;	// the parameter standard deviation after normalized
	double endAccuracy;			// Critère d'arret: normally 1e-2
	int endIterationNumber;		// Si 0, utiliser endAccuracy comme critère d'arret
								//Si non, c'est le nombre d'itération d'EM pour BW

	Obs ** distributeObs(int transType);

	void initTransitionMatrix(int transType);
	void initInitialProbabilities(int transType);
	bool initProObs(Obs * ob);
	bool backward(Obs *ob);
	void destroyHmmVariable();
	bool normalizeTrainBaseParameters();
	void computeNormalizationFactor();

	Obs ** obsor;

public:
	void setInitializationProperty(bool bRandomInit_);
	double computeViterbiHamDistance(int * firstSeq, int firstSeqLen, int * secondSeq, int secondSeqLen);
	bool normalizeObsParameters(Obs *ob);
	void setNStates(int N_);
	void setNGaussians(int M_);
	double getLLba();

	bool assignObs(int K_);

	int getStateNumber();
	int getObsNumber();

	bool isInit();

	HMM(char * modelFile, bool binary);	// charger le modèle à partir d'un fichier
	HMM(int N_, int M_, int transType_ = TRANS_LR, 
		int covMatrixType_ = COV_DIAG,
		bool bRandomInit_ = true,
		bool bParameterNormalization_ = PARAMETER_NORMALIZATION_YES, 
		double parameterStandardDeviation_ = PARAMETER_STANDARD_VARIATION,
		double endAccuracy_ = END_ACCURACY,
		int endIterationNumber_ = END_ITERATION_NUMBER);	// init le nombre d'états et de gaussinnes
	
	bool initHMM();

	double viterbi(Obs *ob, int ** statSeq, bool last_state_exit_oblige);


	double forward(Obs *ob);
	bool baumwelch(double * LL);	// LL: average log-likelihood on all observations
	bool train();
	bool save(char * modelFile, bool binary);

	void dump();
	// Attention: the folowing destructor must be virtual, but when compiling using EVC4
	// it generates an error of linking. 
	~HMM();	
	int getSeqLenK(int);
	int* getSeqK(int);
	void initKObs(int k, Obs* obs);
	void getInitObs(int k);
	double scomputeViterbiHamDistance(int ** firstSeq, int firstSeqLen, int * secondSeq, int secondSeqLen);
	int** getInitInt(void);

	double entropy(Obs *ob, int * statSeq);
	double entropySwig(Obs *ob,int**statSeq);
};

#endif // #if !defined
