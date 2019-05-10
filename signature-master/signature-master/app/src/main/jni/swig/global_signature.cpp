#include "stdafx.h"
#include "global_signature.h"
#include "math.h"
#include "memalc.h"
#include "stdio.h"
#include "memory.h"

global_signature::~global_signature(){
	delete [] F;
}

void global_signature::dump(){
	printf("%d global features:\n", N);
	for(int n=0; n < N; n++)
		printf("%6.2f\t", F[n]);
	printf("\n");
}


bool global_signature::save(char *filename, bool append)
{
	FILE *f;
	if(append)
		f = fopen(filename, "at");
	else
		f = fopen(filename, "wt");

	if(!f) return false;
	
	int i;
	for(i=0;i<N; i++)
		fprintf(f, "%f\t", F[i]);
	fprintf(f, "\n");
	fclose(f);
	return true;
}

void global_signature::normalizeGlobalFeatures(double *factors)
{
	for(int i = 0; i < N ; i++)
		if(factors[i] > 1e-20)
			F[i] /= factors[i];
}

double global_signature::ham_distance(const global_signature *gs){
// compute the euclidian distance to another signature
// send to this function the pointer to avoid copying of object
	double d = 0;
	for(int i = 0; i < N; i++)
		//d += pow(gs->F[i] - F[i], 2);
		d += fabs(gs->F[i] - F[i]);
	// after some experiement, I found that the Hamming distance is 
	// much better than the euclidian distance.
	// reason:! square increases the distance > 1 and descreases
	// the distance < 1
	//d = sqrt(d);
	return d;
}

bool global_signature::copyFeatures(double * des){
	if (N == 0)	// there is no feature to copy
		return false;

	memcpy(des, F, N * sizeof(F[0]));
	return true;
}

int global_signature::getNumberFeatures(){
	return N;
}

