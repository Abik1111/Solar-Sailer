#shader vertex
#version 400 core

const int MAX_JOINTS = 50;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCord;
layout(location = 3) in uvec3 joints;
layout(location = 4) in vec3 weights;

out vec2 v_texCord;
out vec3 v_normal;

uniform mat4 u_viewProjection;
uniform mat4 u_jointTransforms[MAX_JOINTS];

void main()
{
    vec4 totalPos = vec4(0.0);
    vec4 totalNormal = vec4(0.0);

    for(int i=0; i<3; i++)
    {
        mat4 jointTransform = u_jointTransforms[joints[i]];

        //Calculating corresponding position using weight and id
        vec4 posePosition = jointTransform*vec4(position, 1.0);
        totalPos += (posePosition*weights[i]);

        //Calculating corresponding normals
        vec4 worldNormal = jointTransform*vec4(normal, 1.0);
        totalNormal += (worldNormal*weights[i]);
    }

    gl_Position = u_viewProjection*totalPos;
    //gl_Position = u_viewProjection*(1/totalPos.w)*totalPos;
    //gl_Position = u_viewProjection*vec4(position, 1.0);

    v_normal = normalize(vec3(totalNormal));
    v_texCord = texCord;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in  vec2 v_texCord;
in vec3 v_normal;

uniform sampler2D u_slot;

out vec4 color;

void main()
{
    color = texture(u_slot, v_texCord);
}
