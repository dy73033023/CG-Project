#pragma once
#include "Globals.h"

// OBJ 로더
bool LoadObj(const char* path,
    std::vector<GLfloat>& vertices,
    std::vector<unsigned int>& indices);

// 다중 면 오브젝트 생성
void CreateMultiFaceObject(MultiTextureObject& mobj, const std::string& objPath, const glm::vec3& scale,
    const std::vector<GLuint>& textureIds);
