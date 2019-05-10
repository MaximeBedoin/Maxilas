#include "stdafx.h"
#include "signature.h"
#include "memalc.h"
#include "math.h"
#include "definitions.h"
#include "memory.h"
#include "statistics.h"
#include "dtw.h"
#include "stdio.h"



signature::signature(int L_, int T_):Obs(L_, T_){indexOK = false;};	
signature::signature(char * filename):Obs(filename){indexOK = false;};
signature::signature(char * filename, int L_):Obs(filename, L_){indexOK = false;};
signature::signature(char * filename, int L_, int T_):Obs(filename, L_, T_){indexOK = false;};

bool signature::filter()
// appelez trim avant cette fonction, mais pour le SVC, il y a déjà un pré-traitement
// cette fonction filtre les données comme coordonnées, les angles azimuth...
// et normaliser les coordonnées par rapport au centre de gravité de la signature
{
	if(!bInit || !indexOK){// pas de donnée or pas indexer
		return false;
	}else{
		double * fir;
		fir = memalc::dvector(4);
		fir[0] = 0.0457;    fir[1] = 0.4543;    fir[2] = 0.4543;    fir[3] = 0.0457;
		// 1 2 3 4 5 6 7 8 9 10		vecteur
		// 1 2 3 4 5 6 7			convolution avec fir
		//     1 2 3 4 5 6			median 5
		double ** newO = memalc::dmatrix(L, T - 5);
		int t, l, k, i;
		// Copy all data; Sometime, we do not touch a parameter..
		for(t = 2; t < T - 3; t++){
			for(l = 0; l < L; l++){
				newO[l][t-2] = o[l][t];
			}
		}
		// convolution
		for(t = 2; t < T-3; t++){
			newO[xIndex][t-2] = 0;
			for(k = 0; k < 4; k ++){
				newO[xIndex][t-2] += o[xIndex][t + k] * fir[k];
			}
		}
		for(t = 2; t < T-3; t++){
			newO[yIndex][t-2] = 0;
			for(k = 0; k < 4; k ++){
				newO[yIndex][t-2] += o[yIndex][t + k] * fir[k];
			}
		}

		// we filter all angles by the same way (azimuth, altitude) or (thetaX, thetaY)
		// check if the angle parameters exist or not; In the PDA cases, these angles
		// do not exist
		if(azimuthIndex != -1 && thetaXIndex != -1){
			//filtre médian 5
			double *tmp = memalc::dvector(5);
			int firstAngle = azimuthIndex > thetaXIndex ? azimuthIndex:thetaXIndex;
			int secondAngle = altitudeIndex > thetaYIndex ? altitudeIndex:thetaYIndex;
			for(t = 2; t < T - 3; t++){
				for(i = 0; i < 5; i++){
					tmp[i] = o[firstAngle][t - 2 + i];
				}
				newO[firstAngle][t-2] = statistics::median(tmp, 5);
			}

			for(t = 2; t < T - 3; t++){
				for(i = 0; i < 5; i++){
					tmp[i] = o[secondAngle][t - 2 + i];
				}
				newO[secondAngle][t-2] = statistics::median(tmp, 5);
			}
			memalc::free_dvector(tmp);
		}

		memalc::free_dvector(fir);
		memalc::free_dmatrix(o, L);
		T = T - 5;	// après filter, nous perdons 5 points
		o = newO;
		return true;
	}
}

bool signature::trim()
{
	if(bInit == false || indexOK == false){// pas de donnée
		return false;
	}else{
		int first, last; //couper les bouts ayant la pression zero
		for(first = 0; first < T; first++){
			if(o[pressureIndex][first] > 0){
				break;
			}
		}
		for(last = T-1; last >= 0; last--){
			if(o[pressureIndex][last] > 0){
				break;
			}
		}
		
		if(last - first + 1 < N_POINTS_MIN){
			return false;	// pas de donnée valide
		}

		if((first != 0) | (last != T-1)){
			double ** newO = memalc::dmatrix(L, last - first + 1);
			int row, col;
			for(row = 0; row < L; row++){
				for(col = first; col <= last; col ++){
					newO[row][col - first] = o[row][col];
				}
			}
			// desalouer la mémoire d'o
			memalc::free_dmatrix(o, L);
			T = last - first + 1;
			o = newO;
		}
	}
	return true;
}

