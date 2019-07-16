#include "Matrix.hpp"
#include "Model.hpp"
#include "ObjParser.hpp"

#include <random>
#include <time.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <GL/glut.h>

#define WIDTH 500
#define HEIGHT 500
#define M_PI 3.14159265

std::string _fileName = "";
int _width = WIDTH, _height = HEIGHT;
bool _isModelSet = false, _isBoudingBoxDispalyed = false, _isAxisDisplayed = false;
int _polygonMode = 1, _colorMode = 0;
ModelPtr _model;
ObjParser _parser;
Matrix4f _xRotation = Matrix4f::identity(), _yRotation = Matrix4f::identity(), _zRotation = Matrix4f::identity(), _translation = Matrix4f::identity();
float rotatedX = 0, rotatedY = 0, rotatedZ = 0;
Vector3f _positiveBounding = Vector3f::zero();
Vector3f _negativeBounding = Vector3f::zero();
Vector3f _modelMeasurement = Vector3f::zero();

float _modelRadius, _ratio = 1.5;

std::random_device _randomDevice; std::default_random_engine _generator = std::default_random_engine(_randomDevice());
std::uniform_real_distribution<float> _distribution(0,1);
Vector3f _randomColor = Vector3f::zero();

static float degToRad(float x) {
	return (x * (M_PI / 180.0));
}

