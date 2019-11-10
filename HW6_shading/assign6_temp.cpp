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

#include <fstream>
#include <iostream>

#define PI 3.14159265359

using namespace std;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,program;
int illimunationMode = 1;
int shadingMode = 0;
int lightSource = 0;

int colorInd = 0;
float R_ = 1.0;
float G_ = 1.0;
float B_ = 1.0;

float m_, d_, ns_;
GLfloat ambientColorP[4], dissuseColorP[4], specularColorP[4],ambientColorC[4], dissuseColorC[4], specularColorC[4], F_0[4];

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

void loadMaterials(){
    // filestream variable file 
    fstream file; 
    string parameters, filename; 
  
    // filename of the file 
    filename = "materials.dat"; 
  
    // opening file 
    file.open(filename.c_str()); 
  
    // extracting words from the file 
    string key;

	ambientColorP[3] = 1.0;
	dissuseColorP[3] = 1.0;
	specularColorP[3] = 1.0;

	ambientColorC[3] = 1.0;
	dissuseColorC[3] = 1.0;
	specularColorC[3] = 1.0;
	F_0[3] = 1.0;
 	file >> key >> ambientColorP[0] >> ambientColorP[1] >> ambientColorP[2] >> dissuseColorP[0] >> dissuseColorP[1] >> dissuseColorP[2] >> 
 		specularColorP[0] >> specularColorP[1] >> specularColorP[2] >> ns_ >> key >> ambientColorC[0] >> ambientColorC[1] >> ambientColorC[2] >>
 		dissuseColorC[0] >> dissuseColorC[1] >> dissuseColorC[2] >> specularColorC[0] >> specularColorC[1] >> specularColorC[2] 
 		>> F_0[0] >> F_0[1] >> F_0[2] >> d_ >> m_;
    // while (file >> parameters) 
    // { 
    // 	if (parameters == "P")
    // 	{
    // 		cout << parameters << endl; 
    // 	}
    //     // displaying content 
        
    // } 
// cout << F_0[0] << endl; 
// cout << F_0[1] << endl; 
// cout << F_0[2] << endl; 
// cout << d_ << endl; 
// cout << m_ << endl; 
// cout << ns_ << endl; 

}

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

void loadParams(){

	if (illimunationMode == 0) // phong illimination, regardless of shaders
	{
		GLint locns = glGetUniformLocation(program, "ns_");
		if (locns == -1)
	        std::cout << "Warning: can't find uniform variable ns_ !\n";
	    glUniform1f(locns, ns_);
	}

	if (illimunationMode == 1 ) // Cook-Torrance illumination
	{
		GLint locm = glGetUniformLocation(program, "m_");
		if (locm == -1)
	        std::cout << "Warning: can't find uniform variable m_ !\n";
	    glUniform1f(locm, m_);

		GLint locd = glGetUniformLocation(program, "d_");
		if (locd == -1)
	        std::cout << "Warning: can't find uniform variable d_ !\n";
	    glUniform1f(locd, d_);

		GLint locs = glGetUniformLocation(program, "s_");
		if (locs == -1)
	        std::cout << "Warning: can't find uniform variable s_ !\n";
	    GLfloat s_ = 1 - d_;
	    glUniform1f(locs, s_);

		GLint locF0 = glGetUniformLocation(program, "F_0");
		if (locF0 == -1)
	        std::cout << "Warning: can't find uniform variable F_0 !\n";
	    glUniform4f(locF0, F_0[0], F_0[1], F_0[2], F_0[3] );

		GLint locRd = glGetUniformLocation(program, "R_d");
		if (locRd == -1)
	        std::cout << "Warning: can't find uniform variable R_d !\n";
	    glUniform4f(locRd, F_0[0], F_0[1], F_0[2], F_0[3] );
	}

}

void setShaders() {

	char *vs = NULL,*fs = NULL;
	glewInit();

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	//read the shader files and store the strings in corresponding char. arrays.

	if (shadingMode == 0) // phong shader
	{
		vs = shaderFileRead("phongshader.vert");

	 	if (illimunationMode == 0) //phong illumination
		{
			fs = shaderFileRead("phongshader.frag"); //phongshader,
			printf("Using Phong Shader, With Phong Illumination........\n");
		}else{
			fs = shaderFileRead("cooktorrence_phong.frag"); //cooktorrence illumination, phong shader
			printf("Using Phong Shader, With Cook-Torrance Illumination........\n");
		}
	}else{ //gouraud shader

	 	if (illimunationMode == 0) //phong illumination
		{
			vs = shaderFileRead("phong_gouraudshader.vert");
			printf("Using Gouraud Shader, With Phong Illumination........\n");
		}else{
			vs = shaderFileRead("cooktorrence_gouraud.vert"); //cooktorrence illumination, gouraud shader
			printf("Using Gouraud Shader, With Cook-Torrance Illumination........\n");
		}	
		fs = shaderFileRead("gouraudshader.frag");
	}

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


	//attach the shader objects to the program object
	glAttachShader(program,vertex_shader);
	glAttachShader(program,fragment_shader);
	// define unifrom variables

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

	// set unifom variables
	loadParams();
}

void lightSpecification(){

if (lightSource == 0)
{
	printf("Using Primary light source........\n");
	GLfloat lightPosition[] = {CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),1.0}; //same with camera

	// GLfloat ambientColor[] = {1.0,1.0,1.0,1.0};
	// GLfloat dissuseColor[] = {1.0,1.0,1.0,1.0};
	// GLfloat specularColor[] = {1.0,1.0,1.0,1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	if (illimunationMode == 0)
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColorP);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColorP);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularColorP);
	}else{
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColorC);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColorC);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularColorC);		
	}


}else{
	printf("Using Secondary light source........\n");
	GLfloat lightPosition2[] = {7.0,7.0,7.0,1.0}; //same with camera

	GLfloat ambientColor2[] = {R_,G_,B_,1.0};
	GLfloat dissuseColor2[] = {R_,G_,B_,1.0};
	GLfloat specularColor2[] = {R_,G_,B_,1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor2);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColor2);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor2);
}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
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
			if (colorInd % 4 == 0)
			{
				R_ = 1.0; G_ = 0.0; B_ = 0.0; //Red
			}else if(colorInd % 4 == 1){
				R_ = 0.0; G_ = 1.0; B_ = 0.0; //Green
			}else if(colorInd % 4 == 2){
				R_ = 0.0; G_ = 0.0; B_ = 1.0; //Blue
			}else if(colorInd % 4 == 3){
				R_ = 1.0; G_ = 1.0; B_ = 1.0; //White
			}

			colorInd += 1;

		}
		break;

    default:
		break;
    }

	lightSpecification(); // updated

	setShaders();

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

	loadMaterials();

	lightSpecification();

	setShaders();

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
	// else
	// {
	// 	cout << "Successfully loaded " << fn << endl;
	// }
	
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