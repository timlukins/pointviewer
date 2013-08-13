/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_MATH__
#define __PV_MATH__

#ifdef _WIN32			// Friggin' Window$ doesn't have std::cmath!
#include <math.h>
#else
#include <cmath>
using namespace std;
#endif 

/**
* Math Class.
* One single static point of reference for all Point Viewer Math operations.
* Acts as wrapper to hide actual implementation (whether other lib calls or hard coded inline).
*/

class PVMath
{
public:
	
	PVMath ()
  {
  }				// Never actually used... 
			 
	virtual ~ PVMath()
	{
	}

	// Operations on doubles... 

  /** 
  *Square.
  */
  inline static double sqr (double pv_val)
  {
    return pv_val * pv_val;
  }

  /**
  * Square Root.
  */
  inline static double sqroot (double pv_val)
  {
    return sqrt (pv_val);
  }

	// Operations on vectors...

	class Vector3
	{
	public:

		/**
		* Vector3 is simply a x,y and z component.
		*/

		double x, y, z;


		Vector3() {}
		virtual ~Vector3() {}
		Vector3(double px,double py, double pz) { x=px; y=py; z=pz; }

		/**
		* Normalise vector.
		*/

		inline void normalise()
		{
			double n = norm();
			x /= n;
			y /= n;
			z /= n;
		}
	
		/**
		* Return the norm (length) of vector.
		*/

		inline double norm()
		{
			return PVMath::sqroot (PVMath::sqr(x) + PVMath::sqr(y) + PVMath::sqr(z));
		}

		/**
		* Subtract vector from this vector.
		*/

		inline Vector3 operator- (const Vector3& pr)
		{
			Vector3 tmp;
			tmp.x = x-pr.x; tmp.y=y-pr.y; tmp.z=z-pr.z; 
			return tmp;
		}

		/**
		* Add vector to this vector.
		*/

		inline Vector3 operator+ (const Vector3& pr)
		{
			Vector3 tmp;
			tmp.x = x+pr.x; tmp.y=y+pr.y; tmp.z=z+pr.z; 
			return tmp;
		}

		/**
		* Cross product if given other vector.
		*/

		inline Vector3 operator* (const Vector3& pr)
		{
			Vector3 tmp;
			tmp.x = y*pr.z - z*pr.y;
			tmp.y = z*pr.x - x*pr.z;
			tmp.z = x*pr.y - y*pr.x;
			return tmp;
		}

		/**
		* Assignment from other vector.
		*/

		inline void operator= (const Vector3& pv)
		{
			x = pv.x; y=pv.y; z=pv.z;
		}

	};
  

};


#endif
