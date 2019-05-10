#if !defined(memalc_)
#define memalc_
class memalc  
{
public:
	static void free_dmatrix4d(double **** dmat4d, int nm3d, int nm, int nr);
	static double **** dmatrix4d(int nm3d, int nm, int nr, int nc);
	static void free_imatrix(int ** imat, int nr);
	static double *dvector(int nel);
	static double **dmatrix(int nr, int nc);
	static double ***dmatrix3d(int nm, int nr, int nc);

	static int *ivector(int nel);
	static int **imatrix(int nr, int nc);

	static void free_dvector(double * dvec);
	static void free_dmatrix(double ** dmat, int nr);
	static void free_dmatrix3d(double *** dmat3d, int nm, int nr);
	static void free_ivector(int * ivec);

	static short *svector(short nel);
	static short **smatrix(short nr, short nc);	

	static void free_svector(short * svec);
	static void free_smatrix(short ** smat, short nr);

};

#endif //!defined(memalc_)
