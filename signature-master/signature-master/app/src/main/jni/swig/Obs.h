#if !defined(Obs_)
#define Obs_

#define _CRT_SECURE_NO_DEPRECATE	// to disable the warning on fopen, fscanf...

#define		MAX_RADIUS					10000;
#define		MAX_L2W_RATIO				5;
class Obs{
protected:
	bool bInit;
	int L, T;			// L: taille d'un vecteur d'observation
						// T: nombre d'observation

	friend class DTW;
	friend class HMM;
	friend class GMM;
	friend class GMM_SHORT;
	friend class KMEANS;
	friend class PCA;
protected:
	double ** o;	// observation L x T, L lignes, T colonnes
					// indice commence par [0][0]

public:
	bool changeCoordinates(double **v, int lrow, int urow, int nc);
	void reduceParameters(double **mean, double **std);
	void reduceParameters();

	void selectParameters(int * index, int Num);
	void selectParametersSwig(int num);
	void normalizeParameters(double *mean, double *std);
	void normalizeParameters(double * normFactor);
	bool save(char * filename, bool createNew);	// in this case, you can also overload the operator << 
	void transpose();	// transposer la matrices **o
	bool addObs(Obs * obs_supl, int index, int size);	// you can also in this case overload the operator +
	// ajouter une partie d'observation à une autre

	int getObsSize();	// retourner T
	int getVecSize();	// retourner L

	void dump();		// afficher les données
	bool isInit();		// retourner bInit
	Obs(int L, int T);	// Créer une observation vide, comme une racine pour ajouter des vecteur
	Obs(char * filename);// charger l'orservation à partir d'un fichier de texte
	Obs(char * filename, int L_);	// charger l'orservation à partir d'un fichier de texte
									// taille de vecteur d'observation connue 
	Obs(char * filename, int L_, int T_);	// charger l'orservation à partir d'un fichier de texte
											// taille de vecteur d'observation connue 
				// par contre, T_ sera lu à partir du fichier, mais on ajoute un paramètre
				// T_ ici pour distinguer avec les autres
	
	// Attention: the folowing destructor must be virtual, but when compiling using EVC4
	// it generates an error of linking. It works fine on another compiler
	~Obs();
	// Fortunately, when inheriting from Obs (signature), signature does not have any destructor

	Obs* getme();
};

#endif // #if !defined
