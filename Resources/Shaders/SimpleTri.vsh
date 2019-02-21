#version 330 core
// very simple and probably likely to never be needed	

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_colour;

out vec4 v_colour;
uniform mat4 MVP;

void
main() {
    gl_Position = MVP*vec4(a_position, 1);
    v_colour = vec4(a_colour, 1);
}
