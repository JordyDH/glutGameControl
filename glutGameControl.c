////////////////////////////////////////////////////////////////////
//
//	glutGameController by Jordy de Hoon
//	MIT LICENSE , goto www.github.com/JordyDH/glutGameControl
//
////////////////////////////////////////////////////////////////////
#define  GLUT_GAMEC_VERSION "0.1"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "glutGameControl.h"
////////////////// CONTROLS //////////////////
#define GLUTGAME_CONTROL_FORW	'z'	//Moves the camera forwards
#define GLUTGAME_CONTROL_BACK	's'	//Moves the camera backwards
#define GLUTGAME_CONTROL_LEFT	'q'	//Moves the camera left
#define GLUTGAME_CONTROL_RIGHT	'd'	//Moves the camera right
#define GLUTGAME_CONTROL_UP	'a'	//Moves the camera up
#define GLUTGAME_CONTROL_DOWN	'e'	//Moves the camera down
////////////////// PLAYER MODEL //////////////////
#define GLUTGAME_PLAYER_HEIGHT		1.0	//Default player height, possition of the camera
#define GLUTGAME_PLAYER_BASESPEED	0.05	//Default step size
////////////////// FUNCTION POINTERS //////////////////
void (*RenderScene_fnc)();	//Callback function to render the scene
////////////////// LIB VARS //////////////////
double rotation_lr = 0;		//
double rotation_ud = 0;
double xl = 0, yl = 1, zl = 0;
double xPos = 3, yPos = 0, zPos = 5;
int mouse_state_left, mouse_state_right = 0;
int mouse_x_old, mouse_y_old = 0;
double framecounter = 0;
////////////////// LIB FUNCTION //////////////////
void glutGameRenderCamera()
{
	gluLookAt(xPos,yPos+GLUTGAME_PLAYER_HEIGHT,zPos, xPos+xl,yPos+yl+GLUTGAME_PLAYER_HEIGHT,zPos+zl, 0, 1, 0);
}

void glutGameInitCamera(double x, double y, double z)
{
	xPos = x; yPos = y; zPos = z;
}

void glutGameSetRenderScene(void (*fnc_p)())
{
	RenderScene_fnc = fnc_p;
}

void glutGameRenderScene()
{
	(*RenderScene_fnc)();
}

double glutGameGetFPS()
{
	framecounter = 1000/((double)glutGet(GLUT_ELAPSED_TIME));
	return framecounter;
}

void glutGameMouseFunction(int button, int state, int x, int y)
{
        if((state == GLUT_DOWN)&&(button == GLUT_LEFT_BUTTON))
                mouse_state_left = 1;
        else
                mouse_state_left = 0;
}

void glutGameMouseMove(int x, int y)
{
	int delta_x = 0, delta_y = 0;
	if(mouse_state_left)
	{
		//#Calc Delta's
		delta_x = x - mouse_x_old;
		delta_y = y - mouse_y_old;
		glutGameRotateCamera(((double)delta_x/100),((double)delta_y/100));
 		glutPostRedisplay();
	}
	//#Save location in old value
	mouse_x_old = x;
	mouse_y_old = y;
}

void glutGameMoveCamera(int key, int speedmul)
{
	switch(key)
	{
		case GLUTGAME_CONTROL_LEFT :
			xPos += zl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			zPos -= xl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			break;
		case GLUTGAME_CONTROL_RIGHT :
			xPos -= zl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			zPos += xl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			break;
		case GLUTGAME_CONTROL_FORW :
			xPos += xl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			zPos += zl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			break;
		case GLUTGAME_CONTROL_BACK :
			xPos -= xl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			zPos -= zl * GLUTGAME_PLAYER_BASESPEED * speedmul;
			break;
		case GLUTGAME_CONTROL_UP :
			yPos += GLUTGAME_PLAYER_BASESPEED;
			break;
		case GLUTGAME_CONTROL_DOWN :
			yPos -= GLUTGAME_PLAYER_BASESPEED;
			break;
	}
}

void glutGameRotateCamera(double dxa, double dza)
{
	rotation_lr += dxa;
	rotation_ud -= dza;
	xl = sin(rotation_lr);
	zl = -cos(rotation_lr);
	yl = sin(rotation_ud);
}



