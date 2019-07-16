#include <iostream>
#include <algorithm>
#include <GL/glut.h>

#define ABS(x)	(x < 0 ? -x : x)
#define SIGN(x)	(x < 0 ? -1 : (x > 0 ? 1 : 0))

int window_width = 500;
int window_height = 500;

float cells_number = 0;
int cells_clicked_number = 0;
//bool algorithm_selected = false; //false -- midpoint algorithm, true -- 
int draw_mode = 2; //0 --point, 1 --line, 2 -- triangle
float cell_x_clicked_first = 0, cell_y_clicked_first = 0;
float cell_x_clicked_second = 0, cell_y_clicked_second = 0;
float cell_x_clicked_third = 0, cell_y_clicked_third = 0;

static int ipart(float x)
{
	return static_cast<int>(x);
}

static int rnd(float x)
{
	return ipart(x + 0.5f);
}

static float fpart(float x)
{
	return x - ipart(x);
}

static float rfpart(float x)
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

void DrawLineWithAntiAliasing(float x0, float y0, float x1, float y1)
{
	float dx = x1 - x0;
	float dy = y1 - y0;

	if (std::fabs(dx) > std::fabs(dy))
	{
		if (x1 < x0)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		float gradient = dy / dx;
		float xend = static_cast<float>(rnd(x0));
		float yend = y0 + gradient * (xend - x0);
		float xgap = rfpart(x0 + 0.5f);

		int xpxl1 = static_cast<int>(xend);
		int ypxl1 = ipart(yend);

		// Add the first endpoint
		setPixel(xpxl1, ypxl1, rfpart(yend) * xgap);
		setPixel(xpxl1, ypxl1 + 1, fpart(yend) * xgap);

		float intery = yend + gradient;

		xend = static_cast<float>(rnd(x1));
		yend = y1 + gradient * (xend - x1);
		xgap = fpart(x1 + 0.5f);

		int xpxl2 = static_cast<int>(xend);
		int ypxl2 = ipart(yend);

		// Add the second endpoint
		setPixel(xpxl2, ypxl2, rfpart(yend) * xgap);
		setPixel(xpxl2, ypxl2 + 1, fpart(yend) * xgap);

		// Add all the points between the endpoints
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
		{
			setPixel(x, ipart(intery), rfpart(intery));
			setPixel(x, ipart(intery) + 1, fpart(intery));
			intery += gradient;
		}
	}else{
		if (y1 < y0)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		float gradient = dx / dy;
		float yend = static_cast<float>(rnd(y0));
		float xend = x0 + gradient * (yend - y0);
		float ygap = rfpart(y0 + 0.5f);

		int ypxl1 = static_cast<int>(yend);
		int xpxl1 = ipart(xend);

		// Add the first endpoint
		setPixel(xpxl1, ypxl1, rfpart(xend) * ygap);
		setPixel(xpxl1, ypxl1 + 1, fpart(xend) * ygap);

		float interx = xend + gradient;

		yend = static_cast<float>(rnd(y1));
		xend = x1 + gradient * (yend - y1);
		ygap = fpart(y1 + 0.5f);

		int ypxl2 = static_cast<int>(yend);
		int xpxl2 = ipart(xend);

		// Add the second endpoint
		setPixel(xpxl2, ypxl2, rfpart(xend) * ygap);
		setPixel(xpxl2, ypxl2 + 1, fpart(xend) * ygap);

		// Add all the points between the endpoints
		for (int y = ypxl1 + 1; y <= ypxl2 - 1; ++y)
		{
			setPixel(ipart(interx), y, rfpart(interx));
			setPixel(ipart(interx) + 1, y, fpart(interx));
			interx += gradient;
		}
	}
}

bool lineEquation(const int &x0, const int &y0, const int &x1, const int &y1, const int &x2, const int &y2)
{
	int a_x = x1 - x0;
	int a_y = y1 - y0;
	int b_x = x0 - x2;
	int b_y = y0 - y2;
	return a_x * b_y - a_y * b_x >= 0;
}

