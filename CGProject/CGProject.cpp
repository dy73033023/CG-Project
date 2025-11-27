#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp> // glm::value_ptr 사용을 위해 추가
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm> // max, min 사용을 위해 추가
#include "stb_image.h"
using namespace std;
using namespace glm;

// -------------------- 구조체 --------------------
// 면 하나의 렌더링 정보 (FaceRenderObject) - 단일 메쉬를 위해 하나만 사용됨
struct FaceRenderObject {
    GLuint VAO = 0;
    GLuint EBO = 0;
    size_t indexCount = 0;
    GLuint textureID = 0;
};

// 다중 텍스처 오브젝트 정보 (MultiTextureObject)
struct MultiTextureObject {
    GLuint sharedVBO = 0; // 모든 면이 공유하는 정점 버퍼
    std::vector<FaceRenderObject> faces;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

// -------------------- 전역 변수 --------------------
GLuint shaderProgram = 0;

// Uniform 위치 캐시
GLint loc_model = -1, loc_view = -1, loc_projection = -1;
GLint loc_objectColor = -1, loc_hasTexture = -1, loc_lightPos = -1, loc_lightColor = -1, loc_lightIntensity = -1, loc_viewPos = -1;
GLint loc_mainTexture = -1;

// 객체 렌더링
MultiTextureObject hammerObject; // ?? hammer.obj 전용 오브젝트

// 텍스처 ID들을 저장할 벡터
std::vector<GLuint> hammerTextureIDs; // ?? hammerObject 전용 텍스처

// 축 VAO
GLuint axesVAO = 0;

// 상태
bool rotateObject = true;
float rotationAngle = 0.0f;
float lightIntensity = 1.0f;
glm::vec3 lightPos = glm::vec3(5.0f, 8.0f, 5.0f);
glm::vec3 viewPos = glm::vec3(0.0f, 3.0f, 10.0f);

// 회전 관련 변수
float rx = 0, ry = 1; // y축으로 초기 회전

// -------------------- 텍스처 로더 --------------------
GLuint loadTexture(const char* path) {
    int width, height, channels;
    // 강제로 RGBA 포맷 (4채널) 로드
    unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "텍스처 로드 실패: " << path << std::endl;
        // 텍스처가 없는 경우 텍스처 없이 렌더링되도록 0 반환
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    std::cout << "텍스처 로드 성공: " << path << std::endl;
    return textureID;
}

// -------------------- 파일 리더 --------------------
string readFile(const string& path) {
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "파일 로드 실패: " << path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
}

// -------------------- 셰이더 컴파일러 --------------------
GLuint compileShader(GLenum type, const string& src) {
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


// -------------------- 셰이더 프로그램 생성 (오류 발생 시 0 반환) --------------------
void createShaderProgram() {
    // 1. 셰이더 소스 읽기 (실제 파일명으로 변경하세요)
    string vs = readFile("vertex.glsl");
    string fs = readFile("fragment.glsl");

    if (vs.empty() || fs.empty()) {
        cerr << "셰이더 파일 로드 실패. Uniform 캐싱 건너뜀." << endl;
        shaderProgram = 0;
        return;
    }

    // 2. 셰이더 컴파일
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vs);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fs);

    if (vertexShader == 0 || fragmentShader == 0) {
        shaderProgram = 0;
        return;
    }

    // 3. 셰이더 프로그램 링크
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 4. 링크 오류 검사
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
        shaderProgram = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 5. Uniform 위치 캐싱 (Uniform은 프로그램이 유효할 때만 찾습니다)
    loc_model = glGetUniformLocation(shaderProgram, "model");
    loc_view = glGetUniformLocation(shaderProgram, "view");
    loc_projection = glGetUniformLocation(shaderProgram, "projection");
    loc_mainTexture = glGetUniformLocation(shaderProgram, "mainTexture");
    loc_objectColor = glGetUniformLocation(shaderProgram, "objectColor");
    loc_hasTexture = glGetUniformLocation(shaderProgram, "hasTexture");
    loc_lightPos = glGetUniformLocation(shaderProgram, "lightPos");
    loc_lightColor = glGetUniformLocation(shaderProgram, "lightColor");
    loc_lightIntensity = glGetUniformLocation(shaderProgram, "lightIntensity");
    loc_viewPos = glGetUniformLocation(shaderProgram, "viewPos");

    glUseProgram(0);
}


