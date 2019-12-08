varying vec3 vertex_;
varying vec3 normal_;

void main(void)  
{     
   vertex_ = vec3(gl_ModelViewMatrix * gl_Vertex);       
   normal_ = normalize(gl_NormalMatrix * gl_Normal);

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  
}
          