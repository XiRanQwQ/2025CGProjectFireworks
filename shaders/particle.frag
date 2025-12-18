#version 430 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D groundTex;
layout(binding = 1, std140) uniform PointLights {
    vec3  position;
    float constant;
    vec3  color;
    float linear;
    float quadratic;
    float isOn;          // ¡û ²»½Ð active£¬½Ð isOn / enabled / used
} lights[20];

uniform vec3 viewPos;

void main(){
    vec4 texColor = texture(groundTex, TexCoord);
    vec3 albedo   = texColor.rgb;
    vec3 ambient  = 0.25 * albedo;
    vec3 lighting = ambient;
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    for (int i = 0; i < 20; ++i) {
        if (lights[i].isOn == 0.0) continue;   // ÓÃ isOn ÅÐ¶Ï
        vec3  L = normalize(lights[i].position - FragPos);
        vec3  H = normalize(V + L);
        float dist = length(lights[i].position - FragPos);
        float att = 1.0 / (lights[i].constant + lights[i].linear * dist + lights[i].quadratic * dist * dist);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 32.0);
        lighting += lights[i].color * (diff + spec) * att * 4.0f;
    }

    FragColor = vec4(albedo * lighting, texColor.a);
}