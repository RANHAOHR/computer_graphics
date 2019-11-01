// gouraud shading with Phong illumination

#define MAX_LIGHTS 2 

#define ns_ 10.0;
varying vec3 vertex_;
varying vec3 normal_;

void main()
{
	vec3 N_ = normalize(normal_);
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);

   for (int i=0;i<MAX_LIGHTS;i++)
   {
		//compute L, V:
		vec3 L_ = normalize(gl_LightSource[i].position.xyz - vertex_);  
		vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
		//compute H:
		vec3 H_ = (L_ + V_);
		H_ = normalize(H_);

		// ambient:
		vec4 Iamb = gl_FrontLightProduct[i].ambient; 

		//calculate Diffuse Term:  
		vec4 Idiff = gl_FrontLightProduct[i].diffuse * max(dot(N_, L_), 0.0);  

		// calculate Specular Term:
		vec4 Ispec = gl_FrontLightProduct[i].specular * pow(max(dot(H_,V_),0.0), ns_);
        Ispec = clamp(Ispec, 0.0, 1.0);  // between 0-1

	    //output of frag
	    finalColor += Iamb + Idiff + Ispec; 
    }

    gl_FragColor = gl_FrontLightModelProduct.sceneColor + finalColor;
    //gl_FragColor = vec4(0.0,1.0,1.0,1.0); //debug

}
