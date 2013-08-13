/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVControl.h"

/**
 * Constructor.
 */

PVControl::PVControl(PVState* pp_state,PVModel* pp_model) : PVThread()
{
  ap_state = pp_state;
  ap_model = pp_model;
}


/**
 * Destructor.
 */

PVControl::~PVControl ()
{
  SDL_Quit();
}


/**
 * Init.
 */

void
PVControl::init (int pv_argc, char **pp_argv)
throw (PVException &)
{

  // Create default commands...

  ap_commands.push_back(new PVSet(ap_state));

  // Parse and load...

  for (int lv_a=1; lv_a<pv_argc; lv_a++)
  {
    string lv_arg = pp_argv[lv_a];
    int lv_atopt = lv_arg.find_last_of('-')+1;
    string lv_opt = lv_arg.substr(lv_atopt);
    int lv_atsuf = lv_arg.find_last_of('.')+1;
    string lv_suffix = lv_arg.substr(lv_atsuf);

    if (lv_atopt!=0)                              // Anything with a dash is an option - which may be followed by a value...
    {
      //cout << "Option: " << lv_opt << endl;
      if (lv_opt=="full" || lv_opt=="f" || lv_opt=="fullscreen")
        ap_state->window_fullscreen=true;

      if (lv_opt=="width" || lv_opt=="w")
        ap_state->window_width=atoi(pp_argv[++lv_a]);

      if (lv_opt=="height" || lv_opt=="h")
        ap_state->window_height=atoi(pp_argv[++lv_a]);
    }
    else if (lv_atsuf!=0)                         // We assume anything else with a dot in it is a file to try and load...
    {
      //cout << "File: " << lv_suffix << endl;
      try
      {

        if (lv_suffix=="pvm" || lv_suffix=="PVM")
        {
          PVLoadPVM lv_loadpvm;
          lv_loadpvm.perform(ap_model,lv_arg.c_str());
        }

        if (lv_suffix=="xyz" || lv_suffix=="XYZ")
        {
          PVLoadAscii lv_loadxyz;
          lv_loadxyz.perform (ap_model,lv_arg.c_str());
        }

      }
      catch (PVException & lv_prob)
      {
        throw lv_prob;
      }
    }
    else
    {
      throw PVException("PVControl::parse","Unrecognised command line argument!");
    }

  }

}


/**
 * Ready.
 */

void
PVControl::ready()
throw (PVException &)
{
  // Try and actually create screen...

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    throw PVException ("PVControl::show", "Couldn't init SDL!");
  }

  Uint32 lv_flags = SDL_OPENGL;
  if ( ap_state->window_fullscreen)
  {
    lv_flags |= SDL_FULLSCREEN;
  }
  else
  {
    lv_flags |= SDL_RESIZABLE;
    SDL_WM_SetCaption(PV_TITLE,PV_TITLE);
  }
  SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
  if ( (ap_screen=SDL_SetVideoMode(ap_state->window_width, ap_state->window_height, 16, lv_flags)) == NULL )
  {
    SDL_Quit();
    throw PVException ("PVControl::show", "Couldn't set video mode!");
  }

  // Initial window creation = a resize...

  ap_state->reshape();
  ap_state->refresh();

  // Define key repeat rate...

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

}


/**
 * Run
 */

