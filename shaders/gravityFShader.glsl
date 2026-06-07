#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform vec3 col;
uniform sampler2D tex;

void main()
{
    vec4 texColor = texture(tex, texCoord); 
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor * vec4(col, 1.0f);
}