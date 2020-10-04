#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCord;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 v_TexCoord;

void main()
{
    gl_Position = u_projection * u_view * vec4(position, 1.0);
    v_TexCoord = texCord;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in vec2 v_TexCoord;
uniform sampler2D u_slot;

out vec4 color;

void main()
{
    color = texture(u_slot, v_TexCoord);

    if(color.a==0)
        discard;
}
