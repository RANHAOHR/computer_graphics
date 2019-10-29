// gouraud shading with Phong illumination

#define PI 3.14159265

uniform vec4 F_0;
uniform float d_;
uniform float s_; // s+d = 1
uniform vec4 R_d;
//uniform float w1;
//uniform float w2;
uniform float m_;

varying vec3 vertex_;
varying vec3 normal_;

void main()
{
	//compute L, V:
	vec3 L_ = normalize(gl_LightSource[0].position.xyz - vertex_);  
	vec3 V_ = normalize(-vertex_); // reverse because its from vertex to camera
	//compute H:
	vec3 H_ = normalize(L_ + V_);

	// compute F
	float c_ = max(0, dot(V_, H_));
	vec4 n_ =  ( 1 + sqrt(F_0) ) / ( 1 - sqrt(F_0) );
	vec4 g_ = pow(n_, 2) + pow(c_, 2) - 1; 
	vec4 F = 0.5 * pow((g_-c_)/(g_+c_), 2) * ( 1 + pow( c_ * ( g_ + c_) - 1, 2 ) / pow( c_ * ( g_ - c_) + 1, 2 ) );

	//compute D
	float cosalpha = max(0, dot(normal_, H_));
	float k_ = 1 / (m_ * m_ * pow(cosalpha, 4));
	float D = k_ * exp( (cosalpha * cosalpha - 1.0) / (m_ * m_ * cosalpha * cosalpha) );

	// compute G:
	float Gs =  2 * dot(normal_, H_) * dot(normal_, V_) / dot(V_, H_);
	float Gm =  2 * dot(normal_, H_) * dot(normal_, L_) / dot(V_, H_);
	float G = min(1.0, min(Gs, Gm));	

	vec4 R_s = (F * D * G) / ( PI * dot(normal_, L_) * dot(normal_, V_) );

	// ambient:
	vec4 Iamb = gl_FrontLightProduct[0].ambient; 

	//calculate Diffuse Term:  
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(normal_, L_), 0.0);  

	// calculate Specular Term:
	vec4 Ispec = gl_FrontLightProduct[0].specular * max(dot(normal_, L_), 0.0);
	vec4 I_diff_spec = (Idiff + Ispec) * (d_ * R_d + s_ * R_s);

    //output of frag
    gl_FragColor = Iamb + Ispec; 

}
