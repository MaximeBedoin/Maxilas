/****** ATTENTION * ATTENTION * ATTENTION * ATTENTION * ATTENTION ********/
/* need to be reviewed because changing of index: from 0 instead of 1*/
/****** ATTENTION * ATTENTION * ATTENTION * ATTENTION * ATTENTION ********/

#include "stdafx.h"
#include "Obs.h"
#include "memalc.h"
#include "pca.h"
#include "math.h"
#include "stdio.h"
#include "definitions.h"

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);

PCA::PCA(Obs *obs)
{
	L = obs->getVecSize();
	int T = obs->getObsSize();
	if(L > 0){
		A = memalc::dmatrix(L, L);
		x = memalc::dmatrix(L, L);
		lambda = memalc::dvector(L);
		mean = NULL; std = NULL;
		// compute the covariance matrix
		// we should reduce all parameters to have a standard variation 1
		obs->reduceParameters(&mean, &std);
		
		double ** tmp = obs->o;
		int Ai, Aj, t;
		for(Ai = 1; Ai <= L; Ai++){
			for(Aj = 1; Aj <= L; Aj++){
				for(t = 1; t <= T; t++){
					A[Ai][Aj] += tmp[Ai][t] * tmp[Aj][t];
				}
			}
		}
		
		// calculer tout de suite les vecteurs propres et valeurs propres
		int nrot;
		//dump("A.txt");
		jacobi(A, L, lambda, x, &nrot);
		eigsrt(lambda, x, L);

		// change colunm to row eigenvectors
		double ** newx = memalc::dmatrix(L, L);
		int i, j;
		for(i = 1; i <= L; i++){
			for(j = 1; j <= L; j++)
				newx[j][i] = x[i][j];
		}
		memalc::free_dmatrix(x, L);
		x = newx;
	}else{
		A = NULL;
		x = NULL;
		lambda = NULL;
		mean = NULL;
		std = NULL;
	}	
}


void PCA::eigsrt(double *d, double **v, int n)
/*Given the eigenvalues d[1..n] and eigenvectors v[1..n][1..n] as output 
from jacobi (�11.1) or tqli (�11.3), this routine sorts the eigenvalues 
into descending order, and rearranges the columns of v correspondingly. 
The method is straight insertion.*/
{
	int k,j,i;
	double p;
	for (i=1;i<n;i++) {
		p=d[k=i];
		for (j=i+1;j<=n;j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1;j<=n;j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}

void PCA::jacobi(double **a, int n, double *d, double **v, int *nrot)

/*Computes all eigenvalues and eigenvectors of a real symmetric matrix a[1..n][1..n].
On output, elements of a above the diagonal are destroyed. d[1..n] returns the 
eigenvalues of a. v[1..n][1..n] is a matrix whose columns contain, on output, 
the normalized eigenvectors of a. nrot returns the number of Jacobi rotations 
that were required.*/
{
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c,*b,*z;
	b=memalc::dvector(n);
	z=memalc::dvector(n);
	for (ip=1;ip<=n;ip++) { //Initialize to the identity matrix.
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}

	for (ip=1;ip<=n;ip++) { //Initialize b and d to the diagonal of a. 
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0; //This vector will accumulate terms of the form tapq as in equation (11.1.14).
	}
	*nrot=0;
	for (i=1;i<=50;i++) {
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) { //Sum o.-diagonal elements.
			for (iq=ip+1;iq<=n;iq++)
				sm += fabs(a[ip][iq]);
		}

		if (sm == 0.0) { //The normal return, which relies on quadratic convergence to machine under.ow.
			memalc::free_dvector(z);
			memalc::free_dvector(b);
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n); //...on the .rst three sweeps.
		else
			tresh=0.0; //...thereafter.
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a[ip][iq]);//After four sweeps, skip the rotation if the o.-diagonal element is small.
				if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
					&& (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
					a[ip][iq]=0.0;	//468 Chapter 11. Eigensystems

				/*Sample page from NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
				Copyright (C) 1988-1992 by Cambridge University Press. Programs Copyright (C) 1988-1992 by Numerical Recipes Software.
				Permission is granted for internet users to make one paper copy for their own personal use. Further reproduction, or any copying of machinereadable
				files (including this one) to any server computer, is strictly prohibited. To order Numerical Recipes books or CDROMs, visit website
				http://www.nr.com or call 1-800-872-7423 (North America only), or send email to directcustserv@cambridge.org (outside North America).*/

				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((float)(fabs(h)+g) == (float)fabs(h))
						t=(a[ip][iq])/h;		//t = 1/(2.)
					else {
						theta=0.5*h/(a[ip][iq]); //Equation (11.1.10).
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1;j<=ip-1;j++) { //Case of rotations 1 = j < p.
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) { //Case of rotations p < j < q.
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) { //Case of rotations q < j = n.
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}

		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];	//Update d with the sum of tapq,
			z[ip]=0.0;		//and reinitialize z.
		}
	}
	printf("Too many iterations in routine jacobi");
}

PCA::~PCA()
{
	memalc::free_dmatrix(A, L);
	memalc::free_dmatrix(x, L);
	memalc::free_dvector(lambda);
	memalc::free_dvector(mean);
	memalc::free_dvector(std);
}

bool PCA::dump(char *filename)
{
	FILE * fs;
	fs = fopen(filename, "wt");
	if(fs != NULL){
		fprintf(fs, "A:\n");
		int i, j;
		for(i = 1; i <= L; i++){
			for(j = 1; j <= L; j++){
				fprintf(fs, "%f ", A[i][j]);
			}
			fprintf(fs, "\n");
		}

		fprintf(fs, "\n\nx:\n");
		for(i = 1; i <= L; i++){
			for(j = 1; j <= L; j++){
				fprintf(fs, "%f ", x[i][j]);
			}
			fprintf(fs, "\n");
		}
		
		fprintf(fs, "\n\nlambda:\n");
		for(i = 1; i <= L; i++){
			fprintf(fs, "%f ", lambda[i]);
		}
		fprintf(fs, "\n");
		return true;
	}else{
		return false;
	}
}
