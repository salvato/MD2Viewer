#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

uniform mat4 MVP; // our models MVP
uniform mat4 u_LightsMVP; // our lights MVP (using the objects model)
uniform float Lit;

out vec4 v_ShadowTexCoord; // the texture coords for the shadow
out vec2 v_texCoord; // the usual texture coords for the texture
out float v_Lit;

// we need to bring our values into range so this matrix will do that
const mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,
                             0.0, 0.5, 0.0, 0.0,
                             0.0, 0.0, 1.0, 0.0,
                             0.5, 0.5, 0.5, 1.0);

void
main() {

    // Calculate vertex position, seen from the light view point
    // and Normalize texture coords from -1<>1
    v_ShadowTexCoord = biasMatrix * (u_LightsMVP * a_position); // this result gets passed to the frag shader as its a varying

    // do our normal position calculations
    gl_Position = MVP * a_position; // use the models normal camera based MVP

    // pass the texture coord to the frag and our lit flag
    v_texCoord = a_texCoord;
    v_Lit = Lit;
}
