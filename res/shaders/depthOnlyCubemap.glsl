##common
#version 450 core

##shader vertex
layout (location = 0) in vec3 position;
layout (location = 5) in mat4 instanceMatrix;

uniform mat4 u_Model;

void main()
{
    mat4 modelMatrix = u_Model == mat4(0) ? instanceMatrix : u_Model;
    gl_Position = modelMatrix * vec4(position, 1.0f);
}


##shader geometry
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightSpaceMatrix[6];

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMatrix[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}


##shader fragment
uniform int u_debugShowLayer;
uniform vec2 u_nearAndFar;
uniform vec2 u_showDepthRange;
uniform vec3 u_lightPos;
in vec4 FragPos;
out vec4 outColor;

void main()
{             
    float lightDistance = length(FragPos.xyz - u_lightPos);
    lightDistance = lightDistance / u_nearAndFar.y; // map to [0;1] range by dividing by far_plane
    gl_FragDepth = lightDistance;

    //以下仅用于调试时显示深度图
    if(u_debugShowLayer == gl_Layer)
    {
        float d = ((u_nearAndFar.y - u_nearAndFar.x) * lightDistance + u_nearAndFar.x - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x);
        outColor = vec4(vec3(1 - d), 1.0); //越深越黑的输出深度信息
    }
}