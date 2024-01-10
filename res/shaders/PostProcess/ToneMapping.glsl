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
uniform float gamma = 2.2;
uniform bool useACESToneMapping;
uniform float adapted_lum;

vec3 ACESToneMapping(vec3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

void main()
{ 
    vec4 color = texture(u_screenTextures[0], TexCoords);
	if(useACESToneMapping)
	{
		color.rgb = ACESToneMapping(color.rgb, adapted_lum);
	}
    color.rgb = pow(color.rgb, vec3(1.0/gamma));
    FragColor = color;
}