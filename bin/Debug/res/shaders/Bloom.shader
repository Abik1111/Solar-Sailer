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
uniform sampler2D u_slot0;
uniform sampler2D u_slot1;
uniform float u_brightness;

out vec4 color;
void main()
{
    //Handle color here
    vec4 color0 = texture(u_slot0, v_TexCoord);
    vec4 color1 = texture(u_slot1, v_TexCoord);
    color = u_brightness*color0+color1;
}
