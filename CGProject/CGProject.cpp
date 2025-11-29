#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "stb_image.h"
using namespace std;
using namespace glm;

// -------------------- 구조체 (Structs) --------------------

// 면 하나의 렌더링 정보 (FaceRenderObject) - 단일 메쉬를 위해 하나만 사용됨
struct FaceRenderObject {
    GLuint Vao = 0;
    GLuint Ebo = 0;
    size_t IndexCount = 0;
    GLuint TextureId = 0;
};

// 다중 텍스처 오브젝트 정보 (MultiTextureObject)
struct MultiTextureObject {
    GLuint SharedVbo = 0;
    std::vector<FaceRenderObject> Faces;
    glm::mat4 ModelMatrix = glm::mat4(1.0f);
};


// -------------------- 전역 변수 (Global Variables) --------------------
GLuint ShaderProgram = 0;

// Uniform 위치 캐시 (Loc: Location)
GLint LocModel = -1, LocView = -1, LocProjection = -1;
GLint LocObjectColor = -1, LocHasTexture = -1, LocLightPos = -1, LocLightColor = -1, LocLightIntensity = -1, LocViewPos = -1;
GLint LocMainTexture = -1;

// 객체 렌더링 정보
// -- 땅 -- 
MultiTextureObject GroundObject;

// -- 강 --
MultiTextureObject RiverObject;

// -- 구름 -- 
MultiTextureObject CloudObject;

// -- 나무 -- 
MultiTextureObject TreeTrunkObject;
MultiTextureObject TreeLeavesObject;

// -- 잔디 --
MultiTextureObject GrassObject;

// -- 두더지 굴 --
MultiTextureObject HoleObject;

// -- 돌 --
MultiTextureObject StoneObject;

// -- 울타리 -- 
MultiTextureObject FenceObject;

// -- 망치 -- (악마 컨셉)
MultiTextureObject DemonHammerObject;
MultiTextureObject DemonHammer2Object;
MultiTextureObject DemonHammer3Object;


// 텍스처 ID들을 저장할 벡터
// -- 땅 --
std::vector<GLuint> GroundTextureIds;

// -- 강 --
std::vector<GLuint> RiverTextureIds;

// -- 구름 --
std::vector<GLuint> CloudTextureIds;

// -- 나무 --
std::vector<GLuint> TreeTrunkTextureIds;
std::vector<GLuint> TreeLeavesTextureIds;

// -- 잔디 --
std::vector<GLuint> GrassTextureIds;

// -- 두더지 굴 --
std::vector<GLuint> HoleTextureIds;

// -- 돌 --
std::vector<GLuint> StoneTextureIds;

// -- 울타리 -- 
std::vector<GLuint> FenceTextureIds;

// -- 망치 --
// -- 악마 --
std::vector<GLuint> DemonHammerTextureIds;
std::vector<GLuint> DemonHammer2TextureIds;
std::vector<GLuint> DemonHammer3TextureIds;

// 축 VAO
GLuint AxesVao = 0;
// ----------------------------------------------------


// ----------------- 변환 관련 변수 (Transformation Variables) -------------------
bool RotateObject = true;
float RotationAngle = 0.0f;
float LightIntensity = 1.0f;
glm::vec3 LightPos = glm::vec3(0.0f, 50.0f, 50.0f);

float CameraPosZ = 60.0f;

// 회전 축
float Rx = 0, Ry = 1;
// -----------------------------------------------------
//Arcball 기능 관련 : 마우스 이벤트, 드래그 이벤트
#define ARCBALL_SENS 0.2f
GLboolean bArcball = GL_FALSE;
// 마우스로 평행이동 (무시해도됨) 구현 안할꺼
GLboolean bParallel = GL_FALSE;

//아크볼 기능에 의한 MV 회전 및 이동
GLfloat cameraRX = 0.0f; //무시하삼
GLfloat cameraRY = 0.0f; //무시하삼
GLfloat cameraTX = 0.0f;
GLfloat cameraTY = 0.0f;

