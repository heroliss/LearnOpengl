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
        //��ͶӰ��������ȡ��ƽ���Զƽ�棬��������������� (�ò����ظ����㲻Ӧ��Ƭ����ɫ����)
//        float near;
//        float far;
//        if (u_Projection[3][3] == 0) //͸�Ӿ���
//        {
//            near = u_Projection[2][3] / (u_Projection[2][2] - 1);
//            far = u_Projection[2][3] / (u_Projection[2][2] + 1);
//        }
//        else //��������
//        {
//            near = (u_Projection[2][3] + 1) / u_Projection[2][2];
//            far = (u_Projection[2][3] - 1) / u_Projection[2][2];
//        }

    //��Ϊ͸��ͶӰ��Ҫ���Ի���������ͶӰ���ã���������ֱ����������ȰٷֱȲ�ƽ�����
    float linearDepth = LinearizeDepth(gl_FragCoord.z, near_plane, far_plane); //�����������ֵ�������Զ��ƽ��֮������ֵ�����ǰٷֱȣ�
    float d1 = (linearDepth - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //͸�Ӿ������Ʒ�Χ���������Ȱٷֱ�(0��1)
    float d2 = ((far_plane - near_plane) * gl_FragCoord.z + near_plane - u_showDepthRange.x) / (u_showDepthRange.y - u_showDepthRange.x); //�����������Ʒ�Χ���������Ȱٷֱ�(0��1) ����gl_FragCoord.z��0-1����ʾ��Ȱٷֱ�
    //ƽ�����ɣ����Ū�ģ��ɸĽ���
    float t = pow(u_orthoRatio, 12);
    float d = (1 - t) * d1 + t * d2;
    outColor = vec4(vec3(1 - d), 1.0); //Խ��Խ�ڵ���������Ϣ
}