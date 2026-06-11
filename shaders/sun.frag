#version 330 core

uniform vec3 sunColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(sunColor, 1.0);
}