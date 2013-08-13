/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVLoadPVM.h"

/**
Perform
*/

void
PVLoadPVM::perform (PVModel * pp_model, const char* pp_filename)
throw (PVException &)
{
  PVModelFile lv_in;

  if (lv_in.openInput(pp_filename)!=PVModelFile::OK)
    throw new PVException("PVControl::parse","Problem opening file!\n");
  else
  {
    if (lv_in.version()!=1.0) {
      throw PVException("PVImportPVM","Incompatible file version!");
    }

    //printf("Description: [%s]\n",lv_in.description());
    //printf("Version: %f\n",lv_in.version());
    //printf("Number of clouds: %d\n",lv_in.clouds());
    
    while(lv_in.nextCloud()==PVModelFile::OK)
    {
      //printf("Number of points: %d\n",lv_in.points());
      //printf("Number of vectors: %d\n",lv_in.vectors());
      
      PVCloud* lp_cloud = pp_model->newCloud(lv_in.points());
      
      while(lv_in.nextPoint()==PVModelFile::OK)
      {
        float x = 0,y = 0,z = 0,r = 0,g = 0,b = 0;
        //PVPoint* lp_point = lp_cloud->newPoint(lv_in.vectors());
        
        while(lv_in.nextVector()==PVModelFile::OK)
        {
          switch(lv_in.mask())
          {
          case PVM_POSITION:
            {
              x = lv_in.x();
              y = lv_in.y();
              z = lv_in.z();
              //lp_point->setPosition(lv_in.x(),lv_in.y(),lv_in.z());
              break;
            }
          case PVM_RGB:
            {
              r = lv_in.x();
              g = lv_in.y();
              b = lv_in.z();
              //lp_point->setColour(lv_in.x(),lv_in.y(),lv_in.z());
              break;
            }
          default:
            {
              throw new PVException("PVControl::parse","File has unrecognised vector");
              break;
            }
          }
        }
        lp_cloud->newPoint(x,y,z,r,g,b);
      }
      pp_model->endCloud(lp_cloud);
    }
    lv_in.closeInput();
  }
}
