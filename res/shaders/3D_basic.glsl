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
uniform vec3 u_viewPos;

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

//预计算切线空间
out vec3 v_TangentViewPos;
out vec3 v_TangentFragPos;
out vec3 v_TangentLightDir[MAX_LIGHT_COUNT];

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

    //预计算切线空间
    mat3 inversedTBN = transpose(v_TBN); //世界空间转切线空间的矩阵
    v_TangentViewPos  = inversedTBN * u_viewPos;
    v_TangentFragPos  = inversedTBN * v_FragPos;

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
        v_TangentLightDir[i] = inversedTBN * lightDir; //输出切线空间的光照方向
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

//视差贴图
uniform sampler2D u_heightTexture; 
uniform float u_heightTextureScale;
uniform ivec2 u_heightTextureMinAndMaxLayerNum;
uniform bool u_enableHeightTexture;

uniform bool u_ParallaxOffsetLimit;
uniform bool u_ReliefParallax;
uniform bool u_ParallaxOcclusion;
uniform int u_HalfSearchNum;
uniform bool u_enableHeightTextureShadow;

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

in vec2 v_TexCoord;
in mat3 v_TBN;
in vec3 v_FragPos; //世界空间的片元位置
in vec4 v_FragPosLightSpaces[MAX_LIGHT_COUNT]; //光照空间的片元位置
in float v_LightsEnable[MAX_LIGHT_COUNT];

//预计算切线空间
in vec3 v_TangentViewPos;
in vec3 v_TangentFragPos;
in vec3 v_TangentLightDir[MAX_LIGHT_COUNT];

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
    float far_plane = light.shadowNearAndFar.y;
    vec3 lightToFrag = v_FragPos - light.pos; 

    float bias = light.shadowBias;
    float d = dot(normal, lightDir);
    bias = clamp(1 / (light.shadowBiasChangeRate * d), 0, 1) * bias;

    float currentDepth = length(lightToFrag);
    if(currentDepth > far_plane)
        return 0.0;

    //float diskRadius = (light.shadowSampleDiskRadius + (length(u_viewPos - v_FragPos) / far_plane));
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

vec2 ParallaxMapping(vec2 uv, vec3 viewDir, out float finalHeight)
{
    //陡峭视差映射
    // 优化：根据视角来决定分层数(因为视线方向越垂直于平面，纹理偏移量较少，不需要过多的层数来维持精度)
    float layerNum = mix(u_heightTextureMinAndMaxLayerNum.y, u_heightTextureMinAndMaxLayerNum.x, abs(dot(vec3(0,0,1), viewDir)));//层数
    float layerDepth = 1 / layerNum;//层深					
    vec2 deltaTexCoords = vec2(0);//层深对应偏移量
    if (u_ParallaxOffsetLimit) //建议使用偏移量限制，否则视线方向越平行于平面偏移量过大，分层明显
    {
        deltaTexCoords = viewDir.xy / layerNum * u_heightTextureScale;
    }
    else
    {
        deltaTexCoords = viewDir.xy / viewDir.z / layerNum * u_heightTextureScale;
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
float ParallaxShadow(vec2 initialUV, vec3 lightDir, float initialHeight)
{
    float shadowMultiplier = 0;
    if (dot(vec3(0, 0, 1), lightDir) > 0) //只算正对阳光的面
    {
        // 根据光线方向决定层数（道理和视线方向一样）
	float numLayers = mix(u_heightTextureMinAndMaxLayerNum.y, u_heightTextureMinAndMaxLayerNum.x, abs(dot(vec3(0, 0, 1), lightDir)));
	float layerHeight = initialHeight / numLayers; //从当前点开始计算层深（没必要以整个范围）
    vec2 texStep = vec2(0); //层深对应偏移量
    if (u_ParallaxOffsetLimit)
    {
	    texStep = u_heightTextureScale * lightDir.xy / numLayers;
    }
    else
    {
        texStep = u_heightTextureScale * lightDir.xy / lightDir.z / numLayers;
    }
    // 继续向上找是否有相交点
	float currentLayerHeight = initialHeight - layerHeight; //当前相交点前的最后层深
	vec2 currentTexCoords = initialUV + texStep;
	float heightFromTexture = texture(u_heightTexture, currentTexCoords).r;
	int stepIndex = 1; //向上查找次数
        float numSamplesUnderSurface = 0; //统计被遮挡的层数
	while(currentLayerHeight > 0) //直到达到表面
	{
	    if(heightFromTexture < currentLayerHeight) //采样结果小于当前层深则有交点
            {
		numSamplesUnderSurface += 1;              
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
	    shadowMultiplier = 1;
	else 
	    shadowMultiplier = 1 - shadowMultiplier;
    }
    return shadowMultiplier;
}

void main()
{    
    //切线空间中片元指向摄像机的方向
    vec3 tangentViewDir = normalize(v_TangentViewPos - v_TangentFragPos);
    
    //使用视差图获取改变后的纹理坐标
    vec2 texCoords = v_TexCoord;
    float parallaxHeight;
    if(u_enableHeightTexture)
    {
        texCoords = ParallaxMapping(v_TexCoord, tangentViewDir, parallaxHeight);
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    }

    //纹理颜色
    vec4 texColor = texture(u_mainTexture, texCoords);
    
    //基础颜色 (包括透明度)
    vec4 baseColor = texColor * u_objectColor;
    
    //从法线贴图范围[0,1]获取法线
    vec3 normal = texture(u_normalTexture, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); //将法线向量转换为范围[-1,1]
    normal = normalize(v_TBN * normal); //从切线空间转到世界空间

    //片元指向摄像机的方向
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    //镜面贴图（粗糙度）
    vec3 specularTexColor = vec3(texture(u_specularTexture, texCoords));

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
        if(u_enableHeightTexture && u_enableHeightTextureShadow && light.castShadow)
        {
            shadow = clamp(shadow + 1 - ParallaxShadow(texCoords, v_TangentLightDir[i], parallaxHeight), 0, 1);
        }
        //合并所有光照
        allLightsColor += (diffuse + specular) * intensity * (1.0 - shadow);
        allLightsColor *= baseColor.a; //透视会减少光照的反射
    }
    //合并所有颜色
    outColor = vec4(baseColor.rgb * (allLightsColor + u_emission_inside + u_ambient + reflectColor + refractColor) + u_emission_outside, u_enableRefract ? 1 : baseColor.a); //当使用折射时透明度保持为1
    
    float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722)); //转换为灰度得到亮度值
    if(brightness > 1.0)
        BrightColor = vec4(outColor.rgb, 1.0); //输出高亮图像用于泛光效果
};