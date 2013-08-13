/**************************************************
.......
pvmerge
.......

Description::

	Combine floating point tiff depth map, uchar colour image
        together with (optional) mask and reprojection infomation
        to create a single accurate PVM format file providing per
        point position, colour, normal, and size.

Requires::

	libtiff (3.6.*) [www.libtiff.org]

Build::

        See accomanying Makefile (e.g. type 'make' if OK).

Usage::

	pvmbuild <depth.tif> <colour.tif> [<mask.tif>] [<camera.xml>]

Author::

        Tim Lukins (Version 1.0, August 2004)
				Toby Breckon (Version 1.1, December, 2004)

Known Bugs::
				Output to files causes segmentation fault (version 1.1)
				Output of ASCII XYZRGB is hence to stdout in the interim
				Background subtraction is currently not supported with PVM output.

****************************************************/

/* Includes */

// system

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

// libtiff for the image stuff

#include <tiffio.h>

// lib xml for the XML parsing

#include <libxml/parser.h>
#include <libxml/tree.h>

// use GTS library for matrix / vector package
// http://gts.sourceforge.net/

#include <gts.h>

// local includes

#include "rgbhsv.h"
#include "PVModelFile.h"

/////////////////////////////////////////////////////////

/* Define maximum length of the Virtual Clones 
   TIFF XML tag in chars */

#define VC_TIFFTAG_MAXIMUM_XML_LENGTH 2000

/////////////////////////////////////////////////////////

#define PI 3.14159265

#define OUTPUT_PRECISION 10

/////////////////////////////////////////////////////////

/**
* PVMBuild class.
* Composite 3D location, colour, normals, projection
*/

class PVMBuild
{
private:

  float*         ap_depth;
  float*         ap_x;
  float*         ap_y;
  float*         ap_z;
  unsigned char* ap_red;
  unsigned char* ap_green;
  unsigned char* ap_blue;
  bool*          ap_mask;
  float*         ap_nx;
  float*         ap_ny;
  float*         ap_nz;
  float*         ap_r;
  long           av_width;
  long           av_height;
  unsigned long  av_npoints;
  float          av_xyratio;

	bool backgroundSubtraction;
	bool axisAlignedOutput;
	
public:

  // Constructor & Destructor

  PVMBuild();
  virtual ~PVMBuild();

  // Data input
  
  int readDepthTIFF (char*);
  int readColourTIFF (char*);

  // Data output

  int writePVM (char*,long);
	int writeXYZRGB (char*,long);
	
  // Data conversion

  void projectOrthogonal ();

	int projectWithCameraParameters ();

	void blueBackgroundSubtraction ();

	void axisAlignedSampling ();

private:

  // Basic geomtry...
	
	struct Lens {
		double t1; double t2;
		double r1; double r2;
	};

	// camera parameter variables
	
	// intrinsic camera parameter matrix
	
	GtsMatrix* intrinsic_matrix;
	
	// translation[i] [0, 1, 2] = [x, y, z]
	
	GtsVector translation;
	
	// rotation[i] [0, 1, 2, 3] = [x, y, z, a]
	
	GtsVector4 rotation;
	
	// distoration - 2 radial co-efficients r1, r2 and 2 tangential ones
	// t2 and t2
	
	Lens distortion_parameters;

	// parse XML tag info from Virtual clones tiff header
	// and store attributes in class structure
	// return value: 1 = success, 0 = failure
	
	int parseXMLtag(char * tag_buffer);
	
	// Construct camera Pose matrix by
	// converting the rotation specified as (x, y, z, a) i.e. axis and angle
	// values to a 3x3 rotation matrix and inserting translation x, y,z as
  // appropriate to form a 4x4 homogenous transformation matrix
	// Returns a pointer to a new 4x4 matrix
	
	GtsMatrix* constructPoseMatrix();
	
	// given a colour in R G B space returns TRUE if it is part of the specified
	// background
	// currently works on blue but could be extended
	
	bool isPartOfBackGround(int r, int g, int b);
	
public:
  
    PVModelFile ap_out;

};

/////////////////////////////////////////////////////////

/**
* Default Constructor.
*/

PVMBuild::PVMBuild()
{
  av_width = 0;
  av_height = 0;
  av_npoints = 0;

	backgroundSubtraction = FALSE;
	axisAlignedOutput = FALSE;
	
	intrinsic_matrix = NULL;

}

