/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVModelFile.h"

// Default Constructor

PVModelFile::PVModelFile()
{
  _atcloud = 0;
  _atpoint = 0;
  _atvector = 0;
  _mode = UNDEF;
}

// Destructor

PVModelFile::~PVModelFile()
{
}

// New Output 

int
PVModelFile::newOutput(char* pp_file, char* pp_text, long pp_nclouds)
{
  if((_file = fopen(pp_file,"wb"))==NULL) // NOTE: need 'b' for binary (not default on Window$!)
    return OPEN_FILE;

  strncpy(&_header.description[0],pp_text,256);
  _header.version = PVM_VERSION; 
  _header.clouds = pp_nclouds; 
  _atcloud = 0; // Reset 

  if (!fwrite(&_header,sizeof(PVModelFileHeader),1,_file))
    return FAIL_HEADER; 
   
  _mode = OUTPUT;

  return OK;
}


// End Output 

int
PVModelFile::endOutput()
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;
  
  fclose(_file);
  _mode = UNDEF;
  return OK;
}

// Open Input 

int
PVModelFile::openInput(const char* pp_file)
{
  if((_file = fopen(pp_file,"rb"))==NULL) // NOTE: need 'b' for binary (not default on Window$!)
    return OPEN_FILE;
  else 
    _atcloud = 0;

  if (!fread(&_header,sizeof(PVModelFileHeader),1,_file)) 
    return FAIL_HEADER;
  
  _mode = INPUT;

  return OK;
}

// Close Input 

int
PVModelFile::closeInput()
{
  if (_mode!=INPUT)
    return WRONG_MODE;
  
  fclose(_file);
  _mode = UNDEF;
  return OK;
}

// New Cloud

int
PVModelFile::newCloud(unsigned long pv_npoints, PVModelFileMask pv_mask)
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;

  _cloud.points = pv_npoints;
  _cloud.mask = pv_mask;
  
  for (_cloud.vectors = 0; pv_mask!=0; pv_mask >>= 1)
    if (pv_mask & 01) _cloud.vectors++;

  if (!fwrite(&_cloud,sizeof(PVModelFileCloud),1,_file))
		return FAIL_CLOUD;

  _atpoint = 0; // reset
  return OK;
}

// End Cloud

int
PVModelFile::endCloud()
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;
  
  _atcloud++;
 
  //if (_atcloud!=_header.clouds)
  //  return NUMBER_CLOUDS; 
  
  if (_atpoint!=_cloud.points) 
    return NUMBER_POINTS;

	return OK;
}

// New Point

int
PVModelFile::newPoint()
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;
 
  _mask = _cloud.mask; // Ready to "tick" off
  _atvector = 0;
  
  return OK;
}

// End Point

int
PVModelFile::endPoint()
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;
  
  if (_mask!=00) // i.e. haven't ticked off all 
    return NUMBER_VECTORS;
  
  _atpoint++;

  return OK;
}

// New Vector

int
PVModelFile::newVector(PVModelFileMask pp_mask, float pp_x, float pp_y, float pp_z)
{
  if (_mode!=OUTPUT)
    return WRONG_MODE;
  
  if (pp_mask > _mask) // TODO: works?
    return WRONG_ORDER;
  
  PVModelFileMask on = pp_mask & _mask;
  
  if (!on) // Check if this mask was defined for the point
    return WRONG_TYPE;

  _mask &= ~on; // i.e. "tick off" this property 
  
  _atvector++;
 
  _vector.x = pp_x;
  _vector.y = pp_y;
  _vector.z = pp_z;

  if (!fwrite(&_vector,sizeof(PVModelFileVector),1,_file))
    return FAIL_VECTOR; 
  
  return OK;
}

// Next Cloud

int
PVModelFile::nextCloud()
{
  if (_mode!=INPUT)
    return WRONG_MODE;

  if (_atcloud==_header.clouds)
    return END_CLOUDS;

  if (!fread(&_cloud,sizeof(PVModelFileCloud),1,_file)) 
    return FAIL_CLOUD;
  
  _atcloud++;
  _atpoint=0;
  
  return OK;
}

