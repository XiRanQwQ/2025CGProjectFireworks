#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D CloudTexture;   // 新增：2D云纹理（带透明通道）
uniform float CloudTime;               // 云移动时间
uniform vec2 CloudSpeed;          // 云移动速度


void main()
{
    vec3 skyColor = texture(skybox, TexCoords).rgb;
    vec3 dir = normalize(TexCoords);
    
    float theta = atan(dir.z, dir.x); // 方位角（-π~π）
    float phi = acos(clamp(dir.y, -0.99, 0.99)); // 仰角（避开天顶奇点，0~π）

    float zenithFactor = smoothstep(0.9, 1.0, dir.y); // 天顶区域（dir.y>0.9）的过渡因子
    float phiExpanded = phi + zenithFactor * 0.5; // 天顶附近仰角膨胀，UV分散
    vec2 cloudUV = vec2(theta * 0.1591, phiExpanded * 0.3183); // 0.1591=1/(2π), 0.3183=1/π
    
    cloudUV.x += CloudTime * CloudSpeed.x;
    cloudUV = fract(cloudUV);

    vec4 cloudColor = texture(CloudTexture, cloudUV);
    cloudColor.rgb *= 0.8;
    float zenithAlpha = 1.0 - smoothstep(0.85, 1.0, dir.y); 
    cloudColor.a *= clamp(dir.y, 0.0, 1.0) * 0.4 * zenithAlpha;
    //cloudColor.rgb = vec3(1.0);
    //cloudColor.a = 0.8;


    vec3 finalColor = mix(skyColor, cloudColor.rgb, cloudColor.a);

    FragColor = vec4(finalColor, 1.0);
    
   // FragColor = vec4(0.0,0.0,0.0,1.0);
   //FragColor = texture(skybox,TexCoords);
}