/**
* Default Destructor.
*/

PVMBuild::~PVMBuild()
{
  free(ap_depth);
	gts_matrix_destroy(intrinsic_matrix);
}

/**
* Read Colour TIFF
*/

int
PVMBuild::readColourTIFF (char *pp_file)
{
  TIFF *lp_tif;
  char *lp_pixels;
  unsigned char *lp_tiffdata;
  long lv_width;
  long lv_height;
  int lv_nstrips;
  int lv_stripsize;
  int lv_counter;
  int lv_pixel;
  int lv_npixels;
  uint16 lv_bitsper;
  tdata_t lp_buf;
  tstrip_t lv_strip;
	
  // Try and open file...

  if (lp_tif = TIFFOpen (pp_file, "r"))
  {
    // Check data...
    
    TIFFGetField (lp_tif, TIFFTAG_BITSPERSAMPLE, &lv_bitsper);
    if (lv_bitsper!=8) {
      cerr << "Colour data not 8 bit per channel!" << endl;
      TIFFClose(lp_tif);
      return 0;
    }
		
    // Check to see if it's dimensions are as expected...
    
    TIFFGetField (lp_tif, TIFFTAG_IMAGEWIDTH, &lv_width);
    TIFFGetField (lp_tif, TIFFTAG_IMAGELENGTH, &lv_height);

    if (av_npoints==0)
    {
      av_width = lv_width;
      av_height = lv_height;
      av_npoints = lv_width * lv_height;
      av_xyratio = (float)lv_width/(float)lv_height;
    } else if (av_width!=lv_width || av_height!=lv_height) {
      cerr << "Colour file is of different dimensions!" << endl;
      TIFFClose(lp_tif);
      return 0;
    }

    // Allocate memory...

    ap_red = (unsigned char*)malloc(av_npoints*sizeof(unsigned char));
    ap_green = (unsigned char*)malloc(av_npoints*sizeof(unsigned char));
    ap_blue = (unsigned char*)malloc(av_npoints*sizeof(unsigned char));

    // Iterate through it and store data...

    lv_pixel = 0;
    lv_stripsize = TIFFStripSize (lp_tif);
    lv_nstrips = TIFFNumberOfStrips (lp_tif);
    lp_buf = _TIFFmalloc (TIFFStripSize (lp_tif));
    
    lv_counter = 0;
    for (lv_strip = 0; lv_strip < lv_nstrips; lv_strip++)
    {
      TIFFReadEncodedStrip (lp_tif, lv_strip, lp_buf, (tsize_t) - 1);
      lp_tiffdata = (unsigned char*) lp_buf;
      for (lv_pixel = 0; lv_pixel < lv_stripsize; lv_pixel+=3) // NOTE 3 channel RGB assumed!
      {
        if (lv_counter < av_npoints) { 

          // Set values...

          ap_red[lv_counter] = lp_tiffdata[lv_pixel];
          ap_green[lv_counter] = lp_tiffdata[lv_pixel+1];
          ap_blue[lv_counter] = lp_tiffdata[lv_pixel+2];
        } 
        lv_counter++;
      } 
    _TIFFfree (lp_buf);
    } 
   TIFFClose (lp_tif);
  } 
  else return 0;
  
  return 1;
}

/**
* Read Depth TIFF
*/

