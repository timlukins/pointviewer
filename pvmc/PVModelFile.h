#ifndef __PV_MODEL_FILE__
#define __PV_MODEL_FILE__

#include <cstdio>
#include <cstring>
#include <cmath>
#ifndef _WIN32
using namespace std; // Window$ VC++ usually barfs here.
#endif


/**
* PointViewer Model File Class.
* The simplest, minimal, binary file format possible to
* store as many points and their properties as possible!
*
* The basic structure is thus:
*
*             Model
*           1   |
*               |
*           n   |
*             Cloud [i.e a pointset or frame of data]
*           1   |
*               | 
*           p   |
*             Point
*           1   |
*               |
*           32  |   [16 standard and 16 user reserved properties]
*             Vector
*
* NOTE: the number of vectors per point must be the same throughout the cloud.
*/

class PVModelFile
{
public:

  //////////////////////////////////////////////////////
  // PVM FORMAT DEFINITION 
  //////////////////////////////////////////////////////

  #define PVM_VERSION 1.0

  // TODO: big/little endian support!

  /**
  * A Header simply specifies a brief text description,
  * a version and the number of clouds it contains.
  * The first cloud will begin imediately after.
  */

  struct PVModelFileHeader {
    char                description[256];
    float               version;
    long                clouds;
  };

  /**
  * A Cloud begins with the number of points
  * followed by specifying the data vectors 
  * for each point.
  *
  * A number of masks for the vectors defined for
  * each point are defined (up to a limit of
  * 24 max for any given point). 
  * Stored in 4 byte long (last byte unused).
  *
  * These vectors MUST OCCUR in the same order
  * as they are listed, across all points in the cloud!
  *
  * points*vectors Vector structures will then
  * occur immediately in sequence after this. 
  *
  * The definitions of these vectors are:  
  */

  typedef unsigned long PVModelFileMask;

  // Definitions 0-11          reserved for standard

  #define PVM_NONE              0
  #define PVM_POSITION          1
  #define PVM_RGB               2
  #define PVM_NORMAL            4
  #define PVM_TANGENT           8
  #define PVM_VELOCITY          16
  #define PVM_ACCELERATION      32 
  #define PVM_JERK              64 
  #define PVM_RADIUS            128 
  #define PVM_SPECULARITY       256 
  #define PVM_RESERVED1         512
  #define PVM_RESERVED2         1024
  #define PVM_RESERVED3         2048
  
  // Definitions 12-24        user-defined  

  #define PVM_USER12            4096
  #define PVM_USER13            8192            
  #define PVM_USER14            16384 
  #define PVM_USER15            32768 
  #define PVM_USER16            65536 
  #define PVM_USER17            131072 
  #define PVM_USER18            262144 
  #define PVM_USER19            524288 
  #define PVM_USER20            1048576 
  #define PVM_USER21            2097152 
  #define PVM_USER22            4194304
  #define PVM_USER23            8388608 
  #define PVM_USER24            16777216             

  struct PVModelFileCloud {
    unsigned long       points;
    int	         	vectors;
    PVModelFileMask     mask; 
  };

  /**
  * Definition of point property vector/tuple.
  *
  * Note: this is not a homogeneous vector (to save space).
  * And, floats are sufficient for the level of accuracy required.
  */

  struct PVModelFileVector {
    float               x;
    float               y;
    float               z;
  };
 
  //////////////////////////////////////////////////////

private:

  // Working instances of data...

  PVModelFileHeader     _header;
  PVModelFileCloud      _cloud;
  PVModelFileMask       _mask;
  PVModelFileVector     _vector;

  // Handle to actual file...

  FILE*                 _file;

  // Record of current...

  long                  _atcloud;
  unsigned long         _atpoint;
  int                   _atvector;

  // Current mode of operation...

  enum { UNDEF, OUTPUT, INPUT };
  int                   _mode;

public:

  // Simple interface methods...
  
  // Return codes...

  enum {
    OK = 1,
    OPEN_FILE,          // Error performing file access (name?, mode?).
    FAIL_HEADER,        // Error writing/reading file header.
    FAIL_CLOUD,         // Error writing/reading cloud to/from file.
    FAIL_POINT,         // Error writing/reading point to/from file.
    FAIL_VECTOR,        // Error writing/reading vector to/from file.
    WRONG_MODE,         // Wrong input/output mode for function call.
    WRONG_TYPE,         // Wrong vector mask type for previous point definition.
    WRONG_ORDER,        // Wrong vector order for previous point definition.
    NUMBER_CLOUDS,      // Number of clouds created differs from number defined.
    NUMBER_POINTS,      // Number of points created differes from number defined.
    NUMBER_VECTORS,     // Number of vectors created differs from number defined.
    END_CLOUDS,         // End of all clouds when iterating.
    END_POINTS,         // End of points in current cloud when iterating.
    END_VECTORS,        // End of vectors in current point when iterating.      
  } ErrorCode;

  /**
  * Default constructor.
  */

  PVModelFile();

  /**
  * Destructor.
  */

  virtual ~PVModelFile();

  /**
  * Create new output. 
  */

  int newOutput(char*,char*,long);

  /**
  * End new output.
  */

  int endOutput();

  /**
  * Open existing input. 
  */

  int openInput(const char*);

  /**
  * Close existing input.
  */

  int closeInput();

  /**
  * Create a new cloud.
  */
 
  int newCloud(unsigned long, PVModelFileMask);

  /**
  * End a new cloud.
  */

  int endCloud();

  /**
  * Create a new point.
  */

  int newPoint(); 

  /** 
  * End a new point.
  */

  int endPoint();

  /**
  * Create a new vector.
  */

  int newVector(PVModelFileMask, float, float, float);

  /**
  * Next cloud iterator.
  */

  int nextCloud();

  /**
  * Next point iterator.
  */

  int nextPoint();

  /**
  * Next vector iterator.
  */

  int nextVector();

  /**
  * TODO: Read ASCII
  */

  /**
  * Get current header description.
  */

  inline char* description() { return _header.description; }

  /**
  * Get current header version.
  */

  inline float version() { return _header.version; }

  /**
  * Get current header number of clouds.
  */

  inline long clouds() { return _header.clouds; }

	/**
	* Get current cloud number of points.
	*/

	inline long points() { return _cloud.points; }

	/**
	* Get current point number of vectors.
	*/

	inline int vectors() { return _cloud.vectors; }

  /**
  * Get current vector mask.
  */

  inline PVModelFileMask mask() { return _mask; }

  /**
  * Get current vector values.
  */

  inline float x() { return _vector.x; }
  inline float y() { return _vector.y; }
  inline float z() { return _vector.z; }

};


#endif /* _PV_MODEL_FILE_  */
