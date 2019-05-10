#if !defined(statistics_)
#define statistics_

// variance threshold:
// 1e-4 is taken from seat-of-the-pants experience:
// Rabiner's book, pp. 382
#define COV_MIN_VAL				1e-4	

class statistics  
{
public:
	static int combination(int k, int N);
	static double median(double *windows, int size);
	static void variance(double *elements, int number, double * mean_, double * var_);
};

#endif 