int
PVMBuild::readDepthTIFF (char *pp_file)
{
  TIFF *lp_tif;
  float *lp_pixels;
  float *lp_tiffdata;
  long lv_width;
  long lv_height;
  int lv_nstrips;
  int lv_stripsize;
  int lv_counter;
  int lv_pixel;
  int lv_npixels;
  uint16 lv_bitsper;
  tdata_t lp_buf;
  tstrip_t lv_strip;
	char* vc_tifftag = NULL;
	
  // Try and open file...

  if (lp_tif = TIFFOpen (pp_file, "r"))
  {
    // Check data...

    TIFFGetField (lp_tif, TIFFTAG_BITSPERSAMPLE, &lv_bitsper);
    if (lv_bitsper!=32)
    {
      cerr << "Depth data not floating point!" << endl;
      TIFFClose(lp_tif);
      return 0;
    }
    
		// get virtual clones tags. Parse resulting XML and store camera attributes
	
		vc_tifftag = (char *) malloc(VC_TIFFTAG_MAXIMUM_XML_LENGTH * sizeof(char));
		if (!vc_tifftag){
			cerr << "error: not enough memory XML TIFF image tag\n";
			return 0;
		}
		
    if(!TIFFGetField(lp_tif,TIFFTAG_VIRTUALCLONES_CAL_TYPE, &vc_tifftag))
    {   
			fprintf(stderr,"No VC Calibration Type!\n");	
    } else {
		  fprintf(stderr, "Depth image has VC calibration info.\n", vc_tifftag);
		}
		if(!TIFFGetField(lp_tif,TIFFTAG_VIRTUALCLONES_CAL_INFO, &vc_tifftag))
		{
        fprintf(stderr,"No VC Calibration Information!\n");
		} else {
				//fprintf(stderr, "VC calibration info found.\n", vc_tifftag);
			
				if (!parseXMLtag(vc_tifftag)){
					cerr << "error: calibration XML parsing error\n";
					return 0;
				}
		}
    // Check to see if it's dimensions are as expected...
    
    TIFFGetField (lp_tif, TIFFTAG_IMAGEWIDTH, &lv_width);
    TIFFGetField (lp_tif, TIFFTAG_IMAGELENGTH, &lv_height);

    if (av_npoints==0)
    {
      av_width = lv_width;
      av_height = lv_height;
      av_npoints = lv_width * lv_height;
      av_xyratio = (float)lv_width/(float)lv_height;
    } else if (av_width!=lv_width || av_height!=lv_height) {
      cerr << "Depth file is of different dimensions!" << endl;
      TIFFClose(lp_tif);
      return 0;
    }

    // Allocate memory...

    ap_depth = (float*)malloc(av_npoints*sizeof(float));

    // Iterate though and store data...

    lv_pixel = 0;
    lv_npixels = av_width * av_height;
    lv_stripsize = TIFFStripSize (lp_tif) / 4; // NOTE: 4 byte float = 32 bit
    lv_nstrips = TIFFNumberOfStrips (lp_tif);
    lp_buf = _TIFFmalloc (TIFFStripSize (lp_tif));

    lv_counter = 0;
    for (lv_strip = 0; lv_strip < lv_nstrips; lv_strip++)
    {
      TIFFReadEncodedStrip (lp_tif, lv_strip, lp_buf, (tsize_t) - 1);
      lp_tiffdata = (float *) lp_buf;
      for (lv_pixel = 0; lv_pixel < lv_stripsize; lv_pixel++)
      {
        if (lv_counter < lv_npixels)
          ap_depth[lv_counter] = lp_tiffdata[lv_pixel];
          lv_counter++;
        }
    }
    _TIFFfree (lp_buf);
    TIFFClose (lp_tif);
  }
  else return 0;
  
  return 1;
} 

/**
* parse XML header tag and store attributes in class
*/

