##common
#version 450 core
// 光照类型定义
#define MAX_LIGHT_COUNT 4
#define NONE_LIGHT 0
#define PARALLEL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3
struct Light
{
    int type;
    bool useBlinnPhong;
    float brightness;
    bool castShadow;
    int shadowPCFSize;
    float shadowBias;
    float shadowBiasChangeRate;
    float shadowSampleDiskRadius;
    vec3 pos; //光源位置 (平行光无用)
    vec3 direction; //照射方向（点光源无用）
    vec3 color;
    vec3 attenuation; //衰减系数 （分别为常数项、一次项、二次项系数 , 一般常数项固定为1，主要调二次项系数）
    vec2 cutoffAngle; //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)
    vec2 shadowNearAndFar;
    mat4 lightSpaceMatrix; //非点光源用的光照空间矩阵
};

layout(binding = 1, std140) uniform Lights
{
    int u_lightNum;
    Light u_lights[MAX_LIGHT_COUNT];
};

layout(binding = 0, std140) uniform Matrices
{
    mat4 u_View;
    mat4 u_Projection;
//    float near_plane; //这两个可用，但暂时用不上
//    float far_plane;
};


##shader vertex
uniform mat4 u_Model;
//uniform vec3 u_viewPos;

//下面in的变量顺序不能随便改变，和顶点数据结构相关
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;
in mat4 instanceMatrix;

out vec2 v_TexCoord;
out mat3 v_TBN;
out vec3 v_FragPos; //世界空间的片元位置
out vec4 v_FragPosLightSpaces[MAX_LIGHT_COUNT]; //光照空间的片元位置
out float v_LightsEnable[MAX_LIGHT_COUNT];

void main()
{
    mat4 modelMatrix = u_Model == mat4(0) ? instanceMatrix : u_Model;

    vec4 worldPos = modelMatrix * vec4(position, 1.0); 
    v_FragPos = worldPos.xyz; //输出世界空间的顶点位置
    gl_Position = u_Projection * u_View * worldPos; //输出齐次裁剪空间的顶点位置
    v_TexCoord = texCoord; //输出纹理坐标
    
    //计算法线矩阵
    mat3 worldNormalMatrix = mat3(transpose(inverse(modelMatrix)));

    //计算世界空间中的切线空间基的方向
    vec3 T = normalize(worldNormalMatrix * tangent);
    vec3 N = normalize(worldNormalMatrix * normal);
    vec3 B = normalize(worldNormalMatrix * bitangent);

    //TBN向量重正交化(使用格拉姆-施密特正交化算法) //TODO: 这里为什么不对
    //    T = normalize(T - dot(T, N) * N);
    //    vec3 B = cross(T, N);

    v_TBN = mat3(T, B, N); //输出切线空间转世界空间的矩阵
    //mat3 inversedTBN = transpose(v_TBN); //世界空间转切线空间的矩阵

    //计算光照空间的顶点位置
    for (int i = 0; i < u_lightNum; i++)
    {
        Light light = u_lights[i];

        //非点光源时计算片元在光照空间中的位置
        if(light.type != POINT_LIGHT)
            v_FragPosLightSpaces[i] = light.lightSpaceMatrix * worldPos; 

        //TOOD:下面是乱加的：用于解决 加了法线贴图后背面被照亮 和 光源在背面也有高光泄露 的问题（如果开启了阴影并且不是正面剔除，加了法线贴图后背面被照亮没有这个问题了）
        vec3 lightDir;
        if (light.type == PARALLEL_LIGHT) //平行光
        {
            lightDir = -normalize(light.direction);
        }
        else if (light.type == POINT_LIGHT || light.type == SPOT_LIGHT) //点光或聚光
        {
            lightDir = normalize(light.pos - v_FragPos);
        }
        v_LightsEnable[i] = dot(N, lightDir);
    }
}


##shader fragment

