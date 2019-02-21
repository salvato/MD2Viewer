#version 330 core

// This is a base level system for adding shadows to your scene, it uses a simple
// FBO as a texture to gain access to a shadow map.
// The result is an effective but potentially low resolution hard shadow
// (depending on scene size and light spread)
// Softer shadows can be done using PCF or other methods but there is a very significant
// drop in performance on limited core GPU's and you need a better resolution to get
// good results, on phones/SBC's this may be too much.


uniform sampler2D s_texture; // texture0 our normal diffuse texture
uniform sampler2D u_s2dShadowMap; // texture1 our FBO shadows

uniform vec4 Ambient; // not used yet

in vec4 v_ShadowTexCoord; // the coordinates passed from the vertex shader
in vec2 v_texCoord; // passed by our normal textures
in float v_Lit; // a conditional light flag ...not currently use

out vec4 FragColor;

void
main() {
    float fLight = 1.0; // the potential final light value

    /* we need to get the packed light distance data from the shadow texture, texture1. */
    vec2 Depth = texture2DProj(u_s2dShadowMap, v_ShadowTexCoord).xy;
    // its in 2 values scaled down so resize them
    float fDepth = (Depth.x * 10.0 + Depth.y);
    // reconstitute the depth
    float ActualDepth = (10.0-v_ShadowTexCoord.z) + 0.1 - fDepth ;
    if(fDepth > 0.0 && ActualDepth < 0.0) {
        fLight = 0.6; // don't go totally dark, adjust range to suit, and
                      // perhaps have influenced by other light sources
    }

    FragColor = texture2D( s_texture, v_texCoord )*fLight; // we can also add other colours or light effects
    FragColor.w = 1.0;
}
