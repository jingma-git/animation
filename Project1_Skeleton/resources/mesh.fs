#version 150
uniform mat4 view;
uniform mat4 proj;
uniform vec4 fixed_color;

in vec3 position_eye;
in vec3 normal_eye;

uniform vec3 light_position_eye;

vec3 La = vec3(1, 1, 1);
vec3 Ld = vec3(1, 1, 1);
vec3 Ls = vec3(1, 1, 1);

in vec4 Kai;
in vec4 Kdi;
in vec4 Ksi;

uniform float specular_exponent;
uniform float lighting_factor;

out vec4 outColor;

void main(){
    vec3 Ia = La * vec3(Kai);

    vec3 vector_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize(vector_to_light_eye);
    float dot_prod = dot(direction_to_light_eye, normalize(normal_eye));
    vec3 Id = Ld * vec3(Kdi) * dot_prod;

    outColor =  vec4(Id+Ia, 1.0);
    if(fixed_color!=vec4(0.0)) outColor=fixed_color;
}