bool signature::zeroBaseNorm(int *newMaxX, int *newMaxY)
// normaliser coordonnées de la signature, pourque Xmin et Y min soient zéro
{
	if(bInit == false || indexOK == false){// pas de donnée
		return false;
	}else{
		//la marge est utilisée pour le zoom de la signature, dans le mode graphique
		int margin = 20;
		int xIndex = 1;
		int yIndex = 2;
		double maxX = MIN_DOUBLE;
		double minX = MAX_DOUBLE;
		double maxY = MIN_DOUBLE;
		double minY = MAX_DOUBLE;
		int k;
		for(k = 0; k < T; k ++){
			if(maxX < o[xIndex][k])		maxX = o[xIndex][k];
			if(minX > o[xIndex][k])		minX = o[xIndex][k];
			if(maxY < o[yIndex][k])		maxY = o[yIndex][k];
			if(minY > o[yIndex][k])		minY = o[yIndex][k];
		}

		for(k = 0; k < T; k ++){
			o[xIndex][k] -= minX - margin;
			o[yIndex][k] -= minY - margin;
		}
		*newMaxX = (int)(maxX - minX + 2 * margin);
		*newMaxY = (int)(maxY - minY + 2 * margin);
		return true;
	}
}

double signature::normalizeDirection()
// tourner la signature, pour que la direction d'inertie soit 45°
// tout d'abord, calculer la direction d'inertie,
// attention, calculer cette quantité après la fonction "trim"
{
	if(!bInit || !indexOK){
		return 0;
	}

	// calculer le centre de gravité:
	double xg = 0;
	double yg = 0;
	int k;
	for(k = 0; k < T; k++){
		xg += o[xIndex][k];
		yg += o[yIndex][k];
	}
	xg /= T;
	yg /= T;

	double mxx = 0;
	double myy = 0;
	double mxy = 0;

	for(k = 0; k < T; k++){
		mxx += pow(o[xIndex][k] - xg, 2);
		myy += pow(o[yIndex][k] - yg, 2);
		mxy += (o[xIndex][k] - xg) * (o[yIndex][k] - yg);
	}
	// calculer l'angle de la direction d'inertie
	double di;
	if(mxx != myy){
		double tg2di = (2 * mxy) / (myy - mxx);
		if(myy < mxx){
			di = atan(tg2di) / 2;
		}else{
			if(mxy > 0){
				di = atan(tg2di) / 2 - PI / 2;
			}else{
				di = atan(tg2di) / 2 + PI / 2;
			}
		}
	}else{
		if(mxy > 0){ //0 si c'est un point qui n'existe jamais
			di = -PI / 4;
		}else{
			di = PI / 4;
		}
	}
	// rotation, pour que la direction d'inertie soit 45°
	// l'angle à tourner: 
	double alpha = PI/4 - di;

	for(k = 0; k < T; k++){
		double GA = sqrt(pow(o[yIndex][k] - yg, 2) + pow(o[xIndex][k] - xg, 2));
		double beta = atan(fabs((o[yIndex][k] - yg) / (o[xIndex][k] - xg)));

		if((o[xIndex][k] > xg) && (o[yIndex][k] < yg)){ // 1
		}
		if((o[xIndex][k] < xg) && (o[yIndex][k] < yg)){ // 2
			beta = PI - beta;
		}
		if((o[xIndex][k] < xg) && (o[yIndex][k] > yg)){ // 3
			beta = PI + beta;
		}
		if((o[xIndex][k] > xg) && (o[yIndex][k] > yg)){ // 4
			beta = 2*PI - beta;
		}
		o[xIndex][k] = xg + GA * cos(beta + alpha);
		o[yIndex][k] = yg - GA * sin(beta + alpha);
		
		// if the spacial position is saved by Azimuth and Altitude
		// BIOMET database for example
		if(azimuthIndex > 0){
			o[azimuthIndex][k] -= 1800 * alpha / PI;
		}
		

		
		// if the spacial position is saved by thetaX and thetaY
		// Philips database for example
		if(thetaXIndex > 0 || thetaYIndex > 0){
			
			
			
			//**** to implement the change of angles: FUTURE WORKS



		}
	}
	return alpha;
}

bool signature::centralizeCoordinates()
{
	if(!bInit|| !indexOK){// pas de donnée
		return false;
	}else{
		// normaliser les coordonnées par rapport au centre de gravité
		double meanx = 0;
		double meany = 0;
		int t;
		for(t = 0; t < T; t++){
			meanx += o[xIndex][t];
			meany += o[yIndex][t];
		}
		meanx /= T;
		meany /= T;

		for(t = 0; t < T; t++){
			o[xIndex][t] -= meanx;
			o[yIndex][t] -= meany;
		}
		return true;
	}
}

