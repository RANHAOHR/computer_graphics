/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 8-MAPPING
Spring 2006

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>

# include "assign8_temp.h"
#include "read_tga.h"

#define PI 3.14159265359

#define PrintOpenGLError()::PrintOGLError(__FILE__, __LINE__)

using namespace std;

void Teapot();
void Plane();
void Sphere();

//object related information
int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

int mapping_code = 0;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
void meshReader (char *filename,int sign);
GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;
int program=-1;
GLuint id;

float max_x, min_x, max_y, min_y, max_z, min_z; 
float obj_h, obj_r, obj_x, obj_y, obj_z;

//Parameters for Copper (From: "Computer Graphics Using OpenGL" BY F.S. Hill, Jr.) 
GLfloat ambient_cont [] = {0.19125,0.0735,0.0225};
GLfloat diffuse_cont [] = {0.7038,0.27048,0.0828};
GLfloat specular_cont [] = {0.256777,0.137622,0.086014};
GLfloat ns_ = 5000;


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

float norm(point &p_){

	float res = sqrt(p_.x * p_.x + p_.y * p_.y + p_.z * p_.z);
	return res;

}

point cross(point &p1, point &p2){

	point res;
	res.x = p1.y * p2.z - p2.y * p1.z;
	res.y = p1.z * p2.x - p2.z * p1.x;
	res.z = p1.x * p2.y - p1.y * p2.x;
	return res;

}
point textureCoordCylinder(point &v_){ // use center based mapping here
	// compute v first: normalize to 0-1

	point coord;
	coord.y = (v_.y -  min_y) / obj_h; 
	float delt_x = v_.x - obj_x;
	float delt_z = v_.z - obj_z;
	float theta_ = atan2(delt_z, delt_x); // from range (-pi , pi)

	coord.x = (theta_ + PI )/  (2 * PI);
 
	return coord;
}

point textureCoordSphere(point &v_){

	// change center
	obj_y = (max_y - min_y) / 2;

	point coord;
	float delt_x = v_.x - obj_x;
	float delt_z = v_.z - obj_z;
	float theta_ = atan2(delt_z, delt_x);

	coord.x = (theta_ + PI )/  (2 * PI);
	float delt_y = v_.y - obj_y;

	float phi_ = atan2(sqrt(delt_x*delt_x + delt_z*delt_z), delt_y); // return (0, pi)
	coord.y = phi_ / PI;

	return  coord;

}

point textureSphrereNormalmapping(point &v_, point &n_){

	float norm_n = norm(n_);

	n_.x = n_.x / norm_n;
	n_.y = n_.y / norm_n;
	n_.z = n_.z / norm_n;

	//find the intersetion between n_ and sphere, using quadratic
	float a = n_.x * n_.x + n_.y * n_.y + n_.z * n_.z;
	float b = 2 * (n_.x * (v_.x - obj_x) + n_.y * (v_.y - obj_y) + n_.z * (v_.z - obj_z));
	float c = (v_.x - obj_x) * (v_.x - obj_x) + (v_.y - obj_y) * (v_.y - obj_y) + (v_.z - obj_z) * (v_.z - obj_z) - obj_r * obj_r;

	float t_ = (-1 *b + sqrt(b * b - 4 * a * c) ) / (2 * a);

	float delta_z = t_ * n_.z + v_.z - obj_z;
	float delta_x = t_ * n_.x + v_.x - obj_x;
	float delta_y = t_ * n_.y + v_.y - obj_y;
	float theta_ = atan2(delta_z, delta_x);

	point coord;

	coord.x = (theta_ + PI )/  (2 * PI);

	float sin_ = sqrt( ( delta_x*delta_x + delta_z*delta_z ) / (obj_r * obj_r) );

	float phi_ = atan2(sin_, delta_y / obj_r); // return (0, pi)
	coord.y = phi_ / PI;

	return coord;
}

