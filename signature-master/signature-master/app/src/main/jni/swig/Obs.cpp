#include "stdafx.h"
#include "Obs.h"
#include "memalc.h"
#include "stdio.h"
#include "definitions.h"
#include "math.h"
#include "memory.h"
#include "statistics.h"


Obs::Obs(int L_, int T_){
	// initialisation de l'objet, taille d'un vecteur L, taille d'observation T
	// alocation de la m�moire pour o;
	// Dans ce cas l�, il faut donner le contenu � o, util dans le cas de implanter
	// avec matlab.
	// Si on donne T_ = 0, c'est seulement une racine pour une observation, 
	// elle ne contient rien sauf la taille d'un vecteur.
	// Util si on veut ajouter pas � pas les petites suites de vecteur d'observation
	L = L_;
	T = T_;
	o = memalc::dmatrix(L, T);
	bInit = true;
}

Obs::Obs(char * filename){
	// initialisation de l'objet, lecture de donn�es � partir d'un fichier de text
	FILE * in_file;
	in_file = fopen(filename, "rt");
	if (in_file == NULL){
		bInit = false;
		o = NULL;
	}else{
		// lecture de donn�es 
		// le nombre de vecteurs d'observation
		// un contr�le des en-t�tes sera bienvenu
		fscanf(in_file, "%d", &T);
		// La taille d'un vecteur d'observation
		fscanf(in_file, "%d", &L);
		// Alocation de la m�moire pour o: observation
		o = memalc::dmatrix(L, T);
		// bien alou� la matrice o[L][T], ne pas utiliser les indice 0, elles commencent par 1
		// lecture des donn�es � partir du fichier text
		int t, l;
		for(t = 0; t < T; t++){
			// chaque ligne contient un vecteur d'observation
			for(l = 0; l < L; l++){
				if(!feof(in_file)){
					fscanf(in_file, "%lf", o[l] + t);
				}else{// si non, manque de donn�es, due au mauvais "file header": L, T
					fclose(in_file);
					bInit = false;
					break;
				}
			}
		}
		fclose(in_file);
		bInit = true;
	}
}

Obs::Obs(char * filename, int L_){
	// initialisation de l'objet, lecture de donn�es � partir d'un fichier de text
	FILE * in_file;
	in_file = fopen(filename, "rt");
	if (in_file == NULL){
		bInit = false;
		o = NULL;
	}else{
		// lecture de donn�es 
		// le nombre de vecteurs d'observation
		// un contr�le des en-t�tes sera bienvenu
		T = 0;
		char * tmp = new char[512];
		while(!feof(in_file)){
			fgets(tmp, 512, in_file);
			T++;
		}
		T--;	// counted one more time
		delete tmp;
		fclose(in_file);
		if(T < 10){	// fichier vide ou peu de donn�es
			bInit = false;
			return;
		}
		in_file = fopen(filename, "rt");
		// La taille d'un vecteur d'observation, donn�e par Obs. 
		// l'en-t�te contient que le nombre d'observation
		L = L_;
		// Alocation de la m�moire pour o: observation
		o = memalc::dmatrix(L, T);
		// bien alou� la matrice o[L][T], ne pas utiliser les indice 0, elles commencent par 1
		// lecture des donn�es � partir du fichier text
		int t, l;
		for(t = 0; t < T; t++){
			// chaque ligne contient un vecteur d'observation
			for(l = 0; l < L; l++){
				if(!feof(in_file)){
					fscanf(in_file, "%lf", o[l] + t);
				}else{// si non, manque de donn�es, due au mauvais "file header": L, T
					fclose(in_file);
					bInit = false;
					break;
				}
			}
		}
		fclose(in_file);
		bInit = true;
	}
}

Obs::Obs(char * filename, int L_, int T_){
	// initialisation de l'objet, lecture de donn�es � partir d'un fichier de text
	FILE * in_file;
	in_file = fopen(filename, "rt");
	if (in_file == NULL){
		bInit = false;
		o = NULL;
	}else{
		// lecture de donn�es 
		// le nombre de vecteurs d'observation
		// un contr�le des en-t�tes sera bienvenu
		// l'en-t�te contient que le nombre d'observation
		fscanf(in_file, "%d", &T);
		if(T < 10){	// fichier vide ou peu de donn�es
			bInit = false;
			return;
		}
		// La taille d'un vecteur d'observation, donn�e par l'argument de Obs. 
		L = L_;
		// Alocation de la m�moire pour o: observation
		o = memalc::dmatrix(L, T);
		// bien alou� la matrice o[L][T], ne pas utiliser les indice 0, elles commencent par 1
		// lecture des donn�es � partir du fichier text
		int t, l; 
		for(t = 0; t < T; t++){
			// chaque ligne contient un vecteur d'observation
			for(l = 0; l < L; l++){
				if(!feof(in_file)){
					fscanf(in_file, "%lf", o[l] + t);
				}else{// si non, manque de donn�es, due au mauvais "file header": L, T
					fclose(in_file);
					bInit = false;
					break;
				}
			}
		}
		fclose(in_file);
		bInit = true;
	}
}

Obs::~Obs(){
	memalc::free_dmatrix(o, L);
}

bool Obs::isInit()
{
	return bInit;
}

void Obs::dump()
{
	// afficher le contenu d'une observation
	if (bInit){
		printf("Nombre de vecteurs: %d\n", T);
		printf("Taille d'un vecteur: %d\n", L);
		// pour tester le contenu de la matrice
		int t, l;
		for(l = 0; l < L; l++){ // pour chaque ligne
			for(t = 0; t < T; t++){ // et chaque colone
				printf("%f ", o[l][t]);
			}
			printf("\n");
		}
	}else{
		printf("L'observation n'est pas encore initialis�e!\n");
	}
}

