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

glm::vec3 p_0,p_ref, delta_normal_;
glm::mat4 M_VW, M_VO, M_WO;

//GLOBAL USED VALUES
float delta_t = 0.1;
float delta_r = 0.1;
bool right_pressed;
bool left_pressed;

int last_x;
int last_y;
glm::vec3 last_x_axis(1,0,0);

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

/* utility functions */
float norm(glm::vec3& vec_){
  return sqrt(vec_.x * vec_.x + vec_.y * vec_.y + vec_.z * vec_.z );
}

glm::mat4 rotate_(float& angle_, float x, float y, float z){

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

  glm::mat3 rot_ = I + R_1 + R_2;
  glm::mat4 R_(rot_[0][0],rot_[0][1],rot_[0][2],0,rot_[1][0],rot_[1][1],rot_[1][2],0,rot_[2][0],rot_[2][1],rot_[2][2],0,0,0,0,1);
  return R_;
}

glm::mat4 translate_(float x, float y, float z){
  glm::mat4 T_(1,0,0,x, 0,1,0,y, 0,0,1,z, 0,0,0,1);
  return T_;
}

glm::mat4 scale_(float s){
  glm::mat4 S_(s,0,0,0, 0,s,0,0, 0,0,s,0, 0,0,0,1);
  return S_;
}

glm::vec4 mat_vec(glm::mat4& mat_, glm::vec4& vec_){
  float v1 = mat_[0][0] * vec_[0] + mat_[0][1] * vec_[1] + mat_[0][2] * vec_[2] + mat_[0][3] * vec_[3];
  float v2 = mat_[1][0] * vec_[0] + mat_[1][1] * vec_[1] + mat_[1][2] * vec_[2] + mat_[1][3] * vec_[3];
  float v3 = mat_[2][0] * vec_[0] + mat_[2][1] * vec_[1] + mat_[2][2] * vec_[2] + mat_[2][3] * vec_[3];

  return glm::vec4(v1, v2, v3, 1);
}
void transform_world(){

  for (int i = 0; i < 4; ++i)
  {
    printf("M_WO %f,%f,%f, %f\n", M_VO[i][0], M_VO[i][1], M_VO[i][2], M_VO[i][3] );
  }
  for (int i = 0; i < verts; ++i)
  {
    glm::vec4 v_(vertList[i].x,vertList[i].y,vertList[i].z, 1);
    // printf("v_ %f,%f,%f\n", v_.x, v_.y, v_.z );
    glm::vec4 v__ = mat_vec(M_VO, v_);
    // printf("v__ %f,%f,%f\n", v__.x, v__.y, v__.z);
    vertList[i].x = v__.x;
    vertList[i].y = v__.y;
    vertList[i].z = v__.z;
  }

}

