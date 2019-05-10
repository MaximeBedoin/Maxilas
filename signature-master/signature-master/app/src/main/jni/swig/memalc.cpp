#include "stdafx.h"
#include "memalc.h"
#include "definitions.h"

double * memalc::dvector(int nel){
	// alouer la mémoire pour un vecteru des doubles
	// nel: nombre d'éléments
	double * dvec;
	dvec = new double [nel];
	int i;
	for(i = 0; i < nel; i ++){
		dvec[i] = 0.0;
	}
	return dvec;	
}

int * memalc::ivector(int nel){
	// alouer la mémoire pour un vecteur des ints: indice commence par 1
	// nel: nombre d'éléments
	// le contenu des éléments sont mis 0
	int * ivec;
	ivec = new int [nel];
	int i;
	for(i = 0; i < nel; i ++){
		ivec[i] = 0;
	}
	return ivec;	
}

int ** memalc::imatrix(int nr, int nc)
{
	int **imat;
	imat = new int* [nr];
	int r;
	for(r = 0; r < nr; r++){
		imat[r]= ivector(nc);
	}
	return imat;
}

short * memalc::svector(short nel){
	short * svec;
	svec = new short [nel];
	short i;
	for(i = 0; i < nel; i ++){
		svec[i] = 0;
	}
	return svec;	
}

short ** memalc::smatrix(short nr, short nc)
{
	short **smat;
	smat = new short* [nr];
	short r;
	for(r = 0; r < nr; r++){
		smat[r]= svector(nc);
	}
	return smat;
}

double ** memalc::dmatrix(int nr, int nc){
	// alouer la mémoire pour une matrice des doubles
	// nr: nombre de lignes
	// nc: nombre de colonnes
	// pour accéder à un élément: dmat[r][c]
	double ** dmat;
	dmat = new double* [nr];
	int r;
	for(r = 0; r < nr; r++){
		dmat[r]= dvector(nc);
	}
	return dmat;
}

double *** memalc::dmatrix3d(int nm, int nr, int nc){
	// alouer la mémoire pour une matrice des doubles: indice commence par 1
	// nm: nombre de matrice 2D
	// nr: nombre de lignes
	// nc: nombre de colonnes
	// pour accéder à un élément: dmat3d[nm][r][c]
	double *** dmat3d;
	dmat3d = new double ** [ nm];
	int i;
	for (i = 0; i < nm; i++){
		dmat3d[i]=  dmatrix(nr, nc);
	}
	return dmat3d;
}

double **** memalc::dmatrix4d(int nm3d, int nm, int nr, int nc)
{
	double **** dmat4d;
	dmat4d = new double *** [nm3d];
	int i;
	for(i = 0; i < nm3d; i++){
		dmat4d[i] = dmatrix3d(nm, nr, nc);
	}
	return dmat4d;
}

void memalc::free_dvector(double * dvec){
	// if (dvec != NULL) // you do not have to test it, C++ do it for you
		delete [] dvec;
}

void memalc::free_ivector(int * ivec){
		delete [] ivec;
}

void memalc::free_imatrix(int **imat, int nr)
{
	if (imat != NULL){
		int r;
		for (r = 0; r < nr; r++){
			free_ivector(imat[r]);
			imat[r] = NULL;
		}
		delete [] imat;
	}
}

void memalc::free_svector(short * svec){
		delete [] svec;
}

void memalc::free_smatrix(short **smat, short nr)
{
	if (smat != NULL){
		short r;
		for (r = 0; r < nr; r++){
			free_svector(smat[r]);
			smat[r] = NULL;
		}
		delete [] smat;
	}
}

void memalc::free_dmatrix(double ** dmat, int nr){
	if (dmat != NULL){
		int r;
		for (r = 0; r < nr; r++){
			free_dvector(dmat[r]);
			dmat[r] = NULL;
		}
		delete [] dmat;
	}
}

void memalc::free_dmatrix3d(double *** dmat3d, int nm, int nr){
	if (dmat3d != NULL){
		int m;
		for(m = 0; m < nm; m++){
			free_dmatrix(dmat3d[m], nr);
		}
		delete [] dmat3d;
	}
}

void memalc::free_dmatrix4d(double ****dmat4d, int nm3d, int nm, int nr)
{
	if(dmat4d !=NULL){
		int i;
		for(i = 0; i< nm3d; i ++){
			free_dmatrix3d(dmat4d[i], nm, nr);
		}
		delete [] dmat4d;
	}
}

