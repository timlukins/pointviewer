/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVTranslateCenter.h"


void PVTranslateCenter::perform()
  {
    //double lv_norm = 0.0, lv_maxnorm = 1.0;
    double lv_shiftx =0.0, lv_shifty = 0.0, lv_shiftz = 0.0;
    unsigned long lv_pt;
    
    PVCloud *lp_at = ap_model->current();
    
    for (lv_pt = 0; lv_pt < lp_at->npoints; lv_pt++)
    {
      //PVMath::Vector3 lv_pos((*lp_at)[lv_pt]->x,(*lp_at)[lv_pt]->y,(*lp_at)[lv_pt]->z);
      //lv_norm = lv_pos.norm();
      //if (lv_norm > lv_maxnorm)
      //		     lv_maxnorm = lv_norm;
      lv_shiftx += (*lp_at)[lv_pt]->x;
      lv_shifty += (*lp_at)[lv_pt]->y;
      lv_shiftz += (*lp_at)[lv_pt]->z;
    }
    lv_shiftx /= lv_pt;
    lv_shifty /= lv_pt;
    lv_shiftz /= lv_pt;

    ap_state->camera_translationx = lv_shiftx;
    ap_state->camera_translationy = lv_shifty;
    ap_state->camera_translationz = lv_shiftz;
    
  }

