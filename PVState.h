/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#ifndef __PV_STATE__
#define __PV_STATE__

#include <SDL.h>

class PVState
{
public:		
	// TODO: really need a look up on a union by name
	// so that "set" and "get" methods (depending on type) can work when typed in

	// Window attributes

	bool	window_fullscreen;
	int		window_width;
	int		window_height;

	// Mouse motion and mode
	
	int		mouse_newx;
	int		mouse_newy;
	int		mouse_lastx;
	int		mouse_lasty;
	enum {
		IDLE = 0,
		ROTATE = 1,
		ZOOM = 2,
		SELECT = 3
	};
	int     mouse_mode;

	// Camera propeties

	bool   camera_perspective; // otherwise orthographic
	double camera_rscale;
	double camera_zscale;
	float  camera_rotation [4][4];
	double camera_zoom;
	float  camera_fov;
  float  camera_translationx;
  float  camera_translationy;
  float  camera_translationz;

	// Display properties

	double display_near;
	double display_far;
	bool   display_axis;
        float  display_pointsize;
        enum {
          SPATIAL = 1,
          TEMPORAL = 2
        };
        int   display_mode;

        int key_stop;
        int key_toggleaxis;
        int key_incpoint;
        int key_decpoint;
        int key_stepnext;
        int key_stepback;
        int key_togglemode;
        int key_zoomin;
        int key_zoomout;
        int key_centerdata;
        int key_centerclear;
        int key_inctransz;
        int key_dectransz;
        int key_inctransx;
        int key_dectransx;
        int key_inctransy;
        int key_dectransy;

	//idle_on = false;
	//show_normals = false;
	//play_on = false;
	//show_boundbox = false;
	//point_size
	//show_ground
	// show_mode = SPATIAL.

	//char		key_axis; // TODO: enable keys to be re-configured

private:

	// Event flags...

	bool _reshape;
	bool _refresh;

public:

	// Constructor & Destructor

	PVState();

	virtual ~PVState();

	// Events..

	inline void reshape() { _reshape = true; }
	inline bool doReshape() { if (_reshape) { _reshape = false; return true; } else return false; }
	inline void refresh() { _refresh = true; }
	inline bool doRefresh() { if (_refresh) { _refresh = false; return true; } else return false; }

};

#endif

