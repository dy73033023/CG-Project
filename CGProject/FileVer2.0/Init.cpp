#define STB_IMAGE_IMPLEMENTATION // stb_image 구현은 여기에만 둡니다.
#include "Init.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h" // 여기서 구현
#include <gl/freeglut.h> // 컴파일러에 필요할 수 있음

using namespace std;

// -------------------- 파일 리더 (File Reader) --------------------
static string ReadFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "파일 로드 실패: " << path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
}

// -------------------- 셰이더 컴파일러 (Shader Compiler) --------------------
static GLuint CompileShader(GLenum type, const string& src) {
    GLuint shader = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(shader, 1, &c, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        cerr << "ERROR::SHADER::COMPILATION_FAILED of type " << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n" << infoLog << endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


// -------------------- 셰이더 프로그램 생성 (Shader Program Creator) --------------------
void CreateShaderProgram() {
    // 1. 셰이더 소스 읽기
    string vs = ReadFile("vertex.glsl");
    string fs = ReadFile("fragment.glsl");

    if (vs.empty() || fs.empty()) {
        cerr << "셰이더 파일 로드 실패. Uniform 캐싱 건너뜀." << endl;
        ShaderProgram = 0;
        return;
    }

    // 2. 셰이더 컴파일
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fs);

    if (vertexShader == 0 || fragmentShader == 0) {
        ShaderProgram = 0;
        return;
    }

    // 3. 셰이더 프로그램 링크
    ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);

    // 4. 링크 오류 검사
    GLint success;
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(ShaderProgram, 1024, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        ShaderProgram = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 5. Uniform 위치 캐싱 (Uniform Location Caching)
    glUseProgram(ShaderProgram); // Uniform 캐싱 전에 프로그램 사용
    LocModel = glGetUniformLocation(ShaderProgram, "model");
    LocView = glGetUniformLocation(ShaderProgram, "view");
    LocProjection = glGetUniformLocation(ShaderProgram, "projection");
    LocMainTexture = glGetUniformLocation(ShaderProgram, "mainTexture");
    LocObjectColor = glGetUniformLocation(ShaderProgram, "objectColor");
    LocHasTexture = glGetUniformLocation(ShaderProgram, "hasTexture");
    LocLightPos = glGetUniformLocation(ShaderProgram, "lightPos");
    LocLightColor = glGetUniformLocation(ShaderProgram, "lightColor");
    LocLightIntensity = glGetUniformLocation(ShaderProgram, "lightIntensity");
    LocViewPos = glGetUniformLocation(ShaderProgram, "viewPos");

    glUseProgram(0);
}


// -------------------- 텍스처 로더 (Texture Loader) --------------------
GLuint LoadTexture(const char* path) {
    int width, height, channels;
    // 강제로 RGBA 포맷 (4채널) 로드
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "텍스처 로드 실패: " << path << std::endl;
        return 0;
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	std::cout << "텍스처 크기: " << width << "x" << height << ", 채널 수: " << channels << std::endl;
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    std::cout << "텍스처 로드 성공: " << path << std::endl;
    return textureId;
}