// Assignment4Template.cpp
// Computer Graphics - EECS 366/466 - Spring 2006

// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <GL/glut.h>
#include "Assignment4Classes.h"

using namespace std;

// User Interface Variables
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseMiddle = false;
bool MouseRight = false;
bool PerspectiveMode = true;
bool ShowAxes = true;
bool SelectionMode = false;
int SelectedObject = -1;
bool ShowBoundingBoxes = true;
int WindowWidth = 300, WindowHeight = 300;

// Scene Content Variables
Scene* pDisplayScene;
Camera* pDisplayCamera;

void DisplayFunc()
{
	
	Vertex* input;
	Vertex	temp,temp1,temp2,temp3;
	Vertex  orig,xunit,yunit,zunit;
	
	// vertices to be used in drawing the coordiante axes
	orig.x=orig.y=orig.z=xunit.y=xunit.z=yunit.x=yunit.z=zunit.x=zunit.y=0.0;
	orig.h=xunit.x=xunit.h=yunit.y=yunit.h=zunit.z=zunit.h=1.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	
	//MAY WANT TO MODIFY THIS BASED ON ASSIGNMENT REQUIREMENTS
	gluOrtho2D(-1.0,1.0,-1.0,1.0);
	

	if(PerspectiveMode)
	{
		pDisplayCamera->Perspective();
	}
	else
	{
		pDisplayCamera->Orthographic();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw world coordinate frames
	if(ShowAxes)
	{
		glLineWidth(3.0);
		glBegin(GL_LINES);

		temp = Transform(pDisplayCamera->ViewingMatrix,orig);
		temp1 = Transform(pDisplayCamera->ProjectionMatrix,temp);
		temp = Transform(pDisplayCamera->ViewingMatrix,xunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(1, 0, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,yunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 1, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,zunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 0, 1);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
		glEnd();
		glLineWidth(1.0);
	}

	// draw objects
	for(int i = 0; i < pDisplayScene->ObjectCount; i++)
	{
		// Color the selected object yellow and others blue
		if(i == SelectedObject)
			glColor3f(1, 1, 0);
		else
			glColor3f(0, 0, 1);
		//  draw object faces
		for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
		{
			input = new Vertex[3];
			input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
			input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
			input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

			for (int k=0; k<3; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}

			glBegin(GL_POLYGON);
			for(int k = 0; k < 3; k++)
				glVertex2f(input[k].x/input[k].h, input[k].y/input[k].h);
			glEnd();

			delete [] input;
			input = NULL;
		
		}

		// Draw object coordinate frames
		if(ShowAxes)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,orig);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp2 = Transform(pDisplayCamera->ProjectionMatrix,temp1);
			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,xunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,yunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,zunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);
			glEnd();
			glLineWidth(1.0);
		}

		if(ShowBoundingBoxes)
		{
            Vertex* box_vertex;
            box_vertex = new Vertex[8];
			//ADD YOUR CODE HERE: Draw the bounding boxes
            for (int j = 0; j < 8; ++j) {
                box_vertex[j] = pDisplayScene->pObjectList[i].pBoundingBox[j];
                temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,box_vertex[j]);
                temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
                box_vertex[j] = Transform(pDisplayCamera->ProjectionMatrix,temp2);
            }

            glBegin(GL_LINES);
            glColor3f(1, 0, 0);
            glVertex2f(box_vertex[0].x/box_vertex[0].h, box_vertex[0].y/box_vertex[0].h);
            glVertex2f(box_vertex[1].x/box_vertex[1].h, box_vertex[1].y/box_vertex[1].h);

            glVertex2f(box_vertex[1].x/box_vertex[1].h, box_vertex[1].y/box_vertex[1].h);
            glVertex2f(box_vertex[2].x/box_vertex[2].h, box_vertex[2].y/box_vertex[2].h);

            glVertex2f(box_vertex[2].x/box_vertex[2].h, box_vertex[2].y/box_vertex[2].h);
            glVertex2f(box_vertex[3].x/box_vertex[3].h, box_vertex[3].y/box_vertex[3].h);

            glVertex2f(box_vertex[3].x/box_vertex[3].h, box_vertex[3].y/box_vertex[3].h);
            glVertex2f(box_vertex[0].x/box_vertex[0].h, box_vertex[0].y/box_vertex[0].h);

            glVertex2f(box_vertex[4].x/box_vertex[4].h, box_vertex[4].y/box_vertex[4].h);
            glVertex2f(box_vertex[5].x/box_vertex[5].h, box_vertex[5].y/box_vertex[5].h);

            glVertex2f(box_vertex[5].x/box_vertex[5].h, box_vertex[5].y/box_vertex[5].h);
            glVertex2f(box_vertex[6].x/box_vertex[6].h, box_vertex[6].y/box_vertex[6].h);

            glVertex2f(box_vertex[6].x/box_vertex[6].h, box_vertex[6].y/box_vertex[6].h);
            glVertex2f(box_vertex[7].x/box_vertex[7].h, box_vertex[7].y/box_vertex[7].h);

            glVertex2f(box_vertex[7].x/box_vertex[7].h, box_vertex[7].y/box_vertex[7].h);
            glVertex2f(box_vertex[4].x/box_vertex[4].h, box_vertex[4].y/box_vertex[4].h);

            glVertex2f(box_vertex[0].x/box_vertex[0].h, box_vertex[0].y/box_vertex[0].h);
            glVertex2f(box_vertex[4].x/box_vertex[4].h, box_vertex[4].y/box_vertex[4].h);

            glVertex2f(box_vertex[1].x/box_vertex[1].h, box_vertex[1].y/box_vertex[1].h);
            glVertex2f(box_vertex[5].x/box_vertex[5].h, box_vertex[5].y/box_vertex[5].h);

            glVertex2f(box_vertex[2].x/box_vertex[2].h, box_vertex[2].y/box_vertex[2].h);
            glVertex2f(box_vertex[6].x/box_vertex[6].h, box_vertex[6].y/box_vertex[6].h);

            glVertex2f(box_vertex[3].x/box_vertex[3].h, box_vertex[3].y/box_vertex[3].h);
            glVertex2f(box_vertex[7].x/box_vertex[7].h, box_vertex[7].y/box_vertex[7].h);

            glEnd();
		} 

	} // end of object list

	glutSwapBuffers();
}

