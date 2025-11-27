#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;


uniform sampler2D mainTexture;     // 메인 텍스처
uniform vec3 objectColor;          // 단색 틴트 (텍스처 없을 때 사용)
uniform int hasTexture;
// 조명 관련
uniform vec3 lightColor;           // 조명 색상 (보통 흰색)
uniform vec3 lightPos;             // 점광원 위치
uniform vec3 viewPos;              // 카메라 위치
uniform float lightIntensity;      // 조명 세기

void main()
{
    // 1. 축 렌더링 모드 (Unlit Mode)
    // C++ 코드의 drawAxes()에서 lightIntensity를 0.0으로 설정하면 이 블록 실행
    if (lightIntensity == 0.0) {
        // objectColor에 설정된 순수한 색상(R, G, B)을 그대로 출력합니다.
        FragColor = vec4(objectColor, 1.0); 
        return;
    }
    
    // 2. 객체 렌더링 모드 (Lit Mode + Texture)
    
    // 물체의 기본 색상 (텍스처 유무에 따라 결정)
    vec3 baseColor = hasTexture == 1 ? texture(mainTexture, TexCoord).rgb : objectColor;

    // 조명 계산에 사용할 정규화된 법선 벡터
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // 앰비언트 (Ambient) - 그림자 지는 부분의 최소 밝기
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 디퓨즈 (Diffuse) - 광원에서 표면으로의 직접적인 빛
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 스펙큘러 (Specular) - 하이라이트/반사광
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // 32.0은 shininess 값
    vec3 specular = specularStrength * spec * lightColor;

    // 최종 색상 계산 및 밝기 적용
    vec3 result = (ambient + diffuse + specular) * baseColor * lightIntensity;
    FragColor = vec4(result, 1.0);
}