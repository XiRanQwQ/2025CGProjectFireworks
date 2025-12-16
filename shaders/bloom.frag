#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float bloomIntensity;

void main()
{
    // 获取原始场景颜色
    vec3 sceneColor = texture(screenTexture, TexCoords).rgb;
    
    // 获取辉光颜色
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;
    
    // 将原始场景与辉光进行混合
    vec3 finalColor = sceneColor + bloomColor * bloomIntensity;
    
    // Gamma校正
    finalColor = pow(finalColor, vec3(0.4545));
    
    FragColor = vec4(finalColor, 1.0);
}