point environmentMapping(point &v_, point &n_){

	point v_in;
	v_in.x = CameraRadius*cos(CameraTheta)*sin(CameraPhi) - v_.x;
	v_in.y = CameraRadius*cos(CameraPhi) - v_.y;
	v_in.z = CameraRadius*sin(CameraTheta)*sin(CameraPhi) - v_.z;
	float norm_in = norm(v_in);
	float norm_n = norm(n_);
	float cos_ = (v_in.x * n_.x + v_in.y * n_.y + v_in.z * n_.z) / (norm_in * norm_n);

	v_in.x = v_in.x / norm_in;
	v_in.y = v_in.y / norm_in;
	v_in.z = v_in.z / norm_in;

	n_.x = n_.x / norm_n;
	n_.y = n_.y / norm_n;
	n_.z = n_.z / norm_n;

	point v_out;
	v_out.x = 2 * cos_ * n_.x - v_in.x;
	v_out.y = 2 * cos_ * n_.y - v_in.y;
	v_out.z = 2 * cos_ * n_.z - v_in.z;
	float norm_out = norm(v_out);

	v_out.x = v_out.x / norm_out;
	v_out.y = v_out.y / norm_out;
	v_out.z = v_out.z / norm_out;
	//find the intersetion between v_out and sphere, using quadratic
	float a = v_out.x * v_out.x + v_out.y * v_out.y + v_out.z * v_out.z;
	float b = 2 * (v_out.x * (v_.x - obj_x) + v_out.y * (v_.y - obj_y) + v_out.z * (v_.z - obj_z));
	float c = (v_.x - obj_x) * (v_.x - obj_x) + (v_.y - obj_y) * (v_.y - obj_y) + (v_.z - obj_z) * (v_.z - obj_z) - obj_r * obj_r;

	float t_ = (-1 *b + sqrt(b * b - 4 * a * c) ) / (2 * a);

	float delta_z = t_ * v_out.z + v_.z - obj_z;
	float delta_x = t_ * v_out.x + v_.x - obj_x;
	float delta_y = t_ * v_out.y + v_.y - obj_y;
	float theta_ = atan2(delta_z, delta_x);

	point coord;

	coord.x = (theta_ + PI )/  (2 * PI);

	float sin_ = sqrt( ( delta_x*delta_x + delta_z*delta_z ) / (obj_r * obj_r) );

	float phi_ = atan2(sin_, delta_y / obj_r); // return (0, pi)
	coord.y = phi_ / PI;

	return coord;
}

point bumpMappingPlane(point &v_, point &n_){
	//suppose bumping function sin(u) + cos(v)
	point Pu, Pv;
	Pu.x = 1; Pu.y = 0; Pu.z = 0;
	Pv.x = 0; Pv.y = 1; Pv.z = 0;

	float bu = 2 * cos(v_.x);
	float bv = -2 * sin(v_.y);
	
	point N_;
	point dir_1 = cross(Pu, n_);
	point dir_2 = cross(n_, Pv);

	N_.x = n_.x + bu * dir_1.x + bv * dir_2.x;
	N_.y = n_.y + bu * dir_1.y + bv * dir_2.y;
	N_.z = n_.z + bu * dir_1.z + bv * dir_2.z;

	float norm_ = norm(N_);
	N_.x = N_.x / norm_;
	N_.y = N_.y / norm_;
	N_.z = N_.z / norm_;

	return N_;
}



