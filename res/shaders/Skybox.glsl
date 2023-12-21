##shader vertex
#version 450 core

in vec3 position;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 textureDir;

//mat4 scaleMatrix(float scaleX, float scaleY, float scaleZ) {
//    return mat4(
//        vec4(scaleX, 0.0, 0.0, 0.0),
//        vec4(0.0, scaleY, 0.0, 0.0),
//        vec4(0.0, 0.0, scaleZ, 0.0),
//        vec4(0.0, 0.0, 0.0, 1.0)
//    );
//}

void main()
{
    textureDir = position;
    vec4 pos = u_Projection * u_View * vec4(position, 1);
    gl_Position = pos.xyww;
}




##shader fragment
#version 450 core

uniform samplerCube cubemap;

in vec3 textureDir;

out vec4 FragColor;

void main()
{
    FragColor = texture(cubemap, textureDir);
    //FragColor = vec4(0,1,0,1);
}