void
PVControl::run ()
{
  // Main event loop...

  SDL_Event lv_event;

  try
  {

    while ( SDL_PollEvent(&lv_event) )
    {

      switch(lv_event.type)
      {

        // Handle window creation/resize...
        case SDL_VIDEORESIZE:
        {
          ap_state->window_width = lv_event.resize.w;
          ap_state->window_height = lv_event.resize.h;
          // TODO: full screen switch
          SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 1 );
          if ( (ap_screen=SDL_SetVideoMode(lv_event.resize.w, lv_event.resize.h, 16, SDL_OPENGL|SDL_RESIZABLE)) == NULL )
          {
            SDL_Quit();
            throw PVException ("PVControl::show", "Couldn't set video mode!");
          }
          ap_state->reshape();
          ap_state->refresh();
          break;
        }

        case SDL_VIDEOEXPOSE:
        {
          ap_state->refresh();
          break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
          ap_state->mouse_newx = lv_event.button.x;
          ap_state->mouse_newy = lv_event.button.y;
                                                  // Left button (dray)
          if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
            ap_state->mouse_mode=PVState::ROTATE;

          //if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2)) // Middle button (drag)

                                                  // Right button (drag)
          if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))
            ap_state->mouse_mode=PVState::ZOOM;
          break;
        }

        case SDL_MOUSEMOTION:
        {
          if (ap_state->mouse_mode!=PVState::IDLE)// i.e. a mouse button is being held
          {
            ap_state->mouse_lastx = ap_state->mouse_newx;
            ap_state->mouse_lasty = ap_state->mouse_newy;
            ap_state->mouse_newx = lv_event.button.x;
            ap_state->mouse_newy = lv_event.button.y;
            ap_state->refresh();
          }
          break;
        }

        case SDL_MOUSEBUTTONUP:
        {
          ap_state->mouse_mode = PVState::IDLE;
          break;
        }

        case SDL_KEYDOWN:
        {
          // TODO: use pvCommand & if in terminal mode
          Uint8* lv_keys = SDL_GetKeyState(NULL);
          if ( lv_keys[SDLK_ESCAPE] ) this->stop();
          if ( lv_keys[SDLK_a] ) ap_state->display_axis=!ap_state->display_axis;
          if ( lv_keys[SDLK_z] ) ap_state->display_pointsize-=1.0;
          if ( lv_keys[SDLK_x] ) ap_state->display_pointsize+=1.0;
          if ( lv_keys[SDLK_n] ) ap_model->stepForwards();
          if ( lv_keys[SDLK_b] ) ap_model->stepBackwards();
          if ( lv_keys[SDLK_m] )
          {
            if (ap_state->display_mode == PVState::SPATIAL) ap_state->display_mode = PVState::TEMPORAL;
            else ap_state->display_mode = PVState::SPATIAL;
          }
          if ( lv_keys[SDLK_COMMA] )  ap_state->camera_zoom+=ap_state->camera_zscale;
          if ( lv_keys[SDLK_PERIOD] ) ap_state->camera_zoom-=ap_state->camera_zscale;

          if ( lv_keys[SDLK_c] ) { PVTranslateCenter lv_center(ap_state,ap_model); lv_center.perform(); }
          if ( lv_keys[SDLK_s] ) { PVScaleNormalise lv_norm; lv_norm.perform(ap_model); }
          if (lv_keys[SDLK_HOME]) ap_state->camera_translationx = ap_state->camera_translationy = ap_state->camera_translationz = 0.0;
          if (lv_keys[SDLK_UP]) ap_state->camera_translationz+=(float)0.1;
          if (lv_keys[SDLK_DOWN]) ap_state->camera_translationz-=(float)0.1;
          if (lv_keys[SDLK_LEFT]) ap_state->camera_translationx+=(float)0.1;
          if (lv_keys[SDLK_RIGHT]) ap_state->camera_translationx-=(float)0.1;
          if (lv_keys[SDLK_PAGEUP]) ap_state->camera_translationy+=(float)0.1;
          if (lv_keys[SDLK_PAGEDOWN]) ap_state->camera_translationy-=(float)0.1;
          
          ap_state->refresh();

          break;
        }

        case SDL_KEYUP:
        {
          break;
        }

        // Window closed...
        case SDL_QUIT:
        {
          this->stop();
          break;
        }
      }

      // IF play && fps/= amount
      // state->rotatation++;
      // model->current_cloud++;
    }
  }
  catch (PVException&)
  {
  }

  // Crucial wait so that thread does not consume 100% TODO: ok?

  SDL_Delay(20);

}