// not really accurate from 2d point of view
bool checkBoxIntersection(Vertex& V_, Vertex* box_vertex,  int vi, int vj, int vk){

    printf("box_vertex[0] is %f, %f, %f\n", box_vertex[vi].x, box_vertex[vi].y, box_vertex[vi].z);
    printf("box_vertex[1] is %f, %f, %f\n", box_vertex[vj].x, box_vertex[vj].y, box_vertex[vj].z);
    printf("box_vertex[2] is %f, %f, %f\n", box_vertex[vk].x, box_vertex[vk].y, box_vertex[vk].z);

    Vertex D_, V_1, V_2;
    D_.x = pDisplayCamera->Position.x - box_vertex[vi].x;
    D_.y = pDisplayCamera->Position.y - box_vertex[vi].y;
    D_.z = pDisplayCamera->Position.z - box_vertex[vi].z;

    V_1.x = box_vertex[vj].x - box_vertex[vi].x;
    V_1.y = box_vertex[vj].y - box_vertex[vi].y;
    V_1.z = box_vertex[vj].z - box_vertex[vi].z;

    V_2.x = box_vertex[vk].x - box_vertex[vi].x;
    V_2.y = box_vertex[vk].y - box_vertex[vi].y;
    V_2.z = box_vertex[vk].z - box_vertex[vi].z;

    float a1 = V_.y * D_.x - V_.x * D_.y;
    float a2 = V_.z * D_.y - V_.y * D_.z;

    float b1 = V_.y * V_1.x - V_.x * V_1.y;
    float b2 = V_.z * V_1.y - V_.y * V_1.z;

    float c1 = V_.y * V_2.x - V_.x * V_2.y;
    float c2 = V_.z * V_2.y - V_.y * V_2.z;

    float u = (a1 * c2 - c1*a2) / (b1 * c2 - b2 * c1) ;
    float v = (a2*b1 -a1 * b2) / (b1 * c2 - b2 * c1);

    // float t = (u * V_1.x + v * V_2.x - D_.x) / V_.x;

    bool collide;
    printf("u is %f\n", u);
    printf("v is %f\n", v);

    if(v >= 0 && u >= 0 && (u+v) <= 1){
        collide = true;
        printf("collide is true\n");
    } else{ collide = false;}

    return collide;
}