void mappingFunction(int mapping_method, const char *fileName){

	// Load image from tga file
	TGA *TGAImage	= new TGA(fileName);
	//TGA *TGAImage	= new TGA("./cubicenvironmentmap/cm_right.tga");

	// Use to dimensions of the image as the texture dimensions
	uint width	= TGAImage->GetWidth();
	uint height	= TGAImage->GetHeigth();
	
	// The parameters for actual textures are changed
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable( GL_TEXTURE_2D );

	glBindTexture (GL_TEXTURE_2D, id); 

    delete TGAImage;

	for (int i = 0; i < faces; i++)
	{

		glBegin(GL_TRIANGLES);
			point v1, v2, v3, n1, n2, n3, coord1,coord2, coord3;
			v1 = vertList[faceList[i].v1];
			v2 = vertList[faceList[i].v2];
			v3 = vertList[faceList[i].v3];
			n1 = normList[faceList[i].n1];
			n2 = normList[faceList[i].n2];
			n3 = normList[faceList[i].n3];

			if (mapping_method == 1)
			{
				coord1 = textureCoordCylinder(v1);
				coord2 = textureCoordCylinder(v2);
				coord3 = textureCoordCylinder(v3);	
			}else if(mapping_method == 2){
				coord1 = textureCoordSphere(v1);
				coord2 = textureCoordSphere(v2);
				coord3 = textureCoordSphere(v3);					
			}else if(mapping_method == 3){
				coord1 = environmentMapping(v1, n1);
				coord2 = environmentMapping(v2, n2);
				coord3 = environmentMapping(v3, n3);	
			}else if(mapping_method == 4){
				n1 = bumpMappingPlane(v1, n1);
				n2 = bumpMappingPlane(v2, n2);
				n3 = bumpMappingPlane(v3, n3);

				coord1 = textureSphrereNormalmapping(v1, n1);
				coord2 = textureSphrereNormalmapping(v2, n2);
				coord3 = textureSphrereNormalmapping(v3, n3);	
			}

			glNormal3f(n1.x, n1.y, n1.z);
			glTexCoord2f (coord1.x, coord1.y);
			glVertex3f(v1.x, v1.y, v1.z);
	
			glNormal3f(n2.x, n2.y, n2.z);
			glTexCoord2f (coord2.x, coord2.y);
			glVertex3f(v2.x, v2.y, v2.z);
				
			glNormal3f(n3.x, n3.y, n3.z);
			glTexCoord2f (coord3.x, coord3.y);
			glVertex3f(v3.x, v3.y, v3.z);

		glEnd();

	}	
}

void DisplayFunc(void) 
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  0,0,0,
			  0,1,0);

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);

	//	setParameters(program);
	switch(mapping_code % 8)
	{
	    case 0:{
            Plane();
            const char *path1 = "./planartexturemap/abstract2.tga";
            mappingFunction(1, path1);
            break;

        }
	    case 1:{
	        Sphere();
            const char *path2 = "./planartexturemap/abstract2.tga";
            mappingFunction(1, path2);
            break;
	    }
	    case 2:{
	        Teapot();
            const char *path3 = "./planartexturemap/abstract2.tga";
            mappingFunction(1, path3);
            break;
	    }
	    case 3:{
	        Sphere();
            const char *path4 = "./sphericaltexturemap/earth2.tga";
            mappingFunction(2, path4);
            break;
	    }
	    case 4:{
	        Teapot();
            const char *path5 = "./sphericaltexturemap/earth2.tga";
            mappingFunction(2, path5);
            break;
	    }
	    case 5:{
	        Sphere();
            const char *path6 = "./sphericalenvironmentmap/house2.tga";
            mappingFunction(3, path6);
            break;
	    }
	    case 6:{
	        Teapot();
            const char *path7 = "./sphericalenvironmentmap/house2.tga";
            mappingFunction(3, path7);
            break;
	    }
	    case 7:{
            Plane();
            const char *path8 = "./planarbumpmap/abstract2.tga";
            mappingFunction(4, path8);
            break;
	    }
        default:
            break;
	}

	//glutSolidTeapot(1);
//	setParameters(program);
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
		if (CameraPhi > (PI - 0.01))
			CameraPhi = PI - 0.01;
		if (CameraPhi < 0.01)
			CameraPhi = 0.01;
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

//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		mapping_code+=1;
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
	glutCreateWindow("Assignment 8");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	setShaders();

	glutMainLoop();

	return 0;
}

/*************************************************************
Shader related methods,
Setting the shader files
Setting the shader variables
*************************************************************/

void error_exit(int status, char *text)
{

	// Print error message

	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open

	int anyKey = getchar();

	// Exit program

	exit(status);
}

int PrintOGLError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void setShaders() 
{
	glewInit();
	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("texture_shader.vert");
	fs = shaderFileRead("texture_shader.frag");

	const char * vv = vs;
	const char * ff = fs;

	GLint       vertCompiled, fragCompiled;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    if (!vertCompiled || !fragCompiled)
	{
        cout<<"not compiled"<<endl;
	}
	
	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	program =p;
	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);


	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

	    
	setParameters(p);

}

//Gets the location of the uniform variable given with "name" in the memory
//and tests whether the process was successfull.
//Returns the location of the queried uniform variable
int getUniformVariable(GLuint program,char *name)
{
	int location = glGetUniformLocationARB(program, name);
	
	if (location == -1)
	{
 		error_exit(1007, "No such uniform variable");
	}
	PrintOpenGLError();
	return location;
}