int
PVMBuild::parseXMLtag(char * tag_buffer)
{
	xmlDocPtr xml_parse_tree;
	xmlNodePtr xml_parse_tree_node;
	xmlChar *xml_attribute;
	
	int parse_completed = 0; // parse succeeds when this = 4
	
	// store tag info for later point projection
			
	// build parse tree from XML definition

	xml_parse_tree = xmlParseMemory(tag_buffer, strlen(tag_buffer));
	if (xml_parse_tree == NULL ) {
		fprintf(stderr,"error: XML parsing not possible\n");
		return 0;
	}
	
	// search parse tree to extract attribute data
	// (attribute names are hard-coded)
	
	xml_parse_tree_node = xmlDocGetRootElement(xml_parse_tree);
	while (xml_parse_tree_node != NULL) {
		
		// main document element cvCamera
		
		if ((!xmlStrcmp(xml_parse_tree_node->name, (const xmlChar *)"cvCamera")))
		{
			xml_parse_tree_node = xml_parse_tree_node->xmlChildrenNode;
			
			// main document sub-element camPProjR2T2
			
			while (xml_parse_tree_node != NULL) {
				if ((!xmlStrcmp(xml_parse_tree_node->name, 
								(const xmlChar  *)"camPProjR2T2"))) 
				{
					xml_parse_tree_node = xml_parse_tree_node->xmlChildrenNode;
					while (xml_parse_tree_node != NULL) 
					{
						
						// matrix values in XML document path
						// cvCamera->camProjR2T2->mat3
						
						if ((!xmlStrcmp(xml_parse_tree_node->name, 
							(const xmlChar  *)"mat3"))) 
						{
							intrinsic_matrix = gts_matrix_zero(NULL);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m11");
							intrinsic_matrix[0][0] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m12");
							intrinsic_matrix[0][1] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m13");
							intrinsic_matrix[0][2] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m21");
							intrinsic_matrix[1][0] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m22");
							intrinsic_matrix[1][1] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m23");
							intrinsic_matrix[1][2] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m31");
							intrinsic_matrix[2][0] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m32");
							intrinsic_matrix[2][1] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"m33");
							intrinsic_matrix[2][2] = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							parse_completed++;
							
							intrinsic_matrix[0][3] = 0;
							intrinsic_matrix[1][3] = 0;
							intrinsic_matrix[2][3] = 0;
							intrinsic_matrix[3][0] = 0;
							intrinsic_matrix[3][1] = 0;
							intrinsic_matrix[3][2] = 0;
							intrinsic_matrix[3][3] = 1;
							
							xml_parse_tree_node = xml_parse_tree_node->next;
						} else if ((!xmlStrcmp(xml_parse_tree_node->name, 
							(const xmlChar  *)"pProjR2T2"))) 
						{
							// pProjR2T2 values in XML document path
							// cvCamera->camProjR2T2->pProjR2T2
						
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"r1");
							distortion_parameters.r1 = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"r2");
							distortion_parameters.r2 = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"t1");
							distortion_parameters.t1 = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"t2");
							distortion_parameters.t2 = atof((char *) xml_attribute);
							xmlFree(xml_attribute);
							parse_completed++;
							xml_parse_tree_node = xml_parse_tree_node->next;
						} else if ((!xmlStrcmp(xml_parse_tree_node->name, 
							(const xmlChar  *)"pose"))) 
						{
							// pose values in XML sub-element
							// cvCamera->camProjR2T2->pose
							
							xml_parse_tree_node = xml_parse_tree_node->xmlChildrenNode;
							while (xml_parse_tree_node != NULL) {
								
								// rotation / translation values in XML document path
								// cvCamera->camProjR2T2->{rotation | translation}
								
								if (!xmlStrcmp(xml_parse_tree_node->name, 
								(const xmlChar  *)"rotation"))  
								{
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"x");
									rotation[0] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"y");
									rotation[1] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"z");
									rotation[2] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"a");
									rotation[3] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									parse_completed++;
									xml_parse_tree_node = xml_parse_tree_node->next;
								} else if (!xmlStrcmp(xml_parse_tree_node->name, 
								(const xmlChar  *)"translation"))
								{
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"x");
									translation[0] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"y");
									translation[1] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									xml_attribute = xmlGetProp(xml_parse_tree_node, (const xmlChar  *)"z");
									translation[2] = atof((char *) xml_attribute);
									xmlFree(xml_attribute);
									parse_completed++;
									xml_parse_tree_node = xml_parse_tree_node->next;
								} else {
									xml_parse_tree_node = xml_parse_tree_node->next;
								}
							}
						} else {
							xml_parse_tree_node = xml_parse_tree_node->next;
						}
					}
				} else {
					xml_parse_tree_node = xml_parse_tree_node->next;
				}
			}
		} else {
			xml_parse_tree_node = xml_parse_tree_node->next;
		}
	}
				
	xmlFreeDoc(xml_parse_tree);
		
	return (parse_completed == 4);
}

/**
* Construct camera Pose matrix by
* converting the rotation specified as (x, y, z, a) i.e. axis and angle
* values to a 3x3 rotation matrix and inserting translation x, y,z as
* appropriate to form a 4x4 homogenous transformation matrix
* Returns a pointer to a new 4x4 matrix
*/

GtsMatrix* 
PVMBuild::constructPoseMatrix()
{
	GtsVector normalisedAxis;
	
	GtsMatrix * r;
	
	// rotation_vector is an array (pass by reference) 
	// => do normalisation on a copy of it
	
	normalisedAxis[0] = rotation[0];
	normalisedAxis[1] = rotation[1];
	normalisedAxis[2] = rotation[2];
	gts_vector_normalize(normalisedAxis);
	
	// use GTS routine to build rotation matrix from 
	// normalised axis x,y,z & angle a vector.
	
	r = gts_matrix_rotate(NULL, normalisedAxis, rotation[4]);

	// insert translation values as appropriate
	
	r[3][0] = translation[0];
	r[3][1] = translation[1];
	r[3][2] = translation[2];
	
	return r;
}