bool signature::setIndex(int xIndex_, int yIndex_, int pressureIndex_, int thetaXIndex_, int thetaYIndex_, int azimuthIndex_, int altitudeIndex_) 
// default value NULL to all indexes
{
	if(	xIndex_ < -1 || xIndex_ >= L ||
		yIndex_ < -1 || yIndex_ >= L ||
		pressureIndex_ < -1 || pressureIndex_ >= L ||
		thetaXIndex_ < -1 || thetaXIndex_ >= L ||
		thetaYIndex_ < -1 || thetaYIndex_ >= L ||
		azimuthIndex_ < -1 || azimuthIndex_ >= L ||
		altitudeIndex_ < -1 || altitudeIndex_ >= L){
		return false;
	}else{
		xIndex = xIndex_;
		yIndex = yIndex_;
		pressureIndex = pressureIndex_;
		thetaXIndex = thetaXIndex_;
		thetaYIndex = thetaYIndex_;
		azimuthIndex = azimuthIndex_;
		altitudeIndex = altitudeIndex_;
		indexOK = true;
		return true;
	}
}

double signature::normalizeSize(double sigma)
// normaliser la taille de la signature après l'avoir tournée
// pour que leurs écart-types en X et Y soient sigma
// retourner la proportion de normalisation
{
	if(!bInit || !indexOK){
		return 0;
	}

	double yg = 0;
	int k;
	for(k = 0; k < T; k++){
		yg += o[yIndex][k];
	}
	yg /= T;

	double myy = 0;
	for(k = 0; k < T; k++){
		myy += pow(o[yIndex][k] - yg, 2);
	}

	double sigmaY = sqrt(myy / T);
	// proportion à multiplier avec X et Y;
	double ratio = sigma / sigmaY;
	for(k = 0; k < T; k++){
		o[xIndex][k] *= ratio;
		o[yIndex][k] *= ratio;
	}
	return ratio;
}

bool signature::parameterizeXY()	// pour les bases BIOMET, Dolfing
// calculer d'autres paramètres comme la vittesse, l'accélération... 
// à partir des coordonnées. utiliser la regression pour calculer.
// c'est ici de changer le codage des signatures
{
	if(!bInit || !indexOK)
		return false;

	if(T <= 12)// signature is so short
		return false;


	// OK, bien calculer les paramètres.
	// Maintenant, copier les paramètres comme la nouvelle observation

	int newParaNumber;

		newParaNumber = 2;


	double ** newO = memalc::dmatrix(newParaNumber, (T - 12) * sizeof(double));
	memcpy(newO[0], o[xIndex] + 6, (T-12) * sizeof(double));	
	memcpy(newO[1], o[yIndex] + 6, (T-12) * sizeof(double));	



	memalc::free_dmatrix(o, L);
	o = newO;
	T = T - 12;	// après cette opération, nous perdons 12 points
	L = newParaNumber;

	return true;
}


