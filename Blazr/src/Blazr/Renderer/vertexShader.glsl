#version 330 core
layout(location = 0) in vec3 aPos;     // Vertex position
layout(location = 1) in vec2 aTexCoord; // Texture coordinate

uniform mat4 uView;        // View matrix (Camera transformation)
uniform mat4 uProjection;  // Projection matrix (Orthographic)

out vec2 TexCoord;

void main() {
    gl_Position = uProjection * uView * vec4(aPos, 1.0); // Transform the vertex position
    TexCoord = aTexCoord;
}

