#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "shaders/shaders.h"
#include "camera_geometry/camera_geometry.h"
#include "frame_buffer.h"
#include "primitives.h"
#include "color.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>

#define ON 1
#define OFF 0

using namespace std;

// Global variables
int window_width = 500, window_height = 500;    // Window dimensions

const int INITIAL_RES = 400;

FrameBuffer* fb;

Scene* pDisplayScene;
Camera* pDisplayCamera;
Sphere pObjectSphere(-window_height,window_width,3,100);

ShaderSpec sphereShader("sphere.mtl");

glm::vec3 lightPosition(0.0,0.0,0.0);
glm::vec3 ambientColor(1.0,1.0,1.0);
glm::vec3 diffuseColor(1.0,1.0,1.0);
glm::vec3 specularColor(1.0,1.0,1.0);

float ModelMatrix[16];
int disk_x;
int disk_y;

void initModelMatrix(){
    ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
    ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
    ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
    ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14]= 0;
}
void drawRect(double x, double y, double w, double h)
{
	glVertex2f(x,y);
	glVertex2f(x+w,y);
	glVertex2f(x+w,y+h);
	glVertex2f(x, y+h);
}

Color rayTracing( glm::vec3 &vert, glm::vec3 &normal, glm::vec3 &I_local){

    Color I_global;
    glm::vec3 cam_position(pDisplayCamera->Position.x, pDisplayCamera->Position.y, pDisplayCamera->Position.z );
    glm::vec3 in_vec = glm::normalize(vert - cam_position);
    float cos_i = glm::dot(in_vec,normal);
    glm::vec3 I_reflect = in_vec + 2 * cos_i * normal;
    I_reflect = glm::vec3(I_reflect);
    glm::vec3 L_ = lightPosition - vert;
    float cos_r = glm::min(cos_i + (float)0.1, (float)1.0);
    glm::vec3 I_refract = cos_r * normal + sqrt( (1 - cos_i * cos_i)/ (1 - cos_r * cos_r) ) * (cos_i * normal -  L_);
    I_global.r = I_local[0] + sphereShader.Kre * I_reflect[0] + sphereShader.Kra * I_refract[0];
    I_global.g = I_local[1] + sphereShader.Kre * I_reflect[1] + sphereShader.Kra * I_refract[1];
    I_global.b = I_local[2] + sphereShader.Kre * I_reflect[2] + sphereShader.Kra * I_refract[2];

    cout << "r " <<  I_global.r << endl;
    cout << "g " <<  I_global.g << endl;
    cout << "b " <<  I_global.b << endl;

    return I_global;
}

glm::vec3 PhongShader(glm::vec3 &normal, glm::vec3 &vert ){
    //compute L, V:
    glm::vec3 L_ = lightPosition - vert;
    glm::vec3 V_ = glm::normalize(-vert); // reverse because its from vertex to camera
    //compute H:
    glm::vec3 H_ = (L_ + V_);
    H_ = glm::normalize(H_);

    // ambient:
    glm::vec3 K_a;
    K_a[0] = sphereShader.K_a[0];
    K_a[1] = sphereShader.K_a[1];
    K_a[2] = sphereShader.K_a[2];
    glm::vec3 Iamb = K_a * ambientColor;

    //calculate Diffuse Term:
    glm::vec3 K_d;
    K_d[0] = sphereShader.K_d[0];
    K_d[1] = sphereShader.K_d[1];
    K_d[2] = sphereShader.K_d[2];
    glm::vec3 Idiff = K_d * diffuseColor * glm::max(glm::dot(normal, L_), (float)0.0);

    // calculate Specular Term:
    glm::vec3 K_s;
    K_s[0] = sphereShader.K_s[0];
    K_s[1] = sphereShader.K_s[1];
    K_s[2] = sphereShader.K_s[2];
    float ns_ = sphereShader.ns_;
    glm::vec3 Ispec = K_s * specularColor * pow(glm::max(dot(H_,V_), (float)0.0), ns_);
    Ispec = glm::clamp(Ispec, (float)0.0, (float)1.0);  // between 0-1

    glm::vec3 localColor =  Iamb + Idiff + Ispec;

    return localColor;
}

float computeZ(int x_, int y_){
    float z = sqrt(pObjectSphere.r * pObjectSphere.r - ( (float)x_ - disk_x ) * ( (float)x_ - disk_x ) + ( (float)y_ - disk_y ) * ( (float)y_ - disk_y ) );

    return z;
}

glm::vec3 computeNormal(float x, float y, float z){

    float x_ = x - disk_x;
    float y_ = y - disk_y;

    float norm = sqrt(x_ * x_ + y_ * y_ + z * z);
    glm::vec3 normal_(x_ / norm, y_/norm, z/norm);

    return normal_;
}

void SpherePrimitive(FrameBuffer* fb, int x_, int y_, float radius_){

    for (int x = 0; x < fb->GetWidth(); ++x)
    {
        for (int y = 0; y < fb->GetHeight(); ++y)
        {
            float distance_ = sqrt((y - y_) * (y - y_) + (x - x_) * (x - x_) );

            if (distance_ <= radius_)
            {
                float z = computeZ(x, y);

                glm::vec3 normal_ = computeNormal(x, y, z);
                glm::vec3 vert_(x, y, z);
                glm::vec3 I_local = PhongShader(normal_, vert_ );

                fb->buffer[x][y] = rayTracing(vert_, normal_, I_local);
            }
        }
    }
}

