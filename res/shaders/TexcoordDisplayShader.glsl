##common
#version 450 core
struct VS {
    vec2 texCoord;
};

##shader vertex

in vec3 position;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_Model;
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_View;
    mat4 u_Projection;
};

out VS vs;

void main()
{    
    vs.texCoord = texCoord;
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
}


##shader fragment
in VS vs;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vs.texCoord,0,1);
};