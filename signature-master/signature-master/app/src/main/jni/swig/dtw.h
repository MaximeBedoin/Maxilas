#if !defined(DTW_)
#define DTW_
#include "Obs.h"

class DTW  
{
	int lx, ly;
	double dmin;
	int vecSize;
	Obs *ref;
	Obs *tst;

public:
	void setCosts(double substi_cost_, double delete_cost_, double insert_cost_);
	double distance();
	DTW(Obs *tst_, Obs *ref_);
	virtual ~DTW();

private:
	// The costs, by default, they are all set to 1
	double	substi_cost;		//#define		P11		1
	double	delete_cost;		//#define		P21		1
	double	insert_cost;		//#define		P12		1

	// Topology of the DWT
	//                      *
	//                  * /
	// delete_cost  *   / *
	//          *     /  
	//     substi_cost  * 
	//        
	//                *
	//             insert_cost
	double norm(double *vec1, double *vec2, int size);
};

#endif // !DTW_