// ------------------- 완벽한 OBJ 로더 (위치 + UV + 법선 지원) -------------------
bool loadOBJ(const char* path,
    std::vector<GLfloat>& vertices,
    std::vector<unsigned int>& indices)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
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
            // v4, vt4, vn4는 4개 정점일 때만 사용되며, 초기화하지 않아도 sscanf_s가 읽은 만큼만 matches를 반환합니다.
            unsigned int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3, v4, vt4, vn4;
            int matches;

            // 헬퍼 람다 함수: 인덱스 조합이 같으면 재사용, 다르면 새 정점 생성
            auto processVertex = [&](unsigned int v_idx, unsigned int vt_idx, unsigned int vn_idx) -> unsigned int {
                char key[64];
                sprintf_s(key, "%u/%u/%u", v_idx, vt_idx, vn_idx);
                std::string keyStr = key;

                auto it = vertexMap.find(keyStr);
                if (it == vertexMap.end()) {
                    unsigned int newIndex = (unsigned int)temp_vertices.size();
                    vertexMap[keyStr] = newIndex;

                    // 인덱스는 1부터 시작하므로 -1
                    temp_vertices.push_back(positions[v_idx - 1]);
                    // UV/Normal 인덱스가 유효하지 않으면 기본값 (0.0f 또는 (0,1,0)) 사용
                    temp_uvs.push_back((vt_idx > 0 && vt_idx <= uvs.size()) ? uvs[vt_idx - 1] : glm::vec2(0.0f));
                    temp_normals.push_back((vn_idx > 0 && vn_idx <= normals.size()) ? normals[vn_idx - 1] : glm::vec3(0, 1, 0));

                    return newIndex;
                }
                return it->second;
                };

            // -------------------- 1. Quad (4 vertices: v/vt/vn) 처리 시도 --------------------
            // 4개의 정점 세트 (총 12개 인덱스)를 읽습니다.
            matches = sscanf_s(line, "f %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
                &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3, &v4, &vt4, &vn4);

            if (matches == 12) { // **성공: 4개의 정점 세트 (Quad)**
                // Quad(V1, V2, V3, V4)를 Triangle 2개로 삼각화: (T1: V1, V2, V3), (T2: V1, V3, V4)

                // Triangle 1 (V1, V2, V3)
                indices.push_back(processVertex(v1, vt1, vn1));
                indices.push_back(processVertex(v2, vt2, vn2));
                indices.push_back(processVertex(v3, vt3, vn3));

                // Triangle 2 (V1, V3, V4)
                indices.push_back(processVertex(v1, vt1, vn1)); // V1 재사용
                indices.push_back(processVertex(v3, vt3, vn3)); // V3 재사용
                indices.push_back(processVertex(v4, vt4, vn4)); // V4 (새 정점)

                continue; // 4-vertex 처리가 완료되었으므로 다음 라인으로
            }

            // -------------------- 2. Triangle (3 vertices: v/vt/vn) 처리 시도 --------------------
            // matches가 9라면 3개의 정점 세트만 읽은 경우입니다.
            if (matches == 9) { // **성공: 3개의 정점 세트 (Triangle)**
                // T1: V1, V2, V3
                indices.push_back(processVertex(v1, vt1, vn1));
                indices.push_back(processVertex(v2, vt2, vn2));
                indices.push_back(processVertex(v3, vt3, vn3));
                continue;
            }

            // -------------------- 3. UV 또는 법선이 없는 형식 (3 vertices만 처리) --------------------

            // 변수를 다시 읽기 위해 0으로 초기화
            v1 = vt1 = vn1 = v2 = vt2 = vn2 = v3 = vt3 = vn3 = 0;

            // f v//vn 형식 지원 (UV 없는 경우)
            matches = sscanf_s(line, "f %u//%u %u//%u %u//%u", &v1, &vn1, &v2, &vn2, &v3, &vn3);
            if (matches == 6) {
                vt1 = vt2 = vt3 = 0; // 이 포맷은 vt를 읽지 않았으므로 0으로 설정
                indices.push_back(processVertex(v1, vt1, vn1));
                indices.push_back(processVertex(v2, vt2, vn2));
                indices.push_back(processVertex(v3, vt3, vn3));
                continue;
            }

            // f v/vt 형식 지원 (법선 없는 경우)
            v1 = vt1 = vn1 = v2 = vt2 = vn2 = v3 = vt3 = vn3 = 0; // 변수 재초기화
            matches = sscanf_s(line, "f %u/%u %u/%u %u/%u", &v1, &vt1, &v2, &vt2, &v3, &vt3);
            if (matches == 6) {
                vn1 = vn2 = vn3 = 0; // 이 포맷은 vn을 읽지 않았으므로 0으로 설정
                indices.push_back(processVertex(v1, vt1, vn1));
                indices.push_back(processVertex(v2, vt2, vn2));
                indices.push_back(processVertex(v3, vt3, vn3));
                continue;
            }

            // 어떤 형식에도 일치하지 않으면 건너뜁니다.
        }
    }
    fclose(file); // 파일 닫기

    // 인터리빙: pos(3) + uv(2) + normal(3) = 8 floats (기존 코드와 동일)
    for (size_t i = 0; i < temp_vertices.size(); ++i) {
        vertices.push_back(temp_vertices[i].x);
        vertices.push_back(temp_vertices[i].y);
        vertices.push_back(temp_vertices[i].z);

        vertices.push_back(temp_uvs[i].x);
        vertices.push_back(temp_uvs[i].y);

        vertices.push_back(temp_normals[i].x);
        vertices.push_back(temp_normals[i].y);
        vertices.push_back(temp_normals[i].z);
    }

    return !vertices.empty();
}

