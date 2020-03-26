////////////////////////////////////////////////////////////////////
//
//	glutGameController by Jordy de Hoon
//	MIT LICENSE , goto www.github.com/JordyDH/glutGameControl
//
////////////////////////////////////////////////////////////////////
#define  GLUT_GAMEC_VERSION "0.2"
//#define  GLUTGAME_DEBUG_INFO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "glutGameControl.h"
//////////////////////////////////// CONTROLS ////////////////////////////////////
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

//////////////////////////////////// PLAYER MODEL ////////////////////////////////////
#define GLUTGAME_PLAYER_HEIGHT		1.0	//Default player height, possition of the camera
#define GLUTGAME_PLAYER_BASESPEED	0.075	//Default step size
#define GLUTGAME_PLAYER_NEARSIGHT	0.01
#define GLUTGAME_PLAYER_FARSIGHT	100

//////////////////////////////////// RENDER SETTINGS ////////////////////////////////////
//#define GLUTGAME_RENDER_SHOWFPS		//Show the FPS in the terminal, overloads the terminal
#define GLUTGAME_RENDER_FAST			//Renders the scene as fast a possible.
#define GLUTGAME_RENDER_INTERVAL	5	//time in ms between screen renders, ONLY IN GLUTGAME_RENDER_ONTIMER
//#define GLUTGAME_RENDER_DUBBELBUFFER		//Enable dubbel buffering for render.
#define GLUTGAME_SYSTICK_INTERVAL	1000
//////////////////////////////////// FUNCTION POINTERS ////////////////////////////////////
void (*RenderScene_fnc)() = 0x00;	//Callback function to render the 3D scene
void (*RenderScene2D_fnc)() = 0x00;	//Callback function to render the 2D scene

//////////////////////////////////// LIB VARS ////////////////////////////////////
double	rotation_lr = 0;		//
double	rotation_ud = 0;
double	xl = 0, yl = 0, zl = 0;
double	xr = 0, yr = 1, zr = 0;
double	xPos = 3, yPos = 0, zPos = 5;
unsigned int screen_width, screen_height = 0;
int	mouse_state_left, mouse_state_right = 0;
int	mouse_x_old, mouse_y_old = 0;
double	speed_mul = 0.1;
double	framecounter = 0;	//Current setup , fps not supported
int 	render_needed = 0;
unsigned int systick = 0;

//////////////////////////////////// LIB FUNCTION ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ONLY CHANGE THE FUNCTION IF YOU KNOW WHAT YOU ARE DOING OR IF YOU JUST WANT TO EXPERIMENT
// SO HAVE A NICE EXPERIENCE, WHEN YOU HAVE PROBLEMS TO IMPLEMENT A FEATURE (NOT BUG ;) )
// OPEN A FEATURE REQUEST TICKET ON GITHUB

//////////////////////////////////// GLUTGAME CORE FUNCTIONS ////////////////////////////////////

/*
* Function: void glutGameInit()
* -----------------------------
* Initialise the glutGame enviroment with the options defines.
* returns nothing
*/
void glutGameInit()
{
	glutGameControlInit();
	glutDisplayFunc(glutGameRender);
	glutReshapeFunc(glutGameRescale);
	//glutIdleFunc(glutPostRedisplay);
	glutTimerFunc(0,glutGameIdle,0);	//glutIdleFunc laggs the machine
	glutTimerFunc(GLUTGAME_SYSTICK_INTERVAL,glutGameSystickService,0);

}

/*
* Function: void glutGameMainLoop()
* -----------------------------
* Last function to be called, this will start the internal main loop service.
* returns nothing
*/
void glutGameMainLoop()
{
	glutMainLoop();
}

/*
* Function: void glutGameMainIdle()
* -----------------------------
* The internal idle function, is called when there a no events triggerd
* returns nothing
*/
void glutGameIdle()
{
	glutGameRender();
	glutTimerFunc(1,glutGameIdle,0);
}

/*
* Function: void glutSystickService()
* -----------------------------
* Handles the systick of the world.
* returns nothing
*/
void glutGameSystickService(unsigned int systick_old)
{
	if(systick!=systick_old)printf("[GLUTGAME][WARNING] : Callback older systick, skipped a render?\n");
	systick++;
	glutTimerFunc(GLUTGAME_SYSTICK_INTERVAL,glutGameSystickService,systick);
}

/*
* Function: void glutSystickGet()
* -----------------------------
* Returns the current value of the systick of the world
* Value can overflow and return to 0.
*
*/
unsigned int glutGameSystickGet()
{
	return systick;
}

