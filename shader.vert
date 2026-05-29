#version 450

layout(location = 0) in vec3 app_position;

void main()
{
    gl_Position = vec4(app_position, 1.0);
}