// -------------------- 축 렌더링 초기화 함수 (새로 추가) --------------------
void createAxes() {
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

    GLuint axesVBO;
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);

    glBindVertexArray(axesVAO);

    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
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

// -------------------- 축 렌더링 함수 (새로 추가) --------------------
void drawAxes() {
    if (axesVAO == 0) return;

    // 1. Model Matrix: 축은 고정되어 있으므로 Identity 행렬을 사용합니다.
    glm::mat4 identityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(identityModel));

    // 2. 텍스처와 조명을 임시로 비활성화하여 순수한 색상으로 출력합니다.
    glUniform1i(loc_hasTexture, 0);
    glUniform1f(loc_lightIntensity, 0.0f);

    // 3. 선의 두께 설정
    GLfloat originalLineWidth;
    glGetFloatv(GL_LINE_WIDTH, &originalLineWidth); // 기존 두께 저장
    glLineWidth(3.0f);

    glBindVertexArray(axesVAO);

    // X-axis (Red)
    glUniform3f(loc_objectColor, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);

    // Y-axis (Green)
    glUniform3f(loc_objectColor, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);

    // Z-axis (Blue)
    glUniform3f(loc_objectColor, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    glBindVertexArray(0);

    // 4. 원래 상태로 복원
    glUniform1f(loc_lightIntensity, lightIntensity); // 원래 조명 세기로 복원
    glLineWidth(originalLineWidth); // 선 두께 복원
    glUniform3f(loc_objectColor, 1.0f, 1.0f, 1.0f); // 오브젝트 색상 기본값 복원
}


