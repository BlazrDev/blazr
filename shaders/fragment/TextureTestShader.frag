#version 450 core

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;

// Maximum number of textures (matching the MaxTextureSlots in Renderer2DData)

layout (binding = 0) uniform sampler2D u_Textures[32];

vec4 GetTextureColor(int index, vec2 coord) {
    if (index == 0) return texture(u_Textures[0], coord);
    if (index == 1) return texture(u_Textures[1], coord);
    if (index == 2) return texture(u_Textures[2], coord);
    if (index == 3) return texture(u_Textures[3], coord);
    if (index == 4) return texture(u_Textures[4], coord);
    if (index == 5) return texture(u_Textures[5], coord);
    if (index == 6) return texture(u_Textures[6], coord);
    if (index == 7) return texture(u_Textures[7], coord);
    if (index == 8) return texture(u_Textures[8], coord);
    if (index == 9) return texture(u_Textures[9], coord);
    if (index == 10) return texture(u_Textures[10], coord);
    if (index == 11) return texture(u_Textures[11], coord);
    if (index == 12) return texture(u_Textures[12], coord);
    if (index == 13) return texture(u_Textures[13], coord);
    if (index == 14) return texture(u_Textures[14], coord);
    if (index == 15) return texture(u_Textures[15], coord);
    if (index == 16) return texture(u_Textures[16], coord);
    if (index == 17) return texture(u_Textures[17], coord);
    if (index == 18) return texture(u_Textures[18], coord);
    if (index == 19) return texture(u_Textures[19], coord);
    if (index == 20) return texture(u_Textures[20], coord);
    if (index == 21) return texture(u_Textures[21], coord);
    if (index == 22) return texture(u_Textures[22], coord);
    if (index == 23) return texture(u_Textures[23], coord);
    if (index == 24) return texture(u_Textures[24], coord);
    if (index == 25) return texture(u_Textures[25], coord);
    if (index == 26) return texture(u_Textures[26], coord);
    if (index == 27) return texture(u_Textures[27], coord);
    if (index == 28) return texture(u_Textures[28], coord);
    if (index == 29) return texture(u_Textures[29], coord);
    if (index == 30) return texture(u_Textures[30], coord);
    if (index == 31) return texture(u_Textures[31], coord);

    // Default to first texture if no valid index
    return texture(u_Textures[0], coord);
}

void main() {
    // Get the color from the appropriate texture
    vec4 texColor = GetTextureColor(int(v_TexIndex), v_TexCoord * v_TilingFactor);
    
    // Mix the texture color with the vertex color (tint)
    FragColor = texColor * v_Color;
}
