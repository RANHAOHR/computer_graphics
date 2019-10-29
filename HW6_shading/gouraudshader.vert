// minimal vertex shader
// www.lighthouse3d.com

#define MAX_LIGHTS 2 
varying vec4 vertex_color;
uniform float ns_;

void main()
{	

	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);

   for (int i=0; i < MAX_LIGHTS; i++)
   {
		// gl_NormalMatrix = gl_ModelViewMatrix^-T 
		vec3 vertex_ = vec3(gl_ModelViewMatrix * gl_Vertex);
		vec3 normal_ = normalize(gl_NormalMatrix * gl_Normal);

		//compute L, V:
		vec3 L_ = normalize(gl_LightSource[i].position.xyz - vertex_);  
		vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
		//compute H:
		vec3 H_ = (L_ + V_);
		H_ = normalize(H_);

		// ambient:
		vec4 Iamb = gl_FrontLightProduct[i].ambient; 

		//calculate Diffuse Term:  
		vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(dot(normal_, L_), 0.0);  

		// calculate Specular Term:
		vec4 Ispec = gl_FrontLightProduct[i].specular * pow(max(dot(H_, V_), 0.0), ns_);

		finalColor += Iamb + Idiff + Ispec; 

	}

	vertex_color = finalColor;

//	vertex_color = vec4(0.0,1.0,1.0,1.0);  //debug
	gl_Position = ftransform();
}
