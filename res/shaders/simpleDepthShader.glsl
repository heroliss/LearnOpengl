##shader vertex
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 5) in mat4 instanceMatrix;

uniform mat4 lightSpaceMatrix;
uniform mat4 u_Model;

void main()
{
    mat4 modelMatrix = u_Model == mat4(0) ? instanceMatrix : u_Model;
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(position, 1.0f);
}


##shader fragment
#version 450 core

out vec4 color;

void main()
{             
    color = vec4(1 - gl_FragCoord.zzz, 1); //仅用于调试时显示深度图
}