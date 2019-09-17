// The template code for Assignment 2 
//

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "glm/gtc/matrix_transform.hpp"

#define ON 1
#define OFF 0
#define PI 3.1415926;

// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = ON;
int object_code = 0;
bool right_coord_system = true;
glm::vec3 p_0,p_ref,normal_, delta_normal_;

float delta_l = 0.02;
float delta_angle = 0.02;
bool right_pressed;
bool left_pressed;

int last_x;
int last_y;
glm::vec3 last_x_axis(1,0,0);

int center_x =0;
int center_y =0;
int center_z =0;

bool toggle = true;
// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;     // Face List


glm::vec3 x_axis(1,0,0);
glm::vec3 y_axis(0,1,0);
glm::vec3 z_axis(0,0,1);

void initialize(){
  p_0.x = 0.297884;
  p_0.y = 1.715014;
  p_0.z = -5;

  p_ref.x = 0;
  p_ref.y = 0;
  p_ref.z = 0;

  normal_ = p_0 - p_ref;
  normal_ = glm::normalize(normal_);
  delta_normal_ = delta_l * normal_;
  printf("normal_ is %f, %f, %f\n", delta_normal_.x,delta_normal_.y, delta_normal_.z);

}



glm::mat3 rotate_(float& angle_, float x, float y, float z){

  glm::mat3 w_hat(0,-z,y,z,0,-x,-y,x,0);
  glm::mat3 I( 1.0f );

  glm::mat3 R_1;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      R_1[i][j] = w_hat[i][j] * sin(angle_);
    }
  }

  glm::mat3 R_2 = w_hat * w_hat;
    for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      R_2[i][j] = R_2[i][j] *(1- cos(angle_));
    }
  }

  return I + R_1 + R_2;
}


