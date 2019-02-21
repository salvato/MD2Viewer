#version 330 core
// simple flat text display vert shader

uniform vec2 u_HalfSize;
layout(location = 0) in vec2 a_Position; // xy coords
layout(location = 1) in vec2 a_texCoord; // tex coords

out vec2 v_TexCoord;


void
main() {
    // first map the screen coordinates to (-1..1)(-1..1)
    vec2 vert_clipspace = (a_Position/ u_HalfSize) - vec2(1, 1);
    // invert Y coordinate
    gl_Position = vec4(vert_clipspace.x, -vert_clipspace.y, 0, 1);
    v_TexCoord = a_texCoord;
}

