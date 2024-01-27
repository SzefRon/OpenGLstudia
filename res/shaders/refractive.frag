#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform vec4 color;

void main()
{             
    vec3 ratio = vec3(1.00 / 1.05, 1.00 / 1.06, 1.00 / 1.07);
    vec3 I = normalize(FragPos - viewPos);
    vec3 Rr = refract(I, normalize(Normal), ratio.x);
    vec3 Rg = refract(I, normalize(Normal), ratio.y);
    vec3 Rb = refract(I, normalize(Normal), ratio.z);
    float FragR = texture(skybox, Rr).r;
    float FragG = texture(skybox, Rg).g;
    float FragB = texture(skybox, Rb).b;
    FragColor = vec4(mix(vec3(FragR, FragG, FragB), color.rgb, color.a), 1.0);
} 