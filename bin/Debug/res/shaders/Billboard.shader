#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    //gl_Position.z = -1.0;
    //gl_Position = u_projection*gl_Position;
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
    if(color.a<0.1){
        discard;
    }
}