void polygonPrimitive(FrameBuffer* fb, int x_, int y_) {

    Vertex *input_v;
    Vertex *input_n;
    Vertex* output_v;
    int length = 0;
    Vertex temp, temp1, temp2;
    for (int i = 0; i < pDisplayScene->ObjectCount; i++) {
        // Clip and draw object faces
        for (int j = 0; j < pDisplayScene->pObjectList[i].faces; j++) {
            input_v = new Vertex[3];
            input_v[0] = pDisplayScene->pObjectList[i].vertList[pDisplayScene->pObjectList[i].faceList[j].v1];
            input_v[1] = pDisplayScene->pObjectList[i].vertList[pDisplayScene->pObjectList[i].faceList[j].v2];
            input_v[2] = pDisplayScene->pObjectList[i].vertList[pDisplayScene->pObjectList[i].faceList[j].v3];

            input_n = new Vertex[3];
            input_n[0] = pDisplayScene->pObjectList[i].normList[pDisplayScene->pObjectList[i].faceList[j].v1];
            input_n[1] = pDisplayScene->pObjectList[i].normList[pDisplayScene->pObjectList[i].faceList[j].v2];
            input_n[2] = pDisplayScene->pObjectList[i].normList[pDisplayScene->pObjectList[i].faceList[j].v3];

            for (int k = 0; k < 3; k++) {
                temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix, input_v[k]);
                temp1 = Transform(pDisplayCamera->ViewingMatrix, temp);
                input_v[k] = Transform(pDisplayCamera->ProjectionMatrix, temp1);

                //TODO:
                temp2 = Transform(pDisplayScene->pObjectList[i].ModelMatrix, input_n[k]);
                input_n[k] = Transform(pDisplayCamera->ViewingMatrix, temp2);

            }

            output_v = ClipPolygon(input_v, &length);
            for(int k = 0; k < length; k++){

                int x = output_v[k].x/output_v[k].h;
                int y = output_v[k].y/output_v[k].h;
                glm::vec3 normal_(input_n[k].x,input_n[k].y,input_n[k].z);
                glm::vec3 vert_(input_v[k].x,input_v[k].y,input_v[k].z);
                glm::vec3 I_local = PhongShader(normal_, vert_ );

                fb->buffer[x][y] = rayTracing(vert_, normal_, I_local);
            }


            delete[] input_v;
            input_v = NULL;
        }

    }
}
// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	double w = 10/double(fb->GetWidth());
	double h = 10/double(fb->GetHeight());

	Color cl;
	glColor3f(0,0,1);

	glBegin(GL_QUADS);

	printf("width %d, height %d\n", fb->GetWidth(), fb->GetHeight());

    Vertex input;
    input.x = pObjectSphere.x_0;
    input.y = pObjectSphere.y_0;
    input.z = pObjectSphere.z_0;
    cout << "input x "<< input.x << endl;
    cout << "input y "<< input.y << endl;
    cout << "input z "<< input.z << endl;
    cout << "input h "<< input.h << endl;
    Vertex	temp,temp1,temp2;
    temp = Transform(ModelMatrix,input);
    temp2 = Transform(pDisplayCamera->ViewingMatrix,temp);

    input = Transform(pDisplayCamera->ProjectionMatrix,temp2);

    disk_x = input.x/input.h;
    disk_y = input.y/input.h;

//    SpherePrimitive(fb, disk_x, disk_y, pObjectSphere.r);


    for(int y = 0; y < fb->GetHeight(); y++)
	{
		for(int x = 0; x < fb->GetWidth(); x++)
		{
			cl = fb->buffer[x][y].color;
			glColor3f(cl.r, cl.g, cl.b);
			drawRect(w*x, h*y, w, h);
		}
	}

	glEnd();
  glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    
    printf("Resized to %d %d\n",x,y);
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
   ;
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	;
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case 'q':                           /* Quit */
		exit(1);
		break;
	case '-':
		fb->Resize(fb->GetHeight()/2, fb->GetWidth()/2);
            SpherePrimitive(fb, disk_x, disk_y, pObjectSphere.r);
//		  SpherePrimitive(fb, fb->GetWidth()*0.5, fb->GetHeight()*0.5, 10.0, Color(1,0,0));
		break;
	case '=':
		fb->Resize(fb->GetHeight()*2, fb->GetWidth()*2);
            SpherePrimitive(fb, disk_x, disk_y, pObjectSphere.r);
//		  SpherePrimitive(fb, fb->GetWidth()*0.5, fb->GetHeight()*0.5, 10.0, Color(1,0,0));
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{
    pDisplayCamera = new Camera;
    pDisplayCamera->ViewWidth = (float)window_width/32;
    pDisplayCamera->ViewHeight = (float)window_height/32;

    pDisplayCamera->Perspective();

	fb = new FrameBuffer(INITIAL_RES, INITIAL_RES);
    initModelMatrix();

   pDisplayScene = new Scene;
   pDisplayScene->Load("teapot.dat");

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,10,0,10,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
