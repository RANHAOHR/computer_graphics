// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>

#include "shaders.h"
#include <string.h> 
#include <fstream>
#include <iostream>

using namespace std;

LightSpec::LightSpec()
{
	R_ = 1.0;
	G_ = 1.0;
	B_ = 1.0;

	lightCount = 1;

	lightPosition[0] = 2.0; lightPosition[1] = 2.0; lightPosition[2] = 2.0; lightPosition[3] = 1.0; 

	ambientColor[0] = 1.0; ambientColor[1] = 1.0; ambientColor[2] = 1.0; ambientColor[3] = 1.0; 
	dissuseColor[0] = 1.0; dissuseColor[1] = 1.0; dissuseColor[2] = 1.0; dissuseColor[3] = 1.0; 
	specularColor[0] = 1.0; specularColor[1] = 1.0; specularColor[2] = 1.0; specularColor[3] = 1.0; 

}

void LightSpec::lightSpecification(){

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

ShaderSpec::ShaderSpec(	char* file, int illumination){
	illuminationMode = illumination;
	loadMaterials(file);

	cout<< "ns " << ns_ << endl;
	cout<< "d " << d_ << endl;
	cout<< "m " << m_ << endl;
	cout<< "F_0 " << F_0[0] << F_0[1] << F_0[2] << F_0[3] << endl;
	cout<< "Ka " << K_a[0] << K_a[1] << K_a[2] << K_a[3] << endl;
	cout<< "Kd " << K_d[0] << K_d[1] << K_d[2] << K_d[3] << endl;	
	cout<< "Ks " << K_s[0] << K_s[1] << K_s[2] << K_s[3] << endl;
};


char *ShaderSpec::shaderFileRead(char *fn) {


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

void ShaderSpec::setShaders() {

	char *vs = NULL,*fs = NULL;
	glewInit();
	//create the empty shader objects and get their handles
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("phongshader.vert");

 	if (illuminationMode == 0) //phong illumination
	{
		fs = shaderFileRead("phongshader.frag"); //phongshader,
		printf("Using Phong Shader, With Phong Illumination........\n");
	}else{
		fs = shaderFileRead("cooktorrence_phong.frag"); //cooktorrence illumination, phong shader
		printf("Using Phong Shader, With Cook-Torrance Illumination........\n");
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
	SetUniformParams();
}

void ShaderSpec::SetUniformParams(){

	if (illuminationMode == 0) // phong illimination, regardless of shaders
	{
		GLint locns = glGetUniformLocation(program, "ns_");
		if (locns == -1)
	        std::cout << "Warning: can't find uniform variable ns_ !\n";
	    glUniform1f(locns, ns_);

		GLint locKa = glGetUniformLocation(program, "K_a");
		if (locKa == -1)
	        std::cout << "Warning: can't find uniform variable K_a !\n";
	    glUniform4f(locKa, K_a[0], K_a[1], K_a[2], K_a[3] );

		GLint locKd = glGetUniformLocation(program, "K_d");
		if (locKd == -1)
	        std::cout << "Warning: can't find uniform variable K_d !\n";
	    glUniform4f(locKd, K_d[0], K_d[1], K_d[2], K_d[3] );

		GLint locKs = glGetUniformLocation(program, "K_s");
		if (locKs == -1)
	        std::cout << "Warning: can't find uniform variable K_s !\n";
	    glUniform4f(locKs, K_s[0], K_s[1], K_s[2], K_s[3] );
	}

	if (illuminationMode == 1 ) // Cook-Torrance illumination
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

void ShaderSpec::loadMaterials(char* file){
	FILE* materialFile = fopen(file, "r");
	if(!materialFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType[128];
	while(!feof(materialFile))
	{
		fscanf(materialFile, "%s\n", &DataType);
		if(strcmp( DataType, "Ns" ) == 0){
			fscanf(materialFile, "%f\n", &ns_);
		}
		else if(strcmp( DataType, "d" ) == 0){
			fscanf(materialFile, "%f\n", &d_);
		}
		else if(strcmp( DataType, "m" ) == 0){
			fscanf(materialFile, "%f\n", &m_);
		}
		else if(strcmp( DataType, "Rd" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &R_d[0], &R_d[1], &R_d[2]);
			R_d[3] = 1.0;
		}
		else if(strcmp( DataType, "F0" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &F_0[0], &F_0[1], &F_0[2]);
			F_0[3] = 1.0;
		}
		else if(strcmp( DataType, "Ka" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &K_a[0], &K_a[1], &K_a[2]);
			K_a[3] = 1.0;
		}		
		else if(strcmp( DataType, "Kd" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &K_d[0], &K_d[1], &K_d[2]);
			K_d[3] = 1.0;
		}	
		else if(strcmp( DataType, "Ks" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &K_s[0], &K_s[1], &K_s[2]);
			K_s[3] = 1.0;
		}

	}
}