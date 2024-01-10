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
uniform float GrayscalePercentage;

void main()
{ 
    FragColor = texture(u_screenTextures[0], TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    average = GrayscalePercentage * average;
    float p = 1 - GrayscalePercentage;
    FragColor = vec4(average + p * FragColor.x, average + p * FragColor.y, average + p * FragColor.z, 1.0);
}