// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
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


// Vertex arrays

static GLint vertices[] = {-5, -5, 
						   -4, 4, 
						   -3, -3, 
						   -2, -2};

static GLfloat colors[] = {0.5, 0, 0, 
						   0, 0.5, 0,
						   0, 0, 0.5};

// Lighting related

void setLightSource() {
	GLfloat light_ambient[] = { 0.9, 0.1, 0.1, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_specular[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void setMaterialLighting() {
	// Sets material properties
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 90.0 };
	GLfloat mat_emission[] = {0.9, 0.1, 0.1, 0.0};

	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

// Viewing related

void viewTransform() {
	// (0,0,5) -> camera position. -z value behind image
	// (0,0,0) -> where camera pointing at
	// (0,1.0,0) -> up vector
	gluLookAt (0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
// Primitive shapes used

void drawSolidTriangle() {

	glPushMatrix();
		glBegin(GL_TRIANGLES);
			glNormal3f(0,0,5);
			glColor3f(1.0, 0, 0);
			glVertex3f(0,0,0);
			glNormal3f(0,0,1);
			glColor3f(1.0, 0, 0);
			glVertex3f(5,0,0);
			glNormal3f(0,0,1);
			glColor3f(1.0, 0, 0);
			glVertex3f(0,5,0);
		glEnd();
	glPopMatrix();
}

void drawStipleLine() {
	glPushMatrix();
		glLineStipple(1, 0x00FF); // Specifies how line pattern
		glEnable(GL_LINE_STIPPLE);
		glLineWidth(5.0);		// Cannot be changed in Begin End
		glBegin(GL_LINE_STRIP);
			glColor3f(1.0, 1.0, 0);
			glVertex2f(-5, -1.0);
			glVertex2f(-3, 1.0);
			glVertex2f(-1, -2.0);
			glVertex2f(1, 2.0);
			glVertex2f(3, -3.0);
			glVertex2f(5, 3.0);
		glEnd();
	glPopMatrix();
}

void drawZigzag() {
	glPushMatrix();
		glLineWidth(20.0);		// Cannot be changed in Begin End
		glBegin(GL_LINE_STRIP);
			glColor3f(1.0, 1.0, 0);
			glVertex2f(-5, -1.0);
			glVertex2f(-3, 1.0);
			glVertex2f(-1, -2.0);
			glVertex2f(1, 2.0);
			glVertex2f(3, -3.0);
			glVertex2f(5, 3.0);
		glEnd();
	glPopMatrix();
}

void drawTrianglePoints() {
	glPushMatrix();
		glPointSize(10);	// Sets point size
		glBegin(GL_POINTS);
			glColor3f(1.0, 0, 0);
			glVertex2f(-1, 0);
			glColor3f(0.0, 1.0, 0);
			glVertex2f(1, 0);
			glColor3f(0.0, 0, 1.0);
			glVertex2f(0, 2);
		glEnd();
	glPopMatrix();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);
	
	//draw your stuff here
	drawSolidTriangle();
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
	// setSingleLighting();
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
