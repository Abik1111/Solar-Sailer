#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3)in vec3 tangent;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_lightDirection;
uniform vec3 u_viewPosition;

out vec2 v_TexCoord;
out vec3 v_normal;
out vec3 v_viewDirection;
out vec3 v_lightDirection;

void main()
{
    mat4 modelView = u_view * u_model;
    vec4 worldPos = u_model * vec4(position, 1.0);
    vec4 camPosition = u_view * worldPos;
    gl_Position = u_projection * camPosition ;

    v_TexCoord = texCoord;

    v_normal = normalize((modelView*vec4(normal, 1.0)).xyz);
    vec3 cam_tangent = normalize((modelView * vec4(tangent, 0.0)).xyz);
    vec3 cam_biTangent = normalize(cross(v_normal, cam_tangent));

    mat3 tangentSpace = mat3(
        cam_tangent.x, cam_biTangent.x, v_normal.x,
        cam_tangent.y, cam_biTangent.y, v_normal.y,
        cam_tangent.z, cam_biTangent.z, v_normal.z
    );

    v_viewDirection = normalize(tangentSpace * (u_viewPosition - vec3(worldPos)));
    v_lightDirection = normalize(tangentSpace * u_lightDirection);
}


//-----------------------------------------------
#shader fragment
#version 400 core

uniform sampler2D u_colorSampler;
uniform sampler2D u_normalSampler;
uniform vec3 u_lightColor;

in vec2 v_TexCoord;
in vec3 v_normal;
in vec3 v_fPos;
in vec3 v_viewDirection;
in vec3 v_lightDirection;

out vec4 color;

void main()
{
    vec4 texColor = texture(u_colorSampler, v_TexCoord);

    vec4 normal = 2.0*texture(u_normalSampler, v_TexCoord)-1.0;
    vec3 unitNormal = vec3(normalize(normal));

    float diff = max(dot(unitNormal, v_lightDirection), 0.0);
    vec3 diffuse = u_lightColor * diff ;

    vec3 reflectDir = reflect(-v_lightDirection, unitNormal);
    float spec = pow(max(dot(v_viewDirection, reflectDir), 0.0), 32.0);
    vec3 specular =  spec * u_lightColor;

    color = vec4(diffuse*vec3(texColor) + specular, 1.0);
}
