// CS3241Lab2.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <vector>

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

#define PI 3.14159
#define DEG2RAD PI/180
#define numStars 100
#define numPlanets 7

// ======
// Global
// ======

float alpha = 1.0, beta = 0, k=1;
float tx = 0.0, ty=0.0;
bool isClock = false;
float delta_alpha = -0.1;

// Moon
GLfloat moon_color[] = {0.7, 0, 0.2, 1.0};
int moon_angle = 0;
int delta_moon_angle = 1;

// Star
GLfloat star_color[] = {1.0, 1.0, 1.0};
int star_line_width = 2;
float star_line_length = 0.4;

// Planet
int deg[] = {0, 90, 20, 150, 250, 350, 50, 180};
int delta_deg = 1;
float delta_moon_rad = 1;
float moon_radius = 1;

// Timer
int duration = 100;
float sec_t, min_t, hr_t;

void updateClock() {
	time_t current_time = time (NULL);
  	struct tm * timeinfo = localtime(&current_time);
	deg[1] = fmod((float) timeinfo->tm_hour, 12)/12.0 * 360;
	deg[2] = (float) timeinfo->tm_min/60*360;
	deg[3] = (float) timeinfo->tm_sec/60*360;
}

void updateMoonRad() {
	moon_radius += delta_moon_rad;
	if (moon_radius < 2 || moon_radius > 4) delta_moon_rad = -delta_moon_rad;
}

void updateDeg() {
	for (int i = 0; i < numPlanets; i++) {
		deg[i] = (deg[i] + delta_deg) % 361;
	}
}

void updateAlpha() {
	alpha -= delta_alpha;
	moon_angle = (moon_angle + delta_moon_angle) % 361;
	if (alpha < 0 || alpha > 1) delta_alpha = -delta_alpha;
}
// =====
// Color
// =====

GLfloat colors[][4] = {
	{0, 0.8, 0.4, 1.0},		// Light Green
	{0.8, 0.4, 0, 1.0},
	{0.4, 0.8, 0, 1.0},
	{0.4, 0, 0.8, 1.0},
	{0.8, 0, 0.4, 1.0},
	{0.8, 0, 0.8, 1.0},
	{0.8, 0.8, 0, 1.0},
	{0, 0.8, 0.8, 1.0},
	{0.8, 0, 0, 1.0}, 
	{0, 0.8, 0, 1.0}
};

// Time related functions

void Timer(int unused) {
	glutPostRedisplay();
	updateAlpha();
	glutTimerFunc(duration, Timer, 0);
}

std::vector<double> getOrbitPos(float radx, float rady, int theta) {
	std::vector<double> result; 
	result.push_back(sin(theta*DEG2RAD)*radx);
	result.push_back(cos(theta*DEG2RAD)*rady);
	return result;
}

void drawLine(float length, GLfloat color[]) {
	glLineWidth(star_line_width);
	glBegin(GL_LINE_LOOP);
		glColor4f(color[0], color[1], color[2], alpha);
		glVertex2f(0, length/2);	
		glVertex2f(0, -length/2);	
	glEnd();
}

void drawStar(GLfloat color[]) {
	glPushMatrix();
		for (int i=0; i < 4; i++) {
			drawLine(0.2, color);
			glRotatef(60, 0, 0, 1);	
		}
	glPopMatrix();
}

void drawStarField() {
	int offset = 2;
	glPushMatrix();
		for (int i = -8; i < 8; i += 2) {
			glPushMatrix();
				glTranslatef(i + offset, i - offset, 0);
				offset = -offset;
				drawStar(star_color);
			glPopMatrix();
		}
	glPopMatrix();
}

void drawStarNova() {
	glPushMatrix();
		for (int i = 0; i < 3; i++) {
			drawStarField();
			glRotatef(30, 0, 0, 1);
		}
	glPopMatrix();
}

void drawTriangle(double width, double height, GLfloat color[]) {
	glBegin(GL_POLYGON);
		glColor4f(color[0], color[1], color[2], color[3]);
		glVertex2f(-width/2, -height/2);
		glVertex2f(width/2, -height/2);
		glVertex2f(0, height/2);
	glEnd();
}

void drawEllipse(float radx, float rady, GLfloat color[]) {
	glBegin(GL_POLYGON);
		glColor4f(color[0], color[1], color[2], color[3]);
		for (int i=0; i <= 360; i++) {
			float rad = i*DEG2RAD;
			glColor4f(color[0], color[1], color[2], color[3]);
			glVertex2f(cos(rad)*radx, sin(rad)*rady);
		}	
	glEnd();
}

