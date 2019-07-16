#include <iostream>
#include <GL/glut.h>

#define ABS(x)	(x < 0 ? -x : x)
#define SIGN(x)	(x < 0 ? -1 : (x > 0 ? 1 : 0))

int window_width = 500;
int window_height = 500;

float cells_number = 0;
int cells_clicked_number = 0;
bool algorithm_selected = false; //false -- midpoint algorithm, true -- 
float cell_x_clicked_first = 0, cell_y_clicked_first = 0;
float cell_x_clicked_second = 0, cell_y_clicked_second = 0;

void swap(int* a, int*b) // swaps two numbers 
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int ipart(float x) //returns integer part of a floating point number 
{
	return (int)x;
}

float fpart(float x) //returns fractional part of a number 
{
	if (x>0) return x - ipart(x);
	else return x - (ipart(x) + 1);

}

float rfpart(float x) //returns 1 - fractional part of number 
{
	return 1 - fpart(x);
}

void setPixel(int x, int y, float alpha = 1)
{
	glColor4f(1.0f, 0.0f, 0.0f, alpha);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + 1, y);
	glVertex2f(x + 1, y + 1);
	glVertex2f(x, y + 1);
	glEnd();
}

void midpointAlg(int x0, int y0, int x1, int y1)
{
	int	delta_x, delta_y, sign_x, sign_y;
	int	accum;

	delta_x = x1 - x0; delta_y = y1 - y0;
	sign_x = SIGN(delta_x); sign_y = SIGN(delta_y);
	delta_x = ABS(delta_x); delta_y = ABS(delta_y);
	x1 += sign_x; y1 += sign_y;

	if (delta_x > delta_y)
	{
		accum = delta_x >> 1;
		do {
			setPixel(x0, y0);

			accum -= delta_y;
			if (accum < 0)
			{
				accum += delta_x;
				y0 += sign_y;
			}

			x0 += sign_x;
		} while (x0 != x1);
	}
	else {
		accum = delta_y >> 1;
		do {
			setPixel(x0, y0);

			accum -= delta_x;
			if (accum < 0)
			{
				accum += delta_y;
				x0 += sign_x;
			}

			y0 += sign_y;
		} while (y0 != y1);
	}
}

void antiAliasingAlg(int x0, int y0, int x1, int y1)
{
	float dx = ABS(x0 - x1);
	float dy = ABS(y0 - y1);

	bool steep = dy > dx;

	if (steep) {
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if (x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

	dx = x1 - x0;
	dy = y1 - y0;

	float intery; 
	for (int x = x0; x <= x1; x++) {
		intery = y0 + (x - x0) * dy / dx;
		if (x >= 0) {
			if (steep) {
				setPixel(ipart(intery), x, rfpart(intery));
				setPixel(ipart(intery) + 1, x, fpart(intery));
			}
			else {
				setPixel(x, ipart(intery), rfpart(intery));
				setPixel(x, ipart(intery) + 1, fpart(intery));
			}
		}
	}
}

void init() 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	switch (cells_clicked_number)
	{
	case 1:
		setPixel(cell_x_clicked_first, cell_y_clicked_first, 0.0f);
		break;
	case 2:
		if(algorithm_selected)
			antiAliasingAlg(cell_x_clicked_first, cell_y_clicked_first, cell_x_clicked_second, cell_y_clicked_second);
		else
			midpointAlg(cell_x_clicked_first, cell_y_clicked_first, cell_x_clicked_second, cell_y_clicked_second);
		break;
	default:
		break;
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
			cells_clicked_number = false;
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
			cells_clicked_number = false;
			cells_number = 9;
		}
		break;
	case 2:
		if (cells_number != 25)
		{
			cells_clicked_number = false;
			cells_number = 25;
		}
		break;
	case 3:
		if (cells_number != 85)
		{
			cells_clicked_number = false;
			cells_number = 85;
		}
		break;
	default:
		break;
	}
}

void algorithmMenu(int id)
{
	switch (id)
	{
	case 1:
		algorithm_selected = false;
		break;
	case 2:
		algorithm_selected = true;
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

			switch (cells_clicked_number)
			{
			case 0:
				cells_clicked_number++;
				cell_x_clicked_first = cell_x;
				cell_y_clicked_first = cell_y;
				break;
			case 1:
				cells_clicked_number++;
				cell_x_clicked_second = cell_x;
				cell_y_clicked_second = cell_y;
				break;
			case 2:
				cells_clicked_number = 1;
				cell_x_clicked_first = cell_x;
				cell_y_clicked_first = cell_y;
				break;
			default:
				break;
			}
		}
	}
}

int main(int argc, char** argv)
{
	int select_menu, algorithm_menu;

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
	algorithm_menu = glutCreateMenu(algorithmMenu);
	glutAddMenuEntry("Midpoint", 1);
	glutAddMenuEntry("Anti-aliasing", 2);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("Select the number of cells", select_menu);
	glutAddSubMenu("Select the algorithm", algorithm_menu);
	glutAddMenuEntry("Input the number of cells", 1);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}