void update_Light_Position()
{
	
	// Create light components
	GLfloat light_position[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi),			  
			  CameraRadius*cos(CameraPhi) , 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	

}

//Sets the light positions, etc. parameters for the shaders
void setParameters(GLuint program)
{
	int light_loc;
	int ambient_loc,diffuse_loc,specular_loc;
	int exponent_loc;

	//sample variable used to demonstrate how attributes are used in vertex shaders.
	//can be defined as gloabal and can change per vertex
	float tangent = 0.0;
	float tangent_loc;

	update_Light_Position();

	GLint locns = glGetUniformLocation(program, "ns_");
	if (locns == -1)
        std::cout << "Warning: can't find uniform variable ns_ !\n";
    glUniform1f(locns, ns_);

	GLint loc = glGetUniformLocation(program, "texSampler");

	if (loc == -1)
        std::cout << "Warning: can't find uniform variable texture !\n";
    glUniform1f(loc, id);
}


/****************************************************************
Utility methods:
shader file reader
mesh reader for objectt
****************************************************************/
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

void meshReader (char *filename,int sign)
{
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

	verts = faces = norms = 0;
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


	max_x = max_y = max_z = -1000;
	min_x = min_y = min_z = 1000;
  // Read the veritces
	for(i = 0;i < verts;i++)
    {
		fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
		vertList[i].x = x;
		vertList[i].y = y;
		vertList[i].z = z;

		if (min_x > x)
		{
			min_x = x;
		}

	    if (max_x < x)
		{
			max_x = x;
		}


	    if (min_y > y)
		{
			min_y = y;
		}


	    if (max_y < y)
		{
			max_y = y;
		}

	    if (min_z > z)
		{
			min_z = z;
		}

	    if (max_z < z)
		{
			max_z = z;
		}
    }

    obj_h = max_y - min_y;
    obj_x = (max_x + min_x) /2;
    obj_z = (max_z + min_z) /2;
    obj_y = min_y;
    if( (max_x - min_x) > (max_z - min_z) ){
    	obj_r = (max_x - min_x);
    }else{
    	obj_r = (max_z - min_z);
    }

  // Read the faces
	for(i = 0;i < faces;i++)
    {
	fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
	faceList[i].v1 = ix - 1;
	faceList[i].v2 = iy - 1;
	faceList[i].v3 = iz - 1;

	faceList[i].n1 = ix - 1;
	faceList[i].n2 = iy - 1;
	faceList[i].n3 = iz - 1;
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

      normList[faceList[i].n1].x = normList[faceList[i].n1].x + crossP.x;
      normList[faceList[i].n1].y = normList[faceList[i].n1].y + crossP.y;
      normList[faceList[i].n1].z = normList[faceList[i].n1].z + crossP.z;
      normList[faceList[i].n2].x = normList[faceList[i].n2].x + crossP.x;
      normList[faceList[i].n2].y = normList[faceList[i].n2].y + crossP.y;
      normList[faceList[i].n2].z = normList[faceList[i].n2].z + crossP.z;
      normList[faceList[i].n3].x = normList[faceList[i].n3].x + crossP.x;
      normList[faceList[i].n3].y = normList[faceList[i].n3].y + crossP.y;
      normList[faceList[i].n3].z = normList[faceList[i].n3].z + crossP.z;
      normCount[faceList[i].n1]++;
      normCount[faceList[i].n2]++;
      normCount[faceList[i].n3]++;
    }
	for (i = 0;i < verts;i++)
    {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}

void Teapot(){
	meshReader("teapot.obj", 1);
}

// void Plane(){
// 	float width_ = 20;
// 	float height_ = 20;
// 	float x_0 = width_ / 2;
// 	float y_0 = height_ / 2;

// 	float z_ = -8.0; 
// 	int sample_x = 10;
// 	int sample_y = 10;

// 	float delta_x = width_ / sample_x;
// 	float delta_y = height_ / sample_y;

// 	int verts = sample_x * sample_y;

// 	vertList = (point *)malloc(sizeof(point)*verts);
// 	normList = (point *)malloc(sizeof(point)*verts);

// 	for (int i = 0; i < sample_y; ++i) //y
// 	{
// 		for (int j = 0; j < sample_x; ++j) //x
// 		{
// 			vertList[i*sample_x+j].x = j * delta_x;
// 			vertList[i*sample_x+j].y = i * delta_y;
// 			vertList[i*sample_x+j].z = z_;

// 			normList[i*sample_x+j].x = 0;
// 			normList[i*sample_x+j].y = 0;
// 			normList[i*sample_x+j].z = 1;				
// 		}
// 	}

// 	int faces = 2 * (sample_y -1 ) *(sample_x -1 );
// 	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
// 	int face_i = 0;
// 	while (face_i < faces)
// 	{
// 		int v1 = face_i / 2;
// 		faceList[face_i].v1 = v1;
// 		faceList[face_i].v2 = v1 + 1;
// 		faceList[face_i].v3 = v1 + sample_x;

// 		faceList[face_i+1].v1 = v1 + 1;
// 		faceList[face_i+1].v2 = v1 + sample_x;
// 		faceList[face_i+1].v3 = v1 + sample_x + 1;

// 		face_i +=2;
// 	}


// }


// Load an object (.obj) file
void load(char* file, int sign)
{
	FILE* pObjectFile = fopen(file, "r");
	if(!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType[128];
	float x, y, z;
	unsigned int v1, v2, v3, t1, t2, t3, n1, n2, n3;
	// Scan the file and count the faces and vertices
	verts = faces = norms = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%s %f %f %f\n", &DataType, &x, &y, &z);
		if(strcmp( DataType, "v" ) == 0)
            verts++;
		else if(strcmp( DataType, "vn" ) == 0)
			norms++;
		else if(strcmp( DataType, "f" ) == 0)
			faces++;
	}
	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*verts);


	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << verts << endl;
	cout << "Number of faces: " << faces << endl;
	cout << "Number of VN: " << norms << endl;

	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentNormal = 0, CurrentTexture = 0, CurrentFace = 0;
	max_x = max_y = max_z = -1000;
	min_x = min_y = min_z = 1000;
	while(!feof(pObjectFile))
	{

        int res = fscanf(pObjectFile, "%s", DataType);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

		if(strcmp( DataType, "v" ) == 0)
		{
			fscanf(pObjectFile, "%f %f %f\n", &x, &y, &z);
			vertList[CurrentVertex].x = x;
			vertList[CurrentVertex].y = y;
			vertList[CurrentVertex].z = z;

			if (min_x > x)
			{
				min_x = x;
			}

		    if (max_x < x)
			{
				max_x = x;
			}


		    if (min_y > y)
			{
				min_y = y;
			}


		    if (max_y < y)
			{
				max_y = y;
			}

		    if (min_z > z)
			{
				min_z = z;
			}

		    if (max_z < z)
			{
				max_z = z;
			}
	    

		    obj_h = max_y - min_y;
		    obj_x = (max_x + min_x) /2;
		    obj_z = (max_z + min_z) /2;
		    obj_y = min_y;
		    if( (max_x - min_x) > (max_z - min_z) ){
		    	obj_r = (max_x - min_x);
		    }else{
		    	obj_r = (max_z - min_z);
		    }
			CurrentVertex++;
		}else if(strcmp( DataType, "vn" ) == 0){
			fscanf(pObjectFile, "%f %f %f\n", &x, &y, &z);
			normList[CurrentNormal].x = x;
			normList[CurrentNormal].y = y;
			normList[CurrentNormal].z = z;
			CurrentNormal++;
		}
		else if(strcmp( DataType, "f" ) == 0)
		{
			fscanf(pObjectFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3 );

			// Convert to a zero-based index for convenience
			faceList[CurrentFace].v1 = v1 - 1;
			faceList[CurrentFace].v2 = v2 - 1;
			faceList[CurrentFace].v3 = v3 - 1;

			faceList[CurrentFace].n1 = n1 - 1;
			faceList[CurrentFace].n2 = n2 - 1;
			faceList[CurrentFace].n3 = n3 - 1;			

			CurrentFace++;
		}
	}

}


void Plane(){
	load("plane.obj", 1);
}

void Sphere(){
	load("sphere.obj", 1);
}