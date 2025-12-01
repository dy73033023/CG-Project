#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Globals.h"

// // OBB를 주어진 축에 투영하여 최소(min) 및 최대(max) 간격을 계산
void ObjectOBBCollision(const OBB& obb1, const glm::vec3& axis, 
	float& min, float& max);


// 축에 대한 투영 간격이 겹치는지 확인
bool IntervalsOverlap(float min1, float max1, float min2, float max2);


// OBB 충돌 검사 메인 함수
bool CheckOBBCollision(const OBB& obb1, const OBB& obb2);









