#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 direction;
uniform float blurSize;

// 高斯模糊核（9x9）
const int KERNEL_SIZE = 9;
const float kernel[KERNEL_SIZE] = float[] (
    0.0162162162, 0.0540540541, 0.1216216216,
    0.1945945946, 0.2270270270, 0.1945945946,
    0.1216216216, 0.0540540541, 0.0162162162
);

void main()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);
    vec3 blurColor = vec3(0.0);
    
    // 应用高斯模糊
    for(int i = 0; i < KERNEL_SIZE; i++) {
        blurColor += texture(screenTexture, TexCoords + direction * (i - KERNEL_SIZE / 2) * texelSize * blurSize).rgb * kernel[i];
    }
    
    FragColor = vec4(blurColor, 1.0);
}