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

struct VertexData
{
    vec2 TexCoord;
    mat3 TBN; //切线空间转世界空间的矩阵
    mat3 inversedTBN; //世界空间转切线空间的矩阵
    vec3 FragPos; //世界空间的片元位置
    //vec4 FragPosLightSpaces[MAX_LIGHT_COUNT]; //光照空间的片元位置 //预计算
    float LightsEnable[MAX_LIGHT_COUNT];
    //预计算切线空间
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    //vec3 TangentLightDir[MAX_LIGHT_COUNT]; //预计算
};


##shader vertex
uniform mat4 u_Model;
uniform vec3 u_viewPos;

//下面in的变量顺序不能随便改变，和顶点数据结构相关
in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;
in mat4 instanceMatrix;

out VertexData v;

void main()
{
    mat4 modelMatrix = u_Model == mat4(0) ? instanceMatrix : u_Model;

    vec4 worldPos = modelMatrix * vec4(position, 1.0); 
    v.FragPos = worldPos.xyz; //输出世界空间的顶点位置
    gl_Position = u_Projection * u_View * worldPos; //输出齐次裁剪空间的顶点位置
    v.TexCoord = texCoord; //输出纹理坐标
    
    //计算法线矩阵
    mat3 worldNormalMatrix = mat3(transpose(inverse(modelMatrix)));

    //计算世界空间中的切线空间基的方向
    vec3 T = normalize(worldNormalMatrix * tangent);
    vec3 N = normalize(worldNormalMatrix * normal);
    vec3 B = normalize(worldNormalMatrix * bitangent);

    //TBN向量重正交化(使用格拉姆-施密特正交化算法) //TODO: 这里为什么不对
    //    T = normalize(T - dot(T, N) * N);
    //    vec3 B = cross(T, N);

    v.TBN = mat3(T, B, N); //输出切线空间转世界空间的矩阵

    //预计算切线空间
    v.inversedTBN = transpose(v.TBN); //世界空间转切线空间的矩阵
    v.TangentViewPos  = v.inversedTBN * u_viewPos;
    v.TangentFragPos  = v.inversedTBN * v.FragPos;

    //计算光照空间的顶点位置
    for (int i = 0; i < u_lightNum; i++)
    {
        Light light = u_lights[i];

        //非点光源时计算片元在光照空间中的位置
        //if(light.type != POINT_LIGHT)
        //    v.FragPosLightSpaces[i] = light.lightSpaceMatrix * worldPos;  //预计算：光照空间顶点位置

        //TOOD:下面是乱加的：用于解决 加了法线贴图后背面被照亮 和 光源在背面也有高光泄露 的问题（如果开启了阴影并且不是正面剔除，加了法线贴图后背面被照亮没有这个问题了）
        vec3 lightDir;
        if (light.type == PARALLEL_LIGHT) //平行光
        {
            lightDir = -normalize(light.direction);
        }
        else if (light.type == POINT_LIGHT || light.type == SPOT_LIGHT) //点光或聚光
        {
            lightDir = normalize(light.pos - v.FragPos);
        }
        v.LightsEnable[i] = dot(N, lightDir);
        //v.TangentLightDir[i] = v.inversedTBN * lightDir; //预计算：切线空间的光照方向
    }
}



##shader geometry
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData v[];
out VertexData f;
out vec2 TrangleTexCoord[3]; //三角形三个顶点的纹理坐标
out vec3 TrangleFragPos[3];
//out vec3 TrangleFragPosLightSpaces[ 6]; //三角形内三个顶点的光照空间位置  //预计算
//out vec3 TrangleTangentLightDir[ 4];  //预计算

