#version 330 core
// conditional simple MD2 shader, allows a lit or unlit system, actually its the same as the OBJ version
// and you can pair with that if you want.

uniform sampler2D s_texture;
uniform vec4 Ambient; 
in vec2 v_texCoord;

// if lit these are passed by the vertex shader
in float v_lit;
in float v_Colour;

out vec4 FragColor;

void
main() {
    if(v_lit != 0.0) {
        FragColor = texture2D(s_texture, v_texCoord) * v_Colour ; // we could also use Ambient if we wanted??
    }
    else {
        FragColor = texture2D(s_texture, v_texCoord) * Ambient;
    }
}

