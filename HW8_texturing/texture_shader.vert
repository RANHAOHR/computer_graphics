   
// Input vertex data, different for all executions of this shader.
layout(location = 1) in vec2 vertexUV;
/*
===============================================================================
   Phong Shading: Vertex Program
===============================================================================
*/
varying vec3 vertex_;
varying vec3 normal_;
out vec2 UV;
void main(void)  
{     
   vertex_ = vec3(gl_ModelViewMatrix * gl_Vertex);       
   normal_ = normalize(gl_NormalMatrix * gl_Normal);

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  

   UV = vertexUV;
}
          