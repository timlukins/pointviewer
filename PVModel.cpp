/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVModel.h"

/**
Constructors.
*/

PVModel::PVModel ()
{
}

/**
Destructors.
*/

PVModel::~PVModel ()
{
  // TODO: iterate though all clouds, push and delete!   
}

/**
New Cloud
*/

PVCloud *
PVModel::newCloud (unsigned long pv_n)
throw (PVException &)
{
  PVCloud *lv_ok;
  try
  {
    lv_ok = new PVCloud (pv_n);
  } catch (PVException & er_memfail)
  {
    throw er_memfail;
  }
  av_clouds.push_back (lv_ok);
  av_current = av_clouds.size()-1; //i.e. the last one just added
  return lv_ok;
}

/**
New Cloud without specifying size.
*/

PVCloud *
PVModel::newCloud ()
throw (PVException &)
{
  PVCloud *lv_ok;
  try
  {
    lv_ok = newCloud (1);
  }
  catch (PVException & er_memfail)
  {
    throw er_memfail;
  }
  return lv_ok;
}


/**
End Cloud
*/

void
PVModel::endCloud (PVCloud * pp_cloud)
throw (PVException &)
{
  if (pp_cloud->npoints > pp_cloud->atpoint + 1)
    {				// Check if we can shrink this cloud a bit
      try
      {
	pp_cloud->resize (pp_cloud->npoints, pp_cloud->atpoint + 1);
      }
      catch (PVException & er_memfail)
      {
	throw er_memfail;
      }
      pp_cloud->npoints = pp_cloud->atpoint + 1;
    }
  pp_cloud->atpoint = ULONG_MAX;

}

/** 
First Cloud
*/

PVCloud *
PVModel::firstCloud ()
throw (PVException &)
{
  av_atcloud = av_clouds.begin ();
  if (av_atcloud == av_clouds.end ())
    throw PVException ("PVModel", "firstCloud() - no clouds");
  return *av_atcloud;
}

/** 
Next Cloud
*/

PVCloud *
PVModel::nextCloud ()
throw (PVException &)
{
  av_atcloud++;
  if (av_atcloud == av_clouds.end ())
    throw PVException ("PVModel", "nextCloud() - end of clouds");

  return *av_atcloud;
}

/**
Current
*/

PVCloud*
PVModel::current()
throw (PVException&)
{
  if (av_clouds.size()==0)
    throw PVException ("PVModel","select() - no clouds");

  return av_clouds.at(av_current);
}

/**
Step forwards
*/

void
PVModel::stepForwards()
throw (PVException&)
{
  av_current++;
  if (av_current==av_clouds.size()) av_current=0;
}

/**
Step backwards 
*/

void
PVModel::stepBackwards()
throw (PVException&)
{
  if (av_current==0) av_current=av_clouds.size()-1;
  else av_current--;
}