void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
	if (y0 == y1 && y0 == y2) return; // I dont care about degenerate triangles 

	int max_y = std::max(std::max(y0, y1), y2);
	int min_y = std::min(std::min(y0, y1), y2);
	int max_x = std::max(std::max(x0, x1), x2);
	int min_x = std::min(std::min(x0, x1), x2);

	if (y0 > y1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	if (y0 > y2) {
		std::swap(x0, x2);
		std::swap(y0, y2);
	}	
	if (x1 > x2){
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	for (int i = min_x; i < max_x + 1; i++)
	{
		for (int j = min_y; j < max_y + 1; j++)
		{
			bool c1 = lineEquation(x0, y0, x1, y1, i, j);
			bool c2 = lineEquation(x1, y1, x2, y2, i, j);
			bool c3 = lineEquation(x2, y2, x0, y0, i, j);

			if (c1 && c2 && c3)
				setPixel(i, j);
		}
	}

	DrawLineWithAntiAliasing(x0, y0, x1, y1);
	DrawLineWithAntiAliasing(x1, y1, x2, y2);
	DrawLineWithAntiAliasing(x2, y2, x0, y0);
}
//void DrawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
//	if (y0 == y1 && y0 == y2) return; // I dont care about degenerate triangles 
//    // sort the vertices, t0, t1, t2 lower−to−upper										  
//	if (y0 > y1) {
//		std::swap(x0, x1);
//		std::swap(y0, y1);
//	}
//
//	if (y0 > y2) {
//		std::swap(x0, x2);
//		std::swap(y0, y2);
//	}
//
//	if (y1 > y2) {
//		std::swap(x1, x2);
//		std::swap(y1, y2);
//	}
//
//	int total_height = y2 - y0;
//	for (int i = 0; i < total_height; i++) {
//		bool second_half = i > y1 - y0 || y1 == y0;
//		int segment_height = second_half ? y2 - y1 : y1 - y0;
//		float alpha = (float)i / total_height;
//		float beta = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height; //with above conditions no division by zero
//		int xA = x0 + (x2 - x0)*alpha;
//		int yA = y0 + (y2 - y0)*alpha;
//		int xB = second_half ? x1 + (x2 - x1)*beta : x0 + (x1 - x0)*beta;
//		int yB = second_half ? y1 + (y2 - y1)*beta : y0 + (y1 - y0)*beta;
//		if (xA > xB) {
//			std::swap(xA, xB);
//			std::swap(yA, yB);
//		}
//		for (int j = xA; j <= xB; j++) {
//			setPixel(j, y0 + i); //due to int casts y0+i != yA 
//		}
//	}
//
//	DrawLineWithAntiAliasing(x0, y0, x1, y1);
//	DrawLineWithAntiAliasing(x1, y1, x2, y2);
//	DrawLineWithAntiAliasing(x2, y2, x0, y0);
//}

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
		setPixel(cell_x_clicked_first, cell_y_clicked_first);
		break;
	case 2:
		switch (draw_mode)
		{
		case 0:
			setPixel(cell_x_clicked_first, cell_y_clicked_first);
			setPixel(cell_x_clicked_second, cell_y_clicked_second);
			break;
		case 1: case 2:
			DrawLineWithAntiAliasing(cell_x_clicked_first, cell_y_clicked_first, cell_x_clicked_second, cell_y_clicked_second);
			break;
		default:
			break;
		}

		break;
	case 3:
		switch (draw_mode)
		{
		case 0:
			setPixel(cell_x_clicked_first, cell_y_clicked_first);
			setPixel(cell_x_clicked_second, cell_y_clicked_second);
			setPixel(cell_x_clicked_third, cell_y_clicked_third);
			break;
		case 1:
			DrawLineWithAntiAliasing(cell_x_clicked_first, cell_y_clicked_first, cell_x_clicked_second, cell_y_clicked_second);
			DrawLineWithAntiAliasing(cell_x_clicked_second, cell_y_clicked_second, cell_x_clicked_third, cell_y_clicked_third);
			DrawLineWithAntiAliasing(cell_x_clicked_third, cell_y_clicked_third, cell_x_clicked_first, cell_y_clicked_first);
			break;
		case 2:
			DrawTriangle(cell_x_clicked_first, cell_y_clicked_first, cell_x_clicked_second, cell_y_clicked_second, cell_x_clicked_third, cell_y_clicked_third);
		default:
			break;
		}

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
			cells_clicked_number = 0;
			cells_number = 9;
		}
		break;
	case 2:
		if (cells_number != 25)
		{
			cells_clicked_number = 0;
			cells_number = 25;
		}
		break;
	case 3:
		if (cells_number != 85)
		{
			cells_clicked_number = 0;
			cells_number = 85;
		}
		break;
	default:
		break;
	}
}

void modeMenu(int id)
{
	switch (id)
	{
	case 1:
		draw_mode = 0;
		break;
	case 2:
		draw_mode = 1;
		break;
	case 3:
		draw_mode = 2;
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
				cells_clicked_number++;
				cell_x_clicked_third = cell_x;
				cell_y_clicked_third = cell_y;
				break;
			case 3:
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
	int select_menu, mode_menu;

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
	mode_menu = glutCreateMenu(modeMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Triangle", 3);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("Cells' number ", select_menu);
	glutAddSubMenu("Draw mode", mode_menu);
	glutAddMenuEntry("Input cells' number", 1);
	glutAddMenuEntry("Exit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}