/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVCloud.h"

/**
Constructor.
*/

PVCloud::PVCloud (unsigned long pv_n)
throw (PVException &)
{
  npoints = pv_n;
  atpoint = ULONG_MAX;

  // Statically allocate EXACT memory in one big chunk!

  try
  {
    if ((data = (PVPoint **) calloc (pv_n, sizeof (PVPoint *))) == NULL)
      throw PVException ("PVModel", "newCloud() - Out of memory!");
    for (unsigned long lv_n = 0; lv_n < pv_n; lv_n++)
      {
			data[lv_n] = new PVPoint;	// TODO: should check if OK, i.e. use malloc==NULL...
      }
  } catch (PVException & er_memfail)
  {
    throw er_memfail;
  }

}

/**
Destructor.
*/

PVCloud::~PVCloud ()
{

  //TODO!!! - Delete all points & used colours!
}

/**
Resize Cloud
*/

void
PVCloud::resize (unsigned long pv_old, unsigned long pv_new)
throw (PVException &)
{

  if (pv_new > pv_old)
    {				// More, more, must have more!!!

      if ((data =
	   (PVPoint **) realloc (data, sizeof (PVPoint *) * pv_new)) == NULL)
	throw PVException ("PVModel", "newCloud() - Out of memory!");
      for (unsigned long lv_n = pv_old; lv_n < pv_new; lv_n++)
	{
	  data[lv_n] = new PVPoint;
	}
    }
  else
    {				// Shrinking, shrinking, i'm shrinking!!!

      for (unsigned long lv_n = pv_old; lv_n < pv_new; lv_n++)
	{
	  free (data[lv_n]);
	}
      if ((data =
	   (PVPoint **) realloc (data, sizeof (PVPoint *) * pv_new)) == NULL)
	throw PVException ("PVModel", "newCloud() - freeing memory!");
    }
}


/**
[] Operator
*/

PVPoint *
PVCloud::operator[] (unsigned long pv_point)
throw (PVException &)
{
  if (pv_point > npoints)
    throw PVException ("PVModel", "[]operator - point out of range!");

  return data[pv_point];
}

/**
New Point
*/

void
PVCloud::newPoint (double pv_x, double pv_y, double pv_z, float pv_r,
		   float pv_g, float pv_b, double pv_nx,
		   double pv_ny, double pv_nz)
throw (PVException &)
{
  if (atpoint == ULONG_MAX)
    atpoint = 0;
  else
    atpoint++;

  if (atpoint == npoints)
    {				// Check if we need to get more memory!
      try
      {
	resize (npoints, npoints + 1);
      }
      catch (PVException & er_memfail)
      {
	throw er_memfail;
      }
      npoints++;
    }

  data[atpoint]->x = pv_x;
  data[atpoint]->y = pv_y;
  data[atpoint]->z = pv_z;

  // Colour created/assigned as required...

  data[atpoint]->r = pv_r;
  data[atpoint]->g = pv_g;
  data[atpoint]->b = pv_b;

  // Normals...

  data[atpoint]->nx = pv_nx;
  data[atpoint]->ny = pv_ny;
  data[atpoint]->nz = pv_nz;
}

/**
New Point
*/

void
PVCloud::newPoint (double pv_x, double pv_y, double pv_z, float pv_r,
		   float pv_g, float pv_b)
throw (PVException &)
{
  newPoint (pv_x, pv_y, pv_z, pv_r, pv_g, pv_b, 0.0, 0.0, 0.0);
}

