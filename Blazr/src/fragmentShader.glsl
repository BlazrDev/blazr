#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 uColor;  // Color for the sprite

void main() {
    FragColor = uColor; // Set output color
}
