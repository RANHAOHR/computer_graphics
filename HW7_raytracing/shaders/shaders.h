#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
class LightSpec
{
public:
	LightSpec();
	float R_;
	float G_;
	float B_;

	int lightCount;
	GLfloat lightPosition[4];
	GLfloat ambientColor[4];
	GLfloat dissuseColor[4];
	GLfloat specularColor[4];
	void lightSpecification();
};

class ShaderSpec
{
public:
	ShaderSpec(char* file, int illumination);
	int illuminationMode;
	void setShaders();

private:		
	float m_, d_, ns_;
	GLfloat F_0[4], R_d[4], K_a[4], K_d[4], K_s[4];
	GLuint vertex_shader,fragment_shader,program;
	char *shaderFileRead(char *fn);

	void SetUniformParams();
	void loadMaterials(char* file);
};
