#version 330 core
// a conditionally lit Lerping shader for MD2 models modelled
// on the conditional Lit OBJ Shader but it lerps

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_NextPosition;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_NextNormal;
layout(location = 4) in vec2 a_texCoord;

uniform mat4  MVP;
uniform mat4  MV;
uniform vec3  LightPos; // where is the light a const sun type light
uniform float Lit; // if >0 , we are lit, if 0, we are unlit and use ambient colour
uniform float u_Time;

out vec2  v_texCoord;
out float v_Colour;
out float v_lit;


void
main() {
    // Get the lerped pos and normal to work with
    vec3 LerpPos  = mix(a_position, a_NextPosition, u_Time);
    vec3 LerpNorm = mix(a_normal,   a_NextNormal,   u_Time); // actually this is inefficent but visually effective and fast

    if(Lit != 0.0) {
        vec3 VertexNormal  = normalize(vec3(MV * vec4(LerpNorm, 1.0)));// Transform the normal's orientation into world space.
        vec3 SurfaceVertex = vec3(MV * vec4(LerpPos, 1.0));// Transform the vertex position into world space.
        vec3 lightVector   = normalize(LightPos-SurfaceVertex);// get the direction of the light as a result of Light_pos-vert_position
        // pass the output values to the fragment shader
        v_Colour = max(dot(VertexNormal, lightVector), 0.1);  // dont allow totally black
    }
    gl_Position = MVP * vec4(a_position, 1.0);
    v_lit = Lit; // this needs to be given to the fragment shader regardless
    v_texCoord = a_texCoord;
}
