#version 330 core
in vec3 TexCoords;
out vec4 FragColor;

void main()
{
    // 蓝色天空盒

    vec3 color = vec3(0.53f, 0.81f, 0.92f); 
    
    FragColor = vec4(color, 1.0f);
}