void drawDisk(float radius, GLfloat color[], bool canTransparent) {
	float alpha = color[3];
	glBegin(GL_POLYGON);
		for (int i=0; i <= 360; i++) {
			float rad = i*DEG2RAD;
			if (i > 180 && canTransparent) alpha = 0.5;
			glColor4f(color[0], color[1], color[2], alpha);
			glVertex2f(cos(rad)*radius, sin(rad)*radius);
		}	
	glEnd();
}

void drawSpaceCraft(float radx, float rady) {
	glPushMatrix();
		std::vector<double> orbitPos = getOrbitPos(radx, rady, deg[1]);
		glRotatef(90, 0, 0, 1);
		glTranslatef(orbitPos.at(0) , orbitPos.at(1), 0);
		glPushMatrix();
			drawTriangle(0.6, 0.8, colors[4]);
			glTranslatef(0, 1.2, 0);
			drawTriangle(1.2, 1.6, colors[5]);
			drawEllipse(0.3, 1.2, colors[6]);
		glPopMatrix();
	glPopMatrix();
}


void drawMoon(int deg, float orbit_radius, float size) {
	std::vector<double> orbitPos = getOrbitPos(orbit_radius, orbit_radius, deg);
	glTranslatef(orbitPos.at(0) , orbitPos.at(1), 0);
	glRotatef(moon_angle, 0, 0, 1);
	glPushMatrix();
		for (int i = 0; i < 4; i++) {
			glPushMatrix();
				glTranslatef(0, 0.3, 0);
				drawTriangle(0.24, 0.24, colors[6]);
			glPopMatrix();
			glRotatef(90, 0, 0, 1);
		}
	glPopMatrix();
	drawDisk(size, moon_color, false);
}

void drawPlanet(float radius, int orbit_radius, int deg, GLfloat color[], bool hasMoons, bool canTransparent) {
	glPushMatrix();
		std::vector<double> orbitPos = getOrbitPos(orbit_radius, orbit_radius, deg);
		glTranslatef(orbitPos.at(0), orbitPos.at(1), 0);
		drawDisk(radius, color, canTransparent);
		if (hasMoons) drawMoon(deg, moon_radius, 0.3);
	glPopMatrix();
}

class planet
{
public:
	bool hasMoons;	
	bool canTransparent;
	float distFromRef;
	float angularSpeed;
	GLfloat color[4];
	float size;
	float angle;
	int delta;

	planet(float distFromRef, float size, GLfloat color[], bool hasMoons, bool canTransparent)
	{
		this->distFromRef = distFromRef;
		this->size = size;
		this->color[0] = color[0], this->color[1] = color[1], this->color[2] = color[2], this->color[3] = color[3];
		this->hasMoons = hasMoons;
		this->canTransparent = canTransparent;
		angularSpeed = 0;
		angle = 0;
		delta = 1;
	}

	void draw(int deg) {
		drawPlanet(size, distFromRef, deg, color, hasMoons, canTransparent);
	}

};

void drawSolarClock(std::vector<planet> planets) {
	updateClock();
	glPushMatrix();
		for (size_t i = 0; i < 4; i++) {
			planets[i].draw(deg[i]);
		}
	glPopMatrix();
}

void drawSolarSystem(std::vector<planet> planets) {
	glPushMatrix();
		for (size_t i = 0; i < planets.size(); i++) {
			planets[i].draw(deg[i]);
		}
	glPopMatrix();
	updateDeg();
	updateMoonRad();
}

// ===========
// Planet init
// ===========

std::vector<planet>  initPlanets() {
	std::vector<planet> planets;
	
	planets.push_back(planet(0, 1, colors[1], false, false));	// sun
	planets.push_back(planet(3, 0.3, colors[5], false, true));	// planet 1
	planets.push_back(planet(5, 0.4, colors[6], false, true));	// planet 2
	planets.push_back(planet(6, 0.3, colors[0], false, false));	// planet 3
	planets.push_back(planet(7, 0.4, colors[7], true, false));	// planet 4
	planets.push_back(planet(7, 0.5, colors[8], false, true));	// planet 5
	planets.push_back(planet(6, 0.6, colors[0], true, false));	// planet 6
	return planets;
}

std::vector<planet> planets = initPlanets();

// ======
// OpenGL 
// ======

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
	glClearColor (0.0, 0.0, 0.3, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

		/* controls transformation
		glScalef(k, k, k);	
		glTranslatef(tx, ty, 0);	
		glRotatef(beta, 0, 0, 1);
		*/

		//draw stuff here!
		drawStarNova();

		if (isClock) {
			drawSolarClock(planets);
		} else {
			drawSolarSystem(planets);
			drawSpaceCraft(2.0, 6.0);
		}

	glPopMatrix();
	glFlush();
}


void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

		case 't':
			isClock = !isClock;
		case 'a':
			beta+=10;
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
	cout<<"CS3241 Lab 2\n\n";
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
	Timer(0);
	// glutIdleFunc(idle);
	glutReshapeFunc(reshape);	
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
