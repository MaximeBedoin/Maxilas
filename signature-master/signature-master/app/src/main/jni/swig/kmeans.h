#if !defined(KMEANS_)
#define KMEANS_
#ifndef Obs_
#include "Obs.h"
#endif

#define MAX_ITER_KMEANS			20	// les options du Kmeans

class KMEANS
{
	Obs * obs;		// les vecteurs à quantifier
	int ncluster;	// nombre de clusteurs
	int L;			// taille d'un vecteur d'observation
	int T;			// T: nombre d'observation
	int *membership;// vecteur 1 x T contient l'appartenance des vecteurs à chaque classe
					// membership[t] = k; où 0 <= k < ncluster: vecteur t de
					// l'observation est appartenu au groupe k
public:  
	int * getMemberShip();
	double **means;			// les moyennes des K groupes
	double ***covariances;	// les covariances
	int * gains;			// probabilités d'appartenir à chaque groupe
	
	void dump();			// afficher les contenus
	bool isInit();

	KMEANS(Obs *obs, int ncluster, bool bRandomInit);
	virtual ~KMEANS();
private:
	void updateCovariances();
	double distance(int t, int cluster);
	bool updateCluster();
	bool bInit;
	bool cluster(bool randomInit);
};

#endif //#if !defined
