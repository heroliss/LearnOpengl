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

uniform sampler2D screenTexture;
uniform float gamma = 2.2;

void main()
{ 
    vec4 color = texture(screenTexture, TexCoords);
    color.rgb = pow(color.rgb, vec3(1.0/gamma));
    FragColor = color;
}