void meshReader (const char *filename,int sign)
{
  verts = 0;
  faces = 0;

  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
  {
    verts++;
  }
      else
  {
    faces++;
  }
    }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the veritces
  int totla_x = 0;
  int totla_y = 0;
  int totla_z = 0;

  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;

      totla_x += x;
      totla_y += y;
      totla_z += z;
    }

    center_x = totla_x / verts; // get the center of the object
    center_y = totla_y / verts;    
    center_z = totla_z / verts;

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);

  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
    {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++)
    {
      v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
      v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
      v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
      v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
      v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
      v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
      normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
      normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
      normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
      normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
      normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
      normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
      normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
      normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
      normCount[faceList[i].v1]++;
      normCount[faceList[i].v2]++;
      normCount[faceList[i].v3]++;
    }
      for (i = 0;i < verts;i++)
    {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void  display(void)
{

    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
  if (PERSPECTIVE) {
    // Perpective Projection 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); //to be used by gluPerspective
    gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);

    glutSetWindowTitle("Assignment 2 Template (perspective)");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // applied by glMatrixModel
    // Set the camera position, orientation and target

  }
  else {
    // Orthogonal Projection 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.5, 1.5, -1.5,  1.5, -10000, 10000);
    glutSetWindowTitle("Assignment 2 Template (orthogonal)");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
    gluLookAt(p_0.x, p_0.y, p_0.z, p_ref.x, p_ref.y, p_ref.z, 0, 1, 0);
    printf("Look at camera point: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (toggle)
    {
      //world frame
      glColor3f(0,0,1);
        glBegin(GL_LINES);
          glVertex3f(0, 0, 0);
          glVertex3f(x_axis.x,x_axis.y,x_axis.z); 
        glEnd();

        glColor3f(0,1,0);
        glBegin(GL_LINES);
          glVertex3f(0, 0, 0);
          glVertex3f(y_axis.x,y_axis.y,y_axis.z); 
        glEnd();

        glColor3f(1,0,0);
        glBegin(GL_LINES);
          glVertex3f(0, 0, 0);
          glVertex3f(z_axis.x,z_axis.y,z_axis.z); 
        glEnd();

        //object frame

    }

    std::string object_ = "teapot.obj";
    int sign = -1;
    meshReader (object_.c_str(), sign); // read the file first

    p_ref.x = center_x;
    p_ref.y = center_y;
    p_ref.z = center_z;
    printf("Render a teapot\n");
    for (int i = 0; i < faces; ++i)
    {
      glColor3f(1,0,0);  
      glBegin(GL_POLYGON);
          glVertex3f(vertList[faceList[i].v1].x, vertList[faceList[i].v1].y, vertList[faceList[i].v1].z);
          glVertex3f(vertList[faceList[i].v2].x, vertList[faceList[i].v2].y, vertList[faceList[i].v2].z);
          glVertex3f(vertList[faceList[i].v3].x, vertList[faceList[i].v3].y, vertList[faceList[i].v3].z);
      glEnd();
    }

    // (Note that the origin is lower left corner)
    // (Note also that the window spans (0,1) )
    // Finish drawing, update the frame buffer, and swap buffers
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void  resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    if (PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    }
    printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void  mouseButton(int button,int state,int x,int y)
{
    printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);

    if(button == 0){ // clicked left
      left_pressed = true;
    }else{
      left_pressed = false;
    }

    if(button == 2){ //click right
      right_pressed = true;
    }else{
      right_pressed = false;
    }

    last_x = x;
    last_y = y;

    //update
    display();
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void  mouseMotion(int x, int y)
{
  printf("Mouse is at %d, %d\n", x,y);

  int delta_x = x - last_x; //horizontal mouse motion
  int delta_y = y - last_y; //vertial mouse motion

  if(right_pressed){
    if(delta_y > 0){ // UP a vertial motion: closer
      p_0 = p_0 - delta_normal_;
      printf("p_0 is now: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
    }else if(delta_y < 0){ // DOWN : further
      p_0 = p_0 + delta_normal_;
      printf("p_0 is now: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
    }
  }
  if(left_pressed){
    if(delta_x > 0){

      float delta_ = delta_angle;
      glm::mat3 rot_z = rotate_(delta_, 0,1,0 );

      p_0 = rot_z * p_0;

      last_x_axis = rot_z * last_x_axis;
    }else if(delta_x < 0){
      float delta_ = -delta_angle;
      glm::mat3 rot_z = rotate_(delta_, 0,1,0 );
      p_0 = rot_z * p_0;

      last_x_axis = rot_z * last_x_axis;
    }

    if(delta_y > 0){

      float delta_ = delta_angle;
      glm::mat3 rot_x = rotate_(delta_, last_x_axis[0],last_x_axis[1],last_x_axis[2]);

      // for (int i = 0; i < 3; ++i)
      // {
      //   printf("%f, %f, %f\n", rot_z[i][0], rot_z[i][1],rot_z[i][2]);
      // }
 
      p_0 = rot_x * p_0;
      // printf("1 %f, %f, %f\n", p_0[0], p_0[1],p_0[2]); 

    }else if(delta_y < 0){
      float delta_ = -delta_angle;
      glm::mat3 rot_x = rotate_(delta_, last_x_axis[0],last_x_axis[1],last_x_axis[2]);
      p_0 = rot_x * p_0;
    }

  }

  last_x = x;
  last_y = y;

  //update dispay
  display();
}



// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void  keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case '':                           /* Quit */
      exit(1);
      break;
    case 'a':
        // Toggle Projection Type (orthogonal, perspective)
        if (toggle) {
          // switch from perspective to orthogonal
          toggle = false;
        }
        else {
          // switch from orthogonal to perspective
          toggle = true;
        }
      break;
    case 'q':    
     exit(1);
     break;
    case 'p':
      // Toggle Projection Type (orthogonal, perspective)
      if (PERSPECTIVE) {
        // switch from perspective to orthogonal
        PERSPECTIVE = OFF;
      }
      else {
        // switch from orthogonal to perspective
        PERSPECTIVE = ON;
      }
    break;
    default:
    break;
    }
    display();
    // Schedule a new display event
    glutPostRedisplay();
}


// Here's the main
int main(int argc, char* argv[])
{

  initialize(); // initial camera view

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Assignment 2 Template (orthogonal)");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);
  glutKeyboardFunc(keyboard);

  // Initialize GL
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);

  // Switch to main loop
  glutMainLoop();
  return 0;        
}