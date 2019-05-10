// global_signature.h: interface for the global_signature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(GLOBAL_SIGNATURE_)
#define GLOBAL_SIGNATURE_H
#define		MAX_FEATURE_NUMBER	100	// this is the maximum number of global features 
									// it is not so wasteful since it's better than reallocating 
									// the memory each time adding a new feature
#define	RANGE	50
#include "signature.h"
#include "GMM_SHORT.h"

class global_signature : public signature  
{
public:
	void makeShort(short mul_factor);
	bool save(char* filename, bool append);
	void normalizeGlobalFeatures(double * factors);
	global_signature(char *filename):signature(filename){N=0; F=NULL;};
	global_signature(char *filename, int L_):signature(filename, L_){N=0; F=NULL;};
	global_signature(char *filename, int L_, int T_):signature(filename, L_, T_){N=0; F=NULL;};
	global_signature(int L_, int T_): signature(L_, T_){N=0; F=NULL;};

	~global_signature();
	bool	computeGlobalFeatures();
	double	ham_distance(const global_signature *gs);

	bool copyFeatures(double *des);
	int getNumberFeatures();

	void dump();


private:
	int		N;		// number of global features. When N == 0, that means the features have not
					// been computed
	double	*F;		// vector containing the global features of the online signature

};

#endif