bool Obs::addObs(Obs *obs_supl, int index, int size)
{
	// ajouter � l'observation d'une partie d'une autre observation
	// retourne FALSE si l'op�ration �chec
	// A partir de "index", le nombre de vecteur ajout� est "size"

	// V�rifier les informations
	if((index < 0) | (index >= obs_supl->getObsSize())){
		//cout << "Indice n'est pas bonne\n";
		return false;
	}

	if((size < 0) | (size >= (obs_supl->getObsSize() - index + 1))){
		//cout << "Taille n�gative ou trop grande\n";
		return false;
	}

	double **newO;
	newO = memalc::dmatrix(L, T + size);

	// copier les anciennes donn�es de Obs
	int t, l;
	for(t = 0; t < T; t++){
		for(l = 0; l < L; l++){
			newO[l][t] = o[l][t];
		}
	}

	// ajouter les donn�es � partir de obs_supl
	for(t = 0; t < size; t++){
		for(l = 0; l < L; l++){
			newO[l][T + t] = obs_supl->o[l][index + t];
		}
	}
	
	memalc::free_dmatrix(o, L);	//supprimer l'ancienne m�moire pour o
	o = newO;
	T = T + size;
	bInit = true;
	return true;
}

bool Obs::save(char *filename, bool createNew)
{
	FILE * ofs;
	if(createNew){
		ofs = fopen(filename, "wt");
	}else{
		ofs = fopen(filename, "at");
	}

	if(ofs == NULL){
		//cout << "Ne pas pouvoir ouvrir le fichier pour �crire\n";
		return false;
	}else{
		//ofs << T << " " << L << "\n";
		int t, l;
		for(t = 0; t < T; t++){
			for(l = 0; l < L; l++){
				fprintf(ofs, "%5.5f ", o[l][t]);
			}
			fprintf(ofs, "\n");
		}
		fclose(ofs);
		return true;
	}
}

int Obs::getVecSize()
{
	return L;
}

int Obs::getObsSize()
{	
	return T;
}

void Obs::selectParameters(int * index, int num)
{
	// selectionner des param�tres
	double ** newO = memalc::dmatrix(num, T);
	int i;
	for(i = 0; i < num; i ++){
		memcpy(newO[i]+1, o[index[i]]+1, T * sizeof(double));
	}

	memalc::free_dmatrix(o, L);
	L = num;
	o = newO;
}

void Obs::selectParametersSwig(int num)
{
	int * index = new int [num];
	for(int i = 0; i < num; i++){
	index[i] =  i;
	}

	selectParameters(index, num);
	delete [] index;
}

void Obs::transpose()
//Transpose matrice o
{
	double ** newO = memalc::dmatrix(T,L);
	int i, j;
	for(i = 0; i < L; i++){
		for(j = 0; j < T; j++)
			newO[j][i]=o[i][j];
	}
	memalc::free_dmatrix(o, L);
	o=newO;
	int par=T;
	T=L;
	L=par;
}

void Obs::reduceParameters(double **mean, double **std)
// Centrer et r�duire tout les param�tres de l'observation
// La moyenne et l'�carte type sont gard�es dans mean et std
{
	*mean = memalc::dvector(L);
	*std = memalc::dvector(L);
	int l, t;
	for(l=0; l < L; l++){
		statistics::variance(o[l], T, (*mean)+l, (*std)+l);
		(*std)[l] = sqrt((*std)[l]);
		for(t = 0; t < T; t++){
			o[l][t] = (o[l][t] - (*mean)[l]) / (*std)[l];
		}
	}
}

void Obs::reduceParameters()
// Centrer et r�duire tout les param�tres de l'observation
// Cette fonction ne garde pas la moyenne et l'�carte type
{
	double mean, var;
	int l, t;
	for(l=0; l < L; l++){
		statistics::variance(o[l], T, &mean, &var);
		double std = sqrt(var);
		for(t = 0; t < T; t++){
			o[l][t] = (o[l][t] - mean) / std;
		}
	}
}

bool Obs::changeCoordinates(double **v, int lrow, int urow, int nc)
// Faire une multiplication avec les vecteurs propres
// Changement de rep�re / diminuer la dimension
// YOU HAVE TO REVIEW THIS FUNCTION SINCE THE CHANGE OF INDEX/ FROM ZERO
{
	if((nc != L) || (urow >= L) || (lrow < 0)) return false;
	double ** newO = memalc::dmatrix(urow - lrow + 1, T);
	int i, t, l;
	for(i = lrow; i <= urow; i++){
		int rowO = i-lrow;
		for(t = 0; t < T; t++){
			for(l = 0; l < L; l++){
				newO[rowO][t] += v[i][l] * o[l][t];
			}
		}
	}	
	memalc::free_dmatrix(o, L);
	L=urow - lrow + 1;
	o = newO;
	return true;
}

void Obs::normalizeParameters(double *mean, double *std)
// normaliser les observations pour que leur moyenne soit mean(paras) - mean
// et leur �carte type soit std(paras) / std
{
	int l, i;
	for(l = 0; l < L; l++){
		for(i = 0; i < T; i++){
			o[l][i] = (o[l][i] - mean[l]) / std[l];
		}
	}
}

void Obs::normalizeParameters(double * normFactor)
{
	//normalise lin�airement les valeurs, mais toutes ont une m�me variance: PARAMETER_STANDARD_VAR^2
	int l, i;
	for(l = 0; l < L; l++){
		for(i = 0; i < T; i++){
			o[l][i] /= (normFactor[l]);
		}
	}
}
Obs* Obs::getme()
{
	return this;
}
