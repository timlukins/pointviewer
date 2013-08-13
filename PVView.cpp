/* * * * * * * * * * *
 . . . . . . . . . . .

 p o i n t v i e w e r
 . . . . . . . . . . .

 * * * * * * * * * * */

#include "PVView.h"

/**
Constructor.
*/

PVView::PVView (PVState* pp_state, PVModel* pp_model)
{
	ap_state = pp_state;
	ap_model = pp_model;
}

/**
Destructor.
*/

PVView::~PVView ()
{

}

/**
Init
*/

void
PVView::init()
{
  // OpenGL initialisations...

  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
  glClearDepth(1.0f);	
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glEnable (GL_NORMALIZE);
  glEnable (GL_POINT_SMOOTH);

}

/**
* Run
*/

void
PVView::run()
{
	// Check if any refresh to the view...

	if (ap_state->doRefresh())
	{
		// Do we have to resize first?

		if (ap_state->doReshape())
			reshape();

		// See if the mouse has changed our view...

		switch(ap_state->mouse_mode)
		{
			case PVState::IDLE:
			{
				break;
			}
			case PVState::ROTATE:
			{
				// From tutorial at: http://www.cse.ohio-state.edu/~crawfis/cis781/Slides/VirtualTrackball.html

				// Last point vector normal

				PVMath::Vector3 lv_last;
				lv_last.x = (2.0*ap_state->mouse_lastx - ap_state->window_width) / ap_state->window_width;
				lv_last.y = (ap_state->window_height - 2.0*ap_state->mouse_lasty) / ap_state->window_height;
				lv_last.z = 0.0;
				double lv_dl = lv_last.norm();
				lv_dl = (lv_dl<1.0) ? lv_dl : 1.0;
				lv_last.z = PVMath::sqroot(1.001 - PVMath::sqr(lv_dl));
				lv_last.normalise();

				// Current point vector normal

				PVMath::Vector3 lv_current;
				lv_current.x = (2.0*ap_state->mouse_newx - ap_state->window_width) / ap_state->window_width;
				lv_current.y = (ap_state->window_height - 2.0*ap_state->mouse_newy) / ap_state->window_height;
				lv_current.z = 0.0;
				double lv_dn = lv_current.norm();
				lv_dn = (lv_dn<1.0) ? lv_dn : 1.0;
				lv_current.z = PVMath::sqroot(1.001 - PVMath::sqr(lv_dn));
				lv_current.normalise();

				// Calculate direction & velocity

				PVMath::Vector3 lv_direction = lv_current - lv_last;
				double lv_vel = lv_direction.norm();
				if( lv_vel > 0.0001 )
				{
					PVMath::Vector3 lv_axis = lv_last * lv_current; // Cross product
				  double lv_angle = lv_vel * ap_state->camera_rscale;
					glLoadIdentity(); // Clear a GL matrix
					glRotatef(lv_angle, lv_axis.x, lv_axis.y, lv_axis.z); // Move it by calculated rotation
					glMultMatrixf((float*)ap_state->camera_rotation ); // Multiply it by last matrix
					glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)ap_state->camera_rotation ); // Store new matrix

				}
				break;
			}
			case PVState::ZOOM:
			{
				double lv_current = ap_state->mouse_newy;
				double lv_last = ap_state->mouse_lasty;
				if (lv_current>lv_last)
					ap_state->camera_zoom += ap_state->camera_zscale; // * abs(lv_current-lv_last)
				else
					ap_state->camera_zoom -= ap_state->camera_zscale;
			}
		}

		// Start OpenGL rendering...

		glPushMatrix();

		// Perform the view transforms...

		glLoadIdentity(); // Clear a GL matrix
                //glTranslatef(-ap_state->camera_translationx,-ap_state->camera_translationy,-ap_state->camera_translationz);
		glMultMatrixf((float*)ap_state->camera_rotation );	
		glScalef (ap_state->camera_zoom, ap_state->camera_zoom, ap_state->camera_zoom);

		// Modelling transforms/code


		glClearColor(0.0,0.0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth (1.0);	//TODO: another control
		glPointSize (ap_state->display_pointsize);
		
                // Draw axis if required at origin...
                
                if (ap_state->display_axis)
		{
                    glPushMatrix();
			glColor3f (1.0, 0.0, 0.0);
			glBegin (GL_LINES);
			glVertex3f (0, 0, 0);
			glVertex3f (1.0, 0, 0);
			glEnd ();
			glColor3f (0.0, 1.0, 0.0);
			glBegin (GL_LINES);
			glVertex3f (0, 0, 0);
			glVertex3f (0, 1.0, 0);
			glEnd ();
			glColor3f (0.0, 0.0, 1.0);
			glBegin (GL_LINES);
			glVertex3f (0, 0, 0);
			glVertex3f (0, 0, 1.0);
			glEnd ();
                  glPopMatrix();
		}


		// Finally, draw the actual points...
                
                glTranslatef(-ap_state->camera_translationx,-ap_state->camera_translationy,-ap_state->camera_translationz);

		try
		{
                PVCloud* lp_at;
      for (int lv_cl = 0; lv_cl < ap_model->av_clouds.size(); lv_cl++)
      {
        if (ap_state->display_mode==PVState::SPATIAL)
          lp_at = ap_model->av_clouds.at(lv_cl);
        else
          lp_at = ap_model->current();
			
				unsigned long lv_pt;
				glBegin (GL_POINTS);
				for (lv_pt = 0; lv_pt < lp_at->npoints; lv_pt++)
				{
					glColor3f ((*lp_at)[lv_pt]->r, (*lp_at)[lv_pt]->g, (*lp_at)[lv_pt]->b);
					glVertex3f ((*lp_at)[lv_pt]->x, (*lp_at)[lv_pt]->y,(*lp_at)[lv_pt]->z);
                                }
                                glEnd();
                                glBegin (GL_LINES);
				for (lv_pt = 0; lv_pt < lp_at->npoints; lv_pt++)
				{
                                        // See if we have normal information...
				    glColor3f (1.0,1.0,0.0);
                                    glVertex3d ((*lp_at)[lv_pt]->x, (*lp_at)[lv_pt]->y,(*lp_at)[lv_pt]->z);
                                    glVertex3d ((*lp_at)[lv_pt]->x + (*lp_at)[lv_pt]->nx,(*lp_at)[lv_pt]->y + (*lp_at)[lv_pt]->ny, (*lp_at)[lv_pt]->z + (*lp_at)[lv_pt]->nz);
				}
                                glEnd ();
				

                                if (ap_state->display_mode==PVState::TEMPORAL) break;

			}
		}
		catch (PVException &)
		{
		}
                
		// Up the stack...
		
		glPopMatrix ();

		// Swap buffers in...

		glFlush ();
		SDL_GL_SwapBuffers();

	}
}


/**
Reshape.
*/

void
PVView::reshape ()
{
	glMatrixMode (GL_PROJECTION); // TODO: depending on 
  glLoadIdentity ();
  glViewport(0, 0, ap_state->window_width, ap_state->window_height);
	double lv_aspect = (GLfloat) ap_state->window_width / (GLfloat) ap_state->window_height;
	double lv_ymax = ap_state->display_near * tan(ap_state->camera_fov * 0.5 * 0.01745329251994329);
	double lv_ymin = -lv_ymax;
	double lv_xmin= lv_ymin * lv_aspect;
	double lv_xmax = lv_ymax * lv_aspect;
	glFrustum(lv_xmin,lv_xmax,lv_ymin,lv_ymax,ap_state->display_near,ap_state->display_far);
	
	glMatrixMode(GL_MODELVIEW);
}

