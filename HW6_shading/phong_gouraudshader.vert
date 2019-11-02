// minimal vertex shader
// www.lighthouse3d.com

varying vec4 vertex_color;
uniform float ns_;

void main()
{	

	// gl_NormalMatrix = gl_ModelViewMatrix^-T 
	vec3 vertex_ = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 normal_ = normalize(gl_NormalMatrix * gl_Normal);

	//compute L, V:
	vec3 L_ = normalize(gl_LightSource[0].position.xyz - vertex_);  
	vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
	//compute H:
	vec3 H_ = (L_ + V_);
	H_ = normalize(H_);

	// ambient:
	vec4 Iamb = gl_FrontLightProduct[0].ambient; 

	//calculate Diffuse Term:  
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(normal_, L_), 0.0);  

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(H_, V_), 0.0), ns_);

	vertex_color = Iamb + Idiff + Ispec; 

	gl_Position = ftransform();
}
