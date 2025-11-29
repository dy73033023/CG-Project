#include "OBB_Collision.h"
#include <iostream>

// OBB axis 계산
void ObjectOBBCollision(const OBB& obb1, const glm::vec3& axis,
	float& min, float& max) {
    // 1. OBB의 중심(Center)을 투영 축(axis)에 투영
    // 이는 투영된 간격의 중심점
    float centerProjection = glm::dot(obb1.Center, axis);

    // 2. OBB의 축을 월드 공간에서 가져옴
    // OBB.Orientation 행렬의 각 열(Column)이 월드 공간에서 OBB의 세 축 벡터
    glm::vec3 axes[3];
    axes[0] = glm::normalize(glm::vec3(obb1.Orientation[0])); // X 축
    axes[1] = glm::normalize(glm::vec3(obb1.Orientation[1])); // Y 축
    axes[2] = glm::normalize(glm::vec3(obb1.Orientation[2])); // Z 축

    // 3. OBB의 각 축(axes)의 투영 길이를 계산하여 총 투영 반지름(Extent)
    float extent =
        obb1.HalfSizes.x * std::abs(glm::dot(axes[0], axis)) +
        obb1.HalfSizes.y * std::abs(glm::dot(axes[1], axis)) +
        obb1.HalfSizes.z * std::abs(glm::dot(axes[2], axis));

    // 4. 최소(min)와 최대(max) 투영 간격을 계산
    min = centerProjection - extent;
    max = centerProjection + extent;
}


// 축에 대한 투영 간격이 겹치는지 확인
bool IntervalsOverlap(float min1, float max1, float min2, float max2) {
	return max1 >= min2 && max2 >= min1;
}


// OBB 충돌 검사 메인 함수
bool CheckOBBCollision(const OBB& obb1, const OBB& obb2) {
	// OBB1 및 OBB2의 축 벡터 계산
    glm::vec3 axes1[3] = {
        glm::normalize(glm::vec3(obb1.Orientation[0])),
        glm::normalize(glm::vec3(obb1.Orientation[1])),
        glm::normalize(glm::vec3(obb1.Orientation[2]))
    };
    glm::vec3 axes2[3] = {
        glm::normalize(glm::vec3(obb2.Orientation[0])),
        glm::normalize(glm::vec3(obb2.Orientation[1])),
        glm::normalize(glm::vec3(obb2.Orientation[2]))
    };

    // ----------------------------------------------------
    // 1. OBB1과 OBB2의 축 6개에 대해 테스트 (분리 축 1~6)
    // ----------------------------------------------------
    glm::vec3 separationAxes[15];
    int axisCount = 0;

    // OBB1의 축 3개
    for (int i = 0; i < 3; ++i) separationAxes[axisCount++] = axes1[i];
    // OBB2의 축 3개
    for (int i = 0; i < 3; ++i) separationAxes[axisCount++] = axes2[i];


    // 모든 분리 축 후보에 대해 검사
    for (int i = 0; i < axisCount; ++i) {
        glm::vec3 axis = separationAxes[i];

        // 투영 축이 (0,0,0) 벡터이거나 너무 작으면 무시
        if (glm::length(axis) < 1e-6f) continue;

        // 두 OBB를 축에 투영하고 겹침을 확인
        float min1, max1, min2, max2;
        ObjectOBBCollision(obb1, axis, min1, max1);
        ObjectOBBCollision(obb2, axis, min2, max2);

        // 겹치지 않는 축을 찾으면, 충돌X
        if (!IntervalsOverlap(min1, max1, min2, max2)) return false;
    }

    // ----------------------------------------------------
    // 2. 각 축의 외적(Cross Product)으로 생성된 축 9개에 대해 테스트 (분리 축 7~15)
    // ----------------------------------------------------
    for (int i = 0; i < 3; ++i) { // OBB1의 축
        for (int j = 0; j < 3; ++j) { // OBB2의 축
            glm::vec3 axis = glm::cross(axes1[i], axes2[j]);

            // 투영 축이 (0,0,0) 벡터이거나 너무 작으면 무시
            if (glm::length(axis) < 1e-6f) continue;

            // 외적 결과 벡터는 정규화
            axis = glm::normalize(axis);

            float min1, max1, min2, max2;
            ObjectOBBCollision(obb1, axis, min1, max1);
            ObjectOBBCollision(obb2, axis, min2, max2);

            // 겹치지 않는 축을 찾으면, 충돌X
            if (!IntervalsOverlap(min1, max1, min2, max2)) return false;
			else std::cout << "충돌" << std::endl;
        }
    }

    // 15개 모든 축에서 분리되지 않았다면, 충돌
    return true;
}