//드래그 변수(변화 전 커서 위치, 현재 커서 위치) : 모션 벡터용
GLint preCursorX = 0;
GLint preCursorY = 0;
GLint nowCursorX = 0;
GLint nowCursorY = 0;

// 모델 움직임
// 마우스 이동 움직임의 따른 객체 이동량
GLfloat modelMoveTX = 0.0f;
GLfloat modelMoveTY = 0.0f;

// 해머 움직임 상태
bool hammerDown = false;
bool hammerUp = false;
// 마우스 입력시 해머 회전 적용 변수
GLfloat modelhammerRZ = 0.0f;














































































// -------------------- 텍스처 로더 (Texture Loader) --------------------
GLuint LoadTexture(const char* path) {
    int width, height, channels;
    // 강제로 RGBA 포맷 (4채널) 로드
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "텍스처 로드 실패: " << path << std::endl;
        // 텍스처가 없는 경우 텍스처 없이 렌더링되도록 0 반환
        return 0;
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    std::cout << "텍스처 로드 성공: " << path << std::endl;
    return textureId;
}




// -------------------- 파일 리더 (File Reader) --------------------
string ReadFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "파일 로드 실패: " << path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
}

// -------------------- 셰이더 컴파일러 (Shader Compiler) --------------------
GLuint CompileShader(GLenum type, const string& src) {
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


// ------------------- OBJ 로더  -------------------
bool LoadObj(const char* path,
    std::vector<GLfloat>& vertices,
    std::vector<unsigned int>& indices)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;
    std::map<std::string, unsigned int> vertexMap;

    FILE* file;
    if (fopen_s(&file, path, "r") != 0 || !file) {
        std::cerr << "OBJ 파일 열기 실패: " << path << "\n";
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            glm::vec3 v;
            sscanf_s(line, "v %f %f %f", &v.x, &v.y, &v.z);
            positions.push_back(v);
        }
        else if (line[0] == 'v' && line[1] == 't') {
            glm::vec2 uv;
            sscanf_s(line, "vt %f %f", &uv.x, &uv.y);
            uvs.push_back(uv);
        }
        else if (line[0] == 'v' && line[1] == 'n') {
            glm::vec3 n;
            sscanf_s(line, "vn %f %f %f", &n.x, &n.y, &n.z);
            normals.push_back(n);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            // 최대 4개의 정점 세트를 담을 변수 준비 (v1~v4)
            unsigned int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3, v4, vt4, vn4;
            int matches;

            // 헬퍼 람다 함수: 인덱스 조합이 같으면 재사용, 다르면 새 정점 생성
            auto ProcessVertex = [&](unsigned int vIdx, unsigned int vtIdx, unsigned int vnIdx) -> unsigned int {
                char key[64];
                sprintf_s(key, "%u/%u/%u", vIdx, vtIdx, vnIdx);
                std::string keyStr = key;

                auto it = vertexMap.find(keyStr);
                if (it == vertexMap.end()) {
                    unsigned int newIndex = (unsigned int)tempVertices.size();
                    vertexMap[keyStr] = newIndex;

                    // 인덱스는 1부터 시작하므로 -1
                    tempVertices.push_back(positions[vIdx - 1]);
                    // UV/Normal 인덱스가 유효하지 않으면 기본값 사용
                    tempUvs.push_back((vtIdx > 0 && vtIdx <= uvs.size()) ? uvs[vtIdx - 1] : glm::vec2(0.0f));
                    tempNormals.push_back((vnIdx > 0 && vnIdx <= normals.size()) ? normals[vnIdx - 1] : glm::vec3(0, 1, 0));

                    return newIndex;
                }
                return it->second;
                };

            // -------------------- 1. Quad (4 vertices: v/vt/vn) 처리 시도 --------------------
            matches = sscanf_s(line, "f %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
                &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3, &v4, &vt4, &vn4);

            if (matches == 12) { // **성공: 4개의 정점 세트 (Quad)**
                // Quad(V1, V2, V3, V4)를 Triangle 2개로 삼각화: (T1: V1, V3, V2), (T2: V1, V4, V3)

                // Triangle 1 (V1, V3, V2) - 와인딩 순서 반전
                indices.push_back(ProcessVertex(v1, vt1, vn1));
                indices.push_back(ProcessVertex(v3, vt3, vn3));
                indices.push_back(ProcessVertex(v2, vt2, vn2));

                // Triangle 2 (V1, V4, V3) - 와인딩 순서 반전
                indices.push_back(ProcessVertex(v1, vt1, vn1)); // V1 재사용
                indices.push_back(ProcessVertex(v4, vt4, vn4));
                indices.push_back(ProcessVertex(v3, vt3, vn3)); // V3 재사용

                continue;
            }

            // -------------------- 2. Triangle (3 vertices: v/vt/vn) 처리 시도 --------------------
            if (matches == 9) { // **성공: 3개의 정점 세트 (Triangle)**
                // T1: V1, V3, V2 - 와인딩 순서 반전
                indices.push_back(ProcessVertex(v1, vt1, vn1));
                indices.push_back(ProcessVertex(v3, vt3, vn3));
                indices.push_back(ProcessVertex(v2, vt2, vn2));

                continue;
            }

            // -------------------- 3. UV 또는 법선이 없는 형식 (3 vertices만 처리) --------------------

            // 변수를 다시 읽기 위해 0으로 초기화
            v1 = vt1 = vn1 = v2 = vt2 = vn2 = v3 = vt3 = vn3 = 0;

            // f v//vn 형식 지원 (UV 없는 경우)
            matches = sscanf_s(line, "f %u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
            if (matches == 6) {
                vt1 = vt2 = vt3 = 0;
                // 순서 반전: V1, V3, V2
                indices.push_back(ProcessVertex(v1, vt1, vn1));
                indices.push_back(ProcessVertex(v3, vt3, vn3));
                indices.push_back(ProcessVertex(v2, vt2, vn2));
                continue;
            }

            // f v/vt 형식 지원 (법선 없는 경우)
            v1 = vt1 = vn1 = v2 = vt2 = vn2 = v3 = vt3 = vn3 = 0; // 변수 재초기화
            matches = sscanf_s(line, "f %u/%u %u/%u %u/%u", &v1, &vt1, &v2, &vt2, &v3, &vt3);
            if (matches == 6) {
                vn1 = vn2 = vn3 = 0;
                // 순서 반전: V1, V3, V2
                indices.push_back(ProcessVertex(v1, vt1, vn1));
                indices.push_back(ProcessVertex(v3, vt3, vn3));
                indices.push_back(ProcessVertex(v2, vt2, vn2));
                continue;
            }

            // 어떤 형식에도 일치하지 않으면 건너뜁니다.
        }
    }
    fclose(file); // 파일 닫기

    // 인터리빙: pos(3) + uv(2) + normal(3) = 8 floats 
    for (size_t i = 0; i < tempVertices.size(); ++i) {
        vertices.push_back(tempVertices[i].x);
        vertices.push_back(tempVertices[i].y);
        vertices.push_back(tempVertices[i].z);

        vertices.push_back(tempUvs[i].x);
        vertices.push_back(tempUvs[i].y);

        vertices.push_back(tempNormals[i].x);
        vertices.push_back(tempNormals[i].y);
        vertices.push_back(tempNormals[i].z);
    }

    return !vertices.empty();
}