// orthogonal view...
bool checkBoxIntersection(float xw, float yw, Vertex* box_vertex,  int vi, int vj, int vk){

	Vertex V_; // parallel to n
	V_.x = pDisplayCamera->n.i;
	V_.y = pDisplayCamera->n.j;
	V_.z = pDisplayCamera->n.k;

    printf("box_vertex[0] is %f, %f, %f\n", box_vertex[vi].x, box_vertex[vi].y, box_vertex[vi].z);
    printf("box_vertex[1] is %f, %f, %f\n", box_vertex[vj].x, box_vertex[vj].y, box_vertex[vj].z);
    printf("box_vertex[2] is %f, %f, %f\n", box_vertex[vk].x, box_vertex[vk].y, box_vertex[vk].z);

    Vertex D_, V_1, V_2;
    D_.x = xw - box_vertex[vi].x;
    D_.y = yw - box_vertex[vi].y;
    D_.z = -pDisplayCamera->ViewPlane - box_vertex[vi].z;

    V_1.x = box_vertex[vj].x - box_vertex[vi].x;
    V_1.y = box_vertex[vj].y - box_vertex[vi].y;
    V_1.z = box_vertex[vj].z - box_vertex[vi].z;

    V_2.x = box_vertex[vk].x - box_vertex[vi].x;
    V_2.y = box_vertex[vk].y - box_vertex[vi].y;
    V_2.z = box_vertex[vk].z - box_vertex[vi].z;

    float a1 = V_.y * D_.x - V_.x * D_.y;
    float a2 = V_.z * D_.y - V_.y * D_.z;

    float b1 = V_.y * V_1.x - V_.x * V_1.y;
    float b2 = V_.z * V_1.y - V_.y * V_1.z;

    float c1 = V_.y * V_2.x - V_.x * V_2.y;
    float c2 = V_.z * V_2.y - V_.y * V_2.z;

    float u = (a1 * c2 - c1*a2) / (b1 * c2 - b2 * c1) ;
    float v = (a2*b1 -a1 * b2) / (b1 * c2 - b2 * c1);

    bool collide;
    printf("u is %f\n", u);
    printf("v is %f\n", v);

    if(v >= 0 && u >= 0 && (u+v) <= 1){
        collide = true;
        printf("collide is true\n");
    } else{ collide = false;}

    return collide;
}

