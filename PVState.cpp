/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVState.h"

PVState::PVState()
{

	window_fullscreen = false;
	window_width=640; // Also equiv to resolution
	window_height=480;

	mouse_newx = 0;
	mouse_newy = 0;
	mouse_lastx = 0;
	mouse_lasty = 0;
	mouse_mode = IDLE;

	camera_rscale = 200.0;
	camera_zscale = 0.05;
	camera_zoom = 0.5;
	camera_fov = 45.0;
	camera_rotation[0][0] = 1.0; // Identity matrix
	camera_rotation[0][1] = 0.0;
	camera_rotation[0][2] = 0.0;
	camera_rotation[0][3] = 0.0;
	camera_rotation[1][0] = 0.0;
	camera_rotation[1][1] = 1.0;
	camera_rotation[1][2] = 0.0;
	camera_rotation[1][3] = 0.0;
	camera_rotation[2][0] = 0.0;
	camera_rotation[2][1] = 0.0;
	camera_rotation[2][2] = 1.0;
	camera_rotation[2][3] = 0.0;
	camera_rotation[3][0] = 0.0;
	camera_rotation[3][1] = 0.0;
	camera_rotation[3][2] = 0.0;
	camera_rotation[3][3] = 1.0;
  camera_translationx = 0.0; // Zero trans at start
  camera_translationy = 0.0;
  camera_translationz = 0.0;

	display_near = 2.0;
	display_far = 2.0;
	display_axis = false;
        display_pointsize = 1.0;
        display_mode = SPATIAL;

        key_stop        = SDLK_ESCAPE;
        key_toggleaxis  = SDLK_a;
        key_incpoint    = SDLK_z;
        key_decpoint    = SDLK_x;
        key_stepnext    = SDLK_n;
        key_stepback    = SDLK_b;
        key_togglemode  = SDLK_m;
        key_zoomin      = SDLK_COMMA;
        key_zoomout     = SDLK_PERIOD;
        key_centerdata  = SDLK_c;
        key_centerclear = SDLK_HOME;
        key_inctransz   = SDLK_UP;
        key_dectransz   = SDLK_DOWN;
        key_inctransx   = SDLK_LEFT;
        key_dectransx   = SDLK_RIGHT;
        key_inctransy   = SDLK_PAGEUP;
        key_dectransy   = SDLK_PAGEDOWN;
}

PVState::~PVState()
{

}