// -------------------- 축 렌더링 초기화 함수 (Axes Init) --------------------
void CreateAxes() {
    // X, Y, Z 축의 시작점(0,0,0)과 끝점(10, 0, 0 등)을 정의합니다. (위치 3개만 사용)
    GLfloat axesVertices[] = {
        // X-axis (Red)
         -10.0f,  0.0f,  0.0f,
          10.0f,  0.0f,  0.0f,
          // Y-axis (Green)
           0.0f,  -10.0f,  0.0f,
           0.0f,  10.0f,  0.0f,
           // Z-axis (Blue)
            0.0f,  0.0f,  -10.0f,
            0.0f,  0.0f, 10.0f
    };

    GLuint axesVbo;
    glGenVertexArrays(1, &AxesVao);
    glGenBuffers(1, &axesVbo);

    glBindVertexArray(AxesVao);

    glBindBuffer(GL_ARRAY_BUFFER, axesVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);

    // Position attribute (location 0)만 사용합니다.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 다른 속성(UV, 법선)은 사용하지 않으므로 비활성화합니다.
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// -------------------- 축 렌더링 함수 (Draw Axes) --------------------
void DrawAxes() {
    if (AxesVao == 0) return;

    // 1. Model Matrix: 축은 고정되어 있으므로 Identity 행렬을 사용합니다.
    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(LocModel, 1, GL_FALSE, glm::value_ptr(identityModel));

    // 2. 텍스처와 조명을 임시로 비활성화하여 순수한 색상으로 출력합니다.
    glUniform1i(LocHasTexture, 0);
    glUniform1f(LocLightIntensity, 0.0f);

    // 3. 선의 두께 설정
    GLfloat originalLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &originalLineWidth); // 기존 두께 저장
    glLineWidth(3.0f);

    glBindVertexArray(AxesVao);

    // X-axis (Red)
    glUniform3f(LocObjectColor, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);

    // Y-axis (Green)
    glUniform3f(LocObjectColor, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);

    // Z-axis (Blue)
    glUniform3f(LocObjectColor, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    glBindVertexArray(0);

    // 4. 원래 상태로 복원
    glUniform1f(LocLightIntensity, LightIntensity); // 원래 조명 세기로 복원
    glLineWidth(originalLineWidth); // 선 두께 복원
    glUniform3f(LocObjectColor, 1.0f, 1.0f, 1.0f); // 오브젝트 색상 기본값 복원
}

// ------------------- 다중 면 오브젝트 생성 (Create Object) -------------------
void CreateMultiFaceObject(MultiTextureObject& mobj, const string& objPath, const glm::vec3& scale,
    const std::vector<GLuint>& textureIds)
{
    std::vector<float> verts;
    std::vector<unsigned int> indices;

    if (!LoadObj(objPath.c_str(), verts, indices) || verts.empty()) {
        cerr << "OBJ 로드 실패: " << objPath << " 또는 데이터 없음." << endl;
        return;
    }

    mobj.ModelMatrix = glm::scale(glm::mat4(1.0f), scale);
    mobj.Faces.clear();

    // 1. Shared VBO (정점 버퍼) 생성 및 데이터 로드 (모든 면이 공유)
    glGenBuffers(1, &mobj.SharedVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mobj.SharedVbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    // 2. 모든 인덱스를 사용하는 단일 FaceRenderObject 생성 
    FaceRenderObject faceObj;
    faceObj.IndexCount = indices.size();
    // 텍스처가 있다면 첫 번째 텍스처 ID를 사용
    faceObj.TextureId = textureIds.empty() ? 0 : textureIds[0];

    // 3. VAO 및 EBO 생성
    glGenVertexArrays(1, &faceObj.Vao);
    glBindVertexArray(faceObj.Vao);

    glGenBuffers(1, &faceObj.Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceObj.Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceObj.IndexCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // VBO는 공유 (mobj.SharedVbo) -> VAO에 연결
    glBindBuffer(GL_ARRAY_BUFFER, mobj.SharedVbo);

    // 정점 속성 설정 (위치 0, UV 1, 법선 2)
    // 스트라이드: pos(3) + uv(2) + normal(3) = 8 floats
    const size_t stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // VAO 해제

    mobj.Faces.push_back(faceObj);
    cout << "OBJ (" << objPath << ") 로드 완료. 총 버텍스: " << verts.size() / 8 << ", 총 인덱스: " << indices.size() << endl;
}

















































































// -------------------- 렌더링 (Draw Scene) --------------------
void DrawScene() {
    if (ShaderProgram == 0) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(ShaderProgram);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

    // 카메라/뷰 설정 
    glm::vec3 CameraPos = glm::vec3(0.0f, 15.0f, CameraPosZ);

    // View 행렬 조작 없이 일반 View 행렬 그대로 사용
    glm::mat4 View = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)800 / 800, 0.1f, 1000.0f);

    glUniformMatrix4fv(LocView, 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(LocProjection, 1, GL_FALSE, glm::value_ptr(Projection));

    // 조명/카메라 위치 설정
    glUniform3fv(LocLightPos, 1, glm::value_ptr(LightPos));
    glUniform3f(LocLightColor, 1.0f, 1.0f, 1.0f);
    glUniform3fv(LocViewPos, 1, glm::value_ptr(CameraPos));

    // 기본 objectColor 설정
    glUniform3f(LocObjectColor, 1.0f, 1.0f, 1.0f);
    glUniform1f(LocLightIntensity, LightIntensity); // 일반 조명 세기 설정

    // 축 드로우
    DrawAxes();
    glUniform1f(LocLightIntensity, LightIntensity);



    // -------------------- 통합 렌더링 람다 함수 (Draw Logic) --------------------
    auto Draw = [&](const MultiTextureObject& mobj, const glm::mat4& modelMatrix) {
        // 오브젝트가 로드되지 않았으면 그리지 않음
        if (mobj.Faces.empty() || mobj.Faces[0].Vao == 0) return;

        glUniformMatrix4fv(LocModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // 단일 메쉬로 처리하므로 첫 번째(유일한) 면만 렌더링합니다.
        const auto& faceObj = mobj.Faces[0];

        glUniform1i(LocHasTexture, faceObj.TextureId != 0 ? 1 : 0);

        if (faceObj.TextureId != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, faceObj.TextureId);
            glUniform1i(LocMainTexture, 0);
        }
        glBindVertexArray(faceObj.Vao);
        glDrawElements(GL_TRIANGLES, faceObj.IndexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        };
    // -----------------------------------------------------------------------------------


    // ---------------------------- 배경 렌더링 ----------------------------
	// 배경 오브젝트 렌더링 (Static Object)
    // 땅 렌더링 
    glUniform1f(LocLightIntensity, 0.7f); // 조명 세기 조절
    glm::mat4 GroundModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    GroundModel = GroundModel * GroundObject.ModelMatrix;
    Draw(GroundObject, GroundModel);

    // 강 렌더링
    glUniform1f(LocLightIntensity, 1.0f); // 조명 세기 조절
    glm::mat4 RiverModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    RiverModel = RiverModel * RiverObject.ModelMatrix;
    Draw(RiverObject, RiverModel);
    
    // 구름 렌더링
    glUniform1f(LocLightIntensity, 0.0f);
    glm::mat4 CloudModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    CloudModel = CloudModel * CloudObject.ModelMatrix;
    Draw(CloudObject, CloudModel);

    // 나무 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 TreeTrunkModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    TreeTrunkModel = TreeTrunkModel * TreeTrunkObject.ModelMatrix;
    Draw(TreeTrunkObject, TreeTrunkModel);

    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 TreeLeavesModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    TreeLeavesModel = TreeLeavesModel * TreeLeavesObject.ModelMatrix;
    Draw(TreeLeavesObject, TreeLeavesModel);

    // 잔디 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 GrassModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    GrassModel = GrassModel * GrassObject.ModelMatrix;
    Draw(GrassObject, GrassModel);

	// 두더지 굴 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 HoleModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    HoleModel = HoleModel * HoleObject.ModelMatrix;
    Draw(HoleObject, HoleModel);


	// 돌 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 StoneModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    StoneModel = StoneModel * StoneObject.ModelMatrix;
    Draw(StoneObject, StoneModel);

    // 울타리 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 FenceModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    FenceModel = FenceModel * FenceObject.ModelMatrix;
    Draw(FenceObject, FenceModel);

    glUniform1f(LocLightIntensity, 1.0f);
    // 망치 렌더링 (Rotatable Object)
   // --------------------------------------------------------------------------------------------------------------
    const MultiTextureObject& activeObject = DemonHammerObject;
    const MultiTextureObject& activeObject2 = DemonHammer2Object;
    const MultiTextureObject& activeObject3 = DemonHammer3Object;

    /* glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(RotationAngle), glm::vec3(0.0f, 1.0f, 0));*/

    glm::mat4 rotatedModelMatrix = activeObject.ModelMatrix;
    glm::mat4 rotatedModelMatrix2 = activeObject2.ModelMatrix;
    glm::mat4 rotatedModelMatrix3 = activeObject3.ModelMatrix;

    // 회전 행렬 적용
    rotatedModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(modelhammerRZ), glm::vec3(0, 0, 1)) * rotatedModelMatrix;
    rotatedModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(modelMoveTY, 0.0f, -modelMoveTX)) * rotatedModelMatrix;
    rotatedModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * rotatedModelMatrix;

    rotatedModelMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(modelhammerRZ), glm::vec3(0, 0, 1)) * rotatedModelMatrix2;
    rotatedModelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(modelMoveTY, 0.0f, -modelMoveTX)) * rotatedModelMatrix2;
    rotatedModelMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * rotatedModelMatrix2;

    rotatedModelMatrix3 = glm::rotate(glm::mat4(1.0f), glm::radians(modelhammerRZ), glm::vec3(0, 0, 1)) * rotatedModelMatrix3;
    rotatedModelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(modelMoveTY, 0.0f, -modelMoveTX)) * rotatedModelMatrix3;
    rotatedModelMatrix3 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * rotatedModelMatrix3;

    glm::mat4 finalModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * rotatedModelMatrix;
    glm::mat4 finalModelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * rotatedModelMatrix2;
    glm::mat4 finalModelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * rotatedModelMatrix3;

    Draw(activeObject, finalModelMatrix);
    Draw(activeObject2, finalModelMatrix2);
    Draw(activeObject3, finalModelMatrix3);
    // --------------------------------------------------------------------------------------------------------------







    glutSwapBuffers();
}

