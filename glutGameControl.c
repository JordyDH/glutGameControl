////////////////////////////////////////////////////////////////////
//
//	glutGameController by Jordy de Hoon
//	MIT LICENSE , goto www.github.com/JordyDH/glutGameControl
//
////////////////////////////////////////////////////////////////////
#define  GLUT_GAMEC_VERSION "0.1"
#define  GLUTGAME_DEBUG_INFO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "glutGameControl.h"
////////////////// CONTROLS //////////////////
#define GLUTGAME_CONTROL_FORW	'z'	//Moves the camera forwards	--Bit 0 in control reg
#define GLUTGAME_CONTROL_BACK	's'	//Moves the camera backwards	--Bit 1 in control reg
#define GLUTGAME_CONTROL_LEFT	'q'	//Moves the camera left		--Bit 2 in control reg
#define GLUTGAME_CONTROL_RIGHT	'd'	//Moves the camera right	--Bit 3 in control reg
#define GLUTGAME_CONTROL_UP	'a'	//Moves the camera up		--Bit 4 in control reg
#define GLUTGAME_CONTROL_DOWN	'e'	//Moves the camera down		--Bit 5 in control reg
#define GLUTGAME_CONTROL_AXIS	'i'	//Renders the axis		--Bit 6 in control reg
#define GLUTGAME_CONTROL_USEREG
#define GLUTGAME_CONTROL_TIMER	1
uint64_t GLUTGAME_CONTROL_REG = 0;
////////////////// PLAYER MODEL //////////////////
#define GLUTGAME_PLAYER_HEIGHT		1.0	//Default player height, possition of the camera
#define GLUTGAME_PLAYER_BASESPEED	0.05	//Default step size
#define GLUTGAME_PLAYER_NEARSIGHT	0.01
#define GLUTGAME_PLAYER_FARSIGHT	100
////////////////// RENDER SETTINGS //////////////////
//#define GLUTGAME_RENDER_SHOWFPS			//Show the FPS in the terminal, overloads the terminal
#define GLUTGAME_RENDER_ONTIMER			//TRUE to use glutTimerFunc callback to render scene
#define GLUTGAME_RENDER_INTERVAL	10	//time in ms between screen renders
//#define GLUTGAME_RENDER_DUBBELBUFFER		//Enable dubbel buffering for render.
////////////////// FUNCTION POINTERS //////////////////
void (*RenderScene_fnc)();	//Callback function to render the scene
////////////////// LIB VARS //////////////////
double	rotation_lr = 0;		//
double	rotation_ud = 0;
double	xl = 0, yl = 0, zl = 0;
double	xr = 0, yr = 1, zr = 0;
double	xPos = 3, yPos = 0, zPos = 5;
int	mouse_state_left, mouse_state_right = 0;
int	mouse_x_old, mouse_y_old = 0;
double	speed_mul = 0.1;
double	framecounter = 0;	//Current setup , fps not supported
int 	render_needed = 0;
unsigned int systick = 0;
////////////////// LIB FUNCTION //////////////////

void glutGameMainLoop()
{
	#ifdef GLUTGAME_RENDER_ONTIMER
	glutTimerFunc(GLUTGAME_RENDER_INTERVAL, glutGameRender,systick);
	#endif
	//Start glut main loop
	glutMainLoop();
}

void glutGameRenderCamera()
{
	gluLookAt(xPos,yPos+GLUTGAME_PLAYER_HEIGHT,zPos, xPos+xl,yPos+GLUTGAME_PLAYER_HEIGHT+yl,zPos+zl, xr, yr, zr);
}

void glutGameInitCamera(double x, double y, double z)
{
	xPos = x; yPos = y; zPos = z;
	xl = 0; yl = 0; zl = 0;
	rotation_lr = 0;
	rotation_ud = 0;
}

void glutGameSetRenderScene(void (*fnc_p)())
{
	RenderScene_fnc = fnc_p;
}

