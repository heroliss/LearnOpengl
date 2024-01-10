##shader vertex
#version 330 core

in vec2 position;
in vec2 texCoord;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0); 
    TexCoords = texCoord;
}


##shader fragment
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_screenTextures[1];

void main()
{ 
    vec4 color = texture(u_screenTextures[0], TexCoords);
    FragColor = vec4(vec3(1 - color), 1);
}