/**
* Project depth orthogonally to R3 space.
*/

void
PVMBuild::projectOrthogonal ()
{
  // Allocate memory for 3D points and normals...
	
  ap_x = (float*)malloc(av_npoints*sizeof(float));
  ap_y = (float*)malloc(av_npoints*sizeof(float));
  ap_z = (float*)malloc(av_npoints*sizeof(float));
  ap_nx = (float*)malloc(av_npoints*sizeof(float));
  ap_ny = (float*)malloc(av_npoints*sizeof(float));
  ap_nz = (float*)malloc(av_npoints*sizeof(float));
  ap_r = (float*)malloc(av_npoints*sizeof(float));
  
	if (!(ap_x && ap_y && ap_z && ap_nx && ap_nz && ap_r))
	{
		cerr << "error: not enough memory for point allocation\n";
		exit(1);
	}	
	
  // Iterate accross sample grid and project...  
  
  unsigned long lv_at = 0;
  for (long lv_y=1; lv_y <= av_height; lv_y++)
  {
    for (long lv_x=1; lv_x <= av_width; lv_x++)
    {
      // Data simply arrayed in unit space on x/y plane...

      ap_x[lv_at] = (float)lv_x * (av_xyratio*1.0)/(float)av_width;        
      ap_y[lv_at] = (float)lv_y * 1.0/(float)av_height;        
      ap_z[lv_at] = ap_depth[lv_at];

      // Normals then simple up along z axis... 

      ap_nx[lv_at] = 0;
      ap_ny[lv_at] = 0;
      ap_nz[lv_at] = -1;

      // Radius is mean pixel size in unit space...

      ap_r[lv_at] = 1.0/(float)av_width;

      lv_at++;
    }
  }

}

/**
* Project pixels into R3 using camera parameters
*/

int PVMBuild::projectWithCameraParameters (){
	
	GtsPoint pc; 					// point in image plane co-ordinates 
	GtsPoint distortion; 	// where distortion predicts the point should be
	
	double len2; 					// squared length of 2D position vector
	
		
	if(intrinsic_matrix == NULL){
		cerr << "error: calibration information not available for projection\n";
		return 1;
	}
	
	// Allocate memory for 3D points and normals...
	
  ap_x = (float*)malloc(av_npoints*sizeof(float));
  ap_y = (float*)malloc(av_npoints*sizeof(float));
  ap_z = (float*)malloc(av_npoints*sizeof(float));
  ap_nx = (float*)malloc(av_npoints*sizeof(float));
  ap_ny = (float*)malloc(av_npoints*sizeof(float));
  ap_nz = (float*)malloc(av_npoints*sizeof(float));
  ap_r = (float*)malloc(av_npoints*sizeof(float));
  
	if (!(ap_x && ap_y && ap_z && ap_nx && ap_nz && ap_r))
	{
		cerr << "error: not enough memory for point allocation\n";
		return 1;
	}	
	
	// contruct the inverse image transform
	
	GtsMatrix* invT = gts_matrix3_inverse(intrinsic_matrix);
	
	//construct pose matrix
	
	GtsMatrix* P = constructPoseMatrix();
	
  // Iterate accross sample grid and project...  
  
  unsigned long lv_at = 0;
  for (long lv_y=1; lv_y <= av_height; lv_y++)
  {
    for (long lv_x=1; lv_x <= av_width; lv_x++)
    {
      // set up point from data simply arrayed in unit space on x/y plane

			pc.x = (double)lv_x;
			pc.y = (double)lv_y;
      pc.z = 0;
			
			// inverse transform to image place co-ordinates
			
			gts_point_transform(&pc, invT); 
			
			// calculate distortion and subtract from point to undistort
			
			len2 = pc.x*pc.x + pc.y*pc.y;
			distortion.x = pc.x*(distortion_parameters.r1*len2 + distortion_parameters.r2*len2*len2);
			distortion.y = pc.y*(distortion_parameters.r1*len2 + distortion_parameters.r2*len2*len2);
			distortion.x += 2*distortion_parameters.t1*pc.x*pc.y+distortion_parameters.t2*(len2+2*pc.x*pc.x);
			distortion.y += 2*distortion_parameters.t2*pc.x*pc.y+distortion_parameters.t1*(len2+2*pc.y*pc.y);
			
			pc.x -= distortion.x;
			pc.y -= distortion.y;
			
			// project point on image plane to vector in camera co-ordinates 
			
			pc.x = pc.x * ap_depth[lv_at];
			pc.y = pc.y * ap_depth[lv_at];
			pc.z = ap_depth[lv_at];
			
			gts_point_transform(&pc, P);
			
			// store resulting point
			
			ap_x[lv_at] = pc.x;   
      ap_y[lv_at] = pc.y;  
      ap_z[lv_at] = pc.z;
			
			// normals and radius unchanged from orthogonal version - tpb.
			
      // Normals then simple up along z axis... 

      ap_nx[lv_at] = 0;
      ap_ny[lv_at] = 0;
      ap_nz[lv_at] = -1;

      // Radius is mean pixel size in unit space...

      ap_r[lv_at] = 1.0/(float)av_width;

      lv_at++;
    }
  }
	
	return 0;
	
}

