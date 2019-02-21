#version 330 core
// simple shader to gather the depth, its a 1st pass shader designed to render into a bound FBO


layout(location = 0) in vec4 a_position;
uniform mat4 u_lightMVP; // light MVP is the casters Model, the lights view and Projection
out vec4 v_texCoords; // we have to pass coords to the frag shader
 
void
main() {
    gl_Position = u_lightMVP * a_position;
    v_texCoords = gl_Position; // pass this to the fragment which can't access gl_Position
}





