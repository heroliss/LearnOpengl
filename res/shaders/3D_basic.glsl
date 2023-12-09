﻿##common
#version 450 core
// 光照类型定义
#define MAX_LIGHT_COUNT 8   //TODO: 这个值为什么不能太大？会导致program链接失败？
#define NONE_LIGHT 0
#define PARALLEL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3
struct Light
{
    int type;
    vec3 pos; //光源位置 (平行光无用)
    vec3 direction; //照射方向（点光源无用）
    vec3 color;
    vec3 attenuation; //衰减系数 （分别为常数项、一次项、二次项系数 , 一般常数项固定为1，主要调二次项系数）
    vec2 cutoffAngle; //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)
    int useBlinnPhong;
};


##shader vertex
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_viewPos;
uniform Light u_lights[MAX_LIGHT_COUNT];

out vec2 v_TexCoord;
out vec4 v_PosColor;
out vec3 v_TangentViewPos;
out vec3 v_TangentFragPos;
out vec3 v_TangentLightPos[MAX_LIGHT_COUNT];
out vec3 v_TangentLightDir[MAX_LIGHT_COUNT];
out mat3 TBN;

void main()
{
    //输出齐次裁剪空间的顶点位置，注意乘法要从右向左读
    vec4 worldPos = u_Model * vec4(position, 1.0);
    gl_Position = u_Projection * u_View * worldPos;
    v_TexCoord = texCoord; //输出纹理坐标
    
    //计算法线矩阵
    mat3 worldNormalMatrix = mat3(transpose(inverse(u_Model)));
    //计算世界空间中的切线空间基的方向
    vec3 T = normalize(worldNormalMatrix * tangent);
    vec3 B = normalize(worldNormalMatrix * bitangent);
    vec3 N = normalize(worldNormalMatrix * normal);
    //格拉姆-施密特正交化
    // re-orthogonalize T with respect to N
    //T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    //vec3 B = cross(N, T);
    TBN = mat3(T, B, N); //切线空间转世界空间
    mat3 inversedTBN = transpose(TBN); //世界空间转切线空间
    v_TangentViewPos  = inversedTBN * u_viewPos;
    v_TangentFragPos  = inversedTBN * vec3(worldPos);
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        v_TangentLightPos[i] = inversedTBN * u_lights[i].pos;
        v_TangentLightDir[i] = inversedTBN * normalize(u_lights[i].direction);
    }
}


##shader fragment
//光照
uniform vec4 u_objectColor; //整体颜色
uniform vec3 u_ambient;
uniform vec3 u_emission_inside; //内部自发光
uniform vec3 u_emission_outside; //外部自发光
uniform vec3 u_specularColor; //高光反射颜色
uniform float u_shininess; //高光反射范围（光泽度）
uniform Light u_lights[MAX_LIGHT_COUNT];

uniform sampler2D u_mainTexture; //主贴图
uniform sampler2D u_normalTexture; //法线贴图
uniform sampler2D u_specularTexture; //高光贴图
uniform sampler2D u_heightTexture; //高度贴图
uniform samplerCube u_cubemap; //环境立方体贴图

uniform bool u_showDepth;
uniform vec2 u_showDepthRange;
uniform mat4 u_Projection;

uniform bool u_enableRefract;//开启折射（目前仅支持折射天空盒，此时透明度恒为1）
uniform vec3 u_refractColor; //折射颜色
uniform float u_refractiveIndex; //折射率

in vec2 v_TexCoord;

in vec3 v_TangentViewPos;
in vec3 v_TangentFragPos;
in vec3 v_TangentLightPos[MAX_LIGHT_COUNT];
in vec3 v_TangentLightDir[MAX_LIGHT_COUNT];

in mat3 TBN;

out vec4 outColor;


