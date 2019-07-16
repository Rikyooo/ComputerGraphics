#include <iostream>
#include <GL/glut.h>

#define X .525731112119133606
#define Z .850650808352039932

float ratio = sqrt(pow(X, 2) + pow(Z, 2));
int window_width = 1200;
int window_height = 300;
//light source
GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0  };
GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_position[] = { 0.0, 0.0, 10.0, 0.0 };
//

//material source --ruby
GLfloat material_ambient[] = { 0.1745, 0.01175, 0.01175, 1.0 };
GLfloat material_diffuse[] = { 0.61424, 0.04136, 0.04136, 1.0 };
GLfloat material_specular[] = { 0.727811, 0.626959, 0.626959, 1.0 };
GLfloat low_shininess[] = { 0.6 };
//

//icosahedron source
static GLfloat vdata[12][3] = {
	{ -X,0.0,Z },{ X,0.0,Z },{ -X,0.0,-Z },{ X,0.0,-Z },
	{ 0.0,Z,X },{ 0.0,Z,-X },{ 0.0,-Z,X },{ 0.0,-Z,-X },
	{ Z,X,0.0 },{ -Z,X,0.0 },{ Z,-X,0.0 },{ -Z,-X,0.0 },};
static GLuint tindices[20][3] ={
	{ 1,4,0 },{ 4,9,0 },{ 4,5,9 },{ 8,5,4 },{ 1,8,4 },
	{ 1,10,8 },{ 10,3,8 },{ 8,3,5 },{ 3,2,5 },{ 3,7,2 },
	{ 3,10,7 },{ 10,6,7 },{ 6,11,7 },{ 6,0,11 },{ 6,1,0 },
	{ 10,1,6 },{ 11,0,9 },{ 2,11,9 },{ 5,2,9 },{ 11,2,7 }};
GLfloat d1[3], d2[3], n[3];
//

//var
bool polygon_mode = true; //true --fill, false --line
int subdivision_depth = 1;
int rotate_object = 0; //0 --flat, 1 --interpolate, 2 --subdivision
GLfloat rotate_angle_flat[3] = { 0.0, 0.0, 0.0 };
GLfloat rotate_angle_interpolate[3] = { 0.0, 0.0, 0.0 };
GLfloat rotate_angle_subdivision[3] = { 0.0, 0.0, 0.0 };
//

void normalize(float (&v)[3])
{
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= d; v[1] /= d; v[2] /= d;
}

void normCrossProd(float u[3], float v[3], float (&out)[3])
{
	out[0] = u[1] * v[2] - u[2] * v[1];
	out[1] = u[2] * v[0] - u[0] * v[2];
	out[2] = u[0] * v[1] - u[1] * v[0];
	normalize(out);
}

void icoNormVec(int i)
{
	for (int k = 0; k < 3; k++) 
	{
		d1[k] = vdata[tindices[i][0]][k] - vdata[tindices[i][1]][k];
		d2[k] = vdata[tindices[i][1]][k] - vdata[tindices[i][2]][k];
	}
	normCrossProd(d1, d2, n);
	glNormal3fv(n);
}

void drawTriangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3])
{
	glBegin(GL_TRIANGLES);
	glNormal3fv(v1);
	glVertex3fv(v1);
	glNormal3fv(v2);
	glVertex3fv(v2);
	glNormal3fv(v3);
	glVertex3fv(v3);
	glEnd();
}

void subdivide(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
	GLfloat v12[3], v23[3], v31[3]; int i;
	if (depth == 0) {
		drawTriangle(v1, v2, v3); //Draw if no further subdivision requested
		return;
	}
	for (i = 0; i < 3; i++) {
		v12[i] = (v1[i] + v2[i]) / 2.0;
		v23[i] = (v2[i] + v3[i]) / 2.0;
		v31[i] = (v3[i] + v1[i]) / 2.0;
	}
	normalize(v12);
	normalize(v23);
	normalize(v31);
	subdivide(v1, v12, v31, depth - 1);
	subdivide(v2, v23, v12, depth - 1);
	subdivide(v3, v31, v23, depth - 1);
	subdivide(v12, v23, v31, depth - 1);
}

void readme()
{
	std::cout << "W --rotate up along the x-axis" << std::endl;
	std::cout << "S --rotate down along the x-axis" << std::endl;
	std::cout << "A --rotate left along the y-axis" << std::endl;
	std::cout << "D --rotate right along the y-axis" << std::endl;
	std::cout << "Q --rotate left along the z-axis" << std::endl;
	std::cout << "E --rotate right along the z-axis" << std::endl;
	std::cout << "R --reset the current icosahedron" << std::endl;
	std::cout << "C --change the icosahedron controlled" << std::endl;
	std::cout << "P --change polygon mode" << std::endl;
	std::cout << "+ --increase subdivision depth" << std::endl;
	std::cout << "- --decrease subdivision depth" << std::endl << std::endl;
	std::cout << "You are controlling the icosahedron of No.1." << std::endl;
}