void ReshapeFunc(int x,int y)
{
	// Get the correct view proportionality for the new window size
	pDisplayCamera->ViewWidth = pDisplayCamera->ViewWidth*((float)x/WindowWidth);
	pDisplayCamera->ViewHeight = pDisplayCamera->ViewHeight*((float)y/WindowHeight);
	glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !state;

	if(MouseLeft && SelectionMode)
	{
		// Select a new object with (x,y) 
		//ADD YOUR CODE HERE: Select a new object by intersecting the selection ray
        printf("pDisplayCamera->ViewWidth is %f\n", pDisplayCamera->ViewWidth);
        printf("pDisplayCamera->ViewHeight is %f\n", pDisplayCamera->ViewHeight);

        printf("WindowWidth is %d\n", WindowWidth);
        printf("WindowHeight is %d\n", WindowHeight);

        float inverseViewportMatrix[9];

        inverseViewportMatrix[0] = pDisplayCamera->ViewWidth / WindowWidth ; // pDisplayCamera->ViewWidth / WindowWidth;
        inverseViewportMatrix[4] = -pDisplayCamera->ViewHeight / WindowHeight; // -pDisplayCamera->ViewHeight / WindowHeight;

        inverseViewportMatrix[6] = -pDisplayCamera->ViewWidth / 2;
        inverseViewportMatrix[7] = pDisplayCamera->ViewHeight / 2;
        inverseViewportMatrix[8] = 1;

        inverseViewportMatrix[1] = inverseViewportMatrix[2] = inverseViewportMatrix[3] = inverseViewportMatrix[5] = 0;

        float x_w = inverseViewportMatrix[0] * MouseX + inverseViewportMatrix[3] *MouseY + inverseViewportMatrix[6];
        float y_w = inverseViewportMatrix[1] * MouseX + inverseViewportMatrix[4] *MouseY + inverseViewportMatrix[7];

        printf("MouseX is %d\n", MouseX);
        printf("MouseY is %d\n", MouseY);

        printf("x_w is %f\n", x_w);
        printf("y_w is %f\n", y_w);

        Vertex V_;
        Vertex* box_vertex;
        Vertex	temp,temp2;

		int select = -1;
        if (PerspectiveMode)
        {
	        V_.x = x_w - pDisplayCamera->Position.x;
	        V_.y = y_w - pDisplayCamera->Position.y;
	        V_.z = pDisplayCamera->ViewPlane - pDisplayCamera->Position.z; // -d

        	for (int i = 0; i < pDisplayScene->ObjectCount; ++i) {
	            //ADD YOUR CODE HERE: Draw the bounding boxes
	            box_vertex = new Vertex[8];
	            for (int j = 0; j < 8; ++j) {
	                box_vertex[j] = pDisplayScene->pObjectList[i].pBoundingBox[j];
	                temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,box_vertex[j]);
	                temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
	                box_vertex[j] = Transform(pDisplayCamera->ProjectionMatrix,temp2);
	            }

	            if(checkBoxIntersection(x_w, y_w, box_vertex,  0, 1, 2) || checkBoxIntersection(x_w, y_w, box_vertex,  0, 3, 2) ||
	                   checkBoxIntersection(x_w, y_w, box_vertex,  4, 7, 6) || checkBoxIntersection(x_w, y_w, box_vertex,  4, 5, 6) ||
	                   checkBoxIntersection(V_, box_vertex,  0, 4, 5) || checkBoxIntersection(V_, box_vertex,  0, 1, 5) ||
	                   checkBoxIntersection(V_, box_vertex,  3, 7, 6) || checkBoxIntersection(V_, box_vertex,  3, 2, 6) ||
	                   checkBoxIntersection(V_, box_vertex,  1, 5, 6) || checkBoxIntersection(V_, box_vertex,  1, 2, 6) ||
	                    checkBoxIntersection(V_, box_vertex,  0, 4, 7) || checkBoxIntersection(V_, box_vertex,  0, 3, 7)
	                )
	            {
	                select = i;
	            } 

        	}

        }else{

        	for (int i = 0; i < pDisplayScene->ObjectCount; ++i) {
	            //ADD YOUR CODE HERE: Draw the bounding boxes
	            box_vertex = new Vertex[8];
	            for (int j = 0; j < 8; ++j) {
	                box_vertex[j] = pDisplayScene->pObjectList[i].pBoundingBox[j];
	                temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,box_vertex[j]);
	                temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
	                box_vertex[j] = Transform(pDisplayCamera->ProjectionMatrix,temp2);
	            }

	            if(checkBoxIntersection(x_w, y_w, box_vertex,  0, 1, 2) || checkBoxIntersection(x_w, y_w, box_vertex,  0, 3, 2) ||
	                   checkBoxIntersection(x_w, y_w, box_vertex,  4, 7, 6) || checkBoxIntersection(x_w, y_w, box_vertex,  4, 5, 6) ||
	                   checkBoxIntersection(x_w, y_w, box_vertex,  0, 4, 5) || checkBoxIntersection(x_w, y_w, box_vertex,  0, 1, 5) ||
	                   checkBoxIntersection(x_w, y_w, box_vertex,  3, 7, 6) || checkBoxIntersection(x_w, y_w, box_vertex,  3, 2, 6) ||
	                   checkBoxIntersection(x_w, y_w, box_vertex,  1, 5, 6) || checkBoxIntersection(x_w, y_w, box_vertex,  1, 2, 6) ||
	                    checkBoxIntersection(x_w, y_w, box_vertex,  0, 4, 7) || checkBoxIntersection(x_w, y_w, box_vertex,  0, 3, 7)
	                )
	            {
	                select = i;
	            } 

        	}

        }


		SelectedObject = select;	
		glutPostRedisplay();
	}
}