uniform vec3 u_viewPos;
//颜色
uniform vec4 u_objectColor; //整体颜色
uniform vec3 u_ambient;
//自发光
uniform vec3 u_emission_inside; //内部自发光
uniform vec3 u_emission_outside; //外部自发光
//高光
uniform vec3 u_specularColor; //高光反射颜色
uniform float u_shininess; //高光反射范围（光泽度）
//贴图
uniform sampler2D u_mainTexture; //主贴图
uniform sampler2D u_normalTexture; //法线贴图
uniform sampler2D u_specularTexture; //高光贴图
uniform sampler2D u_heightTexture; //高度贴图
uniform samplerCube u_cubemap; //环境立方体贴图
//折射
uniform bool u_enableRefract;//开启折射（目前仅支持折射天空盒，此时透明度恒为1）
uniform vec3 u_refractColor; //折射颜色
uniform float u_refractiveIndex; //折射率
//阴影
uniform sampler2D u_shadowMaps[MAX_LIGHT_COUNT]; //阴影深度纹理，与u_lights对应
uniform samplerCube u_shadowCubemaps[MAX_LIGHT_COUNT]; //阴影立方体深度纹理，与u_lights对应

out vec4 outColor;

in vec2 v_TexCoord;
in mat3 v_TBN;
in vec3 v_FragPos; //世界空间的片元位置
in vec4 v_FragPosLightSpaces[MAX_LIGHT_COUNT]; //光照空间的片元位置
in float v_LightsEnable[MAX_LIGHT_COUNT];

