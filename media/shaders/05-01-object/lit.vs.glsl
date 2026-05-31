#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_worldPosition;
out vec3 v_worldNormal;
out vec2 v_texCoord;

void main()
{
    vec4 worldPosition = u_model * vec4(a_position, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(u_model)));

    v_worldPosition = worldPosition.xyz;
    v_worldNormal = normalize(normalMatrix * a_normal);
    v_texCoord = a_texCoord;

    gl_Position = u_projection * u_view * worldPosition;
}