bool signature::parameterize()	// pour les bases BIOMET, Dolfing
// calculer d'autres paramètres comme la vittesse, l'accélération... 
// à partir des coordonnées. utiliser la regression pour calculer.
// c'est ici de changer le codage des signatures
{
	if(!bInit || !indexOK)
		return false;

	if(T <= 12)// signature is so short
		return false;

	double * vitesse_x = memalc::dvector(T-4);	// 1
	double * vitesse_y = memalc::dvector(T-4);	// 2
	double * vitesse = memalc::dvector(T-4);	// 3
	int k;
	for(k = 2; k < T-2; k ++){
		vitesse_x[k-2] = (2*o[xIndex][k+2] + o[xIndex][k+1] - o[xIndex][k-1] - 2*o[xIndex][k-2])/10;
		vitesse_y[k-2] = (2*o[yIndex][k+2] + o[yIndex][k+1] - o[yIndex][k-1] - 2*o[yIndex][k-2])/10;
		vitesse[k-2] = sqrt(vitesse_x[k-2] * vitesse_x[k-2] + vitesse_y[k-2] * vitesse_y[k-2]);
	}

	double * accel_x = memalc::dvector(T-8);	// 4
	double * accel_y = memalc::dvector(T-8);	// 5
	double * accel = memalc::dvector(T-8);		// 6
	for(k = 2; k < T-6; k ++){
		accel_x[k-2] = (2*vitesse_x[k+2] + vitesse_x[k+1] - vitesse_x[k-1] - 2*vitesse_x[k-2])/10;
		accel_y[k-2] = (2*vitesse_y[k+2] + vitesse_y[k+1] - vitesse_y[k-1] - 2*vitesse_y[k-2])/10;
		accel[k-2] = sqrt(accel_x[k-2] * accel_x[k-2] + accel_y[k-2] * accel_y[k-2]);
	}

	double * accel_t = memalc::dvector(T-8);	// 7
	for(k = 2; k < T-6; k ++){
		accel_t[k-2] = (2*vitesse[k+2] + vitesse[k+1] - vitesse[k-1] - 2*vitesse[k-2])/10;
	}

	double * cos_alpha = memalc::dvector(T-4);	// 8
	double * sin_alpha = memalc::dvector(T-4);	// 9
	double * alpha = memalc::dvector(T-4);		// 10
	// 26 fev 2004: changer les équations concernantes alpha
	for(k = 0; k < T-4; k++){
		if(vitesse[k] != 0){
			cos_alpha[k] = vitesse_x[k] / vitesse[k];
			sin_alpha[k] = - vitesse_y[k] / vitesse[k]; // inverse de Décarte
		}else{
			cos_alpha[k] = sqrt(0.5);
			sin_alpha[k] = sqrt(0.5);
		}
		alpha[k] = asin(sin_alpha[k]);
		/*if(cos_alpha[k] < 0){
			if(sin_alpha[k] > 0){
				alpha[k] = PI - alpha[k];
			}else{
				alpha[k] = -PI - alpha[k];
			}
		}*/
	}
	
	double * cos_phi = memalc::dvector(T-6);	// 11
	double * sin_phi = memalc::dvector(T-6);	// 12
	for(k = 1; k < T-5; k++){
		cos_phi[k-1]=cos_alpha[k-1]*cos_alpha[k+1]+sin_alpha[k-1]*sin_alpha[k+1];
		sin_phi[k-1]=cos_alpha[k-1]*sin_alpha[k+1]-sin_alpha[k-1]*cos_alpha[k+1];
	}

	double * phi = memalc::dvector(T-8);		// 13
	for(k = 2; k < T-6; k++){
		phi[k-2] = (2*alpha[k+2] + alpha[k+1] - alpha[k-1] - 2*alpha[k-2])/10;
	}
    
	double * delta_phi = memalc::dvector(T-12);	// 
	for(k = 2; k < T-10; k++){
		delta_phi[k-2] = (2*phi[k+2] + phi[k+1] - phi[k-1] - 2*phi[k-2])/10;
	}

	double * radius = memalc::dvector(T-12);	// 14
	for(k= 0; k < T-12; k++){
		// the folowing computation of radius may be incorrect, 
		// pointed out by Andrew moris
		/*if(delta_phi[k] != 0){
			radius[k] = accel_t[k+2] / delta_phi[k];
		}else{
			radius[k] = MAX_RADIUS;
		}*/

		// The new computation:
		if(phi[k+2] != 0){
			// it's better to use the absolute value of radius
			radius[k] = fabs((vitesse[k+4] / phi[k+2]));
		}else{
			radius[k] = MAX_RADIUS;
		}

		radius[k] = log(1 + radius[k]);
		//printf("%f\t", radius[k]);
	}

	double * proportion_lh = memalc::dvector(T-4);	// 17
	int j;
	for(k = 2; k < T-2; k ++){
		double maxX = MIN_DOUBLE;
		double minX = MAX_DOUBLE;
		double maxY = MIN_DOUBLE;
		double minY = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxX < o[xIndex][k+j])	maxX = o[xIndex][k+j];
			if(minX > o[xIndex][k+j])	minX = o[xIndex][k+j];
			if(maxY < o[yIndex][k+j])	maxY = o[yIndex][k+j];
			if(minY > o[yIndex][k+j])	minY = o[yIndex][k+j];
		}
		if(maxY != minY){
			proportion_lh[k-2] = (maxX - minX) / (maxY - minY);
		}else{
			proportion_lh[k-2] = MAX_L2W_RATIO;	// statistiquement... 5
		}
	}
	////////////
	double * proportion_lh7 = memalc::dvector(T-6);	// 18
	for(k = 3; k < T-3; k ++){
		double maxX = MIN_DOUBLE;
		double minX = MAX_DOUBLE;
		double maxY = MIN_DOUBLE;
		double minY = MAX_DOUBLE;
		for(j = -3; j <= 3; j++){
			if(maxX < o[xIndex][k+j])	maxX = o[xIndex][k+j];
			if(minX > o[xIndex][k+j])	minX = o[xIndex][k+j];
			if(maxY < o[yIndex][k+j])	maxY = o[yIndex][k+j];
			if(minY > o[yIndex][k+j])	minY = o[yIndex][k+j];
		}
		if(maxY != minY){
			proportion_lh7[k-3] = (maxX - minX) / (maxY - minY);
		}else{
			proportion_lh7[k-3] = MAX_L2W_RATIO;	// statistiquement... 5
		}
	}
	/************/
	double * context_vitesse = memalc::dvector(T-8); //19
	for(k = 2; k < T-6; k ++){
		double maxV = MIN_DOUBLE;
		double minV = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxV < vitesse[k+j])	maxV = vitesse[k+j];
			if(minV > vitesse[k+j])	minV = vitesse[k+j];
		}
		if(maxV != 0){
			context_vitesse[k-2] = minV / maxV;
		}else{
			context_vitesse[k-2] = 0;
		}
	}

	// these parameters concern the angles and pressure
	double * delta_Azimuth = memalc::dvector(T-4);	// 23	
	double * delta_Altitude = memalc::dvector(T-4);	// 24
	double * delta_Pressure = memalc::dvector(T-4);	// 25
	if(azimuthIndex != -1 && thetaXIndex != -1){
		int firstAngle = azimuthIndex > thetaXIndex ? azimuthIndex:thetaXIndex;
		int secondAngle = altitudeIndex > thetaYIndex ? altitudeIndex:thetaYIndex;

		for(k = 2; k < T-2; k ++){
			delta_Azimuth[k-2] = (2*o[firstAngle][k+2] + o[firstAngle][k+1] - o[firstAngle][k-1] - 2*o[firstAngle][k-2])/10;
			delta_Altitude[k-2] = (2*o[secondAngle][k+2] + o[secondAngle][k+1] - o[secondAngle][k-1] - 2*o[secondAngle][k-2])/10;
		}
	}

	if(pressureIndex != -1){
		for(k = 2; k < T-2; k ++){
			delta_Pressure[k-2] = (2*o[pressureIndex][k+2] + o[pressureIndex][k+1] - o[pressureIndex][k-1] - 2*o[pressureIndex][k-2])/10;
		}
	}

	if(thetaXIndex != -1){
		for(k = 2; k < T-2; k ++){
			delta_Altitude[k-2] = (2*o[thetaXIndex][k+2] + o[thetaXIndex][k+1] - o[thetaXIndex][k-1] - 2*o[thetaXIndex][k-2])/10;
		}
	}

	double * context_Altitude = memalc::dvector(T-4);
	for(k = 2; k < T-2; k ++){
		double maxAltitude = MIN_DOUBLE;
		double minAltitude = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxAltitude < o[3][k+j])	maxAltitude = o[3][k+j];
			if(minAltitude > o[3][k+j])	minAltitude = o[3][k+j];
		}
		if(maxAltitude != 0){
			context_Altitude[k-2] = minAltitude / maxAltitude;
		}else{
			context_Altitude[k-2] = 0;
		}
	}

	/*
	double * context_Azimuth = memalc::dvector(T-4);
	for(k = 2; k < T-2; k ++){
		double maxAzimuth = MIN_DOUBLE;
		double minAzimuth = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxAzimuth < o[4][k+j])	maxAzimuth = o[4][k+j];
			if(minAzimuth > o[4][k+j])	minAzimuth = o[4][k+j];
		}
		if(maxAzimuth != 0){
			context_Azimuth[k-2] = minAzimuth / maxAzimuth;
		}else{
			context_Azimuth[k-2] = 0;
		}
	}

	double * context_Altitude = memalc::dvector(T-4);
	for(k = 2; k < T-2; k ++){
		double maxAltitude = MIN_DOUBLE;
		double minAltitude = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxAltitude < o[5][k+j])	maxAltitude = o[5][k+j];
			if(minAltitude > o[5][k+j])	minAltitude = o[5][k+j];
		}
		if(maxAltitude != 0){
			context_Altitude[k-2] = minAltitude / maxAltitude;
		}else{
			context_Altitude[k-2] = 0;
		}
	}

	double * context_Pressure = memalc::dvector(T-4);
	for(k = 2; k < T-2; k ++){
		double maxPressure = MIN_DOUBLE;
		double minPressure = MAX_DOUBLE;
		for(j = -2; j <= 2; j++){
			if(maxPressure < o[3][k+j])	maxPressure = o[3][k+j];
			if(minPressure > o[3][k+j])	minPressure = o[3][k+j];
		}
		if(maxPressure != 0){
			context_Pressure[k-2] = minPressure / maxPressure;
		}else{
			context_Pressure[k-2] = 0;
		}
	}
	************/
	// OK, bien calculer les paramètres.
	// Maintenant, copier les paramètres comme la nouvelle observation

	int newParaNumber;
	if(pressureIndex > 0 && (thetaXIndex > 0 && azimuthIndex > 0)){
		newParaNumber = 25;
	}else{
		newParaNumber = 22;
	}

	double ** newO = memalc::dmatrix(newParaNumber, (T - 12) * sizeof(double));
	memcpy(newO[0], o[xIndex] + 6, (T-12) * sizeof(double));	
	memcpy(newO[1], o[yIndex] + 6, (T-12) * sizeof(double));	

	memcpy(newO[2], vitesse_x + 4, (T-12) * sizeof(double));	//1
	memcpy(newO[3], vitesse_y + 4, (T-12) * sizeof(double));	//2
	memcpy(newO[4], vitesse + 4, (T-12) * sizeof(double));	//3

	memcpy(newO[5], accel_x + 2, (T-12) * sizeof(double));	//4
	memcpy(newO[6], accel_y + 2, (T-12) * sizeof(double));	//5
	memcpy(newO[7], accel + 2, (T-12) * sizeof(double));	//6
	memcpy(newO[8], accel_t + 2, (T-12) * sizeof(double));	//7

	memcpy(newO[9], cos_alpha + 4, (T-12) * sizeof(double));	//8
	memcpy(newO[10], sin_alpha + 4, (T-12) * sizeof(double));	//9	
	memcpy(newO[11], alpha + 4, (T-12) * sizeof(double));		//10

	memcpy(newO[12], cos_phi + 3, (T-12) * sizeof(double));		//11
	memcpy(newO[13], sin_phi + 3, (T-12) * sizeof(double));		//12
	memcpy(newO[14], phi + 2, (T-12) * sizeof(double));			//13

	memcpy(newO[15], radius, (T-12) * sizeof(double));		//14

	memcpy(newO[16], proportion_lh + 4, (T-12) * sizeof(double));//17
	memcpy(newO[17], proportion_lh7 + 3, (T-12) * sizeof(double));//18
	memcpy(newO[18], context_vitesse + 2, (T-12) * sizeof(double));//19
	// base parameters, computed from coordinates
	
	memcpy(newO[19], o[thetaXIndex] + 6, (T-12) * sizeof(double));
	memcpy(newO[20], delta_Altitude + 4, (T-12) * sizeof(double));//25
	memcpy(newO[21], context_Altitude + 4, (T-12) * sizeof(double));

