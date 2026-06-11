#version 330 core

layout (location = 0) in vec2 localPosition; // Local quad position

uniform vec3 sunPosition;
uniform float sunSize;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Extract View matrix axis
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

    // Build position using a billboard method
    vec3 worldPosition = sunPosition + (right * localPosition.x + up * localPosition.y) * sunSize;

    gl_Position = projection * view * vec4(worldPosition, 1.0);
}