const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation_pointLight(int index, vec3 lightDir, vec3 normal)
{
    Light light = u_lights[index];
    float bias = light.shadowBias;
    float d = dot(normal, lightDir);
    bias = clamp(1 / (light.shadowBiasChangeRate * d), 0, 1) * bias;

    float shadow = 0.0;
    vec3 lightToFrag = v_FragPos - u_lights[index].pos; 
    float currentDepth = length(lightToFrag);

    if(currentDepth > light.shadowNearAndFar.y)
        return 0.0;

    //float diskRadius = (light.shadowSampleDiskRadius + (length(u_viewPos - v_FragPos) / light.shadowNearAndFar.y));
    float diskRadius = light.shadowSampleDiskRadius;

    for(int i = 0; i < 20; ++i)
    {
        float closestDepth = texture(u_shadowCubemaps[index], lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= light.shadowNearAndFar.y;   // Undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= 20;

//    //samples需要从外部设置得来
//    float offset = 0.1;
//    for(float x = -offs-et; x < offset; x += offset / (samples * 0.5))
//    {
//        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
//        {
//            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
//            {
//                float closestDepth = texture(u_shadowCubemaps[index], lightToFrag + vec3(x, y, z) * diskRadius).r; 
//                closestDepth *= light.shadowNearAndFar.y;   // Undo mapping [0;1]
//                if(currentDepth - bias > closestDepth)
//                    shadow += 1.0;
//            }
//        }
//    }
//    shadow /= pow(samples, 3);

    return shadow;
}

float ShadowCalculation(int index, vec3 lightDir, vec3 normal)
{
    vec4 fragPosLightSpace = v_FragPosLightSpaces[index];

    //执行透视除法，得到NDC坐标
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    //将NDC坐标变换为0到1的范围
    projCoords = projCoords * 0.5 + 0.5;

    //当前像素超出光源视锥的远平面
    if(projCoords.z > 1.0)
        return 0.0;

    //取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;

    //检查当前片段是否在阴影中
    Light light = u_lights[index];
    float bias = light.shadowBias;
    float d = dot(normal, lightDir);
    bias = clamp(1 / (light.shadowBiasChangeRate * d), 0, 1) * bias;
    //bias = max(bias * (1.0 - d), 0.0001);
    //bias = bias * tan(acos(d));
    //bias = clamp(bias, 0.0, u_lights[index].shadowBias);
     
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMaps[index], 0);
    
    //采样周围边长为shadowPCFSize*2+1正方形区域内的所有阴影深度值，与当前片元的阴影深度作比较（可优化为泊松圆盘采样(Poisson-Disk Sampling)）
    int shadowPCFSize = u_lights[index].shadowPCFSize;
    for(int x = -shadowPCFSize; x <= shadowPCFSize; ++x)
    {
        for(int y = -shadowPCFSize; y <= shadowPCFSize; ++y)
        {
            float pcfDepth = texture(u_shadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias / fragPosLightSpace.w > pcfDepth ? 1.0 : 0.0;      
        }    
    }
    shadow /= pow(1 + shadowPCFSize * 2, 2);
    return shadow;
};

void main()
{
    //纹理颜色
    vec4 texColor = texture(u_mainTexture, v_TexCoord);
    
    //基础颜色 (包括透明度)
    vec4 baseColor = texColor * u_objectColor;
    
    //从法线贴图范围[0,1]获取法线
    vec3 normal = texture(u_normalTexture, v_TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); //将法线向量转换为范围[-1,1]
    normal = normalize(v_TBN * normal); //从切线空间转到世界空间

    //片元指向摄像机的方向
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    //镜面贴图（粗糙度）
    vec3 specularTexColor = vec3(texture(u_specularTexture, v_TexCoord));

    //反射光
    vec3 viewReflectDir = reflect(-viewDir, normal); //世界空间中的视线反射方向
    vec3 reflectColor = texture(u_cubemap, viewReflectDir).rgb; //立方体贴图采样反射光
    reflectColor *= specularTexColor;
    reflectColor *= u_specularColor;
    reflectColor *= baseColor.a; //透明度越大反射光越弱

    //折射光
    vec3 refractColor = vec3(0);
    if(u_enableRefract)
    {
        vec3 viewRefractDir = refract(-viewDir, normal, 1.0f / u_refractiveIndex); //世界空间中的视线折射方向
        refractColor = texture(u_cubemap, viewRefractDir).rgb; //立方体贴图采样折射光
        refractColor *= u_refractColor;
        refractColor *= 1 - baseColor.a; //透明度越大折射光越强
    }
    

    vec3 allLightsColor = vec3(0.0);
    for (int i = 0; i < u_lightNum; i++)
    {
        Light light = u_lights[i];  

        //跳过无亮度的光照
        if (light.type == NONE_LIGHT || light.color == vec3(0) || light.brightness <= 0 || v_LightsEnable[i] <= 0)
        {
            continue;
        }

        vec3 lightDir; //光照方向的反方向(指向光源的方向)
        float lightDistance; //记录光源距离
        float intensity = 1; //局部的光照强度（目前仅用于聚光灯）

        if (light.type == PARALLEL_LIGHT) //平行光
        {
            lightDir = -normalize(light.direction);
            lightDistance = 0; //平行光的光源距离恒为0
        }
        else if (light.type == POINT_LIGHT || light.type == SPOT_LIGHT) //点光或聚光
        {
            vec3 fragToLight = light.pos - v_FragPos;
            lightDir = normalize(fragToLight);
            lightDistance = length(fragToLight);
            if (light.type == SPOT_LIGHT)
            {
                //计算聚光可见范围(TODO:下面很多计算可提取到cpu中计算)
                float cosOuterCutoffAngle = cos(radians(light.cutoffAngle.y));
                float cosInnerCutoffAngle = cos(radians(light.cutoffAngle.x)); 
                float epsilon = cosInnerCutoffAngle - cosOuterCutoffAngle;
                float cosLightAngle = dot(lightDir, -normalize(light.direction));
                intensity = clamp((cosLightAngle - cosOuterCutoffAngle) / epsilon, 0.0, 1.0);
            }
        }
        else
        {
            outColor = vec4(1, 0, 1, 1); //错误警示色
            return;
        }
        
        //衰减
        float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * lightDistance + light.attenuation[2] * (lightDistance * lightDistance));

        //光照值
        vec3 lightColor = light.color * light.brightness * attenuation;

        //漫反射
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;
    
        //高光反射
        float specularStrength;
        if(light.useBlinnPhong){
            //Blinn-Phong
            vec3 midDir = normalize(viewDir + lightDir);
            specularStrength = pow(max(dot(midDir, normal), 0.0), u_shininess);
        }
        else {
            //Phong
            vec3 reflectDir = reflect(-lightDir, normal);
            specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
        }

        vec3 specular = u_specularColor * specularStrength * specularTexColor * lightColor;
        
        //阴影值
        float shadow = light.castShadow ? light.type == POINT_LIGHT ? ShadowCalculation_pointLight(i, lightDir, normal) : ShadowCalculation(i, lightDir, normal) : 0;

        //合并所有光照
        allLightsColor += (diffuse + specular) * intensity * (1.0 - shadow);
        allLightsColor *= baseColor.a; //透视会减少光照的反射
    }
    //合并所有颜色
    outColor = vec4(baseColor.rgb * (allLightsColor + u_emission_inside + u_ambient + reflectColor + refractColor) + u_emission_outside, u_enableRefract ? 1 : baseColor.a); //当使用折射时透明度保持为1
};