void main()
{
    for(int i = 0; i < gl_in.length(); i++) {
        f = v[i];
        TrangleFragPos[i] = f.FragPos;
        TrangleTexCoord[i] = f.TexCoord;
        //传递三角形三个顶点的预计算值
        //for(int j = 0; j < u_lightNum; j++)
        //{
        //    TrangleFragPosLightSpaces[j * 3 + i] = f.FragPosLightSpaces[j].xyz;
        //    TrangleTangentLightDir[j * 3 + i] = f.TangentLightDir[j];
        //}
    }

    for(int i = 0; i < gl_in.length(); i++) {
        f = v[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
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

//视差贴图
uniform sampler2D u_heightTexture; 
uniform float u_heightTextureScale;
uniform float u_heightTextureShadowScale;
uniform ivec2 u_heightTextureMinAndMaxLayerNum;
uniform ivec2 u_parallaxShadowMinAndMaxLayerNum;
uniform bool u_enableHeightTexture;

uniform bool u_ParallaxOffsetLimit;
uniform bool u_ReliefParallax;
uniform bool u_ParallaxOcclusion;
uniform int u_HalfSearchNum;
uniform bool u_enableHeightTextureSelfShadow;
uniform bool u_enableParallaxFragPos;

uniform samplerCube u_cubemap; //环境立方体贴图
//折射
uniform bool u_enableRefract;//开启折射（目前仅支持折射天空盒，此时透明度恒为1）
uniform vec3 u_refractColor; //折射颜色
uniform float u_refractiveIndex; //折射率
//阴影
uniform sampler2D u_shadowMaps[MAX_LIGHT_COUNT]; //阴影深度纹理，与u_lights对应
uniform samplerCube u_shadowCubemaps[MAX_LIGHT_COUNT]; //阴影立方体深度纹理，与u_lights对应

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 BrightColor;

in VertexData f;
in vec2 TrangleTexCoord[3]; //三角形三个顶点的纹理坐标
in vec3 TrangleFragPos[3];
//in vec3 TrangleFragPosLightSpaces[6]; //三角形内三个顶点的光照空间位置 //预计算
//in vec3 TrangleTangentLightDir[4]; //预计算

const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

// 边函数，用于计算重心坐标
float edgeFunction(vec2 a, vec2 b, vec2 c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

// 重心坐标插值计算函数
vec3 calculateBarycentricFromUV(vec2 uv_changed, vec2 uv[3]) {
    // 计算重心坐标
    float areaTotal = edgeFunction(uv[0], uv[1], uv[2]);
    float area0 = edgeFunction(uv[1], uv[2], uv_changed);
    float area1 = edgeFunction(uv[2], uv[0], uv_changed);
    float area2 = edgeFunction(uv[0], uv[1], uv_changed);

    vec3 barycentric;
    barycentric[0] = area0 / areaTotal;
    barycentric[1] = area1 / areaTotal;
    barycentric[2] = area2 / areaTotal;
    return barycentric;
}

vec3 calculateByBarycentric(vec3 barycentric, vec3 pos0, vec3 pos1, vec3 pos2)
{
    vec3 newPosition = barycentric[0] * pos0 +
                            barycentric[1] * pos1 +
                            barycentric[2] * pos2;

    return newPosition;
}

float ShadowCalculation_pointLight(int index, vec3 tangentLightDir, vec3 fragPos)
{
    Light light = u_lights[index];
    float far_plane = light.shadowNearAndFar.y;
    //vec3 lightToFrag = f.FragPos - light.pos; 
    vec3 lightToFrag = fragPos - light.pos; 

    float bias = light.shadowBias;
    float d = dot(tangentLightDir, vec3(0,0,1));
    bias = clamp(1 / (light.shadowBiasChangeRate * d), 0, 1) * bias;

    float currentDepth = length(lightToFrag);
    if(currentDepth > far_plane)
        return 0.0;

    //float diskRadius = (light.shadowSampleDiskRadius + (length(u_viewPos - f.FragPos) / far_plane));
    float diskRadius = light.shadowSampleDiskRadius;

    float shadow = 0.0;
    for(int i = 0; i < 20; ++i)
    {
        float closestDepth = texture(u_shadowCubemaps[index], lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // Undo mapping [0;1]
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
//                closestDepth *= far_plane;   // Undo mapping [0;1]
//                if(currentDepth - bias > closestDepth)
//                    shadow += 1.0;
//            }
//        }
//    }
//    shadow /= pow(samples, 3);

    return shadow;
}

float ShadowCalculation(int index, vec3 tangentLightDir, vec4 fragPosLightSpace) //这里lightDir是
{
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
    float d = dot(tangentLightDir, vec3(0,0,1));
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

vec2 ParallaxMapping(vec2 uv, vec3 tangentViewDir, out float finalHeight)
{
    //陡峭视差映射
    // 优化：根据视角来决定分层数(因为视线方向越垂直于平面，纹理偏移量较少，不需要过多的层数来维持精度)
    float layerNum = mix(u_heightTextureMinAndMaxLayerNum.y, u_heightTextureMinAndMaxLayerNum.x, abs(dot(vec3(0,0,1), tangentViewDir)));//层数
    float layerDepth = 1 / layerNum;//层深					
    vec2 deltaTexCoords = vec2(0);//层深对应偏移量
    if (u_ParallaxOffsetLimit) //建议使用偏移量限制，否则视线方向越平行于平面偏移量过大，分层明显
    {
        deltaTexCoords = tangentViewDir.xy / layerNum * u_heightTextureScale;
    }
    else
    {
        deltaTexCoords = tangentViewDir.xy / tangentViewDir.z / layerNum * u_heightTextureScale;
    }
    vec2 currentTexCoords = uv;//当前层纹理坐标
    float currentDepthMapValue = texture(u_heightTexture, currentTexCoords).r;//当前纹理坐标采样结果
    float currentLayerDepth = 0;//当前层深度
    // unable to unroll loop, loop does not appear to terminate in a timely manner
    // 上面这个错误是在循环内使用tex2D导致的，需要加上unroll来限制循环次数或者改用tex2Dlod
    // [unroll(100)]
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        // currentDepthMapValue = tex2D(_HeightMap, currentTexCoords).r;
        currentDepthMapValue = texture(u_heightTexture, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    finalHeight = currentLayerDepth; //输出当前深度
    vec2 finalTexCoords = currentTexCoords;

    if (u_ReliefParallax)//浮雕视差映射（Relief Parallax Mapping）
    {
        // 二分查找
        vec2 halfDeltaTexCoords = deltaTexCoords / 2;
        float halfLayerDepth = layerDepth / 2;
        currentTexCoords += halfDeltaTexCoords;
        currentLayerDepth += halfLayerDepth;

        for(int i = 0; i < u_HalfSearchNum; i++)
        {
	        halfDeltaTexCoords = halfDeltaTexCoords / 2;
	        halfLayerDepth = halfLayerDepth / 2;
	        currentDepthMapValue = texture(u_heightTexture, currentTexCoords).r;
	        if(currentDepthMapValue > currentLayerDepth)
	        {
		        currentTexCoords -= halfDeltaTexCoords;
		        currentLayerDepth += halfLayerDepth;
	        }
	        else
	        {
		        currentTexCoords += halfDeltaTexCoords;
		        currentLayerDepth -= halfLayerDepth;
	        }
        }
        finalTexCoords = currentTexCoords;
    }

    if (u_ParallaxOcclusion)  //视差遮挡映射（Parallax Occlusion Mapping）
    {
        // get texture coordinates before collision (reverse operations)
        vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
        // get depth after and before collision for linear interpolation
        float afterDepth  = currentDepthMapValue - currentLayerDepth;
        float beforeDepth = texture(u_heightTexture, prevTexCoords).r - currentLayerDepth + layerDepth;
        // interpolation of texture coordinates
        float weight = afterDepth / (afterDepth - beforeDepth);
        finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
    }

    return finalTexCoords;
}

// 输入的initialUV和initialHeight均为视差遮挡映射的结果
float ParallaxShadow(vec2 initialUV, vec3 tangentLightDir, float initialHeight)
{
    float shadowMultiplier = 0;
    if (dot(vec3(0, 0, 1), tangentLightDir) > 0) //只算正对阳光的面 (这句删掉可能会导致崩溃)
    {
        // 根据光线方向决定层数（道理和视线方向一样）
	    float numLayers = mix(u_parallaxShadowMinAndMaxLayerNum.y, u_parallaxShadowMinAndMaxLayerNum.x, abs(dot(vec3(0, 0, 1), tangentLightDir)));
	    float layerHeight = initialHeight / numLayers; //从当前点开始计算层深（没必要以整个范围）
        vec2 texStep = vec2(0); //层深对应偏移量
        if (u_ParallaxOffsetLimit)
        {
	        texStep = u_heightTextureScale * tangentLightDir.xy / numLayers;
        }
        else
        {
            texStep = u_heightTextureScale * tangentLightDir.xy / tangentLightDir.z / numLayers;
        }
        // 继续向上找是否有相交点
	    float currentLayerHeight = initialHeight - layerHeight; //当前相交点前的最后层深
	    vec2 currentTexCoords = initialUV + texStep;
	    float heightFromTexture = texture(u_heightTexture, currentTexCoords).r;
	    int stepIndex = 1; //向上查找次数
        int numSamplesUnderSurface = 0; //统计被遮挡的层数
	    while(currentLayerHeight > 0) //直到达到表面
	    {
	        if(heightFromTexture < currentLayerHeight) //采样结果小于当前层深则有交点
            {
		        numSamplesUnderSurface++;              
                float atten = (1 - stepIndex / numLayers); //阴影的衰减值：越接近顶部（或者说浅处），阴影强度越小
                // 以当前层深到高度贴图采样值的距离作为阴影的强度并乘以阴影的衰减值
		        float newShadowMultiplier = (currentLayerHeight - heightFromTexture) * atten;
		        shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
	        }

	        stepIndex += 1;
	        currentLayerHeight -= layerHeight;
	        currentTexCoords += texStep;
	        heightFromTexture = texture(u_heightTexture, currentTexCoords).r;
	    }

	    if(numSamplesUnderSurface < 1) //没有交点，则不在阴影区域
        {
	        shadowMultiplier = 0;       
        }
    }
    return shadowMultiplier;
}

void main()
{    
    //切线空间中片元指向摄像机的方向 (这个方向不会受到视差贴图的影响)
    vec3 tangentViewDir = normalize(f.TangentViewPos - f.TangentFragPos);
    //片元指向摄像机的方向 (这个方向不会受到视差贴图的影响)
    vec3 viewDir = normalize(u_viewPos - f.FragPos);    

    //使用视差图获取改变后的纹理坐标
    vec2 parallaxTexCoords = f.TexCoord; //视差贴图偏移后的片元的纹理坐标，默认为原坐标
    vec3 parallaxFragPos = f.FragPos; //视差贴图偏移后的片元位置，默认为原位置
    float parallaxHeight;
    vec3 barycentric;
    if(u_enableHeightTexture)
    {
        parallaxTexCoords = ParallaxMapping(f.TexCoord, tangentViewDir, parallaxHeight);
        if(parallaxTexCoords.x > 1.0 || parallaxTexCoords.y > 1.0 || parallaxTexCoords.x < 0.0 || parallaxTexCoords.y < 0.0)
        discard;

        if(u_enableParallaxFragPos)
        {
            barycentric = calculateBarycentricFromUV(parallaxTexCoords, TrangleTexCoord);
            vec3 a = calculateByBarycentric(barycentric, TrangleFragPos[0], TrangleFragPos[1], TrangleFragPos[2]); //浮在表面的视差偏移后的点
            vec3 v = a - f.FragPos;
            vec3 v2 = -viewDir * dot(v, -viewDir);
            parallaxFragPos = f.FragPos + v2;
        }
    }

    //纹理颜色
    vec4 texColor = texture(u_mainTexture, parallaxTexCoords);
    
    //基础颜色 (包括透明度)
    vec4 baseColor = texColor * u_objectColor;
    
    //从法线贴图范围[0,1]获取法线
    vec3 parallaxNormal = texture(u_normalTexture, parallaxTexCoords).rgb;
    parallaxNormal = normalize(parallaxNormal * 2.0 - 1.0); //将法线向量转换为范围[-1,1]
    parallaxNormal = normalize(f.TBN * parallaxNormal); //从切线空间转到世界空间



    //镜面贴图（粗糙度）
    vec3 specularTexColor = vec3(texture(u_specularTexture, parallaxTexCoords));

    //反射光
    vec3 viewReflectDir = reflect(-viewDir, parallaxNormal); //世界空间中的视线反射方向
    vec3 reflectColor = texture(u_cubemap, viewReflectDir).rgb; //立方体贴图采样反射光
    reflectColor *= specularTexColor;
    reflectColor *= u_specularColor;
    reflectColor *= baseColor.a; //透明度越大反射光越弱

    //折射光
    vec3 refractColor = vec3(0);
    if(u_enableRefract)
    {
        vec3 viewRefractDir = refract(-viewDir, parallaxNormal, 1.0f / u_refractiveIndex); //世界空间中的视线折射方向
        refractColor = texture(u_cubemap, viewRefractDir).rgb; //立方体贴图采样折射光
        refractColor *= u_refractColor;
        refractColor *= 1 - baseColor.a; //透明度越大折射光越强
    }
    

    vec3 allLightsColor = vec3(0.0);
    for (int i = 0; i < u_lightNum; i++)
    {
        Light light = u_lights[i];  

        //跳过无亮度的光照
        if (light.type == NONE_LIGHT || light.color == vec3(0) || light.brightness <= 0 || f.LightsEnable[i] <= 0)
        {
            continue;
        }

        vec3 parallaxLightDir; //光照方向的反方向(指向光源的方向)
        float parallaxLightDistance; //记录光源距离
        float intensity = 1; //局部的光照强度（目前仅用于聚光灯）

        if (light.type == PARALLEL_LIGHT) //平行光
        {
            parallaxLightDir = -normalize(light.direction);
            parallaxLightDistance = 0; //平行光的光源距离恒为0
        }
        else if (light.type == POINT_LIGHT || light.type == SPOT_LIGHT) //点光或聚光
        {
            vec3 parallaxFragToLight = light.pos - parallaxFragPos;
            parallaxLightDir = normalize(parallaxFragToLight);
            parallaxLightDistance = length(parallaxFragToLight);
            if (light.type == SPOT_LIGHT)
            {
                //计算聚光可见范围(TODO:下面很多计算可提取到cpu中计算)
                float cosOuterCutoffAngle = cos(radians(light.cutoffAngle.y));
                float cosInnerCutoffAngle = cos(radians(light.cutoffAngle.x)); 
                float epsilon = cosInnerCutoffAngle - cosOuterCutoffAngle;
                float cosLightAngle = dot(parallaxLightDir, -normalize(light.direction));
                intensity = clamp((cosLightAngle - cosOuterCutoffAngle) / epsilon, 0.0, 1.0);
            }
        }
        else
        {
            outColor = vec4(1, 0, 1, 1); //错误警示色
            return;
        }
        
        //衰减
        float attenuation = 1.0 / (light.attenuation[0] + light.attenuation[1] * parallaxLightDistance + light.attenuation[2] * (parallaxLightDistance * parallaxLightDistance));

        //光照值
        vec3 lightColor = light.color * light.brightness * attenuation;

        //漫反射
        vec3 diffuse = max(dot(parallaxNormal, parallaxLightDir), 0.0) * lightColor;
    
        //高光反射
        float specularStrength;
        if(light.useBlinnPhong){
            //Blinn-Phong
            vec3 midDir = normalize(viewDir + parallaxLightDir);
            specularStrength = pow(max(dot(midDir, parallaxNormal), 0.0), u_shininess);
        }
        else {
            //Phong
            vec3 reflectDir = reflect(-parallaxLightDir, parallaxNormal);
            specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
        }

        vec3 specular = u_specularColor * specularStrength * specularTexColor * lightColor;
        
        //阴影值
        float shadow = 0;
        if(light.castShadow)
        {
            //vec4 fragPosLightSpace = f.FragPosLightSpaces[i]; //读取顶点着色器中预计算好的值
            //vec3 tangentLightDir = f.TangentLightDir[i]; //读取顶点着色器中预计算好的值
            //启用上面两句预计算的话，要解注释掉所有“//预计算”注释的语句，并且下面两句通用计算要移动到下面注释掉的if中，因为视差贴图偏移后就不能用预计算的值了
            vec4  fragPosLightSpace = light.lightSpaceMatrix * vec4(parallaxFragPos, 1);
            vec3  tangentLightDir = f.inversedTBN * parallaxLightDir;
            //if(u_enableParallaxFragPos) //使用视差贴图偏移后的位置计算阴影
            //{
                //fragPosLightSpace = vec4(calculateByBarycentric(barycentric, TrangleFragPosLightSpaces[i * 3 + 0], TrangleFragPosLightSpaces[i * 3 + 1], TrangleFragPosLightSpaces[i * 3 + 2]), 1);
                //tangentLightDir = calculateByBarycentric(barycentric, TrangleTangentLightDir[i * 3 + 0], TrangleTangentLightDir[i * 3 + 1], TrangleTangentLightDir[i * 3 + 2]);
            //}
            shadow = light.type == POINT_LIGHT ? ShadowCalculation_pointLight(i, tangentLightDir, parallaxFragPos) : ShadowCalculation(i, tangentLightDir, fragPosLightSpace);
            if(u_enableHeightTextureSelfShadow)  //计算视差贴图的自阴影
            {
                shadow += ParallaxShadow(parallaxTexCoords, tangentLightDir, parallaxHeight) * u_heightTextureShadowScale;
                shadow = clamp(shadow, 0, 1);
            }
        }
        //合并所有光照
        allLightsColor += (diffuse + specular) * intensity * (1.0 - shadow);
        allLightsColor *= baseColor.a; //透视会减少光照的反射
    }
    //合并所有颜色
    outColor = vec4(baseColor.rgb * (allLightsColor + u_emission_inside + u_ambient + reflectColor + refractColor) + u_emission_outside, u_enableRefract ? 1 : baseColor.a); //当使用折射时透明度保持为1
    
    //输出第二个渲染目标
    float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722)); //转换为灰度得到亮度值
    if(brightness > 1.0)
        BrightColor = vec4(outColor.rgb, 1.0); //输出高亮图像用于泛光效果
};