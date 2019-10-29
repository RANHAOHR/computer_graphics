/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 6-Local Illumination and Shading

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>

#include <GL/glew.h>

#include <GL/freeglut.h>

#define PI 3.14159265359

using namespace std;
 

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,program;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;


//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;


float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;



void DisplayFunc(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	glutSolidTeapot(1);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}


void setShaders() {

	char *vs = NULL,*fs = NULL;
	glewInit();

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("phongshader.vert"); //phongshader, gouraudshader
	fs = shaderFileRead("phongshader.frag");

	const char * vv = vs;
	const char * ff = fs;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSource(vertex_shader, 1, &vv,NULL);
	glShaderSource(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);


	//create an empty program object to attach the shader objects
	program = glCreateProgram();

	// define unifrom variables
	GLint location = glGetUniformLocation(program,"ns_");
	GLfloat ns_ = { 10.0f };
	glUniform1f(location ,ns_);

	//F_0
	location = glGetUniformLocation(program,"F_0");
	GLfloat F_0[] = { 0.755, 0.49, 0.095, 1.0 };
	glUniform4fv(location ,4, F_0);

	location = glGetUniformLocation(program,"d_");
	GLfloat d_ = { 0.3 };
	glUniform1f(location ,d_);

	location = glGetUniformLocation(program,"s_");
	GLfloat s_ = 1 - d_;
	glUniform1f(location ,s_);

	location = glGetUniformLocation(program,"R_d");
	GLfloat R_d[] = { 0.755, 0.49, 0.095, 1.0 };
	glUniform4fv(location ,4, R_d);

	location = glGetUniformLocation(program,"m_");
	GLfloat m_ = { 0.1f };
	glUniform1f(location ,m_);

	//attach the shader objects to the program object
	glAttachShader(program,vertex_shader);
	glAttachShader(program,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgram(program);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgram(program);

}

void lightSpecification(){

	GLfloat lightPosition[] = {CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),1.0}; //same with camera

	GLfloat ambientColor[] = {1.0,0.0,0.0,1.0};
	GLfloat dissuseColor[] = {1.0,0.0,0.0,1.0};
	GLfloat specularColor[] = {1.0,0.0,0.0,1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);


	GLfloat lightPosition2[] = {7.0,7.0,7.0,1.0}; //same with camera

	GLfloat ambientColor2[] = {0.0,0.0,1.0,1.0};
	GLfloat dissuseColor2[] = {0.0,0.0,1.0,1.0};
	GLfloat specularColor2[] = {0.0,0.0,1.0,1.0};

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientColor2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dissuseColor2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularColor2);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}


//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
		}
		else
		{
			illimunationMode = 0;
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	case 'd':
	case 'D':
		if (lightSource == 0)
		{
			lightSource =1;
		}
		else
		{
			lightSource =0;
		}
		break;
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	lightSpecification();

	setShaders();

	// const GLubyte *temp;
	// temp=glGetString(GL_VERSION);
	// printf("%s\n",temp);
	// temp=glGetString(GL_VENDOR);
	// printf("%s\n",temp);
	// temp=glGetString(GL_EXTENSIONS);
	// printf("%s\n",temp);

	glutMainLoop();

	return 0;
}


//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}