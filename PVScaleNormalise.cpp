/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVScaleNormalise.h"

/**
Perform
*/

void
PVScaleNormalise::perform (PVModel * pp_model)
throw (PVException &)
{
  double lv_maxnorm = 1.0;
  unsigned long lv_pt;
  try
  {
    PVCloud *lp_at = pp_model->current ();

    //do {
	for (lv_pt = 0; lv_pt < lp_at->npoints; lv_pt++)
	  {
	    PVMath::Vector3 lv_pos((*lp_at)[lv_pt]->x,(*lp_at)[lv_pt]->y,(*lp_at)[lv_pt]->z);
	    if (lv_pos.norm() > lv_maxnorm)
	      lv_maxnorm = lv_pos.norm();
	  }
	for (lv_pt = 0; lv_pt < lp_at->npoints; lv_pt++)
	  {
	    ((*lp_at)[lv_pt]->x) /= lv_maxnorm;
	    ((*lp_at)[lv_pt]->y) /= lv_maxnorm;
	    ((*lp_at)[lv_pt]->z) /= lv_maxnorm;
	  }
    //  } while ((lp_at = pp_model->nextCloud ()) != NULL);
  }
  catch (PVException &)
  {
  }
}
