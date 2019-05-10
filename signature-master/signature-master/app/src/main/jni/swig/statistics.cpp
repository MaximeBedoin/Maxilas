#include "stdafx.h"
#include "statistics.h"

void statistics::variance(double *elements, int number, double *mean_, double *var_)
{
	double mean = 0;
	double var = 0;
	int i;
	for(i = 0; i < number; i ++){
		mean += elements[i];
	}
	mean /= number;

	for(i = 0; i < number; i ++){
		var += (elements[i] - mean) * (elements[i] - mean);
	}
	
	if(number == 1){
		var = 0;
	}else{
		var /= (number - 1);
	}

	if(var == 0) var = COV_MIN_VAL;

	*mean_ = mean;
	*var_ = var;
}

double statistics::median(double *windows, int size)
// Calculer la valeur médianne des éléments de "windows"
{
	double tmp;
	int i, j;
	for(i = 0; i < size; i ++){
		for(j = size - 2; j >= i; j--){
			if(windows[j+1] < windows[j]){
				tmp = windows[j+1];
				windows[j+1] = windows[j];
				windows[j] = tmp;
			}
		}
	}
	int m = int(size/2);
	if( size == 2 * m){
		return (windows[m-1] + windows[m])/2;
	}else{
		return windows[m];
	}
}


int statistics::combination(int k, int N)
{
	// compter le nombre de combinaisons de k parmi N éléments
	if((k > N) || (k <= 0) || (N <= 0)) return 0;
	int ncombi = 1;
	int i, j, l;
	for(i = 1; i <= N; i++)	ncombi *= i;
	for(j = 1; j <= k; j++) ncombi /= j;
	for(l = 1;  l<= (N-k); l++) ncombi /= l;
	return ncombi;	
}