// -------------------- 타이머 & 키보드 (Timer & Keyboard) --------------------
void Timer(int) {
    /* if (RotateObject) RotationAngle += 60.0f * 0.016f;*/
    if (hammerDown) {
        modelhammerRZ += 10.0f;
        if (modelhammerRZ >= 90.0f) {
            modelhammerRZ = 90.0f;
            hammerDown = false;
            hammerUp = true;
        }
    }
    else if (hammerUp) {
        modelhammerRZ -= 10.0f;
        if (modelhammerRZ <= 0.0f) {
            modelhammerRZ = 0.0f;
            hammerUp = false;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}


void Keyboard(unsigned char key, int, int) {
    switch (key) {
    case'w':
        // 디버그 용도 카메라 앞 이동
		CameraPosZ -= 2.0f;
        break;
    case's':
        // 디버그 용도 카메라 뒤 이동
        CameraPosZ += 2.0f;
        break;

    case 'x': Rx = 1; Ry = 0; break; // X축 회전 활성화
    case 'y': Rx = 0; Ry = 1; break; // Y축 회전 활성화
    case 'c': {
        // 상태 초기화
        RotateObject = true;
        RotationAngle = 0.0f;
        LightIntensity = 1.0f;
        Rx = 0, Ry = 1;
        break;
    }
    case 'q':exit(0); break;
    }
    glutPostRedisplay();
}

// -------------------- 마우스 이벤트 (Mouse Events) --------------------
void DoMouse(GLint button, GLint state, GLint x, GLint y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        bArcball = GL_TRUE;
        preCursorX = x;
        preCursorY = y;
        hammerDown = true;
        hammerUp = false;
    }
    else {
        bArcball = GL_FALSE;
    }
}

void DoMotion(GLint x, GLint y) {
    if (bArcball == GL_TRUE) {
        float deltaX = (float)(x - preCursorX);
        float deltaY = (float)(preCursorY - y);

        // 감도 조절
        float sensitivity = 0.1f;

        // 마우스 좌우 이동
        modelMoveTX += deltaX * sensitivity;
        modelMoveTY += deltaY * sensitivity;

        // 현재 마우스 위치 업데이트
        preCursorX = x;
        preCursorY = y;
    }

    glutPostRedisplay();
}
// ----------------------------------------------------------

// -------------------- 메인 함수 (Main) --------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CGProject");

    // GLEW 초기화 (OpenGL 함수 포인터 로드)
    glewInit();

    // 셰이더 생성 및 유효성 검사 
    CreateShaderProgram();
    if (ShaderProgram == 0) {
        cerr << "프로그램 초기화 실패. 종료합니다." << endl;
        return -1;
    }

    // 텍스처 로드 
    // 환경 - (땅)
    GroundTextureIds.push_back(LoadTexture("Ground.jpg"));

    // 환경 - (강)
    RiverTextureIds.push_back(LoadTexture("River.jpg"));

    // 환경 - (구름)
    CloudTextureIds.push_back(LoadTexture("Cloud.jpg"));

    // 환경 - (나무)
    TreeTrunkTextureIds.push_back(LoadTexture("TreeTrunk.jpg"));
    TreeLeavesTextureIds.push_back(LoadTexture("TreeLeaves.jpg"));

    // 환경 - (잔디)
    GrassTextureIds.push_back(LoadTexture("Grass.jpg"));

    // 환경 - (두더지 굴)
	HoleTextureIds.push_back(LoadTexture("Hole.jpg"));

    // 환경 - (돌)
	StoneTextureIds.push_back(LoadTexture("Stone.jpg"));

    // 환경 - (울타리)
	FenceTextureIds.push_back(LoadTexture("Fence.jpg"));

	// 오브젝트 - (망치)
    // -- 악마 --
    DemonHammerTextureIds.push_back(LoadTexture("DemonHammer.jpg"));
    DemonHammer2TextureIds.push_back(LoadTexture("DemonHammer2.jpg"));
    DemonHammer3TextureIds.push_back(LoadTexture("DemonHammer3.jpg"));


    // 축 생성
    CreateAxes();

    // ---- 오브젝트 생성 ----
    // 환경 - (땅)
    CreateMultiFaceObject(GroundObject, "Ground.obj", glm::vec3(1.0f, 1.0f, 1.0f), GroundTextureIds);

    // 환경 - (강)
    CreateMultiFaceObject(RiverObject, "River.obj", glm::vec3(1.0f, 1.0f, 1.0f), RiverTextureIds);

    // 환경 - (구름)
    CreateMultiFaceObject(CloudObject, "Cloud.obj", glm::vec3(1.0f, 1.0f, 1.0f), CloudTextureIds);

    // 환경 - (나무)
	CreateMultiFaceObject(TreeTrunkObject, "TreeTrunk.obj", glm::vec3(1.0f), TreeTrunkTextureIds);
	CreateMultiFaceObject(TreeLeavesObject, "TreeLeaves.obj", glm::vec3(1.0f), TreeLeavesTextureIds);

    // 환경 - (잔디)
    CreateMultiFaceObject(GrassObject, "Grass.obj", glm::vec3(1.0f), GrassTextureIds);
    
    // 환경 - (두더지 굴)
    CreateMultiFaceObject(HoleObject, "Hole.obj", glm::vec3(1.0f), HoleTextureIds);

	// 환경 - (돌)
	CreateMultiFaceObject(StoneObject, "Stone.obj", glm::vec3(1.0f), StoneTextureIds);

    // 환경 - (울타리)
    CreateMultiFaceObject(FenceObject, "Fence.obj", glm::vec3(1.0f), FenceTextureIds);

    // 오브젝트 - (망치) 
    // -- 악마 --
    CreateMultiFaceObject(DemonHammerObject, "DemonHammer.obj", glm::vec3(1.0f), DemonHammerTextureIds);
	CreateMultiFaceObject(DemonHammer2Object, "DemonHammer2.obj", glm::vec3(1.0f), DemonHammer2TextureIds);
    CreateMultiFaceObject(DemonHammer3Object, "DemonHammer3.obj", glm::vec3(1.0f), DemonHammer3TextureIds);

    // 렌더링 설정 및 메인 루프 시작
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glutDisplayFunc(DrawScene);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(DoMouse);
    glutMotionFunc(DoMotion);
    glutTimerFunc(0, Timer, 0);
    glutMainLoop();
    return 0;
}