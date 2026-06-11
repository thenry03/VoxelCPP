#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    // Basic lighting (Alpha Minecraft canonical values)
    float lightFactor;
    if (Normal.y > 0.5)
        lightFactor = 1.0;          // top (+Y)
    else if (Normal.y < -0.5)
        lightFactor = 0.5;          // bottom (-Y)
    else if (abs(Normal.x) > 0.5)
        lightFactor = 0.6;          // east/west (±X)
    else
        lightFactor = 0.8;          // north/south (±Z)
    FragColor = texture(texture1, TexCoord) * vec4(vec3(lightFactor), 1.0);
}