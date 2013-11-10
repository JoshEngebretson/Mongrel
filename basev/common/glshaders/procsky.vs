#version 110

varying vec3 EyeV;

void main(void) {

    EyeV = gl_Normal;

    gl_Position = gl_Vertex;

}
