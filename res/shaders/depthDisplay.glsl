##common
#version 450 core
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_View;
    mat4 u_Projection;
    float near_plane;
    float far_plane;
};


##shader vertex
uniform mat4 u_Model;
in vec3 position;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1);
}


##shader fragment
uniform float u_orthoRatio;
uniform vec2 u_showDepthRange;
out vec4 outColor;

float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
        //从投影矩阵中提取近平面和远平面，用来计算线性深度 (该部分重复计算不应在片段着色器中)
//        float near;
//        float far;
//        if (u_Projection[3][3] == 0) //透视矩阵
//        {
//            near = u_Projection[2][3] / (u_Projection[2][2] - 1);
//            far = u_Projection[2][3] / (u_Projection[2][2] + 1);
//        }
//        else //正交矩阵
//        {
//            near = (u_Projection[2][3] + 1) / u_Projection[2][2];
//            far = (u_Projection[2][3] - 1) / u_Projection[2][2];
//        }

    //因为透视投影需要线性化，而正交投影不用，所以下面分别计算两个深度百分比并平滑结合
    float linearDepth = LinearizeDepth(gl_FragCoord.z, near_plane, far_plane); //计算线性深度值（结果是远近平面之间的深度值，不是百分比）
    float d1 = (linearDepth - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //透视矩阵限制范围后的线性深度百分比(0到1)
    float d2 = ((far_plane - near_plane) * gl_FragCoord.z + near_plane - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //正交矩阵限制范围后的线性深度百分比(0到1) 这里gl_FragCoord.z是0-1，表示深度百分比
    //平滑过渡（随便弄的，可改进）
    float t = pow(u_orthoRatio, 12);
    float d = (1 - t) * d1 + t * d2;
    outColor = vec4(vec3(1 - d), 1.0); //越深越黑的输出深度信息
}