#include "stdafx.h"
#include "dtw.h"
#include "memalc.h"
#include "definitions.h"
#include "math.h"
#include "memory.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

DTW::DTW(Obs *tst_, Obs *ref_)
// Constructor
// Compute the DTW between 2 object "Obs" (or "Signature")
// To speed up the memory acces, we have to transpose the matrix "o" of "Obs"
// acces by line is faster than by column
{
	ref = ref_;
	tst = tst_;
	// to get the length of Obs, we use getVecSize enstead of getObsSize, since the matrix "o"
	// has been transposed
	lx = ref->getVecSize();    
	ly = tst->getVecSize();    
	// And of course, to obtain the size of a vector, call getObsSize enstead of getVectorSize
	vecSize = tst->getObsSize();
	// set the default value od the costs
	setCosts(1, 1, 1);
}

DTW::~DTW()
{
}

double DTW::distance()
{
	if((lx > 2*ly) || (ly > 2*lx)){
		return MAX_DOUBLE;
	}

	int ix, iy;
	double d, d11, d21, d12;
	double ** D = memalc::dmatrix(lx, ly);	// the accumulated cost matrix 

	for(ix = 0; ix < lx; ix++){
		for(iy = 0; iy < ly; iy++){
			D[ix][iy] = MAX_DOUBLE;
		}
	}
	// Initialize the accumulated cost matrix
	D[0][0] = norm(ref->o[0], tst->o[0], vecSize);
	D[1][1] = D[0][0] + substi_cost * norm(ref->o[1], tst->o[1], vecSize);
	D[1][2] = D[0][0] + delete_cost * norm(ref->o[1], tst->o[2], vecSize);
	D[2][1] = D[0][0] + insert_cost * norm(ref->o[2], tst->o[1], vecSize);

	// we will not search every index of the cost matrix, so we have to compute the y limit
	int y_un, y_up;
	for(ix=2; ix < lx; ix++) {
		// see Rabiner book for computation of y_un, y_up
		y_un = max(ly * ix / (2 * lx), ly * (2 * ix - lx) / lx - 5); 
														// minus 5 to enlarge a little the zone
		y_un = max(y_un , 2);	// greater than 2
		y_up = min(2 * ly * ix / lx, ly * ix / (2 * lx) + ly / 2 + 5);
														// plus 5 to enlarge a little the zone
		y_up = min(y_up, ly-1);	// smaller than ly - 1
	    for(iy=y_un; iy <= y_up; iy++) {
			d=norm(ref->o[ix], tst->o[iy], vecSize);
			// Compute the cumulated distance in each direction
			d11=D[ix-1][iy-1] + substi_cost * d;
			d21=D[ix-2][iy-1] + delete_cost * d;
			d12=D[ix-1][iy-2] + insert_cost * d;
			//Choose the min and fill in D
			if(d21 > d12) {
				if(d12 > d11) {			/* Diagonal move */
					D[ix][iy] = d11;
				}else{					/* Vertical move */
					D[ix][iy] = d12;
				}
			}else if(d21 > d11) {
				D[ix][iy] = d11;
			}else {						/* Horizontal move */
				D[ix][iy] = d21;
			}
		}
	}
	
	dmin = D[lx-1][ly-1] / (lx + ly);

	/*
	// the following commands save the cost matrix in a text file
	ofstream ofs;
	ofs.open("D.txt", ios::app);
	if(ofs.is_open()){
		for(ix = 0; ix < lx; ix ++){
			for(iy = 0; iy < ly; iy ++){
				if(D[ix][iy] < MAX_DOUBLE){
					ofs << D[ix][iy] << "\t";
				}else{
					ofs << "-------\t";
				}
			}
			ofs << "\n";
		}
		ofs.close();
	}*/
		
	memalc::free_dmatrix(D, lx);
	return dmin;
}


double DTW::norm(double *vec1, double *vec2, int size)
// Compute the euclidien distance between two vectors
{
	double nor = 0;
	int i;
	for(i=0; i < size; i++){
		nor += pow(vec1[i] - vec2[i], 2); 
	}
	nor = sqrt(nor);
	return nor;
}

void DTW::setCosts(double substi_cost_, double delete_cost_, double insert_cost_)
{
	substi_cost = substi_cost_;
	delete_cost = delete_cost_;
	insert_cost = insert_cost_;
}
