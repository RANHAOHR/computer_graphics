#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>

class ShaderSpec
{
public:
	ShaderSpec(char* file);
	void setShaders();

private:		
	float ns_, Kre, Kra;
	GLfloat Ia[4], Id[4], Is[4], K_a[4], K_d[4], K_s[4];
	GLuint vertex_shader,fragment_shader,program;
	char *shaderFileRead(char *fn);

	void SetUniformParams();
	void loadMaterials(char* file);
};
