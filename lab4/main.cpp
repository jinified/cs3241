// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>
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

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8
#define NO_CTRLPOINTS 4

using namespace std;

// Global variables that you can use
struct Point {
	int x,y;
};

// Colors

GLfloat colors[][4] = { 
	{0.3, 0, 0.8, 0.75},
	{0.8, 0, 0.3, 0.75},
	{0.0, 0.8, 0.3, 0.75}
};

// Storage of control points
int nPt = 0;
Point ptList[MAXPTNO];
Point ptListOri[MAXPTNO];

// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false;
bool C1Continuity = false;
	
bool shouldCalculateJointPoint(int index, int curveSize) {
	return C1Continuity && index > 4 && curveSize == 2;
}

Point calculateNewPoint(Point p4, Point p3) {
	Point result = {0, 0};
	result.x = 2 * p4.x - p3.x;
	result.y = 2 * p4.y - p3.y;
	return result;
}

float nChooseK(int n, int k) {
	float coeff = 1.0;
	if (k > 0) {
		for (int i = 1; i <= k ; i++) {
			coeff *= ((float)(n - (k - i)) / i);
		}
	}
	return coeff;
}

float calcBezierCoefficient(int i, float t, int n) {
	return nChooseK(n, i)*pow((1 - t), n - i)*pow(t, i);
}

void drawStar(GLfloat *color, int LINE_WIDTH) {
	glPushMatrix();
		glLineWidth(LINE_WIDTH);
		glBegin(GL_LINE_LOOP);
			glColor3f(color[0], color[1], color[2]);
			glVertex2f(0, 0);
			glVertex2f(30.0, 10.0);
			glVertex2f(40.0, 30.0);
			glVertex2f(50.0, 10.0);
			glVertex2f(80, 0);
			glVertex2f(50.0, -10.0);
			glVertex2f(40.0, -30.0);
			glVertex2f(30.0, -10.0);
		glEnd();
	glPopMatrix();
}

void drawSinglePillar(GLfloat *color, float width, float height, float begin) {
	float top = begin + height;
	glBegin(GL_POLYGON);
		glColor4f(color[0], color[1], color[2], color[3]);
		glVertex2f(-width, begin);
		glVertex2f(-width, top -3);
		glVertex2f(0, top);
		glVertex2f(width, top -3);
		glVertex2f(width, begin);
	glEnd();
}

void drawRightArrow()
{
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0,0);
		glVertex2f(100,0);
		glVertex2f(95,5);
		glVertex2f(100,0);
		glVertex2f(95,-5);
	glEnd();
}

void drawTangentVector(vector<Point> ctrlPoints, float i, int n) {
	Point vertex = {0, 0};

	float dx = 0;
	float dy = 0;
	for(int k = 0; k < n - 1 ; k++) {
		float coeff = calcBezierCoefficient(k, i, n - 2);
		dx += coeff * (n - 1) * (ctrlPoints.at(k + 1).x - ctrlPoints.at(k).x);
		dy += coeff * (n - 1) * (ctrlPoints.at(k + 1).y - ctrlPoints.at(k).y);
	}

	for(int j = 0; j < n ; j++) {
		float coeff = calcBezierCoefficient(j, i, n - 1);
		Point curr = ctrlPoints.at(j);
		vertex.x += coeff * curr.x;
		vertex.y += coeff * curr.y;
	}

	float angle = atan2(dy, dx);
	glPushMatrix();
		glTranslatef(vertex.x, vertex.y, 0);
		glRotatef(angle * 180 /  M_PI, 0, 0, 1);
		drawRightArrow();
		if (displayObjects) {
			drawStar(colors[0], 2);
		}
	glPopMatrix();
}

void drawBezierPoints(vector<Point> ctrlPoints) {
	int n = ctrlPoints.size();

	glColor3f(1.0, 0, 0);	
	glBegin(GL_LINE_STRIP);
		for (float i = 0.0; i <= 1.0; i += (1/(float)NLINESEGMENT)) {
			Point vertex = {0, 0};

			for(int j = 0; j < n ; j++) {
				float coeff = calcBezierCoefficient(j, i, n - 1);
				Point curr = ctrlPoints.at(j);
				vertex.x += coeff * curr.x;
				vertex.y += coeff * curr.y;
			}
			glVertex2d(vertex.x, vertex.y);
		}
	glEnd();

	if (displayTangentVectors) {
		for (float i = 0.0; i <= 1.0; i += (1/(float)NOBJECTONCURVE)) {
			drawTangentVector(ctrlPoints, i, n);
		}
	}
}