void setModel() {
	try {
		_model = _parser.parseObj(_fileName);
		_isModelSet = true;
	}
	catch (std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

	_model->centroid();
	_model->center();
	_positiveBounding = Vector3f::zero();
	_negativeBounding = Vector3f::zero();
	_modelRadius = _model->radius(&_positiveBounding, &_negativeBounding);
	_modelMeasurement(0) = _positiveBounding(0) > abs(_negativeBounding(0)) ? _positiveBounding(0) : abs(_negativeBounding(0));
	_modelMeasurement(1) = _positiveBounding(1) > abs(_negativeBounding(1)) ? _positiveBounding(1) : abs(_negativeBounding(1));
	_modelMeasurement(2) = _positiveBounding(2) > abs(_negativeBounding(2)) ? _positiveBounding(2) : abs(_negativeBounding(2));
	//std::cout << _boundingBox(0) << " " << _boundingBox(1) << " " << _boundingBox(2) << std::endl;
	_height = _width * _modelMeasurement(1) / _modelMeasurement(0);
	if (_height > 1000) _height = 1000;
	glutSetWindowTitle(_fileName.c_str());
	glutReshapeWindow(_width, _height);
}

void reset()
{
	rotatedX = rotatedY = rotatedZ = 0;
	_xRotation = Matrix4f::identity();
	_yRotation = Matrix4f::identity();
	_zRotation = Matrix4f::identity();
	_translation = Matrix4f::identity();
}

void translation(float x, float y, float z)
{
	_translation(3, 0) += x;
	_translation(3, 1) += y;
	_translation(3, 2) += z;
}

void rotation(float angle, GLfloat x, GLfloat y, GLfloat z, Matrix4f* matrix)
{
	GLfloat c = cos(angle * M_PI / 180);
	GLfloat s = sin(angle * M_PI / 180);
	GLfloat t = 1.0 - c;

	//normalize
	double magnitude = sqrt(x*x + y * y + z * z);
	if (magnitude != 1)
	{
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}

	(*matrix)(0, 0) = c + x * x * t;
	(*matrix)(1, 1) = c + y * y * t;
	(*matrix)(2, 2) = c + z * z * t;
	(*matrix)(0, 1) = x * y * t + z * s;
	(*matrix)(1, 0) = x * y * t - z * s;
	(*matrix)(0, 2) = x * z * t - y * s;
	(*matrix)(2, 0) = x * z * t + y * s;
	(*matrix)(1, 2) = y * z * t + x * s;
	(*matrix)(2, 1) = y * z * t - x * s;
}

void init()
{
	/*glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	/*glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);*/
	glShadeModel(GL_SMOOTH);

	//light values and coordinates
	GLfloat whiteLight[] = { 0.45f, 0.45f, 0.45f, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { 0.f, 25.0f, 20.0f, 0.0f };

	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	//setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
}

void idle() {
	glutPostRedisplay();
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-_modelMeasurement(0) *_ratio, _modelMeasurement(0)*_ratio, -_modelMeasurement(1)*_ratio, _modelMeasurement(1)*_ratio, _modelRadius, _modelRadius*3.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, _modelRadius*2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	if (_isModelSet)
	{
		switch (_polygonMode)
		{
		case 0:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		default:
			break;
		}

		if (_isAxisDisplayed)
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex3f(-_modelMeasurement(0)*_ratio, 0.0f, 0.0f);
			glVertex3f(_modelMeasurement(0)*_ratio, 0.0f, 0.0f);
			glVertex3f(0.0f, -_modelMeasurement(1)*_ratio, 0.0f);
			glVertex3f(0.0f, _modelMeasurement(1)*_ratio, 0.0f);
			glEnd();
		}

		switch (_colorMode)
		{
		case 0:
			glColor3f(0.0f, 0.0f, 0.0f);
			break;
		case 1:
			glColor3f(_randomColor(0), _randomColor(1), _randomColor(2));
			break;
		default:
			break;
		}

		//rotation
		glMultMatrixf(_xRotation.data());
		glMultMatrixf(_yRotation.data());
		glMultMatrixf(_zRotation.data());

		//translation
		glMultMatrixf(_translation.data());

		_model->render();

		if (_isBoudingBoxDispalyed)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_LOOP);
			glVertex3f(_positiveBounding(0), _positiveBounding(1), _positiveBounding(2));
			glVertex3f(_positiveBounding(0), _negativeBounding(1), _positiveBounding(2));
			glVertex3f(_negativeBounding(0), _negativeBounding(1), _positiveBounding(2));
			glVertex3f(_negativeBounding(0), _positiveBounding(1), _positiveBounding(2));
			glEnd();

			glBegin(GL_LINE_LOOP);
			glVertex3f(_positiveBounding(0), _positiveBounding(1), _negativeBounding(2));
			glVertex3f(_positiveBounding(0), _negativeBounding(1), _negativeBounding(2));
			glVertex3f(_negativeBounding(0), _negativeBounding(1), _negativeBounding(2));
			glVertex3f(_negativeBounding(0), _positiveBounding(1), _negativeBounding(2));
			glEnd();

			glBegin(GL_LINES);
			glVertex3f(_positiveBounding(0), _positiveBounding(1), _positiveBounding(2));
			glVertex3f(_positiveBounding(0), _positiveBounding(1), _negativeBounding(2));

			glVertex3f(_positiveBounding(0), _negativeBounding(1), _positiveBounding(2));
			glVertex3f(_positiveBounding(0), _negativeBounding(1), _negativeBounding(2));

			glVertex3f(_negativeBounding(0), _negativeBounding(1), _positiveBounding(2));
			glVertex3f(_negativeBounding(0), _negativeBounding(1), _negativeBounding(2));

			glVertex3f(_negativeBounding(0), _positiveBounding(1), _positiveBounding(2));
			glVertex3f(_negativeBounding(0), _positiveBounding(1), _negativeBounding(2));
			glEnd();
		}
	}

	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	_width = width;_height = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
}

void mainMenu(int id)
{
	switch (id)
	{
	case 1: 
		exit(0);
	default: 
		break;
	}
}

void fileMenu(int id)
{
	switch (id)
	{
	case 1: 
		_fileName = "./objfile/gourd.obj";
		break;
	case 2: 
		_fileName = "./objfile/lamp.obj";
		break;
	case 3:
		_fileName = "./objfile/octahedron.obj";
		break;
	case 4:
		_fileName = "./objfile/teapot.obj";
		break;
	case 5:
		_fileName = "./objfile/teddy.obj";
		break;
	default: 
		break;
	}

	setModel();
	//_model->printFace();
}

void renderMenu(int id)
{
	switch (id)
	{
	case 1:
		_polygonMode = 0;
		break;
	case 2: 
		_polygonMode = 1;
		break;
	case 3: 
		_polygonMode = 2;
		break;
	default:
		break;
	}
}

void colorMenu(int id)
{
	switch (id)
	{
	case 1:
		_colorMode = 0;
		break;
	case 2:
		_colorMode = 1;
		_randomColor(0) = _distribution(_generator);
		_randomColor(1) = _distribution(_generator);
		_randomColor(2) = _distribution(_generator);
		break;
	default: 
		break;
	}
}

