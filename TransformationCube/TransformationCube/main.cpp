/*
mouse:
LEFT - set a veter

keyboard:
A - turn left around y-axis 
D - turn right around y-axis
W - turn left around x-axis
S - turn right around x-axis
Q - turn left around z-axis
E - turn right around z-axis
Z - turn left around customized axis
X - turn right around customized axis
R - reset all

authored by Rikyo
*/

#include <math.h>
#include <GL/glut.h>
#include<stdio.h>
#define PI 3.14159265

//variable
float translatedX = 0, translatedY = 0, translatedZ = 0;
float rotatedX = 0, rotatedY = 0, rotatedZ = 0, rotatedW = 0;
float clickVector[3] = { 0,0,0 };
bool isPointSet = false;
//
GLfloat rotateMatrixX[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 };

GLfloat rotateMatrixY[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 };

GLfloat rotateMatrixZ[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 };

GLfloat rotateMatrixW[] = { 
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 }; //customized axis

GLfloat translateMatrix[] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0 };

void rotationMatrix(float angle, GLfloat x, GLfloat y, GLfloat z, GLfloat (*rotateMatrix)[16])
{
	GLfloat c = cos(angle * PI / 180);
	GLfloat s = sin(angle * PI / 180);
	GLfloat t = 1.0 - c;

	//normalize
	double magnitude = sqrt(x*x + y * y + z * z);
	if (magnitude == 0) printf("Please set apoint first!\n");
	else if (magnitude != 1)
	{
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}
	//

	(*rotateMatrix)[0] = c + x * x * t;
	(*rotateMatrix)[5] = c + y * y * t;
	(*rotateMatrix)[10] = c + z * z * t;
	(*rotateMatrix)[1] = x * y * t + z * s;
	(*rotateMatrix)[4] = x * y * t - z * s;
	(*rotateMatrix)[2] = x * z * t - y * s;
	(*rotateMatrix)[8] = x * z * t + y * s;
	(*rotateMatrix)[6] = y * z * t + x * s;
	(*rotateMatrix)[9] = y * z * t - x * s;
}

void translationMatrix()
{
	translateMatrix[12] = translatedX;
	translateMatrix[13] = translatedY;
	translateMatrix[14] = translatedZ;
}

void resetRotation()
{
	rotatedX = 0;
	rotationMatrix(rotatedX, 1, 0, 0, &rotateMatrixX);
	rotatedY = 0;
	rotationMatrix(rotatedY, 0, 1, 0, &rotateMatrixY);
	rotatedZ = 0;
	rotationMatrix(rotatedZ, 0, 0, 1, &rotateMatrixZ);

	if (isPointSet)
	{
		rotatedW = 0;
		rotationMatrix(rotatedW, clickVector[0], clickVector[1], clickVector[2], &rotateMatrixW);
	}
}

void resetTranslation()
{
	translatedX = 0;
	translatedY = 0;
	translatedZ = 0;
	translationMatrix();
}

void SetupRC()
{
	//light values and coordinates
	GLfloat whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { 0.f, 25.0f, 20.0f, 0.0f };

	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	//setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); //load the projection matrix
	glLoadIdentity();
	glOrtho(-10, 10, -10, 10, -10, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderScence(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10.0f, 0, 0, 0, 0, 1, 0);

	//draw x, y line
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -10.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glEnd();

	//draw the click point and line
	if (isPointSet)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(clickVector[0], clickVector[1], clickVector[2]);
		glEnd();

		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3f(clickVector[0], clickVector[1], clickVector[2]);
		glEnd();

		glMultMatrixf(rotateMatrixW);
	}

	//rotation
	/*glRotatef(rotatedX, 1.0, 0.0, 0.0);
	glRotatef(rotatedY, 0.0, 1.0, 0.0);
	glRotatef(rotatedZ, 0.0, 0.0, 1.0);*/
	glMultMatrixf(rotateMatrixX);
	glMultMatrixf(rotateMatrixY);
	glMultMatrixf(rotateMatrixZ);
	

	//translation
	//glTranslatef(translatedX, translatedY, translatedZ);
	glMultMatrixf(translateMatrix);

	//draw cube
	glColor3f(0.6f, 0.2f, 1.0f);
	glutSolidCube(6);
	glFlush();
	glutSwapBuffers();
}

void myKeyboard(unsigned char key, int x, int y)
{
	//keyboard§@
	switch (key)
	{
	case 'r':
		//resetAxis();
		resetRotation();
		resetTranslation();
		isPointSet = false;
		break;
	case 'a':
		rotatedY -= 3;
		rotationMatrix(rotatedY, 0, 1, 0, &rotateMatrixY);
		break;
	case 'd':
		rotatedY += 3;
		rotationMatrix(rotatedY, 0, 1, 0, &rotateMatrixY);
		break;
	case 'w':
		rotatedX -= 3;
		rotationMatrix(rotatedX, 1, 0, 0, &rotateMatrixX);
		break;
	case 's':
		rotatedX += 3;
		rotationMatrix(rotatedX, 1, 0, 0, &rotateMatrixX);
		break;
	case 'q':
		rotatedZ += 3;
		rotationMatrix(rotatedZ, 0, 0, 1, &rotateMatrixZ);
		break;
	case 'e':
		rotatedZ -= 3;
		rotationMatrix(rotatedZ, 0, 0, 1, &rotateMatrixZ);
		break;
	case 'z':
		rotatedW += 3;
		rotationMatrix(rotatedW, clickVector[0], clickVector[1], clickVector[2], &rotateMatrixW);
		break;
	case 'x':
		rotatedW -= 3;
		rotationMatrix(rotatedW, clickVector[0], clickVector[1], clickVector[2], &rotateMatrixW);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void mySpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		translatedX -= 1;
		break;
	case GLUT_KEY_RIGHT:
		translatedX += 1;
		break;
	case GLUT_KEY_UP:
		translatedY += 1;
		break;
	case GLUT_KEY_DOWN:
		translatedY -= 1;
		break;
	default:
		break;
	}

	translationMatrix();
	glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		isPointSet = true;
		clickVector[0] = float(x - 200) / 20;
		clickVector[1] = float(200 - y) / 20;

		glutPostRedisplay();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(600, 80);
	glutCreateWindow("Transformation Cube");

	//init();
	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScence);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKey);
	glutMouseFunc(myMouse);

	glutMainLoop();
	return 0;
}