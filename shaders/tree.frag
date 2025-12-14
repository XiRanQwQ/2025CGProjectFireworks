#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D trunkTex;
uniform bool useTexture = true;
uniform vec3 leafColor = vec3(0.2, 0.8, 0.2);

void main()
{
    if (useTexture) {
        FragColor = texture(trunkTex, TexCoords);
    } else {
        FragColor = vec4(leafColor, 1.0);
    }
}