void boundingMenu(int id)
{
	switch (id)
	{
	case 1: 
		_isBoudingBoxDispalyed = true;
		break;
	case 2: 
		_isBoudingBoxDispalyed = false;
		break;
	default:
		break;
	}
}

void axisMenu(int id)
{
	switch (id)
	{
	case 1: 
		_isAxisDisplayed = true;
		break;
	case 2: 
		_isAxisDisplayed = false;
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		reset();
		break;
	case 'a':
		rotatedY -= 3;
		rotation(rotatedY, 0, 1, 0, &_yRotation);
		break;
	case 'd':
		rotatedY += 3;
		rotation(rotatedY, 0, 1, 0, &_yRotation);
		break;
	case 'w':
		rotatedX -= 3;
		rotation(rotatedX, 1, 0, 0, &_xRotation);
		break;
	case 's':
		rotatedX += 3;
		rotation(rotatedX, 1, 0, 0, &_xRotation);
		break;
	case 'q':
		rotatedZ += 3;
		rotation(rotatedZ, 0, 0, 1, &_zRotation);
		break;
	case 'e':
		rotatedZ -= 3;
		rotation(rotatedZ, 0, 0, 1, &_zRotation);
		break;
	case '+':
		_ratio -= (_ratio > 1.05) ? 0.05 : 0;
		//std::cout << _ratio << std::endl;
		break;

	case '-':
		_ratio += (_ratio < 4.95) ? 0.05 : 0;
		//std::cout << _ratio << std::endl;
		break;
	case 27:	//escape
		exit(0);
		break;
	}
}

void specialKey(int key, int x, int y) {
	Vector3f xAxis = Vector3f::zero();
	Vector3f yAxis = Vector3f::zero();

	xAxis(0) = 1.0f;
	yAxis(1) = 1.0f;

	switch (key) {
	case GLUT_KEY_UP:
		translation(0.0f, 0.1f, 0.0f);
		break;
	case GLUT_KEY_DOWN:
		translation(0.0f, -0.1f, 0.0f);
		break;
	case GLUT_KEY_LEFT:
		translation(-0.1f, 0.0f, 0.0f);
		break;
	case GLUT_KEY_RIGHT:
		translation(0.1f, 0.0f, 0.0f);
		break;
	}
}

void mouse(int button, int state, int x, int y) {
		if (3 == button) {	//hack for mousewheel up
			_ratio -= (_ratio > 1.05) ? 0.05 : 0;
			//std::cout << _ratio << std::endl;
		}
		else if (4 == button) {	//hack for mousewheel down
			_ratio += (_ratio < 4.95) ? 0.05 : 0;
			//std::cout << _ratio << std::endl;
		}
}

int main(int argc, char** argv)
{
	int file_menu, render_menu, color_menu, bounding_menu, axis_menu;

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(_width, _height);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Obj Praser");

	/* Register all callback functions. */
	init();
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
	glutMouseFunc(mouse);

	/* Create the menu structure and attach it to the right mouse button. */
	file_menu = glutCreateMenu(fileMenu);
	glutAddMenuEntry("gourd.obj", 1);
	glutAddMenuEntry("lamp.obj", 2);
	glutAddMenuEntry("octahedron.obj", 3);
	glutAddMenuEntry("teapot.obj", 4);
	glutAddMenuEntry("teddy.obj", 5);
	render_menu = glutCreateMenu(renderMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Face", 3);
	color_menu = glutCreateMenu(colorMenu);
	glutAddMenuEntry("Single Color", 1);
	glutAddMenuEntry("Random Colors", 2);
	bounding_menu = glutCreateMenu(boundingMenu);
	glutAddMenuEntry("On", 1);
	glutAddMenuEntry("Off", 2);
	axis_menu = glutCreateMenu(axisMenu);
	glutAddMenuEntry("On", 1);
	glutAddMenuEntry("Off", 2);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("File", file_menu);
	glutAddSubMenu("Render Mode", render_menu);
	glutAddSubMenu("Color Mode", color_menu);
	glutAddSubMenu("Bounding Box", bounding_menu);
	glutAddSubMenu("Axis", axis_menu);
	glutAddMenuEntry("Exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	glutMainLoop();
	return 0;
}