/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVLoadAscii.h"

/**
Perform
*/

void
PVLoadAscii::perform (PVModel * pp_model, const char* pp_filename)
throw (PVException &)
{
  double lv_x, lv_y, lv_z, lv_r, lv_g, lv_b, lv_nx, lv_ny, lv_nz;
  int lv_nfields = 0;

  // Try opening file...

  ifstream ls_fin (pp_filename, ios::in);
  if (!ls_fin)
    {
      throw PVException ("PVFlatFileImporter", "File does not exist!");
    }
  // Read first line and establish format... (TODO: could be better!)
  char lv_c;
  while ((lv_c = ls_fin.get ()) != '\n')
    if (isspace (lv_c))
      lv_nfields++;
  lv_nfields++;			// For last one...

  // Rewind to beginning...
  ls_fin.seekg (0, ios::beg);

  // OK... all seems fine, so create a cloud (DYNAMIC allocation)...
  PVCloud *lv_cloud = pp_model->newCloud ();

  // Read and parse each line accordingly into cloud...
  ls_fin.precision (10);
  while (!ls_fin.eof ())
    {				// TODO: what about comments?
      switch (lv_nfields)
	{
	case 3:
	  {
	    ls_fin >> fixed >> lv_x >> lv_y >> lv_z;
	    lv_cloud->newPoint (lv_x, lv_y, lv_z,1.0,1.0,1.0);
	    break;
	  }
	case 6:
	  {
	    ls_fin >> fixed >> lv_x >> lv_y >> lv_z >> lv_r >> lv_g >> lv_b;
            lv_cloud->newPoint (lv_x,lv_y,lv_z,lv_r,lv_g,lv_b); 
	    break;
	  }
	case 9:
	  {
	    ls_fin >> fixed >> lv_x >> lv_y >> lv_z >> lv_r >> lv_g >> lv_b >> lv_nx >> lv_ny >> lv_nz;
	    lv_cloud->newPoint (lv_x, lv_y, lv_z,lv_r,lv_g,lv_b,lv_nx,lv_ny,lv_nz);
	    break;
	  }
        default:
          {
            throw PVException("PVLoadAscii","Number of fields per line not 3-9");
            break; 
          }
        }
    }

  // End of cloud..
  pp_model->endCloud (lv_cloud);
  ls_fin.close ();
}
