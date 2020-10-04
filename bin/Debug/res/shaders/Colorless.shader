#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCord;

uniform mat4 mvp;
out vec2 v_texCord;

void main()
{
    gl_Position = mvp*vec4(position,1.0);
    v_texCord = texCord;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in vec2 v_texCord;
out vec4 color;
uniform sampler2D texSlot;

void main()
{
    color = texture(texSlot, v_texCord);
    if(color.a < 0.5)
        discard;
}