//////////////////////////////////// [GLUTGAME RENDER FUNCTION]  ////////////////////////////////////


/*
* Function: void glutGameRenderSceneSet()
* -----------------------------
* Binds a callback fucntion to render the world
*/
void glutGameRenderSceneSet(void (*fnc_p)())
{
	RenderScene_fnc = fnc_p;
}

/*
* Function: void glutGameRenderScene()
* -----------------------------
* Calls the callback function to render or define the world
*/
void glutGameRenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutGameCameraRender();

	if(RenderScene_fnc!=0x00)
		(*RenderScene_fnc)();
	glutGameRenderLocalAxis();
}


void glutGameRender2DScene()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0,screen_width,screen_height,0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	if(RenderScene2D_fnc != 0x00)
		(*RenderScene2D_fnc)();
	glutGameRenderOnScreenInfo();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

/*
* Function: void glutGameRender()
* -----------------------------
* The main function glut backend uses to render the world
* and player view.
*/
void glutGameRender()
{
	glutGameRenderScene();
	glutGameRender2DScene();
	//glutGameCameraRender();
	//glutGameRenderOnScreenInfo();
	#ifdef GLUTGAME_RENDER_DUBBELBUFFER
		glutSwapBuffers();
	#else
		glFlush();
	#endif
	glutGameRenderFPS();
	#ifdef GLUTGAME_RENDER_SHOWFPS
	printf("[GLUTGAME][RENDER] FPS: %.3lf\n",glutGameRenderGetFPS());
	#endif
}

/*
* Function: void glutGameRescale()
* -----------------------------
* Used to update the camera aspect ratio and parameters
* when the screen size is changed.
* Inputs:
*	n_w : new width in pixels of the window
*	n_h : new height in pixels of the window
*/
void glutGameRescale(GLint n_w, GLint n_h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)n_w/n_h,  GLUTGAME_PLAYER_NEARSIGHT, GLUTGAME_PLAYER_FARSIGHT);
	glViewport(0, 0, n_w, n_h);
	screen_width = n_w; screen_height = n_h;
}

/*
* Function: void glutGameRenderFPS()
* -----------------------------
* Calculate the FPS of the current session.
*/
void glutGameRenderFPS()
{
	//TODO Rename to session_fps or other
	framecounter = (double)1000/(glutGet(GLUT_ELAPSED_TIME));
	#ifdef GLUTGAME_RENDER_SHOWFPS
	printf("[GLUTGAME][RENDER] FPS : %.2lf\n",framecounter);
	#endif
}

/*
* Function: void glutGameGetFPS()
* -----------------------------
* Returns the current FPS of the running session.
*/
double glutGameRenderGetFPS()
{
	return framecounter;
}

//////////////////////////////////// [GLUTGAME CAMERA FUNCTIONS] ////////////////////////////////////
// These are the function to control (translate, rotate) the camera) around the world.

/*
* Function: void glutGameInitCamera() --> glutGameCameraInit()
* -----------------------------
* Intialise the camera with the defaulth values.
* Inputs:
*	x : x location of the camera
* 	y : y location of the camera (GLUTGAME_PLAYER_HEIGHT is not included)
*	z : z location of the camera
* returns nothing
*/
void glutGameCameraInit(double x, double y, double z)
{
	xPos = x; yPos = y; zPos = z;
	xl = 0; yl = 0; zl = 0;
	rotation_lr = 0;
	rotation_ud = 0;
}

/*
* Function: void glutGameRenderCamera() --> glutGameCameraRender()
* -----------------------------
* Renders the camera at a specific location with a specific rotation.
* returns nothing
*/
void glutGameCameraRender()
{
	gluLookAt(xPos,yPos+GLUTGAME_PLAYER_HEIGHT,zPos, xPos+xl,yPos+GLUTGAME_PLAYER_HEIGHT+yl,zPos+zl, xr, yr, zr);
}

void glutGameCameraPlace(double x, double y, double z, double rotation_xz, double rotation_y)
{
	xPos = x; yPos = y; zPos = z;
	rotation_lr = rotation_xz;
	rotation_ud = rotation_y;
}

//////////////////////////////////// [GLUTGAME CONTROL FUNCTIONS] ////////////////////////////////////

void glutGameControlInit()
{
	glutGameMouseInit();
	glutGameKeyboardInit();
	glutTimerFunc(GLUTGAME_CONTROL_TIMER,glutGameControlUpdate,0);
}