bool global_signature::computeGlobalFeatures(){
	if(!bInit)	// cannot compute the global if the signature has not been init.
		return false;

	delete [] F; // no problem if you call computeGlobalFeatures twice
	F = memalc::dvector(MAX_FEATURE_NUMBER);
	N = 0;

	int k;
	double *veloc_x = o[2];
	double *veloc_y = o[3];
	double *veloc =	  o[4];
	double *accel_x = o[5];
	double *accel_y = o[6];
	double *accel=    o[7];
	double *accel_t = o[8];

	double max_veloc_x = veloc_x[0];
	double min_veloc_x = veloc_x[0];
	double max_veloc_y = veloc_y[0];
	double min_veloc_y = veloc_y[0];

	for(k = 1; k < T; k ++){
		if(max_veloc_x < veloc_x[k])	
			max_veloc_x = veloc_x[k];
		if(min_veloc_x > veloc_x[k])
			min_veloc_x = veloc_x[k];
		if(max_veloc_y < veloc_y[k])	
			max_veloc_y = veloc_y[k];
		if(min_veloc_y > veloc_y[k])	
			min_veloc_y = veloc_y[k];
	}

	/*****************************************************************/
	//Signature Duration					1
	F[N++] = double(T)/100;
	/*****************************************************************/
	//NO of sign changes in X				2
	// it also the number of strocks
	int count = 0;
	for(k=1; k<T; k++){
		if (veloc_x[k - 1] * veloc_x[k] < 0)
			count++;
	}
	F[N++] = double(count) / 10;
	/*****************************************************************/

	/*****************************************************************/
	//No of Sign changes in Y				3
	count = 0;
	for(k=1; k<T ; k++){
		if (veloc_y[k - 1] * veloc_y[k] < 0)
			count++;
	}
	F[N++] = double(count) / 10;	
	/*****************************************************************/

	/*****************************************************************/
	//Std Deviation of accel_x by amax_x	4
	double mean_accel_x = 0;
	double std_accel_x=0;
	double max_accel_x = accel_x[0];
	for(k = 0; k < T; k ++){
		mean_accel_x += accel_x[k];
		if (max_accel_x < accel_x[k])
			max_accel_x = accel_x[k];	
	}
	mean_accel_x /= T;
	for(k = 0; k < T; k++)
		std_accel_x = std_accel_x + pow(accel_x[k]-mean_accel_x,2);
	
	std_accel_x = sqrt(std_accel_x/T);
	F[N++] = std_accel_x / max_accel_x;

//	F[N++] = std_accel_x;	//Bao
	/*****************************************************************/
	
	/*****************************************************************/
	//Std Devn of accel_y by amax_y			5
	double mean_accel_y = 0;
	double std_accel_y = 0;
	double max_accel_y	= accel_y[0];
	for(k = 0; k < T; k ++){
		mean_accel_y += accel_y[k];
		if (accel_y[k] > max_accel_y)
			max_accel_y = accel_y[k];	
	}
	mean_accel_y /= T;

	for(k = 0; k < T; k++)
		std_accel_y = std_accel_y + pow(accel_y[k]-mean_accel_y,2);

	std_accel_y = sqrt(std_accel_y / T);
	F[N++] = std_accel_y / max_accel_y;
//	F[N++] = std_accel_y;	//Bao
	/*****************************************************************/

	/*****************************************************************/
	//stddev_velocityx by vmaxx()					6
	double mean_veloc_x = 0;
	double std_veloc_x = 0;
	for(k = 0; k < T; k ++){
		mean_veloc_x += veloc_x[k];
	}
	mean_veloc_x /= T;

	for(k=0; k < T; k++)
		std_veloc_x = std_veloc_x + pow(veloc_x[k]-mean_veloc_x,2);
	
	std_veloc_x = sqrt(std_veloc_x / T);
	F[N++] = std_veloc_x / max_veloc_x;
//	F[N++] = std_veloc_x;	//Bao
	/*****************************************************************/
	//avg_vel_by_vxmax()							7
	F[N++] = mean_veloc_x / max_veloc_x;			
//	F[N++] = mean_veloc_x; //Bao

	/*****************************************************************/
	//stddev_velocityy by vmaxy()					8
	double mean_veloc_y = 0;
	double std_veloc_y = 0;

	for(k = 0; k < T; k ++){
		mean_veloc_y = mean_veloc_y + veloc_y[k];
	}
	mean_veloc_y /= T;

	for(k=0; k < T; k++)
		std_veloc_y = std_veloc_y + pow(veloc_y[k]-mean_veloc_y,2);
	
	std_veloc_y = sqrt(std_veloc_y/T);
	F[N++] = std_veloc_y / max_veloc_y;
//	F[N++] = std_veloc_y; //Bao
	/*****************************************************************/
	//avg_vel_by_vymax()							9
	F[N++] = mean_veloc_y / max_veloc_y;
//	F[N++] = mean_veloc_y; //Bao
	/*****************************************************************/

	/*****************************************************************/
	//rmsy_by_dely() changed from stddev_y_by_dely()	10				
	double rmsY = 0;
	double minY = o[yIndex][0];
	double maxY = o[yIndex][0];
	
	for(k = 0; k < T; k ++)
	{
		if(maxY < o[yIndex][k])	
			maxY = o[yIndex][k];
		if(minY > o[yIndex][k])
			minY = o[yIndex][k];
		rmsY += o[yIndex][k]*o[yIndex][k];
	}
	
	rmsY = sqrt(rmsY/T);
	F[N++] = rmsY / (maxY - minY);
//	F[N++] = rmsY; //Bao
	/*****************************************************************/

	/*****************************************************************/
	//ratio_vel_change()					11

	int count1 = 0, count2 = 0;
	for(k = 0; k < T; k ++){
		if(veloc_y[k] * veloc_x[k] < 0)
			count1++;
		else
			count2++;
	}

	//Ration is not good, for the same writer, signatures are very different
	//Save them as two different feature
	//if(count2 != 0)
		F[N++] = double(count1) / 100;
	//else
		F[N++] = double(count2) / 100;
	/*****************************************************************/

	/*****************************************************************/
	//avg_vel_by_vmax()						12
	double mean_veloc = 0;

	double rms_veloc = 0;
	
	double max_veloc	= veloc[0];

	for(k = 0; k < T; k ++){
		if(veloc[k] > max_veloc	)
			max_veloc	= veloc[k];
		mean_veloc += veloc[k];
		rms_veloc += pow(veloc[k],2);
	}
	mean_veloc /= T;
	F[N++] = mean_veloc/max_veloc;
//	F[N++] = mean_veloc;	//Bao
//	F[N++] = max_veloc;		//Bao
	/*****************************************************************/
	/*****************************************************************/
	//rms_vel_by_vmax()						13
	rms_veloc = sqrt(rms_veloc/T);
	F[N++] = rms_veloc / max_veloc;
//	F[N++] = rms_veloc;		//Bao
	/*****************************************************************/

	/*****************************************************************/
	//coord_ratio()							14 - 15 - 16

	double maxX = o[xIndex][0];
	double minX = o[xIndex][0];
	double meanX = 0;
	double meanY = 0;
	
	for(k = 1; k < T; k ++)
	{
		if(maxX < o[xIndex][k])	
			maxX = o[xIndex][k];
		if(minX > o[xIndex][k])	
			minX = o[xIndex][k];
		meanX += o[yIndex][k];
		meanY += o[xIndex][k];
	}

	meanX = meanX /T;
	meanY = meanY /T;
	F[N++] = (maxY-minY)/(maxX-minX);
	F[N++] = (meanY-minY)/(maxY-meanY);
	F[N++] = (meanX-minX)/(maxX-meanX);
	/*****************************************************************/

	/*****************************************************************/
	//signature_global::velcorr_by_vsq()		17
	double sigxy=0, sigx=0, sigy=0, sigxsq=0, sigysq=0;
	for(k = 0; k < T; k ++){
		sigx += veloc_x[k];
		sigy += veloc_y[k];
		sigxsq += pow(veloc_x[k],2);
		sigysq += pow(veloc_y[k],2);
		sigxy += veloc_x[k]*veloc_y[k];
	}
	
	double corr = (T*sigxy-sigx*sigy)/((sqrt(T*sigxsq - sigx*sigx))*(sqrt(T*sigysq - sigy*sigy)));

	F[N++] = corr;	//Better or not????		/(max_veloc*max_veloc);
	/*****************************************************************/
	/*****************************************************************/
	//rmsa_by_amax()							18	
	
	double rms_accel = 0;
	double max_accel = accel[0];

	for(k = 0; k < T; k ++){
		if (accel[k] > max_accel)
			max_accel = accel[k];
		rms_accel += pow(accel[k],2);
	}
	
	rms_accel = sqrt(rms_accel/T);
	F[N++] = rms_accel/max_accel;
//	F[N++] = rms_accel; //Bao
	/*****************************************************************/

	/*****************************************************************/
	//tanacc_by_amax()						19
	
	double mean_accel_t = 0;
	double max_accel_t = accel_t[0];
	for(k = 0; k < T; k ++){
		if (accel_t[k] > max_accel_t)
			max_accel_t = accel_t[k];
		mean_accel_t += accel_t[k];
	}
	mean_accel_t /= T;
	F[N++] = mean_accel_t / max_accel_t;
//	F[N++] = mean_accel_t;	//Bao
	/*****************************************************************/	
	/*****************************************************************/
	//diff_by_meanx()						20
	//not used for : any

	double rmsX = 0;
	for(k=0; k<T ; k++)
		rmsX += o[xIndex][k]*o[xIndex][k];
	rmsX = sqrt(rmsX /T);
	F[N++] = (rmsX - minX)/rmsX;
	/*****************************************************************/
	// mean of x by del x changed from std dev of X by del X	21			18
	F[N++] = rmsX / (maxX - minX);
	/*****************************************************************/
//	F[N++] = rmsX;	//Bao
	/*****************************************************************/
	//19rms_velx_by_vxmax()					22
	double rms_veloc_x = 0;
	for(k = 0; k < T; k ++)
		rms_veloc_x += pow(veloc_x[k],2);
	
	rms_veloc_x = sqrt(rms_veloc_x/T);
	F[N++] = rms_veloc_x/max_veloc_x;
	/*****************************************************************/
//	F[N++] = rms_veloc_x;	//Bao
	/*****************************************************************/
	//rms_vely_by_vymax()					23	

	double rms_veloc_y = 0;
	for(k = 0; k < T; k ++)
		rms_veloc_y += pow(veloc_y[k],2);
	
	rms_veloc_y = sqrt(rms_veloc_y/T);

	F[N++] = rms_veloc_y/max_veloc_y;
	/*****************************************************************/
//	F[N++] = rms_veloc_y;	//Bao
	/*****************************************************************/
	//velocity_ratio()						24
	F[N++] = (max_veloc_y - min_veloc_y)/(max_veloc_x - min_veloc_x);
	/*****************************************************************/

	/*****************************************************************/
	//accel_ratio()								25

	double max_accel_X = accel_x[0];
	double min_accel_X = accel_x[0];
	double max_accel_Y = accel_y[0];
	double min_accel_Y = accel_y[0];
	
	for(k = 1; k < T-8; k ++)
	{
		if(max_accel_X < accel_x[k])
		{
			max_accel_X = accel_x[k];
		}
		if(min_accel_X > accel_x[k])
		{
			min_accel_X = accel_x[k];
		}
		if(max_accel_Y < accel_y[k])
		{
			max_accel_Y = accel_y[k];
		}
		if(min_accel_Y > accel_y[k])
		{
			min_accel_Y = accel_y[k];
		}
	}

	F[N++] = (max_accel_Y - min_accel_Y)/(max_accel_X - min_accel_X);
	/*****************************************************************/
	
	/*****************************************************************/
	//signature_global::corrxy()				26
	sigxy=0;sigx=0;sigy=0;sigxsq=0;sigysq=0;

	for(k = 0; k < T; k ++)
	{
		sigx += o[xIndex][k];
		sigy += o[yIndex][k];
		sigxsq += pow(o[xIndex][k],2);
		sigysq += pow(o[yIndex][k],2);
		sigxy += o[xIndex][k]*o[yIndex][k];
	}
	
	corr = ((T)*sigxy-sigx*sigy)/((sqrt((T)*sigxsq - pow(sigx,2)))*(sqrt((T)*sigysq - pow(sigy,2))));

	F[N++] = corr;
	/*****************************************************************/

	/*****************************************************************/
	//signature_global::acccorr_by_asq()		27
	sigxy=0;sigx=0;sigy=0;sigxsq=0;sigysq=0;

	for(k = 0; k < T; k ++){
		sigx += accel_x[k];
		sigy += accel_y[k];
		sigxsq += pow(accel_x[k],2);
		sigysq += pow(accel_y[k],2);
		sigxy += accel_x[k]*accel_y[k];
	}
	
	corr = (T*sigxy-sigx*sigy)/((sqrt(T*sigxsq - pow(sigx,2)))*(sqrt(T*sigysq - pow(sigy,2))));
	F[N++] = corr;	// better or not?	/(max_accel*max_accel);
	/*****************************************************************/
	
	/*****************************************************************/
	//diff_by_meany()							28
	//not used for : any
	F[N++] = (rmsY - minY)/rmsY;
	/*****************************************************************/
	//diff_by_velx_mean()						29
	F[N++] = (rms_veloc_x - min_veloc_x)/rms_veloc_x;
	/*****************************************************************/

	/*****************************************************************/
	//diff_by_vely_mean()						30
	F[N++] = (rms_veloc_y - min_veloc_y)/rms_veloc_y;
	/*****************************************************************/
	
	/*****************************************************************/
	//diff_by_vel_mean()						31
	
	double min_veloc = veloc[0];
	for(k=0; k<T ; k++)
	{
		if (veloc[k] < min_veloc)
			min_veloc = veloc[k];
	}
	F[N++] = (rms_veloc - min_veloc)/rms_veloc;
	/*****************************************************************/
	
	/*****************************************************************/
	//diff_by_accx_mean()						32
	
	double rms_accel_x = 0;
	double min_accel_x = accel_x[0];

	for(k=0; k<T; k++)
	{
		if (accel_x[k] < min_accel_x)
			min_accel_x = accel_x[k];
		rms_accel_x += accel_x[k]*accel_x[k];
	}
	rms_accel_x = sqrt(rms_accel_x / T);
	F[N++] = (rms_accel_x  - min_accel_x )/rms_accel_x;
	/*****************************************************************/
//	F[N++] = rms_accel_x; //Bao
	/*****************************************************************/
	//30diff_by_vely_mean()						33

	double rms_accel_y = 0;
	double min_accel_y = accel_y[0];
	for(k=0; k<T ; k++)
	{
		if (accel_y[k] < minY)
			min_accel_y = accel_y[k];
		rms_accel_y = rms_accel_y + accel_y[k]*accel_y[k];
	}
	rms_accel_y = sqrt(rms_accel_y / T);

	F[N++] = (rms_accel_y  - min_accel_y)/rms_accel_y ;
	/*****************************************************************/
//	F[N++] = rms_accel_y;	//Bao
	/*****************************************************************/
	//diff_by_accel_mean()						34
	double mean_accel = 0;
	double min_accel = accel[0];
	
	for(k=0; k<T; k++)
	{
		if (accel[k] < min_accel)
			min_accel = accel[k];
		mean_accel = mean_accel + accel[k];
	}
	mean_accel = mean_accel/T;

	F[N++] = (mean_accel - min_accel)/mean_accel;
	/*****************************************************************/
//	F[N++] = mean_accel;	//Bao
	/*****************************************************************/
	//time of max velocity by total time		35
	
	min_veloc = veloc[0];
	int max_stamp = 0;

	for(k=0; k<T; k++)
	{
		if(veloc[k]>min_veloc)
		{
			min_veloc = veloc[k];
			max_stamp = k;
		}
	}

	F[N++] = double(max_stamp)/T;
	/*****************************************************************/	

	/*****************************************************************/
	//Number of strokes							36
	int strCounter=0;
	for(k=1; k<T-1 ; k++){
		if(veloc[k-1]>veloc[k] && veloc[k+1]>veloc[k])
		{
			strCounter++;
		}
	}
	
	F[N++] = double(strCounter) / 10;
	/*****************************************************************/
	
	/*****************************************************************/
	//mean of positive and negative velocity,x		37-38
	
	double mean_positive_veloc_x = 0;
	double mean_negative_veloc_x = 0;
	int posi_counter = 1;	// we count one more for each 
	int nega_counter = 1;	// to avoid the division by zero

	for(k=0; k<T ; k++){
		if(veloc_x[k]>0){
			mean_positive_veloc_x = mean_positive_veloc_x + veloc_x[k];
			posi_counter++;
		}else{
			mean_negative_veloc_x = mean_negative_veloc_x + veloc_x[k];
			nega_counter++;
		}
	}

	F[N++] = mean_positive_veloc_x / posi_counter / 10;
	F[N++] = - mean_negative_veloc_x / nega_counter / 10;	// positif value
	/*****************************************************************/

	/*****************************************************************/
	//mean of positive and negative velocity,y			39-40
	
	double mean_positive_veloc_y = 0;
	double mean_negative_veloc_y = 0;
	posi_counter = 1;
	nega_counter = 1;

	for(k=0; k<T ; k++){
		if(veloc_y[k]>0){
			mean_positive_veloc_y = mean_positive_veloc_y + veloc_y[k];
			posi_counter++;
		}else{
			mean_negative_veloc_y = mean_negative_veloc_y + veloc_y[k];
			nega_counter++;
		}
	}

	F[N++] = mean_positive_veloc_y / posi_counter / 10;
	F[N++] = - mean_negative_veloc_y / nega_counter / 10;	// positif value

	return true;
}

void global_signature::makeShort(short	mul_factor)
{
	for(int i=0; i<N; i++){
		F[i] = GMM_SHORT::ToShort(mul_factor * F[i]);
	}
}