/* if(pressureIndex > 0 && (thetaXIndex > 0 || azimuthIndex > 0)){
		memcpy(newO[19], o[thetaXIndex + azimuthIndex + 1] + 6, (T-12) * sizeof(double));		
		memcpy(newO[20], o[thetaYIndex + altitudeIndex + 1] + 6, (T-12) * sizeof(double));
		memcpy(newO[21], o[pressureIndex] + 6, (T-12) * sizeof(double));	

		memcpy(newO[22], delta_Azimuth + 4, (T-12) * sizeof(double));//23
		memcpy(newO[23], delta_Altitude + 4, (T-12) * sizeof(double));//24
		memcpy(newO[24], delta_Pressure + 4, (T-12) * sizeof(double));//25
	}
	memcpy(newO[25], context_Azimuth + 4, (T-12) * sizeof(double));
	memcpy(newO[26], context_Altitude + 4, (T-12) * sizeof(double));
	memcpy(newO[27], context_Pressure + 4, (T-12) * sizeof(double));*/

	memalc::free_dmatrix(o, L);
	o = newO;
	T = T - 12;	// après cette opération, nous perdons 12 points
	L = newParaNumber;

	//memalc::free_dvector(normFactor);
	memalc::free_dvector(vitesse_x);
	memalc::free_dvector(vitesse_y);
	memalc::free_dvector(vitesse);
	memalc::free_dvector(accel_x);
	memalc::free_dvector(accel_y);
	memalc::free_dvector(accel);
	memalc::free_dvector(accel_t);
	memalc::free_dvector(cos_alpha);
	memalc::free_dvector(sin_alpha);
	memalc::free_dvector(alpha);
	memalc::free_dvector(cos_phi);
	memalc::free_dvector(sin_phi);
	memalc::free_dvector(phi);
	memalc::free_dvector(delta_phi);
	memalc::free_dvector(radius);
	memalc::free_dvector(delta_Azimuth);
	memalc::free_dvector(delta_Altitude);
	memalc::free_dvector(delta_Pressure);
	memalc::free_dvector(proportion_lh);
	memalc::free_dvector(proportion_lh7);
	memalc::free_dvector(context_vitesse);
	memalc::free_dvector(context_Altitude);
	/*memalc::free_dvector(context_Azimuth);
	memalc::free_dvector(context_Altitude);
	memalc::free_dvector(context_Pressure);*/
	return true;
}


