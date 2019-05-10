#if !defined(signature_)
#define signature_
#include "Obs.h"
#include "definitions.h"

#define		N_POINTS_MIN	40	//minimal number of points for a valid signature

class signature : public Obs 
{
protected:
	// Because we process differently each signature's parameter
	// it is needed to index all signature's parameters 
	// These index depend on the type of database, for example, 
	// the Philips database use thetaX et thetaY, but the BIOMET 
	// or SVC2004 databases use azimuth and altitude to save the 
	// spacial position of the pen. This information is very important
	// when making the signature rotation, filter the parameters or 
	// extract another informations. 
	// When one index is NULL, that this this parameter is not exist
	int	xIndex, yIndex, pressureIndex;
	int thetaXIndex, thetaYIndex;	// Philips database
	// or
	int azimuthIndex, altitudeIndex;// BIOMET or SVC2004 database
	bool indexOK;// this variable is used to check if all parameters are indexed 

public:
	bool parameterize4PDA();
	double rotation();
	// We reimplement the constructors of Obs to index the signature's parameters
	signature(int L_, int T_);	
	signature(char * filename);
	signature(char * filename, int L_);
	signature(char * filename, int L_, int T_);
	// if you declare a destructor of signature, Ginas will break:)
	//virtual ~signature();

	bool setIndex(int xIndex_, int yIndex_, int pressureIndex_, int thetaXIndex_, int thetaYIndex_, int azimuthIndex_, int altitudeIndex_);

	bool centralizeCoordinates();
	bool zeroBaseNorm(int *newMaxX, int *NewMaxY);
	void interpolateCoordinates();

	double normalizeSize(double sigma);
	double normalizeDirection();
	bool trim();	// indice de la pression pour couper
	bool filter();	// pour filter (x, y) et les angles
	bool parameterize();
	bool parameterizeXY();


private:
	void splint(double *xa, double *ya, double *y2a, int n, double x, double *y);
	void spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
};

#endif
