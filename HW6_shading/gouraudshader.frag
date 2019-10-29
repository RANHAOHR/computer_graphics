// gouraud shading with Phong illumination

varying vec4 vertex_color;

void main()
{
    //output of frag
    gl_FragColor = vertex_color;

}