// ------------------- 다중 면 오브젝트 생성 (Generic OBJ용으로 수정됨) -------------------
void createMultiFaceObject(MultiTextureObject& mobj, const string& objPath, const glm::vec3& scale,
    const std::vector<GLuint>& textureIDs)
{
    std::vector<float> verts;
    std::vector<unsigned int> indices;

    if (!loadOBJ(objPath.c_str(), verts, indices) || verts.empty()) {
        cerr << "OBJ 로드 실패: " << objPath << " 또는 데이터 없음." << endl;
        return;
    }

    mobj.modelMatrix = glm::scale(glm::mat4(1.0f), scale);
    mobj.faces.clear();

    // 1. Shared VBO (정점 버퍼) 생성 및 데이터 로드 (모든 면이 공유)
    glGenBuffers(1, &mobj.sharedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mobj.sharedVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    // 2. 모든 인덱스를 사용하는 단일 FaceRenderObject 생성 (복잡한 OBJ용)
    FaceRenderObject faceObj;
    faceObj.indexCount = indices.size();
    // OBJ는 보통 단일 텍스처를 사용하며, 텍스처가 있다면 첫 번째 텍스처 ID를 사용
    faceObj.textureID = textureIDs.empty() ? 0 : textureIDs[0];

    // 3. VAO 및 EBO 생성
    glGenVertexArrays(1, &faceObj.VAO);
    glBindVertexArray(faceObj.VAO);

    glGenBuffers(1, &faceObj.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceObj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceObj.indexCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // VBO는 공유 (mobj.sharedVBO) -> VAO에 연결
    glBindBuffer(GL_ARRAY_BUFFER, mobj.sharedVBO);

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

    mobj.faces.push_back(faceObj);
    cout << "OBJ (" << objPath << ") 로드 완료. 총 버텍스: " << verts.size() / 8 << ", 총 인덱스: " << indices.size() << endl;
}

// -------------------- 렌더링 --------------------
void drawScene() {
    if (shaderProgram == 0) return; // 셰이더 실패 시 드로우 방지

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    // 카메라/뷰 설정 (고정된 카메라 사용)
    glm::vec3 camPos = glm::vec3(5.0f, 5.0f, 5.0f);

    // View 행렬 조작 없이 일반 View 행렬 그대로 사용
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / 800, 0.1f, 1000.0f);

    glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(projection));

    // 조명/카메라 위치 설정
    glUniform3fv(loc_lightPos, 1, glm::value_ptr(lightPos));
    glUniform3f(loc_lightColor, 1.0f, 1.0f, 1.0f);
    glUniform3fv(loc_viewPos, 1, glm::value_ptr(camPos));

    // ? 기본 objectColor 설정
    glUniform3f(loc_objectColor, 1.0f, 1.0f, 1.0f);
    glUniform1f(loc_lightIntensity, lightIntensity); // 일반 조명 세기 설정


    // -------------------- 통합 렌더링 람다 함수 --------------------
    auto draw = [&](const MultiTextureObject& mobj, const glm::mat4& modelMatrix) {
        // 오브젝트가 로드되지 않았으면 그리지 않음
        if (mobj.faces.empty() || mobj.faces[0].VAO == 0) return;

        glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // 단일 메쉬로 처리하므로 첫 번째(유일한) 면만 렌더링합니다.
        const auto& faceObj = mobj.faces[0];

        glUniform1i(loc_hasTexture, faceObj.textureID != 0 ? 1 : 0);

        if (faceObj.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, faceObj.textureID);
            glUniform1i(loc_mainTexture, 0);
        }
        glBindVertexArray(faceObj.VAO);
        glDrawElements(GL_TRIANGLES, faceObj.indexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        };
    // -----------------------------------------------------------------------------------

    // 1. 축 드로우
    drawAxes();
    // drawAxes에서 조명이 0.0이 되었으므로 다시 복원
    glUniform1f(loc_lightIntensity, lightIntensity);


    // 2. 해머 오브젝트 드로우 (Rotatable Object)
    const MultiTextureObject& activeObject = hammerObject;

    // 회전 행렬 적용
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(rx, ry, 0));

    // 해머 모델 행렬 (스케일)과 회전 행렬을 결합
    glm::mat4 rotatedModelMatrix = rotationMatrix * activeObject.modelMatrix;

    // 해머를 약간 아래로 이동시켜 축의 중앙에 오도록 조정
    glm::mat4 finalModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * rotatedModelMatrix;

    draw(activeObject, finalModelMatrix);

    glutSwapBuffers();
}

// -------------------- 타이머 & 키보드 --------------------
void timer(int) {
    if (rotateObject) rotationAngle += 60.0f * 0.016f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        // case 'c' (큐브), 'p' (피라미드) 키는 제거됨.
    case 'x': rx = 1; ry = 0; break; // X축 회전 활성화
    case 'y': rx = 0; ry = 1; break; // Y축 회전 활성화
    case 's': {
        // 상태 초기화
        rotateObject = true;
        rotationAngle = 0.0f;
        lightIntensity = 1.0f;
        rx = 0, ry = 1;
        break;
    }
    case 'q':exit(0); break;
    }
    glutPostRedisplay();
}


// -------------------- 메인 (GLUT 초기화 포함) --------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG Single OBJ Renderer (Hammer)");

    // 2. GLEW 초기화 (OpenGL 함수 포인터 로드)
    glewInit();

    // 3. 셰이더 생성 및 유효성 검사 (오류 발생 시 프로그램 종료)
    createShaderProgram();
    if (shaderProgram == 0) {
        cerr << "프로그램 초기화 실패. 종료합니다." << endl;
        return -1;
    }

    // 4. 텍스처 로드 (해머 OBJ는 보통 단일 텍스처를 사용)
    //// "front.jpg" 파일이 해머의 텍스처로 사용됩니다.
    hammerTextureIDs.push_back(loadTexture("right.jpg"));

    // 5.1 축 생성
    createAxes();

    // 5.2 오브젝트 생성
    // hammer.obj 파일 로드 및 hammerObject에 저장
    // 스케일은 1.0f로 유지합니다.
    createMultiFaceObject(hammerObject, "hammer.obj", glm::vec3(1.0f), hammerTextureIDs);

    // 6. 렌더링 설정 및 메인 루프 시작
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}