void drawBezierCurve() {
	vector<Point> ctrlPoints;
	for (int i = 0 ; i < nPt; i++) {
		ctrlPoints.push_back(ptList[i]);
		// Draw when there are 4 points or the rest of the points
		if (ctrlPoints.size() == NO_CTRLPOINTS || i == nPt - 1) {
			printf("Pt %d Drawing with %d points\n", i, ctrlPoints.size());
			drawBezierPoints(ctrlPoints);
			ctrlPoints.clear();
			ctrlPoints.push_back(ptList[i]);
		}
	}
	glPointSize(1);
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	vector<Point> curve;
	if(displayControlPoints) {

		glPointSize(5);
		glBegin(GL_POINTS);

		for(int i=0;i<nPt; i++) {
			curve.push_back(ptList[i]);
			glColor3f(0,0,0);
			if (shouldCalculateJointPoint(i + 1, curve.size())) {
				glColor3f(1.0, 0, 0);
				ptList[i] = calculateNewPoint(ptList[i - 1], ptList[i - 2]);
			} 

			if (curve.size() == NO_CTRLPOINTS) {
				curve.clear();
				curve.push_back(ptList[i]);
			}

			if (!C1Continuity) {
				ptList[i] = ptListOri[i];
			}

			glVertex2d(ptList[i].x,ptList[i].y);
		}
		glEnd();
		glPointSize(1);

	}

	if(displayControlLines)
	{
		glColor3f(0,1,0);
		glBegin(GL_LINE_STRIP);
			for(int i=0;i<nPt; i++) {
				glVertex2d(ptList[i].x,ptList[i].y);
			}
		glEnd();
		drawBezierCurve();
	}

	glPopMatrix();
	glutSwapBuffers ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void init(void)
{
	glClearColor (1.0,1.0,1.0, 1.0);
}

void readFile()
{

	std::ifstream file;
    file.open("savefile.txt");
	file >> nPt;

	if(nPt>MAXPTNO)
	{
		cout << "Error: File contains more than the maximum number of points." << endl;
		nPt = MAXPTNO;
	}

	for(int i=0;i<nPt;i++)
	{
		file >> ptList[i].x;
		file >> ptList[i].y;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile()
{
	std::ofstream file;
    file.open("savefile.txt");
    file << nPt << endl;

	for(int i=0;i<nPt;i++)
	{
		file << ptList[i].x << " ";
		file << ptList[i].y << endl;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case 'r':
		case 'R':
			readFile();
		break;

		case 'w':
		case 'W':
			writeFile();
		break;

		case 'T':
		case 't':
			displayTangentVectors = !displayTangentVectors;
		break;

		case 'o':
		case 'O':
			displayObjects = !displayObjects;
		break;

		case 'p':
		case 'P':
			displayControlPoints = !displayControlPoints;
		break;

		case 'L':
		case 'l':
			displayControlLines = !displayControlLines;
		break;

		case 'C':
		case 'c':
			C1Continuity = !C1Continuity;
		break;

		case 'e':
		case 'E':
			nPt = 0;

		break;

		case 27:
			exit(0);
		break;

		default:
		break;
	}

	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y)
{
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */
	enum
	{
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};
	if((button == MOUSE_LEFT_BUTTON)&&(state == GLUT_UP))
	{
		if(nPt==MAXPTNO)
		{
			cout << "Error: Exceeded the maximum number of points." << endl;
			return;
		}
		ptList[nPt].x=x;
		ptList[nPt].y=y;
		ptListOri[nPt].x=x;
		ptListOri[nPt].y=y;
		nPt++;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 4"<< endl<< endl;
	cout << "Left mouse click: Add a control point"<<endl;
	cout << "ESC: Quit" <<endl;
	cout << "P: Toggle displaying control points" <<endl;
	cout << "L: Toggle displaying control lines" <<endl;
	cout << "E: Erase all points (Clear)" << endl;
	cout << "C: Toggle C1 continuity" <<endl;	
	cout << "T: Toggle displaying tangent vectors" <<endl;
	cout << "O: Toggle displaying objects" <<endl;
	cout << "R: Read in control points from \"savefile.txt\"" <<endl;
	cout << "W: Write control points to \"savefile.txt\"" <<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS3241 Assignment 4");
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