double signature::rotation()
{
	if(!bInit){
		return 0;
	}
	// tourner la signature, pour que la direction d'inertie soit 45°
	// tout d'abord, calculer la direction d'inertie,
	// attention, calculer cette quantité après la fonction "trim"
	// calculer le centre de gravité:

	double xg = 0;
	double yg = 0;

	int k;
	for(k = 0; k < T; k++){
		xg += o[xIndex][k];
		yg += o[yIndex][k];
	}
	xg /= T;
	yg /= T;

	double mxx = 0;
	double myy = 0;
	double mxy = 0;

	for(k = 0; k < T; k++){
		mxx += pow(o[xIndex][k] - xg, 2);
		myy += pow(o[yIndex][k] - yg, 2);
		mxy += (o[xIndex][k] - xg) * (o[yIndex][k] - yg);
	}
	// calculer l'angle de la direction d'inertie
	double di;
	if(mxx != myy){
		double tg2di = (2 * mxy) / (myy - mxx);
		if(myy < mxx){
			di = atan(tg2di) / 2;
		}else{
			if(mxy > 0){
				di = atan(tg2di) / 2 - PI / 2;
			}else{
				di = atan(tg2di) / 2 + PI / 2;
			}
		}
	}else{
		if(mxy > 0){ //0 si c'est un point qui n'existe jamais
			di = -PI / 4;
		}else{
			di = PI / 4;
		}
	}
	// rotation, pour que la direction d'inertie soit 45°
	// l'angle à tourner: 
	double alpha = PI/4 - di;

	for(k = 0; k < T; k++){
		double GA = sqrt(pow(o[yIndex][k] - yg, 2) + pow(o[xIndex][k] - xg, 2));
		double beta = atan(fabs((o[yIndex][k] - yg) / (o[xIndex][k] - xg)));

		if((o[xIndex][k] > xg) && (o[yIndex][k] < yg)){ // 1
		}
		if((o[xIndex][k] < xg) && (o[yIndex][k] < yg)){ // 2
			beta = PI - beta;
		}
		if((o[xIndex][k] < xg) && (o[yIndex][k] > yg)){ // 3
			beta = PI + beta;
		}
		if((o[xIndex][k] > xg) && (o[yIndex][k] > yg)){ // 4
			beta = 2*PI - beta;
		}
		o[xIndex][k] = xg + GA * cos(beta + alpha);
		o[yIndex][k] = yg - GA * sin(beta + alpha);
		o[azimuthIndex][k] -= 1800 * alpha / PI;
	}
	return alpha;
}

