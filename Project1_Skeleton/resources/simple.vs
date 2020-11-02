#version 150
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normal_matrix;

in vec3 position;
in vec3 normal;

out vec3 ourColor;

void main(){
    gl_Position = proj * vec4(position, 1.0);
    ourColor = vec3(1.0, 0.0, 0.0);
}