// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <math.h>
#include <iostream>

/* Include header files depending on platform */
#ifdef _WIN32
	#include "glut.h"
	#define M_PI 3.14159
#elif __APPLE__
	#include <OpenGL/gl.h>
	#include <GLUT/GLUT.h>
#else
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

using namespace std;

float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;

// Colors

GLfloat throne_color[][4] = { 
	{0.3, 0, 0.8, 0.75},
	{0.8, 0, 0.3, 0.75},
	{0.0, 0.8, 0.3, 0.75}
};

GLfloat star_color[] = {0.9, 0.3, 0.0};

// Lighting initialization

void addLightSource() {
	GLfloat ambientColor0[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuseColor0[] = {0.9, 0.0, 0.5, 1.0};
	GLfloat specularColor0[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lightPos0[] = {2.0, 4.0, 0.0, 0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	GLfloat ambientColor1[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuseColor1[] = {0.5, 0.0, 0.9, 1.0};
	GLfloat specularColor1[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lightPos1[] = {-2.0, 4.0, 0.0, 0};

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientColor1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseColor1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

void setLighting() {
	addLightSource();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
}

// Blending initilization

void setBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Primitive shapes used

void drawStar(GLfloat *color, int width) {
	int LINE_WIDTH = 7 - width;
	glPushMatrix();
		glLineWidth(LINE_WIDTH);
		glBegin(GL_LINE_LOOP);
			glColor3f(color[0], color[1], color[2]);
			glVertex2f(0, 0);
			glVertex2f(-1.0, -2.0);
			glVertex2f(-2.0, -3.0);
			glVertex2f(-1.0, -4.0);
			glVertex2f(0, -6.0);
			glVertex2f(1.0, -4.0);
			glVertex2f(2.0, -3.0);
			glVertex2f(1.0, -2.0);
		glEnd();
	glPopMatrix();
}

void drawMultiStar() {
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0x00FF);
	glPushMatrix();
		for (int i=0; i < 3; i++) {
			drawStar(star_color, i);
			glScalef(0.5, 1, 1);
		}
	glPopMatrix();
	glDisable(GL_LINE_STIPPLE);
}

void drawPoint(float x, float y, int size) {
	glPointSize(size);	// Sets point size
	glBegin(GL_POINTS);
		glColor3f(1.0, 1.0, 0.0);
		glVertex2f(x, y);
	glEnd();
}

void drawSinglePillar(GLfloat *color, float width, float height, float begin, int size) {
	float top = begin + height;
	glBegin(GL_POLYGON);
		glColor4f(color[0], color[1], color[2], color[3]);
		glNormal3f(2, 2, 0);
		glVertex2f(-width, begin);
		glVertex2f(-width, top -3);
		glVertex2f(0, top);
		glVertex2f(width, top -3);
		glVertex2f(width, begin);
	glEnd();
	drawPoint(0.0, top, size);
}

void drawThrone() {
	int SIZE = 15;
	float WIDTH = 0.8;
	float BEGIN = -8;
	float MIN_HEIGHT = 2;
	glPushMatrix();
		for (int i=6; i > 0; i--) {
			drawSinglePillar(throne_color[i%3], WIDTH, MIN_HEIGHT + (i*2), BEGIN, SIZE);
			WIDTH-= 0.1;
			SIZE-= 2;
			glTranslatef(-1.3, 0, 0);
			glRotatef(8.0, 0, 0, 1);
		}
	glPopMatrix();
}

void drawFullThrone() {
	glPushMatrix();
		drawThrone();
		glScalef(-1, 1, 1);
		drawThrone();
	glPopMatrix();

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	// background image should not change 
	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);
	
	//draw your stuff here
	drawFullThrone();
	drawMultiStar();
	glPopMatrix();
	glFlush ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	setLighting();
	setBlending();
	// Set Line Stipple Patterns
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

		case 'a':
			alpha+=10;
			glutPostRedisplay();
		break;

		case 'd':
			alpha-=10;
			glutPostRedisplay();
		break;

		case 'q':
			k+=0.1;
			glutPostRedisplay();
		break;

		case 'e':
			if(k>0.1)
				k-=0.1;
			glutPostRedisplay();
		break;

		case 'z':
			tx-=0.1;
			glutPostRedisplay();
		break;

		case 'c':
			tx+=0.1;
			glutPostRedisplay();
		break;

		case 's':
			ty-=0.1;
			glutPostRedisplay();
		break;

		case 'w':
			ty+=0.1;
			glutPostRedisplay();
		break;
			
		case 27:
			exit(0);
		break;

		default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 1\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n";
	cout <<"ESC: Quit\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
