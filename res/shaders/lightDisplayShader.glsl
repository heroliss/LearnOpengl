##common
#version 450 core


##shader vertex
layout (location = 0) in vec4 aPos;

uniform mat4 u_Model;

void main()
{
    gl_Position = u_Model * aPos;
}



##shader geometry
// 光照类型定义(从3D_basic.shader复制来的)
#define MAX_LIGHT_COUNT 12
#define NONE_LIGHT 0
#define PARALLEL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3
struct Light
{
    int type;
    bool useBlinnPhong;
    float brightness;
    vec3 pos; //光源位置 (平行光无用)
    vec3 direction; //照射方向（点光源无用）
    vec3 color;
    vec3 attenuation; //衰减系数 （分别为常数项、一次项、二次项系数 , 一般常数项固定为1，主要调二次项系数）
    vec2 cutoffAngle; //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)
};

layout(points) in;
layout(line_strip, max_vertices = 27) out;

layout(binding = 0, std140) uniform Matrices
{
    mat4 u_View;
    mat4 u_Projection;
};
layout(binding = 1, std140) uniform Lights
{
    int u_lightNum;
    Light u_lights[MAX_LIGHT_COUNT];
};

uniform int u_lightIndex;
uniform float u_size;

out vec4 gs_Color;

void DrawLine_point(vec3 dir)
{
    Light light = u_lights[u_lightIndex];
    vec4 worldPos = gl_in[0].gl_Position;
    
    gs_Color = vec4(light.color, 0);
    gl_Position = u_Projection * u_View * (worldPos - vec4(dir, 0) * u_size * 1.5);
    EmitVertex();
        
    gs_Color = vec4(light.color, 1);
    gl_Position = u_Projection * u_View * worldPos;
    EmitVertex();
        
    gs_Color = vec4(light.color, 0);
    gl_Position = u_Projection * u_View * (worldPos + vec4(dir, 0) * u_size * 1.5);
    EmitVertex();
    EndPrimitive();
}

//dir：模拟平行光线线条相对于中心点的方向  lastRootPos：光线根部连接上一个点的位置
vec4 DrawLine_parallel(vec4 lastRootPos, vec3 dir) 
{
    Light light = u_lights[u_lightIndex];
    vec4 worldPos = gl_in[0].gl_Position;
    
    gs_Color = vec4(light.color, 1);
    gl_Position = u_Projection * u_View * lastRootPos;
    EmitVertex();

    gs_Color = vec4(light.color, 1);
    vec4 rootPos = worldPos + vec4(dir, 0) * u_size * 0.6;
    gl_Position =  u_Projection * u_View * rootPos;
    EmitVertex();
        
    gs_Color = vec4(light.color, 0);
    gl_Position = u_Projection * u_View * (rootPos + vec4(light.direction, 0) * u_size * 3.0);
    EmitVertex();
    EndPrimitive();

    return rootPos;
}

void DrawLine_spot(vec3 dir) 
{
    Light light = u_lights[u_lightIndex];
    vec4 worldPos = gl_in[0].gl_Position;
    
    //计算外圈的方向向量
    float radian = radians(light.cutoffAngle.y);
    float cosAngle = cos(radian);
    float sinAngle = sin(radian);
    vec3 rayDir = light.direction * cosAngle + dir * sinAngle;

    gs_Color = vec4(light.color, 1);
    gl_Position =  u_Projection * u_View * worldPos;
    EmitVertex();
        
    gs_Color = vec4(light.color, 0);
    gl_Position = u_Projection * u_View * (worldPos + vec4(rayDir, 0) * u_size * 5.0);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    Light light = u_lights[u_lightIndex];
    vec4 worldPos = gl_in[0].gl_Position;

    vec3 forward = light.direction;
    vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
    vec3 up = cross(right, forward);
    
    if (light.type == POINT_LIGHT)
    {
        DrawLine_point(forward);
        DrawLine_point(right);
        DrawLine_point(up);
        DrawLine_point(normalize(forward + right));
        DrawLine_point(normalize(forward + up));
        DrawLine_point(normalize(right + up));      
        DrawLine_point(normalize(forward - right));
        DrawLine_point(normalize(forward - up));
        DrawLine_point(normalize(right - up));
    }
    else if(light.type == PARALLEL_LIGHT)
    {
        vec4 pos1 = DrawLine_parallel(worldPos + vec4(normalize(right - up), 0) * u_size * 0.6, right);
        vec4 pos2 = DrawLine_parallel(pos1, normalize(right + up));   
        vec4 pos3 = DrawLine_parallel(pos2, up);
        vec4 pos4 = DrawLine_parallel(pos3, normalize(-right + up));   
        vec4 pos5 = DrawLine_parallel(pos4, -right);        
        vec4 pos6 = DrawLine_parallel(pos5, normalize(-right - up));   
        vec4 pos7 = DrawLine_parallel(pos6, -up);
        vec4 pos8 = DrawLine_parallel(pos7, normalize(right - up));   
    }
    else if(light.type == SPOT_LIGHT)
    {
        DrawLine_spot(right);
        DrawLine_spot(normalize(right + up));   
        DrawLine_spot(up);
        DrawLine_spot(normalize(-right + up));   
        DrawLine_spot(-right);        
        DrawLine_spot(normalize(-right - up));   
        DrawLine_spot(-up);
        DrawLine_spot(normalize(right - up));   
    }
}



##shader fragment
in vec4 gs_Color;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = gs_Color;
};