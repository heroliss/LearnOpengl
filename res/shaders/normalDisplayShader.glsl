##common
#version 330 core
struct VS {
    vec3 normal;      //观察空间法线
    vec3 worldNormal; //世界空间法线
    vec2 texCoord;
    mat3 TBA;
    mat3 viewNormalMatrix;
};

##shader vertex

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
//uniform mat4 u_Projection;

out VS vs;

void main()
{    
    vs.texCoord = texCoord;

    gl_Position = u_View * u_Model * vec4(position, 1.0);

    mat3 worldNormalMatrix = mat3(transpose(inverse(u_Model)));
    vs.worldNormal = normalize(worldNormalMatrix * normal);

    vs.viewNormalMatrix = mat3(transpose(inverse(u_View)));
    vs.normal = normalize(vs.viewNormalMatrix * vs.worldNormal);

    //mat3 normalMatrix = viewNormalMatrix * worldNormalMatrix;

    vec3 T = normalize(worldNormalMatrix * tangent);
    vec3 B = normalize(worldNormalMatrix * bitangent);
    vec3 N = vs.worldNormal;
    vs.TBA = mat3(T, B, N); //由切线空间转为世界空间的矩阵

}



##shader geometry
layout (triangles) in;
layout (line_strip, max_vertices = 128) out; //经测试这里最大只能128

in VS vs[];

out vec4 gs_Color;

uniform mat4 u_Projection;
uniform float size;
uniform bool showVertexNormal;
uniform bool showFragmentNormal;
uniform float showFragmentNormalInterval;
uniform vec2 showFragmentNormalSamplesSize;
uniform sampler2D u_normalTexture; //法线贴图

void GenerateLine(int index)
{
    gs_Color = vec4(abs(vs[index].worldNormal), 1);
    gl_Position = u_Projection * gl_in[index].gl_Position;
    EmitVertex();
    
    gs_Color = vec4(0, 0, 0, 0);
    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(vs[index].normal, 0.0) * size);
    EmitVertex();
    EndPrimitive();   
}

void main()
{
    if(showVertexNormal)
    {
        GenerateLine(1); // 第一个顶点法线
        GenerateLine(0); // 第二个顶点法线
        GenerateLine(2); // 第三个顶点法线
    }

    if(showFragmentNormal)
    {
        mat3 TBA = vs[1].TBA;
        mat3 viewNormalMatrix = vs[1].viewNormalMatrix;
        vec2 uv1 = vs[1].texCoord;
        vec2 uv2 = vs[0].texCoord;
        vec2 uv3 = vs[2].texCoord;
        
        vec3 v1 = vec3(gl_in[1].gl_Position);
        vec3 v2 = vec3(gl_in[0].gl_Position);
        vec3 v3 = vec3(gl_in[2].gl_Position);
        
        //给定的采样数
        float pv3Num = showFragmentNormalSamplesSize.y;
        float edge1Num = showFragmentNormalSamplesSize.x;
        //根据给定的间隔距离，计算出两个垂直方向各自需要多少采样点
        if(showFragmentNormalInterval > 0)
        {
            //找到v3在边(v1,v2)上的垂点p和垂线pv3
            vec3 edge1 = v2 - v1;
            vec3 edge2 = v3 - v1;
            vec3 edge1Dir = normalize(edge1);
            float v1pLength = dot(edge1Dir, edge2);
            vec3 v1p = edge1Dir * v1pLength;
            vec3 p = v1 + v1p;
            vec3 pv3 = v3 - p;
            //计算采样点数量
            float pv3Length = length(pv3);
            float edge1Length = length(edge1);
            pv3Num = pv3Length / showFragmentNormalInterval;
            edge1Num = edge1Length / showFragmentNormalInterval;   
        }
        
        if(pv3Num * edge1Num > 1000) return; //避免太多采样点卡死


        //使用重心坐标得到所有采样点
        for(float j = 0; j < pv3Num; j++)
        {
            float t3 = j / pv3Num;
            float less = 1-t3;
            float num = less * edge1Num;
            for (float i = 0; i < num; i++)
            {
                float t2 = i / num;
                float t1 = 1 - t2;
                t2 *= less;
                t1 *= less;
                vec3 v = t1 * v1 + t2 * v2 + t3 * v3; //采样点

                //提取法线
                vec2 uv = t1 * uv1 + t2 * uv2 + t3 * uv3; 
                vec3 tangentNormal = texture(u_normalTexture, uv).rgb;
                tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
                vec3 worldNormal = TBA * tangentNormal;
                vec3 viewNormal = viewNormalMatrix * worldNormal;

                gs_Color = vec4(abs(worldNormal), 1);
                gl_Position = u_Projection * vec4(v, 1);
                EmitVertex();
                gs_Color = vec4(0, 0, 0, 0);
                gl_Position = u_Projection * vec4(v + viewNormal * size, 1);
                EmitVertex();
                EndPrimitive();  
             }
        }
    }
}



##shader fragment

layout(location = 0) out vec4 color;  //layout可以不写

//uniform vec4 u_Color;
in vec4 gs_Color;

void main()
{
    color = gs_Color;
};