#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float threshold;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    // 计算亮度值（使用标准的RGB转亮度公式）
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    // 只保留亮度超过阈值的部分
    if(brightness > threshold) {
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}