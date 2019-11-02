// gouraud shading with Phong illumination

#define PI 3.14159265

uniform vec4 F_0;
uniform vec4 R_d;
//uniform float w1;
//uniform float w2;
uniform float m_;
uniform float d_;
uniform float s_;


varying vec3 vertex_;
varying vec3 normal_;

void main()
{
	// vec4 F_0 = vec4(0.755, 0.49, 0.095, 1.0);
	// vec4 R_d = vec4(0.755, 0.49, 0.095, 1.0); //....

	vec3 N_ = normalize(normal_);

	//compute L, V:
	vec3 L_ = normalize(gl_LightSource[0].position.xyz - vertex_);  
	vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
	//compute H:
	vec3 H_ = normalize(L_ + V_);

	// compute F
	float VH = max(0.0, dot(V_, H_));
	float NV = max(0.0, dot(N_, V_));
	float NH = max(0.0, dot(N_, H_));
	float NL = max(0.0, dot(N_, L_));

	//compute D
	float k_ = 1.0 / (m_ * m_ * NH * NH * NH * NH);
	float D = k_ * exp( (NH * NH - 1.0) / (m_ * m_ * NH * NH) ); // 

	// compute G:
	float Gs =  2.0 * NH * NV / VH;
	float Gm =  2.0 * NH * NL / VH;
	float G = min(1.0, min(Gs, Gm));	

	vec4 n = ( 1.0+sqrt(F_0) ) / ( 1.0-sqrt(F_0) );
	vec4 g = sqrt( n*n + VH * VH - 1.0 );
	vec4 F = 0.5 * (g - VH) * (g - VH) / ( (g + VH) * (g + VH) ) * ( 1.0 + ( VH * (g+VH) - 1.0 ) * ( VH * (g+VH) - 1.0 ) / ( ( VH * (g-VH) + 1.0 ) * ( VH * (g-VH) + 1.0 ) ) );

	vec4 R_s = F * D * G / ( PI * NL * NV );

	// ambient:
	vec4 Iamb = gl_FrontLightProduct[0].ambient; 

	//calculate Diffuse Term:  
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * NL;  

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular * NL;
	vec4 I_diff_spec = (Idiff + Ispec) * (d_* R_d + s_ * R_s);

    //output of frag
    gl_FragColor = Iamb + I_diff_spec; 
//    gl_FragColor = vec4(0.0,1.0,1.0,1.0); //debug

}
