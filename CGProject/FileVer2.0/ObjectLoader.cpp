#include "ObjectLoader.h"
#include "Globals.h"
#include <iostream>
#include <map>
#include <vector>
#include <cstdio> // for fopen_s/sscanf_s

using namespace std;
using namespace glm;

// ------------------- OBJ 로더 -------------------
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
            uv.y = 1.0f - uv.y;
            uvs.push_back(uv);
        }
        else if (line[0] == 'v' && line[1] == 'n') {
            glm::vec3 n;
            sscanf_s(line, "vn %f %f %f", &n.x, &n.y, &n.z);
            normals.push_back(n);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            unsigned int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3, v4, vt4, vn4;
            int matches;

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
                // Triangle1 (V1, V3, V2) - 와인딩 순서 반전
                indices.push_back(ProcessVertex(v1, vt1, vn1));
                indices.push_back(ProcessVertex(v3, vt3, vn3));
                indices.push_back(ProcessVertex(v2, vt2, vn2));

                // Triangle2 (V1, V4, V3) - 와인딩 순서 반전
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
            v1 = vt1 = vn1 = v2 = vt2 = vn2 = v3 = vt3 = vn3 = 0; // 변수 재초기화
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
				// 순서 반전 : V1, V3, V2
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
    

    // OBB 계산을 위한 정점 좌표계 초기화
    glm::vec3 minVertex(std::numeric_limits<float>::max());
    glm::vec3 maxVertex(std::numeric_limits<float>::lowest());

    // 스트라이드: 위치(3) + UV(2) + 법선(3) = 8 floats
    const int stride_floats = 8;

    // 1. 모든 정점 위치를 순회하며 최소/최대 좌표를 찾습니다.
    // 'verts' 벡터를 stride_floats 단위로 순회합니다.
    for (size_t i = 0; i < verts.size(); i += stride_floats) {
        // i: X 좌표 시작 인덱스
        // i+1: Y 좌표 인덱스
        // i+2: Z 좌표 인덱스

        minVertex.x = std::min(minVertex.x, verts[i]);
        minVertex.y = std::min(minVertex.y, verts[i + 1]);
        minVertex.z = std::min(minVertex.z, verts[i + 2]);

        maxVertex.x = std::max(maxVertex.x, verts[i]);
        maxVertex.y = std::max(maxVertex.y, verts[i + 1]);
        maxVertex.z = std::max(maxVertex.z, verts[i + 2]);
    }

    // 2. 로컬 OBB 생성 (mobj 변수에 저장)
    mobj.obbLocal.Center = (minVertex + maxVertex) * 0.5f;
    mobj.obbLocal.HalfSizes = (maxVertex - minVertex) * 0.5f;
    mobj.obbLocal.Orientation = glm::mat4(1.0f); // 초기 로컬 OBB의 회전은 단위 행렬

    // 3. 월드 OBB도 초기 로컬 OBB 값으로 설정 (main.cpp에서 ModelMatrix에 맞춰 업데이트됨)
    mobj.obbWorld = mobj.obbLocal;

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