/****** ATTENTION * ATTENTION * ATTENTION * ATTENTION * ATTENTION ********/
/* need to be reviewed because changing of index: from 0 instead of 1*/
/****** ATTENTION * ATTENTION * ATTENTION * ATTENTION * ATTENTION ********/

#if !defined(PCA_)
#define PCA_
class PCA
{
public:
	bool dump(char * filename);
	int L;			// nombre de paramètre/taille de la matrice A
	double ** A;	// matrice de covariance, symétrique
	double ** x;	// eigenvectors, les lignes sont les vecteurs propres
	double *lambda;	// eigenvalues
	double *mean;
	double *std;

	void jacobi(double **a, int n, double *d, double **v, int *nrot);
	void eigsrt(double *d, double **v, int n);
	PCA(Obs *obs);
	virtual ~PCA();

};

#endif