void glutGameRenderScene()
{
	//glutGameGetFPS();
	(*RenderScene_fnc)();
	glutGameRenderLocalAxis();
	#ifdef GLUTGAME_RENDER_DUBBELBUFFER
		glFinish();
		glutSwapBuffers();
	#else
		glFlush();
	#endif
	glutGameGetFPS();
}

void glutGameRender(int systick_old)
{
	#ifdef GLUT_RENDER_ONTIMER
	if(systick!=systick_old)printf("[GLUTGAME][WARNING] : Callback older systick, skipped a render?\n");
	#endif
	systick += 1;
	if(render_needed)
	{
		glutGameRenderCamera();
		glutPostRedisplay();
		render_needed = 0;
	}
	#ifdef GLUTGAME_RENDER_ONTIMER
	glutTimerFunc(GLUTGAME_RENDER_INTERVAL,glutGameRender,systick);
	#endif
}

void glutGameRescale(GLint n_w, GLint n_h)
{
	GLdouble grens;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(60.0, (double)n_w/n_h,  GLUTGAME_PLAYER_NEARSIGHT, GLUTGAME_PLAYER_FARSIGHT);
	gluPerspective(60.0, 1.0,  GLUTGAME_PLAYER_NEARSIGHT, GLUTGAME_PLAYER_FARSIGHT);
	glViewport(0, 0, n_w, n_h);

}

double glutGameGetFPS()
{
	framecounter = (double)1000/(glutGet(GLUT_ELAPSED_TIME));
	#ifdef GLUTGAME_RENDER_SHOWFPS
	printf("[GLUTGAME][RENDER] FPS : %.2lf\n",framecounter);
	#endif
	return framecounter;
}

void glutGameMouseFunction(int button, int state, int x, int y)
{
        if((state == GLUT_DOWN)&&(button == GLUT_LEFT_BUTTON))
                mouse_state_left = 1;
        else
                mouse_state_left = 0;
	if((state == GLUT_DOWN)&&(button == GLUT_RIGHT_BUTTON))
                mouse_state_right = 1;
        else
                mouse_state_right = 0;
}

void glutGameMouseMove(int x, int y)
{
//	glutWarpPointer(0,0);
	int delta_x = 0, delta_y = 0;
	if(mouse_state_left)
	{
		//#Calc Delta's
		delta_x = x - mouse_x_old;
		delta_y = y - mouse_y_old;
		glutGameRotateCamera(((double)delta_x/100),((double)delta_y/100));
 		//glutPostRedisplay();
		render_needed = 1;
	}
	//#Save location in old value
	mouse_x_old = x;
	mouse_y_old = y;
}

void glutGameKeyboardInit()
{
	#ifdef GLUTGAME_CONTROL_USEREG
	glutIgnoreKeyRepeat(0x01);
	glutKeyboardFunc(glutGameKeyboardPressed);
	glutKeyboardUpFunc(glutGameKeyboardReleased);
	glutTimerFunc(GLUTGAME_CONTROL_TIMER,glutGameMoveCamera,0);
	#else
	glutKeyboardFunc(glutGameKeyboardPressed);
	#endif
}

void glutGameKeyboardPressed(int key, int x, int y)
{
	#ifdef GLUTGAME_DEBUG_INFO
	printf("[GLUTGAME][KEYBOARD] Key pressed :%c\n",key);
	#endif
	#ifdef GLUTGAME_CONTROL_USEREG
	switch(key)
	{
		case GLUTGAME_CONTROL_FORW:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 0);
			break;
		case GLUTGAME_CONTROL_BACK:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 1);
			break;
		case GLUTGAME_CONTROL_LEFT:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 2);
			break;
		case GLUTGAME_CONTROL_RIGHT:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 3);
			break;
		case GLUTGAME_CONTROL_UP:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 4);
			break;
		case GLUTGAME_CONTROL_DOWN:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG | (0x01 << 5);
			break;
		case GLUTGAME_CONTROL_AXIS:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG ^ (0x01 << 6);	//Toggle function
			break;
	}
	#else
	glutGameMoveCamera(key);
	#endif
}