void MotionFunc(int x, int y)
{
	if(MouseLeft && !SelectionMode)
		pDisplayCamera->Pan(((float) x - MouseX)/128, ((float) y - MouseY)/128);

    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'O':
	case 'o':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(M_PI/32, 0, 0);
		break;
	case 'I':
	case 'i':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(-M_PI/32, 0, 0);
		break;
	case 'L':
	case 'l':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, M_PI/32, 0);
		break;
	case 'K':
	case 'k':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, -M_PI/32, 0);
		break;
	case ',':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, M_PI/32);
		break;
	case 'M':
	case 'm':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, -M_PI/32);
		break;
	case '6':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(M_PI/32, 0, 0);
		break;
	case '4':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(-M_PI/32, 0, 0);
		break;
	case '8':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, M_PI/32, 0);
		break;
	case '2':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, -M_PI/32, 0);
		break;
	case '9':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, M_PI/32);
		break;
	case '1':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, -M_PI/32);
		break;
	case ']':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(M_PI/32, 0, 0);
		break;
	case '[':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(-M_PI/32, 0, 0);
		break;
	case 39:	// Apostrophe
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, M_PI/32, 0);
		break;
	case 59:	// Semicolon
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, -M_PI/32, 0);
		break;
	case '/':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, M_PI/32);
		break;
	case '.':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, -M_PI/32);
		break;
	case '=':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(1.05);
		break;
	case '-':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(0.95);
		break;
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'B':
	case 'b':
		ShowBoundingBoxes = !ShowBoundingBoxes;
		break;
	case 'N':
	case 'n':
		SelectionMode = !SelectionMode;
		if(SelectionMode)
			cout << "Selection Mode: select an object" << endl;
		else
			cout << "Camera Mode" << endl;
		break;
	case 'P':
	case 'p':
		PerspectiveMode = !PerspectiveMode;
		if(PerspectiveMode)
			glutSetWindowTitle("Assignment 5 (Perspective)");
		else
			glutSetWindowTitle("Assignment 5 (Orthogonal)");
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'Y':
	case 'y':
		pDisplayCamera->MoveView(0.5);
		break;
	case 'U':
	case 'u':
		pDisplayCamera->MoveView(-0.5);
		break;
	case 'H':
	case 'h':
		pDisplayCamera->ScaleView(0.95);
		break;
	case 'J':
	case 'j':
		pDisplayCamera->ScaleView(1.05);
		break;
    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{

    pDisplayScene = new Scene;
	//YOU MAY ENTER YOUR OWN SCENE FILE OR PASS IT AS AN ARGUMENT TO THE PROGRAM
	pDisplayScene->Load("scene2.dat");
	pDisplayCamera = new Camera;
	pDisplayCamera->ViewWidth = (float)WindowWidth/32;
	pDisplayCamera->ViewHeight = (float)WindowHeight/32;

	// Initialize GLUT
    glutInit(&argc, argv);
	glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 4 (Perspective)");
    glutDisplayFunc(DisplayFunc);
    glutReshapeFunc(ReshapeFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

    // Switch to main loop
    glutMainLoop();

	return 0;
}