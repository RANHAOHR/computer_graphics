// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>

#include "shaders.h"
#include <string.h> 
#include <fstream>
#include <iostream>

using namespace std;

ShaderSpec::ShaderSpec(	char* file){
	loadMaterials(file);

	cout<< "ns " << ns_ << endl;
	cout<< "Ka " << K_a[0] << K_a[1] << K_a[2] << K_a[3] << endl;
	cout<< "Kd " << K_d[0] << K_d[1] << K_d[2] << K_d[3] << endl;	
	cout<< "Ks " << K_s[0] << K_s[1] << K_s[2] << K_s[3] << endl;
	cout<< "Ia " << Ia[0] << Ia[1] << Ia[2] << Ia[3] << endl;
	cout<< "Id " << Id[0] << Id[1] << Id[2] << Id[3] << endl;	
	cout<< "Is " << Is[0] << Is[1] << Is[2] << Is[3] << endl;
};


char *ShaderSpec::shaderFileRead(char *fn) {


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

void ShaderSpec::setShaders() {

	char *vs = NULL,*fs = NULL;
	glewInit();
	//create the empty shader objects and get their handles
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("phongshader.vert");


	fs = shaderFileRead("phongshader.frag"); //phongshader,


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

		GLint locns = glGetUniformLocation(program, "ns_");
		if (locns == -1)
	        std::cout << "Warning: can't find uniform variable ns_ !\n";
	    glUniform1f(locns, ns_);

		GLint locKra = glGetUniformLocation(program, "Kra");
		if (locKra == -1)
	        std::cout << "Warning: can't find uniform variable Kra !\n";
	    glUniform1f(locKra, Kra);

		GLint locKre = glGetUniformLocation(program, "Kre");
		if (locKre == -1)
	        std::cout << "Warning: can't find uniform variable Kre !\n";
	    glUniform1f(locKre, Kre);

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

		GLint locIa = glGetUniformLocation(program, "Ia");
		if (locIa == -1)
	        std::cout << "Warning: can't find uniform variable Ia !\n";
	    glUniform4f(locIa, Ia[0], Ia[1], Ia[2], Ia[3] );

		GLint locId = glGetUniformLocation(program, "Id");
		if (locId == -1)
	        std::cout << "Warning: can't find uniform variable Id !\n";
	    glUniform4f(locId, Id[0], Id[1], Id[2], Id[3] );

		GLint locIs = glGetUniformLocation(program, "Is");
		if (locIs == -1)
	        std::cout << "Warning: can't find uniform variable Is !\n";
	    glUniform4f(locIs, Is[0], Is[1], Is[2], Is[3] );	    

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
		else if(strcmp( DataType, "Kre" ) == 0){
			fscanf(materialFile, "%f\n", &Kre);
		}
		else if(strcmp( DataType, "Kra" ) == 0){
			fscanf(materialFile, "%f\n", &Kra);
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
		else if(strcmp( DataType, "Ia" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &Ia[0], &Ia[1], &Ia[2]);
			Ia[3] = 1.0;
		}		
		else if(strcmp( DataType, "Id" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &Id[0], &Id[1], &Id[2]);
			Id[3] = 1.0;
		}	
		else if(strcmp( DataType, "Is" ) == 0){
			fscanf(materialFile, "%f %f %f\n", &Is[0], &Is[1], &Is[2]);
			Is[3] = 1.0;
		}


	}
}