void glutGameKeyboardReleased(int key, int x, int y)
{
	#ifdef GLUTGAME_DEBUG_INFO
	printf("[GLUTGAME][KEYBOARD] Key released :%c\n",key);
	#endif
	switch(key)
	{
		case GLUTGAME_CONTROL_FORW:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 0));
			break;
		case GLUTGAME_CONTROL_BACK:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 1));
			break;
		case GLUTGAME_CONTROL_LEFT:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 2));
			break;
		case GLUTGAME_CONTROL_RIGHT:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 3));
			break;
		case GLUTGAME_CONTROL_UP:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 4));
			break;
		case GLUTGAME_CONTROL_DOWN:
			GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 5));
			break;
		case GLUTGAME_CONTROL_AXIS:
			//GLUTGAME_CONTROL_REG = GLUTGAME_CONTROL_REG & (~(0x01 << 6));
			break;
	}
}

void glutGameMoveCamera(int key)
{
	render_needed = 1;
	#ifdef GLUTGAME_CONTROL_USEREG
		if(GLUTGAME_CONTROL_REG & (0x01 << 0))		//GLUTGAME_CONTROL_FORW
		{
			xPos += xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos += zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		if(GLUTGAME_CONTROL_REG & (0x01 << 1))		//GLUTGAME_CONTROL_BACKW
		{
			xPos -= xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos -= zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		if(GLUTGAME_CONTROL_REG & (0x01 << 2))		//GLUTGAME_CONTROL_LEFT
		{
			xPos += zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos -= xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		if(GLUTGAME_CONTROL_REG & (0x01 << 3))		//GLUTGAME_CONTROL_RIGHT
		{
			xPos -= zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos += xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		if(GLUTGAME_CONTROL_REG & (0x01 << 4))		//GLUTGAME_CONTROL_UP
		{
			yPos += GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		if(GLUTGAME_CONTROL_REG & (0x01 << 5))		//GLUTGAME_CONTROL_DOWN
		{
			yPos -= GLUTGAME_PLAYER_BASESPEED * speed_mul;
		}
		glutTimerFunc(1,glutGameMoveCamera,0);
	#else
	switch(key)
	{
		case GLUTGAME_CONTROL_LEFT :
			xPos += zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos -= xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		case GLUTGAME_CONTROL_RIGHT :
			xPos -= zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos += xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		case GLUTGAME_CONTROL_FORW :
			xPos += xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos += zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		case GLUTGAME_CONTROL_BACK :
			xPos -= xl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			zPos -= zl * GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		case GLUTGAME_CONTROL_UP :
			yPos += GLUTGAME_PLAYER_BASESPEED;
			break;
		case GLUTGAME_CONTROL_DOWN :
			yPos -= GLUTGAME_PLAYER_BASESPEED;
			break;
		default:
			render_needed = 0;
			break;
	}
	#endif
}

void glutGameRotateCamera(double dxa, double dza)
{
	rotation_lr += dxa;
	rotation_ud -= dza;
	xl = sin(rotation_lr) * cos(rotation_ud);
	zl = -cos(rotation_lr) * cos(rotation_ud);
	yl = sin(rotation_ud);
}

void glutGameRenderLocalAxis()
{
	//Draw the world axis in front of the camera
	double lengt = 0.05;
	double mod_x, mod_z;
	if(GLUTGAME_CONTROL_REG & (0x01 << 6))
	{
		mod_x = sin(rotation_lr)*0.20;
		mod_z = -cos(rotation_lr)*0.20;
		glLineWidth(3);
		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3d(xPos+xl+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl+mod_z);
			glVertex3d(xPos+xl+lengt+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl+mod_z);
			glColor3f(0,1,0);
			glVertex3d(xPos+xl+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl+mod_z);
			glVertex3d(xPos+xl+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT+lengt,zPos+zl+mod_z);
			glColor3f(0,0,1);
			glVertex3d(xPos+xl+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl+mod_z);
			glVertex3d(xPos+xl+mod_x,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl+lengt+mod_z);
		glEnd();
	}
}

void glutGameRenderOnScreenInfo()
{
	
}