void signature::interpolateCoordinates()
/*
This function ised to interpolate the coordinates (x, y) of the signature captured by the PDA touch screen.
In fact, when using the touch screen, we loose many points of the signature du to the high sampling rate (>100Hz)

We interpolate the coordinates using the SPLINE methode
*/
{
	int maxWindowsSize = 4;

	if(!bInit || !indexOK){// pas de donnée or pas indexer
		return;
	}else{
		int i, j;
		for(i = 0; i < T; i++){
			if(o[xIndex][i] == 0){
				int tmp[2];
				tmp[0] = xIndex;
				tmp[1] = yIndex;
				int inTmp; 
				for(inTmp = 0; inTmp < 2; inTmp++){
					int index = tmp[inTmp];
					int windowsSize = (maxWindowsSize < i) ? maxWindowsSize : i;
					double * windows = memalc::dvector(windowsSize+1);
					for(j = 0; j < windowsSize; j++){
						windows[j] = o[index][i - windowsSize + j];
					}
					int margin = i;
					while(o[index][margin] == 0){
						margin = margin + 1;
					}
					windows[windowsSize] = o[index][margin];
					double *coor = memalc::dvector(windowsSize+1); 
					for(j = 0; j < windowsSize; j ++){
						coor[j] = j;
					}
					coor[windowsSize] = margin - i + windowsSize;
					double *interpolatedValue = memalc::dvector(margin - i);
					double *y2 = memalc::dvector(windowsSize+1);
					// computation of the first derivative
					spline(coor, windows, windowsSize+1, 0, 0, y2);	
					int k;
					// interpolate all missed value
					for(k = 0; k < margin - i; k++){
						splint(coor, windows, y2, windowsSize+1, windowsSize + k, interpolatedValue+k);
					}
					//data copy 
					for(k = 0; k < margin - i; k++){
						o[index][i+k] = interpolatedValue[k];
					}
					// desalocate memory...
					memalc::free_dvector(windows);
					memalc::free_dvector(coor);
					memalc::free_dvector(interpolatedValue);
					memalc::free_dvector(y2);
				}
			}
		}
	}
}

