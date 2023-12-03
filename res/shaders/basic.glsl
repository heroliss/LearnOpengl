##shader vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 posColor;
layout(location = 3) in float texIndex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform float u_TrapezoidFactor;

out vec2 v_TexCoord;
out vec4 v_PosColor;
out float v_TexIndex;


//坐标转换为梯形，下底不变，上边宽度由Factor指定，0为全宽，1缩为一个点
vec4 trapezoidTransform(vec4 p, float factor)
{
    float scaleX = 1.0f - 0.5f * factor * (p.y + 1);
    return vec4(p.x * scaleX, p.y, p.z, p.w);
    return p;
}

void main()
{
    //注意乘法要从右向左读
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1);
    //gl_Position = trapezoidTransform(gl_Position, u_TrapezoidFactor);
    v_TexCoord = texCoord;
    v_PosColor = posColor;
    v_TexIndex = texIndex;
}




##shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_PosColor;
in float v_TexIndex;

uniform vec4 u_Color;
uniform sampler2D u_Textures[2];

void main()
{
    vec4 texColor = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
    color = texColor * u_Color * v_PosColor;
};