/** 
* set a flag such that blue background is not output by the output routines
*/ 
void
PVMBuild::blueBackgroundSubtraction(){
	backgroundSubtraction = TRUE;
}

/**
* set a flag such that the subsampling used in the output is axis aligned and moire
* type effects in the X/Y spacing of the output is avoided. 
*/

void
PVMBuild::axisAlignedSampling () {
	axisAlignedOutput = TRUE;
}


/**
* given a colour in R G B space returns TRUE if it is part of the specified
*  background
*/
bool 
PVMBuild::isPartOfBackGround(int r, int g, int b){
	
	float blue = 4.189; 		// Usual location of pure blue in radians (from red at 0 , back 
													// to red at  6.283).
	float tolerance = 0.1; 	// i.e. 10% either side of this
	
	float from = blue * (1.0-tolerance); 	// + check if under zero...
	float to = blue * (1.0+tolerance); 		// + check if greater than 2pi...

	RGBType RGBvalue;
	
	RGBvalue.R = r / 255.0;
	RGBvalue.G = g / 255.0;
	RGBvalue.B = b / 255.0;
	
	HSVType HSVvalue = RGB_to_HSV(RGBvalue);
	
	// now correct the H value, as the routine returns H=(0->6),
	// so the value in radians above works correctly 
	
	HSVvalue.H = (HSVvalue.H / 6) * (2 * PI);

	if ((HSVvalue.H >=from) && (HSVvalue.H<=to))
	{
    // Part of mask => return TRUE
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
* Write data to PVM file. 
*/

int
PVMBuild::writePVM (char* pp_file, long pv_res)
{
  int axis_aligned_tmp;
  cout << "Writing PVM file..." << endl;
  ap_out.newOutput(pp_file,"pvmbuild",1); // Only one object cloud
	
  // Go through all the data and write in sequence...

  unsigned long lv_pts = (unsigned long)av_npoints/pv_res;
  ap_out.newCloud(lv_pts,PVM_POSITION|PVM_RGB);

  unsigned long lv_at, lv_pt;
  for (lv_at=0, lv_pt=0; lv_at<av_npoints; lv_at+=pv_res, lv_pt++)
  {
    if (lv_pt >= lv_pts) break;
/*
    if (axisAlignedOutput){
      axis_aligned_tmp = (int) fmod((double) lv_at, (double) av_width);
      if ((lv_at != 0) && (axis_aligned_tmp != 0) && (axis_aligned_tmp < pv_res)){
        lv_at -= axis_aligned_tmp;
      } 

      if(fmod((double) lv_at, (double) av_width) == 0)
      {
        lv_at += (pv_res * av_width);
      }
    }

    if(!(backgroundSubtraction && 
      isPartOfBackGround((unsigned int) ap_red[lv_at], (unsigned int) ap_green[lv_at], (unsigned int) ap_blue[lv_at])))
    {
*/
      if (ap_out.newPoint()!=PVModelFile::OK) cout << "Huh!" << endl;  
      
      if (ap_out.newVector(PVM_POSITION,ap_x[lv_at],ap_y[lv_at],ap_z[lv_at])!=PVModelFile::OK
      || ap_out.newVector(PVM_RGB,(float)(ap_red[lv_at]/255.0),(float)(ap_green[lv_at]/255.0),(float)(ap_blue[lv_at]/255.0))!=PVModelFile::OK)
        cout << "Errk!" << endl;
      
      if (ap_out.endPoint()!=PVModelFile::OK) cout << "Darn!" << endl; 
  } 
cout << lv_pt << " != " << lv_pts << endl;
  
  if (ap_out.endCloud()!=PVModelFile::OK) cout << "Urgh!" << endl;
  if( ap_out.endOutput()!=PVModelFile::OK) cout << "Argh!" << endl;

  return 1;
}

/**
* Write data to XYZ RGB file. 
*/

int
PVMBuild::writeXYZRGB (char* pp_file, long pv_res)
{
	int axis_aligned_tmp;
	int height_counter;
	
	// open file
	
	/* fstream pl_xyzrgb("test.txt");
	
	if (!pl_xyzrgb.is_open())
  {
		cerr << "Can't open xyzrgb file" << endl;
    return 0;
  } */
	
	cout << setprecision(OUTPUT_PRECISION);
	cout << setiosflags(ios::fixed);
	
	// Go through all the data and write in sequence...
  
  for (unsigned long lv_at=0; lv_at < av_npoints; lv_at+=pv_res)
  {
		
		// perform axis aligned sub-sampling if required so that the samples
		// are equally spaced in the X/Y grid by step sizes of the specified sample
		// rate
		
		if (axisAlignedOutput){
			axis_aligned_tmp = (int) fmod((double) lv_at, (double) av_width);
			if ((lv_at != 0) && (axis_aligned_tmp != 0) && (axis_aligned_tmp < pv_res)){
				lv_at -= axis_aligned_tmp;
			} 
			
			if(fmod((double) lv_at, (double) av_width) == 0)
			{
				lv_at += (pv_res * av_height);
			}
			
		}
		
		if(!(backgroundSubtraction && 
				 isPartOfBackGround((unsigned int) ap_red[lv_at], (unsigned int) ap_green[lv_at],
		                        (unsigned int) ap_blue[lv_at])))
		{
			cout << ap_x[lv_at] << " " << ap_y[lv_at] << " " << ap_z[lv_at] << " ";
			cout << (unsigned int) ap_red[lv_at] << " " << (unsigned int) ap_green[lv_at]
					 << " " << (unsigned int) ap_blue[lv_at] << "\n";
		}
	}
	
	// pl_xyzrgb.close();
	
	return 1;
};

/////////////////////////////////////////////////////////

/* Entry */

int 
main(int pv_argc, char** pp_argv)
{
  char* lp_dfile = NULL;
  char* lp_cfile = NULL;
  char* lp_ofile = NULL;
  char* lp_mfile = NULL;
  char* lp_vfile = NULL;
	char* lp_xfile = NULL;
  long  lv_sample = 10;;

	bool useOrthogonalProjection = FALSE;
	bool useMaskOnBlue = FALSE;
	bool useAxisAlignedSampling = FALSE;
	
  // Check arguments...

  for (int lv_a=1; lv_a<pv_argc; lv_a+=2) {
    char lv_flag = '*';
    if (strlen(pp_argv[lv_a])>1)
    {
      if (pp_argv[lv_a][0]=='-') {
        //if (lv_a+1<pv_argc)
          lv_flag = pp_argv[lv_a][1];
        }
    }
    switch(lv_flag) {
      case 'o': 
        lp_ofile = pp_argv[lv_a+1];
        break;
			case 'O': 
        useOrthogonalProjection = TRUE;
			  lv_a--;
                        break;
			case 'M': 
        useMaskOnBlue = TRUE;
			  lv_a--;
                          break;
        break;
      case 'd': 
        lp_dfile = pp_argv[lv_a+1];
        break;
      case 'c': 
        lp_cfile = pp_argv[lv_a+1];
        break;
      case 'm': 
        lp_mfile = pp_argv[lv_a+1];
        break;
      case 'v': 
        lp_vfile = pp_argv[lv_a+1];
        break;
      case 's': 
        lv_sample = atoi(pp_argv[lv_a+1]);
        break;
			case 'A': 
        lv_sample = atoi(pp_argv[lv_a+1]);
				useAxisAlignedSampling = TRUE;
        break;
			case 'x': 
        lp_xfile = pp_argv[lv_a+1];
        break;
      case '*':
        cerr << "Unrecognised command: " << pp_argv[lv_a] << endl;
        cerr << "Try -help form more information." << endl;
        return 0;
      case 'h': default: 
        cerr << endl;
        cerr << "                          PointViewerModel Builder" << endl;
        cerr << " Usage: pvmbuild [<option + data]+" << endl;
        cerr << "   Where available options are:" << endl << endl;
        cerr << "   -d|pth <depth.tif>    floating point depthmap" << endl;
        cerr << "   -a|sci <point.xyz>    text position vectors" << endl;
        cerr << "   -o|utp <output.pvm>   designated output file" << endl;
			  cerr << "   -O|rthogonal          use orthogonal projection" << endl;
        cerr << "   -c|olr <color.tif>    3 channel rgb 0-255" << endl;
        cerr << "   -m|ask <mask.tif>     1 channel gray 0-255" << endl;
			  cerr << "   -M|askonblue          mask out BLUE background" << endl;
        cerr << "   -s|amp <s>            sampling of every s point" << endl;
			  cerr << "   -A|samp <s>           axis aligned sampling of every s point" << endl;	
			  cerr << "                         to avoid moire patterns in XYZ output" << endl;
        cerr << "   -p|roj <m11,,..,m33>  projection matrix" << endl;
        cerr << "   -l|ens <r1,r2,t1,t2>  lens distortion" << endl;
        cerr << "   -r|otq <x,y,z,theta>  rotation quaternion" << endl;
        cerr << "   -t|ran <x,y,z>        translation vector" << endl;
        cerr << "   -j|oin <*.pvm>        multiple files to combine" << endl;
        cerr << "   -v|erb <*.txt>        verbatim vector output" << endl;
			  cerr << "   -x|yzrgb <*.txt>      output ASCII xyz rgb file" << endl;
        cerr << "   -h|elp                this message" << endl << endl;;
        cerr << " e.g. pvmbuild -o test.pvm -d face.tif -c img.tif" << endl << endl; 
        return 0;
    }
  }
  if (lp_dfile==NULL) {
    cerr << "Must at least specify a depth file with '-d'!" << endl;
    return 0;
  }
  if (lp_ofile==NULL && lp_vfile==NULL && lp_xfile==NULL) {
    cerr << "Must specify at least one output file with '-o', -x or '-v'!" << endl;
    return 0;
  }
  
  // Create a PVMBuild class to bring it all together...

  PVMBuild lv_data;

  // Load up depth...

  if (!lv_data.readDepthTIFF(lp_dfile))
  {
    cerr << "Can't read expected depth file: " << lp_dfile << endl;
    return 0;
  }
 
  // Load up colour...
  
  if (!lv_data.readColourTIFF(lp_cfile))
  {
    cerr << "Can't read expected colour file: " << lp_cfile << endl;
    return 0;
  }
  
  // Convert data into R3 space and calc normals...

	if (useOrthogonalProjection){
		lv_data.projectOrthogonal();
	} else {
		if (lv_data.projectWithCameraParameters() == 1){
				cerr << "Aborting." << endl;
				exit(1);
		}
	}
	
	// Mask out blue background if requested
	
	if(useMaskOnBlue){
		lv_data.blueBackgroundSubtraction();
	}
	
	// use axis aligned sampling if requested
	
	if(useAxisAlignedSampling){
		lv_data.axisAlignedSampling();
	}
	
	// output data in specified format
	
	if (lp_ofile != NULL){
	
		 // Output data in PVM file
		
		if (!lv_data.writePVM(lp_ofile,lv_sample))
		{
			cerr << "Cant' write data to PVM file: " << lp_ofile << endl;
			return 0;
		}
	} else if (lp_xfile != NULL){
		
		// output data in XYZ format
		
		if (!lv_data.writeXYZRGB(lp_xfile,lv_sample))
		{
			cerr << "Cant' write data to XYZ RGB file: " << lp_xfile << endl;
			return 0;
		}
		
	}
  // Exit OK... (which is 0, 1 is error!)

  return 0;
}