void init() 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
}

void SetupRC()
{
	//setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//setup material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);
	//enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void idle() 
{
	glutPostRedisplay();
}

void display(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.6f, 0.2f, 1.0f);
	if (!polygon_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	
	//flat
	glShadeModel(GL_FLAT);
	glPushMatrix();
	glTranslatef(-2.5f, 0.0f, 0.0f);
	glRotatef(rotate_angle_flat[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_angle_flat[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rotate_angle_flat[2], 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 20; i++)
	{
		/*if (i == 0) glColor3f(1.0, 0.0, 0.0); 
		else glColor3f(1.0, 1.0, 1.0);*/
		icoNormVec(i);
		glVertex3fv(vdata[tindices[i][0]]);
		glVertex3fv(vdata[tindices[i][1]]);
		glVertex3fv(vdata[tindices[i][2]]);
	}
	glEnd();
	glPopMatrix();
	
	//interpolate
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glRotatef(rotate_angle_interpolate[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_angle_interpolate[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rotate_angle_interpolate[2], 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 20; i++)
		subdivide(vdata[tindices[i][0]],
			      vdata[tindices[i][1]],
			      vdata[tindices[i][2]],
			      0);
	glPopMatrix();

	//subdivision
	glPushMatrix();
	glTranslatef(2.5f, 0.0f, 0.0f);
	glRotatef(rotate_angle_subdivision[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rotate_angle_subdivision[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rotate_angle_subdivision[2], 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 20; i++)
		subdivide(vdata[tindices[i][0]],
			      vdata[tindices[i][1]],
			      vdata[tindices[i][2]],
			      subdivision_depth);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);

	glFlush();
	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(1.0f, 80.0f, 1.0f, 20.0f);
	glOrtho(-4.0f, 4.0f, -1.0f, 1.0f, -10.0f, 10.0f);
	glMatrixMode(GL_MODELVIEW);

	window_width = width;
	window_height = height;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':
		if (rotate_object == 2) rotate_object = 0;
		else rotate_object++;
		std::cout << "You are controlling the icosahedron of No." << rotate_object + 1 << "." << std::endl;
		break;
	case 'a':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[1] += 3;
			break;
		case 1:
			rotate_angle_interpolate[1] += 3;
			break;
		case 2:
			rotate_angle_subdivision[1] += 3;
			break;
		default:
			break;
		}
		break;
	case 'd':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[1] -= 3;
			break;
		case 1:
			rotate_angle_interpolate[1] -= 3;
			break;
		case 2:
			rotate_angle_subdivision[1] -= 3;
			break;
		default:
			break;
		}
		break;
	case 'w':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[0] += 3;
			break;
		case 1:
			rotate_angle_interpolate[0] += 3;
			break;
		case 2:
			rotate_angle_subdivision[0] += 3;
			break;
		default:
			break;
		}
		break;
	case 's':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[0] -= 3;
			break;
		case 1:
			rotate_angle_interpolate[0] -= 3;
			break;
		case 2:
			rotate_angle_subdivision[0] -= 3;
			break;
		default:
			break;
		}
		break;
	case 'q':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[2] += 3;
			break;
		case 1:
			rotate_angle_interpolate[2] += 3;
			break;
		case 2:
			rotate_angle_subdivision[2] += 3;
			break;
		default:
			break;
		}
		break;
	case 'e':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[2] -= 3;
			break;
		case 1:
			rotate_angle_interpolate[2] -= 3;
			break;
		case 2:
			rotate_angle_subdivision[2] -= 3;
			break;
		default:
			break;
		}
		break;
	case '+':
		subdivision_depth++;
		std::cout << "You have set the subdivision depth to " << subdivision_depth << "." << std::endl;
		break;
	case '-':
		if (subdivision_depth > 0) subdivision_depth--;
		std::cout << "You have set the subdivision depth to " << subdivision_depth << "." << std::endl;
		break;
	case 'p':
		polygon_mode = !polygon_mode;
		break;
	case 'r':
		switch (rotate_object)
		{
		case 0:
			rotate_angle_flat[0] = 0;
			rotate_angle_flat[1] = 0;
			rotate_angle_flat[2] = 0;
			break;
		case 1:
			rotate_angle_interpolate[0] = 0;
			rotate_angle_interpolate[1] = 0;
			rotate_angle_interpolate[2] = 0;
			break;
		case 2:
			rotate_angle_subdivision[0] = 0;
			rotate_angle_subdivision[1] = 0;
			rotate_angle_subdivision[2] = 0;
			break;
		default:
			break;
		}
		std::cout << "you have reset the icosahedrons of No." << rotate_object << "." << std::endl;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	int select_menu, mode_menu;

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - window_width) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2);
	glutCreateWindow("Shading Subdivision");

	/* Register all callback functions. */
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	init();
	SetupRC();

	readme();

	glutMainLoop();
	return 0;
}