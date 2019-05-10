// GMM_SHORT.h: interface for the GMM_SHORT class.
// This class simulate the GMM computation using only 2 byte short number
//////////////////////////////////////////////////////////////////////

#if !defined(GMM_SHORT_)
#define GMM_SHORT_
#include "gmm.h"

#define MIN_SHORT	-32768
#define MAX_SHORT	32767
#define LL_IMPROVEMENT	10

class GMM_SHORT
{
public:
	short getTrainingLogLikelihood();
	short test(Obs *ob);
	bool save(char * modelFile, bool binary);
	GMM_SHORT(GMM *gmm);
	virtual ~GMM_SHORT();
	static short ToShort(double dl);

private:
	bool bInit;
	short M, L;			// M: number of gaussians 
						// L: size of the observation vector / number of parameters

	short LLBA;
	short ** mu;		// L x M		:gaussian centers / gaussian means 
	short ** u;			// M x L		:Covariance matrix. Since our covariance 
						//				matrix is diagonal, we use vector to save 
						//				only the diagonal of this matrix, then M vector for M matrix
						// attention: this is a pruning version, specified for PDA and smart card

	/* Instead of saving the mixture gain c[1:M], we save the logarithm of the 
	coeffician of the gaussian:
	log(c[m] * pow(2 * PI, double(-L/2)) / sqrt(det(u[m])))
	by this way, we obtain more precision than saving separately the log_c and XXX
    */
	short *log_gaus_coef;	// 1 x M	
};

#endif // !defined(GMM_SHORT_)
