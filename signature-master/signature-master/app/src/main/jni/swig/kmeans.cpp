#include "stdafx.h"
#include "kmeans.h"
#include "memalc.h"
#include "definitions.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

//#include "Winbase.h"

KMEANS::KMEANS(Obs *obs_, int ncluster_, bool bRandomInit)
{
	obs = obs_;
	ncluster = ncluster_;
	// récupérer la taille d'un vecteur d'observation
	L = obs->getVecSize();
	// et la longueur de l'observation
	T = obs->getObsSize();

	if (T >= ncluster){	// une condition plus stricte: T >> ncluster
		// alocation de la mémoire pour "membership", "means", "covariances" et "gains"
		membership = memalc::ivector(T); // les contenus sont 0
		means = memalc::dmatrix(L, ncluster);
		covariances = memalc::dmatrix3d(ncluster, L, L);
		gains = memalc::ivector(ncluster);
		bInit = true;
		
		// appeler tout de suit cluster pour regrouper les vecteurs de l'observation obs 
		// en "ncluster" groupes
		bool b = cluster(bRandomInit);
		int counter = 1;
		while (!b){
			counter ++;
			b = cluster(bRandomInit);
			if(counter == MAX_ITER_KMEANS){
				bInit = false;
				printf("depasser le nombre d'essais maximal pour K_moyennes!\n");
				break;
			}
		}
	}else{
		membership = NULL;
		means = NULL;
		covariances = NULL;
		gains = NULL;
		bInit = false;	// ne pas pouvoir faire kmeans
	}

	if(bInit){
		//calculer les matrices de covariance
		updateCovariances();
	}
}

bool KMEANS::cluster(bool randomInit)
{
	// implanter vraiment la fonction k_moyennes calculer "membership" et " means"
	int ti;
	if(randomInit){
		// the time function does not exist in the POKET PC 2003
#ifdef _WIN32_WCE
		ti = 0;
#else // #ifdef WIN32
		ti = time(0);	// initialize randomly
#endif
	}else{
		ti = 0;			//fix initialization
	}
	//printf("Seed pour Kmeans: %d\n", ti); // utiliser pour le débogage
	srand(ti);
	// initialiser les K moyennes:
	// pour la première fois, membership contient les -1s
	int t, nc, l;
	for(t = 0; t < T; t++){
		membership[t] = -1;
	}
	int ind;
	//cout << "Membership for Initialization of Kmeans: ";
	for(nc = 0; nc < ncluster; nc ++){
		ind = rand()%T;
		while (membership[ind] != -1){
			// si ce vecteur est déjà choisi
			ind = rand()%T;
		} // ok, trouvé le vecteur pas encore choisi
		//cout << "  " << ind;
		membership[ind] = nc;
		// init les moyennes
		for(l = 0; l < L; l++){
			means[l][nc] = obs->o[l][ind];
		}
	}// bien init les K moyennes par K vecteurs différents
	//cout << "\n";
	//cout.flush();

	int * oldMembership = memalc::ivector(T);
	bool arret = false;
	int loop = 0;
	//printf("Kmeans iteration:\n");

	while(!arret){
		loop++;
		//printf("%d  ", loop);
		memcpy(oldMembership, membership, T * sizeof(int));
		// parcourir tous les vecteurs
		int t;
		for(t = 0; t < T; t++){
			double dis = MAX_DOUBLE;
			int clusterNearest = 0;
			// chercher le groupe le plus proche
			for(nc = 0; nc < ncluster; nc ++){
				double dis1 = distance(t, nc);
				if(dis1 < dis){
					dis = dis1;
					clusterNearest = nc;
				}
			}
			// OK, affecter le vecteur des membres
			membership[t] = clusterNearest;
		}

		if (updateCluster()){
			// si on peut arreter le processus d'apprentissage???
			for(t = 0; t < T; t++){
				arret = true;
				if(oldMembership[t] != membership[t]){
					arret = false;
					break;
				}
			}
		}else{
			// refaire kmeans avec une autre initialisation
			memalc::free_ivector(oldMembership);
			return false;
		}
	}
	//printf("\n");
	memalc::free_ivector(oldMembership);
	return true;
}

bool KMEANS::updateCluster()
{
	// gains contient les 0
	// s'il y a un groupe qui ne contient aucun vecteur, cette opération est loupée
	int nc, l, t;
	for(nc = 0; nc < ncluster; nc ++){
		gains[nc] = 0;
	}

	// remet à 0 les means;
	for(nc = 0; nc < ncluster; nc ++){
		for(l = 0; l < L; l++){
			means[l][nc] = 0;
		}
	}

	for(t = 0; t < T; t++){
		for(l = 0; l < L; l++){
			means[l][membership[t]] = means[l][membership[t]] + obs->o[l][t];
		}
		gains[membership[t]] ++;
	}
	
	for(nc = 0; nc < ncluster; nc++){
		for(l = 0; l < L; l++){
			if (gains[nc] != 0){
				means[l][nc] /= gains[nc];
			}else{
				//refaire cluster avec une autre initialisation
				return false;
			}
		}
	}

	//dump();

	return true;
}

void KMEANS::updateCovariances()
{
	int nc, r, c, t;
	for(nc = 0; nc < ncluster; nc ++){
		for(r = 0; r < L; r++){
			for(c = 0; c < L; c++){
				covariances[nc][r][c] = 0;
			}
		}
	}

	int cluster;
	double tmp;

	for(t = 0; t < T; t++){
		cluster = membership[t];
		for(r = 0; r < L; r++){
			tmp = obs->o[r][t] - means[r][cluster];
			tmp = tmp * tmp;
			covariances[cluster][r][r] += tmp;
		}
	}

	for(nc = 0; nc < ncluster; nc++){
		for(r = 0; r < L; r++){
			if(gains[nc] > 1){
				covariances[nc][r][r] /= (gains[nc] - 1);
			}else{
				covariances[nc][r][r] = 0;
			}
		}
	}
}

double KMEANS::distance(int t, int cluster)
// calculer la distance quadratique moyenne entre le t-ième vecteur de obs et le groupe "cluster" 
{
	double dis = 0;
	double tmp;
	int l;
	for(l = 0; l < L; l++){
		tmp = (means[l][cluster] - obs->o[l][t]);
		dis = dis + tmp * tmp;
	}
	dis = sqrt(dis);
	return dis;
}

void KMEANS::dump()
// pour afficher les clusters... et leurs caractéristiques
{
	printf("Gain:\n");
	int nc, t, l, r;
	for(nc = 0; nc < ncluster; nc++){
		printf("%f ", gains[nc]);
	}
	printf("\n");

	printf("Member ship:\n");
	for(t = 0; t < T; t++){
		printf("%d ", membership[t]);
	}
	printf("\n");
	for(nc = 0; nc < ncluster; nc ++){
		printf("Cluster %d\n",  nc);
		for(l = 0; l < L; l++){
			printf("%f " , means[l][nc]); 
		}
		printf("\nCovariance: ");
		for(r = 0; r < L; r++){
			printf("%f ", covariances[nc][r][r]);
		}
		printf("\n\n");
	}
}

KMEANS::~KMEANS()
{
	// pour obs, c'est un pointeur à la mémoire créée par une autre fonction
	// ne désaloue pas, c'est dangereux!
	memalc::free_ivector(gains);
	memalc::free_ivector(membership);
	memalc::free_dmatrix(means, L);
	memalc::free_dmatrix3d(covariances, ncluster, L);
}

bool KMEANS::isInit()
{
	return bInit;
}

int * KMEANS::getMemberShip()
{
	return membership;
}
