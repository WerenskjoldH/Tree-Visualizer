#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <iostream>
#include <GL\glut.h>

#include "stdio.h"
#include "stdlib.h"

#include <vector>

#include <stack>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT WINDOW_WIDTH

#define NODE_SIZE 10.0

struct vec2 {
	float x;
	float y;
};

struct node {
	node(vec2 pos, node* par, int num)
	{
		this->position	= pos;
		this->parent	= par;
		this->number	= num;
	}

	vec2 position;
	int number;
	std::vector<node*> children;
	node* parent;
};

// Since I don't want to traverse the tree everytime I add a new node I use an arraylist to store all the nodes
std::vector<node*> nodeList;

bool rootPlaced = 0;
bool placingChild = 0;
int parentNodeNum = -1;

vec2 mousePos;
bool mouseState=0;
bool prevMouseState=0;

void traversal()
{
	std::cout << "Traversal: ";

	std::stack<node*> q;
	q.push(nodeList[0]);
	
	while (!q.empty())
	{
		node* t = q.top();

		std::cout << t->number << " ";

		q.pop();
		for (std::vector<node*>::reverse_iterator it = t->children.rbegin(); it != t->children.rend(); ++it)
			q.push((*it));
	}

	std::cout << std::endl;
}

int check_collision(vec2 pos)
{
	for (std::vector<node*>::iterator it = nodeList.begin(); it != nodeList.end(); ++it)
	{
		float dX = (*it)->position.x - pos.x;
		float dY = (*it)->position.y - pos.y;
		if (sqrtf(pow(dX, 2.f) + pow(dY, 2.f)) < NODE_SIZE)
			return (*it)->number;
	}
	return -1;
}

void render()
{

	// Set state values for particles
	glColor3f(0, 0, 0);
	glPointSize(NODE_SIZE);

	for (int x = 0; x < nodeList.size(); x++)
	{		
		// Draw Lines
		glLineWidth(2.5);
		glColor3f(1.f, 0.f, 0.f);
		for (std::vector<node*>::iterator it = nodeList.at(x)->children.begin(); it != nodeList.at(x)->children.end(); ++it)
		{
			glBegin(GL_LINES);
			glVertex3f(nodeList.at(x)->position.x, nodeList.at(x)->position.y, 0.f);
			glVertex3f((*it)->position.x, (*it)->position.y, 0.f);
			glEnd();
		}
		
		// Draw Nodes
		glColor3f(0.f, 0.f, 0.f);
		glBegin(GL_POINTS);
		glVertex2d(nodeList.at(x)->position.x, nodeList.at(x)->position.y);
		glEnd();

	}

}

void update()
{
	// Beginning State ( Placing Root )


	// Parent Selection ( Adding Children )
	/*

		if ( click && !placingChild)
			if ( collision with a node )
				- mark as parent
				- flip placingChild to true
		else if ( click && placingChild)
			- create new node at location with pointer to marked node as parent
			- add pointer to child to parent's children list
			- flip placingChild to false
	*/
	if (mouseState && !prevMouseState && !rootPlaced)
	{
		nodeList.push_back(new node(mousePos, nullptr, nodeList.size()));
		rootPlaced++;
	}
	else if (mouseState && !prevMouseState && rootPlaced && !placingChild)
	{
		int tempNodeNum = check_collision(mousePos);
		if (tempNodeNum >= 0)
		{
			parentNodeNum = tempNodeNum;
			placingChild = 1;
		}
	}
	else if (mouseState && !prevMouseState && rootPlaced && placingChild)
	{
		nodeList.push_back(new node(mousePos, nodeList[parentNodeNum], nodeList.size()));
		nodeList[parentNodeNum]->children.push_back(nodeList[nodeList.size()-1]);
		placingChild = 0;
	}

	prevMouseState = mouseState;
}

void display()
{
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 1.0f);

	update();

	render();

	glutSwapBuffers();
}

// Redisplays the window's normal plane
void idle()
{
	glutPostRedisplay();
}

// Prevents reshaping of the window
void reshape(int width, int height) {
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
}

// Processes keyboard inputs
void process_keyboard(unsigned char key, int x, int y)
{
	if (key == ' ')
	{
		// Preorder traversal
		traversal();
	}
}

// Processes passive mouse movements
void process_mouse_movement(int x, int y)
{
	// This is inefficient, if you want to be more efficient move this to only occur when necessary
	mousePos.x = x;
	mousePos.y = -1 * y + WINDOW_HEIGHT;
}

// Processes mouse inputs
void process_mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		mouseState = 1;
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mouseState = 0;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Tree Visualizer - Hunter W");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);

	glutIgnoreKeyRepeat(false);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(process_keyboard);
	glutPassiveMotionFunc(process_mouse_movement);
	glutMouseFunc(process_mouse);
	glutMainLoop();


    return 0;
}

