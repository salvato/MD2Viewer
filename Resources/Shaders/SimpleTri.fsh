#version 330 core
// very simple and probably likely to never be needed

in vec4 v_colour;
out vec4 FragColor;


void
main() {
    FragColor = v_colour; // show a plane colour
}