void signature::spline(double *x, double *y, int n, double yp1, double ypn, double *y2)
/**** Taken and modified from Numerical Recipes ****/
/* Given arrays x[0..n-1] and y[0..n-1] containing a tabulated function, i.e., yi = f(xi), with
x0 < x2 < .. . < xN-1, and given values yp1 and ypn for the first derivative of the interpolating
function at points 1 and n, respectively, this routine returns an array y2[0..n-1] that contains
the second derivatives of the interpolating function at the tabulated points xi. If yp1 and/or
ypn are equal to 1 × 1030 or larger, the routine is signaled to set the corresponding boundary
condition for a natural spline, with zero second derivative on that boundary.*/
{
	int i,k;
	double p,qn,sig,un,*u;
	u = memalc::dvector(n-1);  //ORIGINE: u=vector(1,n-1);
	if (yp1 > 0.99e30){ //The lower boundary condition is set either to be "natural"
		y2[0]=u[0]=0.0;
	}else{				//or else to have a specified first derivative.
		y2[0] = -0.5;
		u[0]=(3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	}

	for (i=1;i< n-1;i++) { //This is the decomposition loop of the tridiagonal algorithm.
							//y2 and u are used for temporary storage of the decomposed factors.
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0;
		y2[i]=(sig-1.0)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30) //The upper boundary condition is set either to be “natural” 
		qn=un=0.0;
	else{				//or else to have a specified first derivative.
		qn=0.5;
		un=(3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}
	y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0);
	for (k=n-2;k>=0;k--) //This is the backsubstitution loop of the tridiagonal	algorithm. 
		y2[k]=y2[k]*y2[k+1]+u[k];
	memalc::free_dvector(u);
}

void signature::splint(double *xa, double *ya, double *y2a, int n, double x, double *y)
/**** Taken and modified from Numerical Recipes ****/
/*Given the arrays xa[0..n-1] and ya[0..n-1], which tabulate a function (with the xai’s in order),
and given the array y2a[0..n-1], which is the output from spline above, and given a value of
x, this routine returns a cubic-spline interpolated value y.*/
{
	//void nrerror(char error_text[]);
	int klo,khi,k;
	double h,b,a;
	
	klo=0; /*We will find the right place in the table by means of
			bisection. This is optimal if sequential calls to this
			routine are at random values of x. If sequential calls
			are in order, and closely spaced, one would do better
			to store previous values of klo and khi and test if
			they remain appropriate on the next call.*/
	khi=n-1;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) 
			khi=k;
		else 
			klo=k;
	}	//klo and khi now bracket the input value of x.
	h=xa[khi]-xa[klo];
//	if (h == 0.0) nrerror("Bad xa input to routine splint"); //The xa’s must be distinct.
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h; //Cubic spline polynomial is now evaluated.
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

bool signature::parameterize4PDA()
//This function is esspecially dedicated to PDA signatures, that I know the number of parameters...
//30/08/06: changed du to the different indexing of vector and matrix: zero-base instead of one-base
{
	const int timeThreshold = 18;
	const int timeIndex = 2;
	int startPoint, endPoint;
	startPoint = 0;
	signature processedSign(19, 0);
	while(true){
		signature piece(L, 0);
		//find out the penup point using the third column
		endPoint = startPoint + 1;
		while((o[timeIndex][endPoint] < timeThreshold) && (endPoint < T))
			endPoint++;
		//endPoint point to the begin of the piece of the signature
		//copy from startPoint to (endPoint-1) to piece
		piece.addObs(this, startPoint, endPoint - startPoint);
		piece.setIndex(0, 1, 2, -1, -1, -1, -1);
		if(piece.parameterize())
			processedSign.addObs(&piece, 0, piece.getObsSize());
		//touch the end of the current signature
		if(endPoint >= T)	break;
		//OK, next piece of the signature;
		startPoint = endPoint;
	}
	//process all the signature, copy processedSign to "this"
	memalc::free_dmatrix(o, L);
	L = processedSign.getVecSize();
	T = processedSign.getObsSize();
	o = memalc::dmatrix(L, T);
	//copying...
	int l;
	for(l = 0; l < L; l++){
		memcpy(o[l], processedSign.o[l], T * sizeof(double));
	}
	return true;
}
