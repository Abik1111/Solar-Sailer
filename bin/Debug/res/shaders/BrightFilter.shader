#shader vertex
#version 400 core

layout(location = 0) in vec2 position;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(position, -1.0, 1.0);
    v_TexCoord = position.xy/2.0+0.5;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in vec2 v_TexCoord;
uniform sampler2D u_slot;
uniform int u_width;
uniform int u_height;

out vec4 color;
void main()
{
    //Handle color here
    color = texture(u_slot, v_TexCoord);
    float brightness = (color.r*0.2126)+(color.g*0.7152)+(color.b*0.0722);
    color = brightness*color;
}
