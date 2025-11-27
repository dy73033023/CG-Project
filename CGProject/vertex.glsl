#version 330 core

layout (location = 0) in vec3 aPos;      // 위치
layout (location = 1) in vec2 aTexCoord;   // 법선 (없으면 0,0,0)
layout (location = 2) in vec3 aNormal; // 텍스처 좌표 (없으면 0,0)

out vec3 FragPos;     // 월드 공간 위치
out vec3 Normal;      // 월드 공간 법선
out vec2 TexCoord;    // 텍스처 좌표

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 월드 공간으로 변환
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 법선 변환 (정확히 하려면 inverse transpose 사용, 스케일 있으면 필수)
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);

    // 텍스처 좌표 그대로 전달
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}