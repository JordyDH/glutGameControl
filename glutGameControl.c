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
#define FORW	'z'	//Moves the camera forwards
#define BACK	's'	//Moves the camera backwards
#define LEFT	'q'	//Moves the camera left
#define RIGHT	'd'	//Moves the camera right
#define UP	'a'	//Moves the camera up
#define DOWN	'e'	//Moves the camera down
////////////////// PLAYER MODEL //////////////////
#define PHEIGHT	1.0
////////////////// FUNCTION POINTERS //////////////////
void (*RenderScene_fnc)();
////////////////// LIB VARS //////////////////
double rotation = 0;
double Angle2 = 0;
double xl = 0, yl = 1, zl = 0;
double xPos = 3, yPos = 0, zPos = 5;
int mouse_state_left, mouse_state_right = 0;
int mouse_x_old, mouse_y_old = 0;
double framecounter = 0;
////////////////// LIB FUNCTION //////////////////
void glutGameRenderCamera()
{
	gluLookAt(xPos,yPos+PHEIGHT,zPos, xPos+xl,yPos+yl+PHEIGHT,zPos+zl, 0, 1, 0);
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
		case LEFT :
			xPos += zl * 0.1 * speedmul;
			zPos -= xl * 0.1 * speedmul;
			break;
		case RIGHT :
			xPos -= zl * 0.1 * speedmul;
			zPos += xl * 0.1 * speedmul;
			break;
		case FORW :
			xPos += xl * 0.1 * speedmul;
			zPos += zl * 0.1 * speedmul;
			break;
		case BACK :
			xPos -= xl * 0.1 * speedmul;
			zPos -= zl * 0.1 * speedmul;
			break;
	}
}

void glutGameRotateCamera(double dxa, double dza)
{
	rotation += dxa;
	Angle2 -= dza;
	xl = sin(rotation);
	zl = -cos(rotation);
	yl = sin(Angle2);
}