void initialize(){
  p_0.x = 0;
  p_0.y = 0;
  p_0.z = -5;

  p_ref.x = 0;
  p_ref.y = 0;
  p_ref.z = 0;

  glm::vec3 n_ = p_0 - p_ref;
  n_ = glm::normalize(n_);

  glm::vec3 V(0,1,0); //ususally y-axis of world

  glm::vec3 u_ = glm::cross(V, n_);

  u_ = glm::normalize(u_);

  glm::vec3 v_ = glm::cross(n_, u_);

  glm::mat4 T_;//default is identity
  T_[0][3] = -p_0.x;
  T_[1][3] = -p_0.y;
  T_[2][3] = -p_0.z;
  
  glm::mat4 R_(u_[0],u_[1],u_[2],0, v_[0],v_[1],v_[2],0, n_[0],n_[1],n_[2],0, 0,0,0,1);

  M_VW = R_ * T_;
  for (int i = 0; i < 4; ++i)
  {
    printf("M_VW %f,%f,%f, %f\n", M_VW[i][0], M_VW[i][1], M_VW[i][2], M_VW[i][3] );
  }

  M_VO = M_VW * M_WO; //M_WO is intialize as identity when defined

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
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;

    }

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
    glutSetWindowTitle("Assignment 3 Template (orthogonal)");
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
    gluLookAt(p_0.x, p_0.y, p_0.z, p_ref.x, p_ref.y, p_ref.z, 0, 1, 0); //this should not change
    printf("Look at camera point: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for (int i = 0; i < 4; ++i)
  {
    printf("M_VO %f,%f,%f, %f\n", M_VO[i][0], M_VO[i][1], M_VO[i][2], M_VO[i][3] );
  }


    if (toggle)
    {

      glm::vec3 xw_(M_VW[0][0],M_VW[1][0],M_VW[2][0]);
      glm::vec3 yw_(M_VW[0][1],M_VW[1][1],M_VW[2][1]);
      glm::vec3 zw_(M_VW[0][2],M_VW[1][2],M_VW[2][2]);
      //world frame
      glColor3f(0,0,1);
        glBegin(GL_LINES);
          glVertex3f(M_VW[0][3], M_VW[1][3], M_VW[2][3]);
          glVertex3f(M_VW[0][3] + xw_.x, M_VW[1][3] +xw_.y, M_VW[2][3]+xw_.z); 
        glEnd();

        glColor3f(0,1,0);
        glBegin(GL_LINES);
          glVertex3f(M_VW[0][3], M_VW[1][3], M_VW[2][3]);
          glVertex3f(M_VW[0][3] + yw_.x, M_VW[1][3] +yw_.y, M_VW[2][3]+yw_.z); 
        glEnd();

        glColor3f(1,0,0);
        glBegin(GL_LINES);
          glVertex3f(M_VW[0][3], M_VW[1][3], M_VW[2][3]);
          glVertex3f(M_VW[0][3] + zw_.x, M_VW[1][3] +zw_.y, M_VW[2][3]+zw_.z); 
        glEnd();
        glFlush();
        
        //object frame
        glm::vec3 x_(M_VO[0][0],M_VO[1][0],M_VO[2][0]);
        glm::vec3 y_(M_VO[0][1],M_VO[1][1],M_VO[2][1]);
        glm::vec3 z_(M_VO[0][2],M_VO[1][2],M_VO[2][2]);
        printf("x %f, %f, %f\n", x_.x,x_.y,x_.z);
        printf("y %f, %f, %f\n", y_.x,y_.y,y_.z);
        printf("z %f, %f, %f\n", z_.x,z_.y,z_.z);
       glColor3f(0,0,1);
        glBegin(GL_LINES);
          glVertex3f(M_VO[0][3], M_VO[1][3], M_VO[2][3]);
          glVertex3f(x_.x + M_VO[0][3],x_.y + M_VO[1][3], x_.z + M_VO[2][3]); 
        glEnd();

        glColor3f(0,1,0);
        glBegin(GL_LINES);
          glVertex3f(M_VO[0][3], M_VO[1][3], M_VO[2][3]);
          glVertex3f(y_.x+ M_VO[0][3], y_.y+ M_VO[1][3], y_.z+ M_VO[2][3]); 
        glEnd();

        glColor3f(1,0,0);
        glBegin(GL_LINES);
          glVertex3f(M_VO[0][3], M_VO[1][3], M_VO[2][3]);
          glVertex3f(z_.x+ M_VO[0][3], z_.y+ M_VO[1][3] ,z_.z + M_VO[2][3]); 
        glEnd();
        glFlush();
    }
  // printf("before %f,%f,%f\n", vertList[0].x,vertList[0].y, vertList[0].z);
    transform_world();
  // printf("after %f,%f,%f\n", vertList[0].x,vertList[0].y, vertList[0].z);
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

    // if(button == 0){ // clicked left
    //   left_pressed = true;
    // }else{
    //   left_pressed = false;
    // }

    // if(button == 2){ //click right
    //   right_pressed = true;
    // }else{
    //   right_pressed = false;
    // }

    // last_x = x;
    // last_y = y;

    //update
    display();
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void  mouseMotion(int x, int y)
{
  printf("Mouse is at %d, %d\n", x,y);

  // int delta_x = x - last_x; //horizontal mouse motion
  // int delta_y = y - last_y; //vertial mouse motion

  // if(right_pressed){
  //   if(delta_y > 0){ // UP a vertial motion: closer
  //     p_0 = p_0 - delta_normal_;
  //     printf("p_0 is now: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
  //   }else if(delta_y < 0){ // DOWN : further
  //     p_0 = p_0 + delta_normal_;
  //     printf("p_0 is now: %f, %f, %f\n", p_0.x, p_0.y, p_0.z);
  //   }
  // }
  // if(left_pressed){
  //   if(delta_x > 0){

  //     float delta_ = delta_angle;
  //     glm::mat3 rot_z = rotate_(delta_, 0,1,0 );

  //     p_0 = rot_z * p_0;

  //     last_x_axis = rot_z * last_x_axis;
  //   }else if(delta_x < 0){
  //     float delta_ = -delta_angle;
  //     glm::mat3 rot_z = rotate_(delta_, 0,1,0 );
  //     p_0 = rot_z * p_0;

  //     last_x_axis = rot_z * last_x_axis;
  //   }

  //   if(delta_y > 0){

  //     float delta_ = delta_angle;
  //     glm::mat3 rot_x = rotate_(delta_, last_x_axis[0],last_x_axis[1],last_x_axis[2]);

  //     // for (int i = 0; i < 3; ++i)
  //     // {
  //     //   printf("%f, %f, %f\n", rot_z[i][0], rot_z[i][1],rot_z[i][2]);
  //     // }
 
  //     p_0 = rot_x * p_0;
  //     // printf("1 %f, %f, %f\n", p_0[0], p_0[1],p_0[2]); 

  //   }else if(delta_y < 0){
  //     float delta_ = -delta_angle;
  //     glm::mat3 rot_x = rotate_(delta_, last_x_axis[0],last_x_axis[1],last_x_axis[2]);
  //     p_0 = rot_x * p_0;
  //   }

  // }

  // last_x = x;
  // last_y = y;

  //update dispay
  display();
}



// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void  keyboard(unsigned char key, int x, int y)
{
  float dx,dy,dz,dw,ds;
  glm::mat4 T_;
  glm::mat4 R_;
  glm::mat4 S_;
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
    case 'c':
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
    //rigid body transformations
    case '4': // translate -x in world frame
      dx = -delta_t;
      T_ = translate_(dx,0,0);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;
      printf("4 pressed \n");
      break;
    case '6': //+x in world
      dx = delta_t;
      T_ = translate_(dx,0,0);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;
      break;
    case '8': //+y in world
     dy = delta_t;
      T_ = translate_(0,dy,0);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;    
      break;
    case '2': //-y in world
      dy = -delta_t;
      T_ = translate_(0,dy,0);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;   
     break;
    case '9': //+z in world
      dz = delta_t;
      T_ = translate_(0,0,dz);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;   
     break;
    case '1': //-z in world
      dz = -delta_t;
      T_ = translate_(0,0,dz);
      M_WO = T_ * M_WO;
      M_VO = M_VW * M_WO;  
      break;
    case '[': //rotate -x world
      dw = -delta_r;
      R_ = rotate_(dw, 1, 0, 0);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case ']': //rotate +x world
       dw = delta_r;
      R_ = rotate_(dw, 1, 0, 0);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO;       
      break;
    case ';': //rotate +x world
       dw = -delta_r;
      R_ = rotate_(dw, 0, 1, 0);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case ':': //rotate +x world, having trouble denoting this
       dw = delta_r;
      R_ = rotate_(dw, 0, 1, 0);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case '.': //rotate -z world
       dw = -delta_r;
      R_ = rotate_(dw, 0, 0, 1);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case '/': //rotate +z world
       dw = delta_r;
      R_ = rotate_(dw, 0, 0, 1);
      M_WO = R_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case '=': //+scale world
      ds = 1.1;
      S_ = scale_(ds);
      M_WO = S_ * M_WO;
      M_VO = M_VW * M_WO; 
      break;
    case '-': //-scale world
       ds = 0.9;
      S_ = scale_(ds);
      M_WO = S_ * M_WO;
      M_VO = M_VW * M_WO; 
     break;
    }

    // Schedule a new display event
    glutPostRedisplay();
    display();
}


// Here's the main
int main(int argc, char* argv[])
{

  initialize(); // initial camera view

  std::string object_ = "teapot.obj";
  int sign = -1;
  meshReader (object_.c_str(), sign); // read the file first

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