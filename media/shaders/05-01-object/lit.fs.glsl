#version 460 core

struct SunLight
{
    int enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    int enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

in vec3 v_worldPosition;
in vec3 v_worldNormal;
in vec2 v_texCoord;

uniform vec3 u_viewPos;
uniform vec3 u_albedo;
uniform float u_shininess;
uniform float u_specularStrength;
uniform SunLight u_sunLight;
uniform PointLight u_pointLights[3];

out vec4 fragColor;

vec3 applySunLight(SunLight light, vec3 normal, vec3 viewDir, vec3 baseColor)
{
    if (light.enabled == 0)
    {
        return vec3(0.0);
    }

    vec3 lightDir = normalize(-light.direction);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess) * u_specularStrength;

    vec3 ambient = light.ambient * baseColor;
    vec3 diffuse = light.diffuse * diffuseFactor * baseColor;
    vec3 specular = light.specular * specularFactor;
    return ambient + diffuse + specular;
}

vec3 applyPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir, vec3 baseColor)
{
    if (light.enabled == 0)
    {
        return vec3(0.0);
    }

    vec3 toLight = light.position - fragPosition;
    float distanceToLight = length(toLight);
    vec3 lightDir = normalize(toLight);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess) * u_specularStrength;
    float attenuation = 1.0 / (light.constant +
                               light.linear * distanceToLight +
                               light.quadratic * distanceToLight * distanceToLight);

    vec3 ambient = light.ambient * baseColor;
    vec3 diffuse = light.diffuse * diffuseFactor * baseColor;
    vec3 specular = light.specular * specularFactor;
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 normal = normalize(v_worldNormal);
    vec3 viewDir = normalize(u_viewPos - v_worldPosition);
    vec3 baseColor = u_albedo;

    vec3 lighting = applySunLight(u_sunLight, normal, viewDir, baseColor);
    for (int i = 0; i < 3; ++i)
    {
        lighting += applyPointLight(u_pointLights[i], normal, v_worldPosition, viewDir, baseColor);
    }

    fragColor = vec4(lighting, 1.0);
}