void main()
{
    //深度图
    if (u_showDepth)
    {
        //从投影矩阵中提取近平面和远平面，用来计算线性深度 (该部分重复计算不应在片段着色器中)
        float near;
        float far;
        if (u_Projection[3][3] == 0) //透视矩阵
        {
            near = u_Projection[2][3] / (u_Projection[2][2] - 1);
            far = u_Projection[2][3] / (u_Projection[2][2] + 1);
        }
        else //正交矩阵
        {
            near = (u_Projection[2][3] + 1) / u_Projection[2][2];
            far = (u_Projection[2][3] - 1) / u_Projection[2][2];
        }
        //计算线性深度
        float z = gl_FragCoord.z * 2.0 - 1.0;
        float linearDepth = (2.0 * near * far) / (far + near - z * (far - near));
        //将线性深度根据给定的范围压缩至0-1之间
        float d = (far - near) / (u_showDepthRange.y - u_showDepthRange.x) * (linearDepth - u_showDepthRange.x) / (far - near); //限制范围后的线性深度值(0到1)
        //越深越黑的输出深度信息
        outColor = vec4(vec3(1 - d), 1.0);
        return;
    }

    //纹理颜色
    vec4 texColor = texture(u_mainTexture, v_TexCoord);
    
    //基础颜色 (包括透明度)
    vec4 baseColor = texColor * u_objectColor;
    
    //从法线贴图范围[0,1]获取法线
    vec3 norm = texture(u_normalTexture, v_TexCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0); //将法线向量转换为范围[-1,1]

    //摄像机方向
    vec3 viewDir = normalize(v_TangentViewPos - v_TangentFragPos);

    //镜面贴图（粗糙度）
    vec3 specularTexColor = vec3(texture(u_specularTexture, v_TexCoord));

    //反射光
    vec3 viewReflectDir = TBN * reflect(-viewDir, norm); //世界空间中的视线反射方向
    vec3 reflectColor = texture(u_cubemap, viewReflectDir).rgb; //立方体贴图采样反射光
    reflectColor *= specularTexColor;
    reflectColor *= u_specularColor;
    reflectColor *= baseColor.a; //透明度越大反射光越弱

    //折射光
    vec3 refractColor = vec3(0);
    if(u_enableRefract)
    {
        vec3 viewRefractDir = TBN * refract(-viewDir, norm, 1.0f / u_refractiveIndex); //世界空间中的视线折射方向
        refractColor = texture(u_cubemap, viewRefractDir).rgb; //立方体贴图采样折射光
        refractColor *= u_refractColor;
        refractColor *= 1 - baseColor.a; //透明度越大折射光越强
    }
    

    vec3 allLightsColor = vec3(0.0);
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        Light light = u_lights[i];
        vec3 lightDir; //光照方向的反方向(指向光源的方向)
        float lightDistance; //记录光源距离
        float intensity = 1;
        
        if (light.type == NONE_LIGHT || light.color == vec3(0)) //跳过无亮度的光照
        {
            continue;
        }
        else if (light.type == PARALLEL_LIGHT) //平行光
        {
            lightDir = -v_TangentLightDir[i];
            lightDistance = 0; //平行光的光源距离恒为0
        }
        else if (light.type == POINT_LIGHT || light.type == SPOT_LIGHT) //点光或聚光
        {
            vec3 fragTolight = v_TangentLightPos[i] - v_TangentFragPos;
            lightDir = normalize(fragTolight);
            lightDistance = length(fragTolight);
            if (light.type == SPOT_LIGHT)
            {
                float cosInnerCutoffAngle = cos(radians(light.cutoffAngle.x)); //下面很多计算可提取到cpu中计算
                float cosOuterCutoffAngle = cos(radians(light.cutoffAngle.y));
                float cosLightAngle = dot(lightDir, -v_TangentLightDir[i]);
                float epsilon = cosInnerCutoffAngle - cosOuterCutoffAngle;
                intensity = clamp((cosLightAngle - cosOuterCutoffAngle) / epsilon, 0.0, 1.0);
            }
        }
        else
        {
            outColor = vec4(1, 0, 1, 1); //错误警示色
            return;
        }
        
        if(dot(vec3(0,0,1), lightDir) > 0.0) //TOOD:自己乱加的：用于解决加了法线贴图后背面被照亮的问题
        {
            //漫反射
            vec3 diffuse = max(dot(norm, lightDir), 0.0) * light.color;
    
            //高光反射
            float specularStrength;
            if(light.useBlinnPhong == 0){
                //Phong
                vec3 reflectDir = reflect(-lightDir, norm);
                specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
            }
            else {
                //Blinn-Phong
                vec3 midDir = normalize(viewDir + lightDir);
                specularStrength = pow(max(dot(midDir, norm), 0.0), u_shininess);
            }

            vec3 specular = u_specularColor * specularStrength * light.color * specularTexColor;
    
            //衰减
            float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * lightDistance + light.attenuation[2] * (lightDistance * lightDistance));
        
            //合并所有光照
            allLightsColor += (diffuse + specular) * attenuation * intensity;
            allLightsColor *= baseColor.a; //透视会减少光照的反射
        }
    }
    //合并所有颜色
    outColor = vec4(baseColor.rgb * (allLightsColor + u_emission_inside + u_ambient + reflectColor + refractColor) + u_emission_outside, u_enableRefract ? 1 : baseColor.a); //当使用折射时透明度保持为1
};
