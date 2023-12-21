##shader vertex
#version 450 core

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 u_Model;
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1);
}


##shader fragment
#version 450 core

uniform vec4 u_singleColor;
out vec4 color;

void main()
{
    color = u_singleColor;
}