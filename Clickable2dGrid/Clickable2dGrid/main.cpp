#include <iostream>
#include <GL/glut.h>

int window_width = 500;
int window_height = 500;

float cells_number = 0;
bool isClicked = false;
float cell_x_clicked = 0, cell_y_clicked = 0;

void init() 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void idle() 
{
	glutPostRedisplay();
}

void display(void) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, cells_number, 0.0f, cells_number, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	glColor3f(0.6f, 0.2f, 1.0f);
	for (float x = 0; x < cells_number; x++)
	{
		for (float y = 0; y < cells_number; y++)
		{
			glBegin(GL_LINE_LOOP);
			glVertex2f(x, y);
			glVertex2f(x+1, y);
			glVertex2f(x+1, y+1);
			glVertex2f(x, y+1);
			glEnd();
		}
	}

	if (isClicked)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(cell_x_clicked, cell_y_clicked);
		glVertex2f(cell_x_clicked + 1, cell_y_clicked);
		glVertex2f(cell_x_clicked + 1, cell_y_clicked + 1);
		glVertex2f(cell_x_clicked, cell_y_clicked + 1);
		glEnd();
	}

	glFlush();
	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, cells_number, 0.0f, cells_number, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	window_width = width;
	window_height = height;
	std::cout << "current window's size: "<< window_width << "x" << window_height << std::endl;
}

void mainMenu(int id)
{
	switch (id)
	{
	case 1:
		int input_number;
		std::cout << "Please input the number of cells: ";
		std::cin >> input_number;
		if (cells_number != input_number)
		{
			isClicked = false;
			cells_number = input_number;
		}
		break;
	case 2:
		exit(0);
	default:
		break;
	}
}

void selectMenu(int id)
{
	switch (id)
	{
	case 1:
		if (cells_number != 9)
		{
			isClicked = false;
			cells_number = 9;
		}
		break;
	case 2:
		if (cells_number != 25)
		{
			isClicked = false;
			cells_number = 25;
		}
		break;
	case 3:
		if (cells_number != 85)
		{
			isClicked = false;
			cells_number = 85;
		}
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) 
{
	int y_fixed = window_height - y - 1;
	int cell_x, cell_y, cell_x_fixed, cell_y_fixed;

	if (GLUT_LEFT_BUTTON == button && GLUT_UP == state) 
	{
		if (cells_number != 0)
		{
			cell_x = x / (window_width / cells_number);
			cell_x_fixed = cell_x - int(cells_number / 2);
			cell_y = y_fixed / (window_height / cells_number);
			cell_y_fixed = cell_y - int(cells_number / 2);
			std::cout << "The cell you clicked: (" << cell_x_fixed << ", " << cell_y_fixed << ")" << std::endl;

			isClicked = true;
			cell_x_clicked = cell_x;
			cell_y_clicked = cell_y;
		}
	}
}

int main(int argc, char** argv)
{
	int select_menu;

	/* Set graphics window parameters. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - window_width) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - window_height) / 2);
	glutCreateWindow("Clickable 2D Grid");

	/* Register all callback functions. */
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);

	init();

	/* Create the menu structure and attach it to the right mouse button. */
	select_menu = glutCreateMenu(selectMenu);
	glutAddMenuEntry("9x9", 1);
	glutAddMenuEntry("25x25", 2);
	glutAddMenuEntry("85x85", 3);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("Select the number of cells", select_menu);
	glutAddMenuEntry("Input the number of cells", 1);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}