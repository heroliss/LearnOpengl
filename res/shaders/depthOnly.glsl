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
uniform bool u_LinearizeDepth;
uniform vec2 u_nearAndFar;
uniform vec2 u_showDepthRange;
out vec4 outColor;

float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

//片元着色器部分仅用于调试
void main()
{             
    float near_plane = u_nearAndFar.x;
    float far_plane = u_nearAndFar.y;

    //透视投影需要线性化，而正交投影不用
    float d;
    if(u_LinearizeDepth)
    {
        float linearDepth = LinearizeDepth(gl_FragCoord.z, near_plane, far_plane); //计算线性深度值（结果是远近平面之间的深度值，不是百分比）
        d = (linearDepth - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //透视矩阵限制范围后的线性深度百分比(0到1)
    }
    else
    {
        d = ((far_plane - near_plane) * gl_FragCoord.z + near_plane - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //正交矩阵限制范围后的线性深度百分比(0到1) 这里gl_FragCoord.z是0-1，表示深度百分比
    }
    outColor = vec4(vec3(1 - d), 1.0); //越深越黑的输出深度信息
}