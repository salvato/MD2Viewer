#version 330 core
out vec4 v_texCoords;
out vec4 FragColor;

// Generate shadow map
// We need to gather the depth values, bring them into range
// and then encode them into the x and y values of the
// colour value we create
// This shader should be linked with ShadowMap.vsh and used with a bound FBO
	

void
main() {
    float value = 10.0 - v_texCoords.z; // 10 is the most usual range
    float num = floor(value);
    float f = value - num;	// get the floating point value
    float vn = num * 0.1;
    FragColor = vec4(vn, f, 0.0, 1.0);
}
