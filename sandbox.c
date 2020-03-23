#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "glutGameControl.h"

GLdouble xlens = 1.0, ylens = 1.0, zlens = 3.0;
GLdouble xref = 1.0, yref = 1.0, zref = 0.0;
GLdouble xvw = 0.0, yvw = 1.0, zvw = 0.0;
GLdouble xmin = -2.0, xmax = 4.0, ymin = -2.0, ymax = 3.0;
GLdouble near = 0.1, far = 10.0;
GLdouble hoek = 60.0;
GLfloat ttx = 0.25, tty = 0.25, ttz = 0.25;
GLint lijnen = 0;
GLubyte projectie = 'p';

int togle = 0;
void kubus();

void myinit(void)
{
	glClearColor(0.8, 0.8, 0.8, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}

void fullkubus(int x, int y, int z, float size)
{
	glColor3f(1,0,0);
	//Voorkant
	glBegin(GL_QUADS);
		glVertex3d(x-(size/2),y-(size/2),z+(size/2));
		glVertex3d(x+(size/2),y-(size/2),z+(size/2));
		glVertex3d(x+(size/2),y+(size/2),z+(size/2));
		glVertex3d(x-(size/2),y+(size/2),z+(size/2));
	glEnd();
	//Links
	glColor3f(0,1,0);
	glBegin(GL_QUADS);
		glVertex3d(x+(size/2),y-(size/2),z+(size/2));
		glVertex3d(x+(size/2),y-(size/2),z-(size/2));
		glVertex3d(x+(size/2),y+(size/2),z-(size/2));
		glVertex3d(x+(size/2),y+(size/2),z+(size/2));
	glEnd();
	//Achter
	glColor3f(0,0,1);
	glBegin(GL_QUADS);
		glVertex3d(x-(size/2),y-(size/2),z-(size/2));
		glVertex3d(x+(size/2),y-(size/2),z-(size/2));
		glVertex3d(x+(size/2),y+(size/2),z-(size/2));
		glVertex3d(x-(size/2),y+(size/2),z-(size/2));
	glEnd();
	//Rechts
	glColor3f(1,0,1);
	glBegin(GL_QUADS);
		glVertex3d(x-(size/2),y-(size/2),z+(size/2));
		glVertex3d(x-(size/2),y-(size/2),z-(size/2));
		glVertex3d(x-(size/2),y+(size/2),z-(size/2));
		glVertex3d(x-(size/2),y+(size/2),z+(size/2));
	glEnd();
	//Onder
	glColor3f(0,1,1);
	glBegin(GL_QUADS);
		glVertex3d(x-(size/2),y-(size/2),z-(size/2));
		glVertex3d(x+(size/2),y-(size/2),z-(size/2));
		glVertex3d(x+(size/2),y-(size/2),z+(size/2));
		glVertex3d(x-(size/2),y-(size/2),z+(size/2));
	glEnd();
	//Boven
	glColor3f(1,1,0);
	glBegin(GL_QUADS);
		glVertex3d(x-(size/2),y+(size/2),z-(size/2));
		glVertex3d(x+(size/2),y+(size/2),z-(size/2));
		glVertex3d(x+(size/2),y+(size/2),z+(size/2));
		glVertex3d(x-(size/2),y+(size/2),z+(size/2));
	glEnd();
}

void toetsen( unsigned char key, int x, int y)
{
	//glutGameMoveCamera(key,1.0);
	//glutPostRedisplay();
	//kubus();
}
void as(double lengte)
{
	if ( lengte > 2.0 )
	{
		glColor3f(0.0, 0.1, 0.8);
	}
	else if ( lengte < 2.0 )
	{
		glColor3f(0.8, 0.1, 0.0);
	}
	else
	{
		glColor3f(0.0, 0.8, 0.1);
	}
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, lengte);
	glEnd();
}

void kubus(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutGameRenderCamera();
	glLineWidth(1.5);
	as(1.8);       // z-as
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	as(2.5);       // x-as
	glPopMatrix();
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	as(2.0);       // y-as
	glPopMatrix();
	glLineWidth(2.5);
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(ttx, tty, ttz);
	glutWireCube(0.5);
	fullkubus(0,0,0,0.5);
	if ( lijnen )
	{
		glLineWidth(1.5);
		glLineStipple(2, 0xaaaa);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINES);
			glVertex3d(0.25, 0.25, -0.25);
			glVertex3d(0.25, 0.25, -5.5);
			glVertex3d(-0.25, 0.25, -0.25);
			glVertex3d(-0.25, 0.25, -5.5);
		glEnd();
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
			glVertex3d(-0.25, 0.25, -0.25);
			glVertex3d(-5.25, 0.25, -0.25);
			glVertex3d(-0.25, 0.25, 0.25);
			glVertex3d(-5.25, 0.25, 0.25);
		glEnd();
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
			glVertex3d(-0.25, -0.25, 0.25);
			glVertex3d(-0.25, -5.25, 0.25);
			glVertex3d(0.25, -0.25, 0.25);
			glVertex3d(0.25, -5.25, 0.25);
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	glPopMatrix();
	//glutWireTeapot(1);
}
void herschaal(GLint n_w, GLint n_h) 
{
	GLdouble grens;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch ( projectie )
	{
		case 'o':
				glOrtho(xmin, xmax,  ymin, ymax, near, far);
				break;
		case 'f':
				glFrustum(-1, 1,  -1, 1, 2*near, 8*far);
				break;
		case 'F':
				grens = near*tan(M_PI*(hoek/2.0)/180.0);
				glFrustum(-grens, grens,  -grens, grens, near, far);
				break;
		default:
		case 'p':
				gluPerspective(hoek, (double)16/9,  near, far);
				break;
	}
	glViewport(0, 0, n_w, n_h);
	/*
	if(n_w <= n_h)
		glViewport(0, 0, n_w, n_w);
	else
		glViewport(0, 0, n_h, n_h);
	*/
}
int main( int argc, char * argv[])
{
	char titel[20] = "kubus projectie    ";
	if ( argc > 1 )
	{
		projectie = argv[1][0];
	}
	else
		projectie = 'p';      /* default symmetrisch perspectief  */
	titel[16] = projectie;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(800, 800);
	glutCreateWindow(titel);
    	myinit();
	//glutKeyboardFunc(toetsen);
	glutGameKeyboardInit();
	glutReshapeFunc(glutGameRescale);
	glutDisplayFunc(glutGameRenderScene);
	glutMouseFunc(glutGameMouseFunction);
	glutMotionFunc(glutGameMouseMove);
	glutPassiveMotionFunc(glutGameMouseMove);

	glutGameSetRenderScene(kubus);
	glutGameMainLoop();

	//glutMainLoop();
}