void glutGameControlUpdate()
{
	#ifdef GLUTGAME_CONTROL_USEREG
	glutGameMoveCamera(0);
	#endif
	glutTimerFunc(GLUTGAME_CONTROL_TIMER,glutGameControlUpdate,0);
}

void glutGameMouseInit()
{
	glutMouseFunc(glutGameMouseKeys);
	glutMotionFunc(glutGameMouseMove);
	glutPassiveMotionFunc(glutGameMouseMove);
}

/*
* Function: void glutGameMouseKeys()
* -----------------------------
* Tracks the state of the buttons of the mouse.
* - mouse_left = state tracking of the left mouse button
* - mouse_right = state tracking of the right mouse button
* Return nothing.
*/
void glutGameMouseKeys(int button, int state, int x, int y)
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

/*
* Function: void glutGameMouseMove()
* -----------------------------
* Tracks the delta movement of the mouse on the window.
* Can call callback functions when a mouse state is active
* Returns nothing.
*/
void glutGameMouseMove(int x, int y)
{
//	glutWarpPointer(0,0);		//Problem on some machines, TODO need to look more into before adding
	int delta_x = 0, delta_y = 0;
	delta_x = x - mouse_x_old;
	delta_y = y - mouse_y_old;
	if(mouse_state_left)
	{
		if((delta_x!=0)||(delta_y!=0))
		{
			//TODO Add callback function to bind to buttons
			glutGameRotateCamera(((double)delta_x/100),((double)delta_y/100));
			render_needed = 1;
		}
	}
	//#Save location in old value
	mouse_x_old = x;
	mouse_y_old = y;
}

/*
* Function: void glutGameKeyboardInit()
* -----------------------------
* Initialise the keyboard functions.
* When GLUTGAME_CONTROL_USEREG is defined
* this will enable keyboard state tracking on the used buttons
* Returns nothing
*/
void glutGameKeyboardInit()
{
	#ifdef GLUTGAME_CONTROL_USEREG
	glutIgnoreKeyRepeat(0x01);
	glutKeyboardFunc(glutGameKeyboardPressed);
	glutKeyboardUpFunc(glutGameKeyboardReleased);
	#else
	glutKeyboardFunc(glutGameKeyboardPressed);
	#endif
}

/*
* Function: void glutGameKeyboardPressed()
* -----------------------------
* Function to track when a key is pressed.
* Returns nothing
*/
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

/*
* Function: void glutGameKeyboardReleased()
* -----------------------------
* Function to track when a key is released.
* Returns nothing
*/
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

/*
* Function: void glutGameKeyboardReleased()
* -----------------------------
* Function to track when a key is released.
* Returns nothing
*/
void glutGameMoveCamera(int key)
{
	render_needed = 1;
	//#HACK : overwrite xl, zl globals with fix xz plane values
	double xl = sin(rotation_lr);
	double zl = -cos(rotation_lr);
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
		//glutTimerFunc(1,glutGameMoveCamera,0);
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
			yPos += GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		case GLUTGAME_CONTROL_DOWN :
			yPos -= GLUTGAME_PLAYER_BASESPEED * speed_mul;
			break;
		default:
			break;
	}
	#endif
}

void glutGameRotateCamera(double dxa, double dya)
{
	rotation_lr += dxa;
	rotation_ud -= dya;
	if(rotation_ud > 1.5) rotation_ud = 1.5;
	else if(rotation_ud < -1.5) rotation_ud = -1.5;
	xl = sin(rotation_lr) * cos(rotation_ud);
	zl = -cos(rotation_lr) * cos(rotation_ud);
	yl = sin(rotation_ud);
}

//////////////////////////////////// GLUTGAME IN GAME DEBUG FUNCTIONS ////////////////////////////////////


void glutGameRenderLocalAxis()
{
	//Draw the world axis in front of the camera
	//TODO Change lengt into define
	double lengt = 0.05;
	double mod_x, mod_y, mod_z;
	if(GLUTGAME_CONTROL_REG & (0x01 << 6))
	{
		glLineWidth(3);
		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3d(xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl);
			glVertex3d(xPos+xl+lengt,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl);
			glColor3f(0,1,0);
			glVertex3d(xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl);
			glVertex3d(xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT+lengt,zPos+zl);
			glColor3f(0,0,1);
			glVertex3d(xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl);
			glVertex3d(xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+lengt+zl);
		glEnd();
	}
}

void glutGameRenderOnScreenInfo()
{
	glRasterPos2f(screen_width/2,screen_height/2);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'X');
}
