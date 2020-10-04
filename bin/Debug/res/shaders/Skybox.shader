#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
uniform mat4 u_mvp;
out vec3 v_TexCoord;

void main()
{
    gl_Position = u_mvp * vec4(position, 1.0);
    v_TexCoord = position;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in vec3 v_TexCoord;

uniform samplerCube cubeMap;

out vec4 color;
void main()
{
    color = texture(cubeMap, v_TexCoord);
}
