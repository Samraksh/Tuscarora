////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 
#include <cmath>


double Factorial (uint64_t k, uint64_t factlimit)
{
	double fact = 1;
  if (factlimit > k) factlimit = k;
  while (factlimit > 0)
    {
      fact *= k;
      --factlimit;
      --k;
    }
  return fact;
}
double Binomial (uint64_t k, double p, uint64_t n)
{
	 double retval = 1 ;
	 assert(n>k);

	 if(k == 0) retval = std::pow (1 - p, static_cast<double> (n));
	 else if(n  <= k * 100){//If k is NOT small relative to n
		 if(k == 0) retval = 1;
		 else if(k == n) retval = 1;
		 else if( n - k > k ){
			 retval = double(Factorial (n, k)) / double(Factorial(k ,k));
		 }
		 else{
			 retval = double(Factorial (n, n - k)) / double(Factorial(n - k, n - k));
		 }

		retval *= std::pow (p, static_cast<double> (k));
		retval*= std::pow (1 - p, static_cast<double> (n - k));

	 }
	 else{ //If k is small relative to n
		 double lambda = n*p;
		 if ( n > 1000){ //if n is large
			 retval =  std::pow(lambda,k);
			 retval /= Factorial(k,k);
			 retval *= exp(-1*lambda);
		 }
		 else{ //if n is large
			 retval =  std::pow(lambda,k) / Factorial(k,k) * std::pow(static_cast<double>(1-p),static_cast<double>(n));
		 }
	 }
    return retval;
}