// Next Point

int
PVModelFile::nextPoint()
{
  if (_mode!=INPUT)
    return WRONG_MODE;

  if (_atpoint==_cloud.points)
    return END_POINTS;

  _atpoint++;
  _atvector=0;

  return OK;
}

// Next Vector

int
PVModelFile::nextVector()
{
  if (_mode!=INPUT)
    return WRONG_MODE;
 
  while (_atvector<(sizeof(PVModelFileMask)*8)) // all x8 bits
  {
    if ((_cloud.mask >> _atvector) & 01)
    {
       _mask = (unsigned long)pow(2.0,_atvector);
     
       if (!fread(&_vector,sizeof(PVModelFileVector),1,_file))
         return FAIL_VECTOR;
       
       _atvector++;
     
       return OK;
    }
    _atvector++;
  }
  return END_VECTORS;

}

// TEST PROGRAM

#ifdef TEST_PVMODELFILE

int
main(int pv_argc, char** pp_argv)
{
  PVModelFile lv_in, lv_out;
 
  // Create a new file ///////////////////////////////////////////

  if (lv_out.newOutput("test.pvm","A test PVM file",5)!=PVModelFile::OK)
  {
    printf("Problem creating new file!\n");
    return 0;
  }
  for (int c=0; c<5; c++)
  {
    if (lv_out.newCloud(10,PVM_POSITION | PVM_RGB | PVM_VELOCITY)!=PVModelFile::OK)
    {
      printf("Problem creating cloud!\n");
    }
    for (int p=0; p<10; p++)
    {
      // NOTE: the class enforces that the specification for a point must
      // remain the same across all points of the same cloud, and that the
      // vectors must meet this specification in the order/type they are defined.

      if(lv_out.newPoint()!=PVModelFile::OK)
      {
        printf("Problem creating point!\n");
      }
      if(lv_out.newVector(PVM_POSITION,p,p,p)!=PVModelFile::OK 
      || lv_out.newVector(PVM_RGB,p*10, p*10, p*10)!=PVModelFile::OK
      || lv_out.newVector(PVM_VELOCITY,0.1,0.1,0.1)!=PVModelFile::OK)
      {
        printf("Problem creating vector!\n");
      }
      if (lv_out.endPoint()!=PVModelFile::OK)
      {
        printf("Number of vectors created differs!\n"); 
      }
    }
    if (lv_out.endCloud()!=PVModelFile::OK)
    {
      printf("Number of points created differs!\n");
    }
  }
  lv_out.endOutput();
 
  // Open it and read /////////////////////////////////////////// 

  if (lv_in.openInput("test.pvm")!=PVModelFile::OK)
  {
    printf("Problem opening file!\n");
    return 0;
  }
  printf("Description: [%s]\n",lv_in.description());
  printf("Version: %f\n",lv_in.version());
  printf("Number of clouds: %d\n",lv_in.clouds());
  
  while(lv_in.nextCloud()==PVModelFile::OK)
  {
    printf("Next cloud...\n");
    printf("Number of points: %i\n",lv_in.points());
    printf("Number of vectors: %i\n",lv_in.vectors());

    while(lv_in.nextPoint()==PVModelFile::OK)
    {
      printf("\tNext point...\n");
      while(lv_in.nextVector()==PVModelFile::OK)
      {
        switch(lv_in.mask())
        {
          case PVM_POSITION:
          {
            printf("\t\t[POS: %f %f %f]\n",lv_in.x(),lv_in.y(),lv_in.z());
            break;
          }
          case PVM_VELOCITY:
          {
            printf("\t\t[VEL: %f %f %f]\n",lv_in.x(),lv_in.y(),lv_in.z());
            break;
          }
          case PVM_RGB:
          {
            printf("\t\t[RGB: %f %f %f]\n",lv_in.x(),lv_in.y(),lv_in.z());
            break;
          }
          default:
          {
            printf("\t\tUnrecognised vector mask!\n");
            break;
          }
        }
      }
    }
  }
  lv_in.closeInput();

}

#endif
