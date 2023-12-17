#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

struct PointLight {    
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;  
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct SpotLight {    
    vec3 position;
    vec3 direction;
    vec3 color;

    float cutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform DirLight dirLight;

#define NR_SPOT_LIGHTS 1
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    // Point Lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir);
    }

    // Directional Light
    result += CalcDirLight(dirLight, Normal, viewDir);

    // Spot Lights
    for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
        result += CalcSpotLight(spotLights[i], Normal, FragPos, viewDir);
    }

    vec4 finalResult = vec4(result, 1.0) * texture(texture_diffuse1, texCoord);
    FragColor = finalResult;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = 0.1 * light.color;
    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // combine results
    vec3 ambient  = 0.1 * light.color;
    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;
    return (ambient + diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient  = 0.1 * light.color;
    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}