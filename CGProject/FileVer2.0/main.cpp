#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gtc/type_ptr.hpp>
#include <iostream>

// 분리된 헤더 파일들
#include "Globals.h"
#include "Init.h"
#include "ObjectLoader.h"
#include "OBB_Collision.h"

using namespace std;
using namespace glm;

// -------------------- OBB 월드 변환 헬퍼 함수 --------------------
// 주어진 객체의 ModelMatrix를 사용하여 로컬 OBB를 월드 OBB로 업데이트
void UpdateOBB(MultiTextureObject& mobj, const glm::mat4& modelMatrix) {
    // 로드된 OBB 데이터가 유효한지 간단히 확인
    if (mobj.obbLocal.HalfSizes.x == 0.0f && mobj.obbLocal.HalfSizes.y == 0.0f && mobj.obbLocal.HalfSizes.z == 0.0f) {
        return;
    }

    // 1. OBB의 월드 공간 중심점 업데이트
    glm::vec4 centerWorld = modelMatrix * glm::vec4(mobj.obbLocal.Center, 1.0f);
    mobj.obbWorld.Center = glm::vec3(centerWorld);

    // 2. OBB의 월드 공간 회전/방향 업데이트
    mobj.obbWorld.Orientation = glm::mat4(glm::mat3(modelMatrix));

    // 3. HalfSizes는 로컬 OBB의 값을 그대로 유지
    mobj.obbWorld.HalfSizes = mobj.obbLocal.HalfSizes;
}











































// OBB 렌더링을 위한 VAO/VBO 초기화
void InitOBBRenderer() {
    // OBB는 8개의 꼭짓점을 연결하는 12개의 선으로 구성됩니다.
    // 각 꼭짓점은 3개의 float(x, y, z)로 표현됩니다.

    // 1. VBO 생성 (8개의 꼭짓점 데이터를 저장할 공간)
    glGenBuffers(1, &OBBVbo);

    // 2. VAO 생성
    glGenVertexArrays(1, &OBBVao);
    glBindVertexArray(OBBVao);

    // 3. VBO를 VAO에 연결
    glBindBuffer(GL_ARRAY_BUFFER, OBBVbo);
    // VBO의 크기를 미리 확보합니다. OBB 렌더링 직전에 데이터는 업데이트될 것입니다.
    // 8개 꼭짓점 * 3 성분/꼭짓점 * sizeof(float)
    glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // 4. 정점 속성 설정 (위치만 사용, Layout Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 정리
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// OBB를 그리는 함수
void DrawOBB(const OBB& obb, const glm::vec3& color, bool& drawOBB) {
    if (OBBVao == 0 || !DrawOBB) return;

    // 1. OBB의 8개 꼭짓점(World Space) 계산
    glm::vec3 halfSizes = obb.HalfSizes;

    // 로컬 OBB 꼭짓점 (AABB 기준)
    glm::vec3 localVertices[8] = {
        glm::vec3(halfSizes.x,  halfSizes.y,  halfSizes.z),
        glm::vec3(-halfSizes.x,  halfSizes.y,  halfSizes.z),
        glm::vec3(-halfSizes.x, -halfSizes.y,  halfSizes.z),
        glm::vec3(halfSizes.x, -halfSizes.y,  halfSizes.z),

        glm::vec3(halfSizes.x,  halfSizes.y, -halfSizes.z),
        glm::vec3(-halfSizes.x,  halfSizes.y, -halfSizes.z),
        glm::vec3(-halfSizes.x, -halfSizes.y, -halfSizes.z),
        glm::vec3(halfSizes.x, -halfSizes.y, -halfSizes.z)
    };

    // 2. 꼭짓점을 월드 공간으로 변환
    glm::mat4 orientation = obb.Orientation;
    // OBB의 중심점을 포함한 변환 행렬
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), obb.Center) * orientation;

    // 최종 월드 공간 꼭짓점 저장 배열 (float 배열)
    float worldVertices[8 * 3];

    for (int i = 0; i < 8; ++i) {
        glm::vec4 worldPos = ModelMatrix * glm::vec4(localVertices[i], 1.0f);
        worldVertices[i * 3 + 0] = worldPos.x;
        worldVertices[i * 3 + 1] = worldPos.y;
        worldVertices[i * 3 + 2] = worldPos.z;
    }


    // 3. 렌더링 설정 (기본 셰이더를 다시 사용, 색상만 변경)
    // Model 행렬을 단위 행렬로 설정 (꼭짓점이 이미 월드 공간에 있으므로)
    glm::mat4 IdentityModel = glm::mat4(1.0f);
    glUniformMatrix4fv(LocModel, 1, GL_FALSE, glm::value_ptr(IdentityModel));

    // 텍스처 사용 비활성화 및 색상 설정
    glUniform1i(LocHasTexture, 0);
    glUniform3fv(LocObjectColor, 1, glm::value_ptr(color));
    glUniform1f(LocLightIntensity, 0.0f); // 조명 영향 끄기

    // 4. VBO에 새로운 꼭짓점 데이터 전송
    glBindBuffer(GL_ARRAY_BUFFER, OBBVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(worldVertices), worldVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 5. OBB 그리기 (12개의 선)
    glBindVertexArray(OBBVao);

    // 인덱스 배열: 8개 꼭짓점을 연결하는 12개의 선
    // {0, 1, 1, 2, 2, 3, 3, 0} (앞면)
    // {4, 5, 5, 6, 6, 7, 7, 4} (뒷면)
    // {0, 4, 1, 5, 2, 6, 3, 7} (연결)
    unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // Z+ 면
        4, 5, 5, 6, 6, 7, 7, 4, // Z- 면
        0, 4, 1, 5, 2, 6, 3, 7  // 연결 모서리
    };

    // OBB를 그릴 때 깊이 테스트를 비활성화하여 항상 앞에 그려지도록 할 수 있습니다. (선택 사항)
    // GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);
    if(drawOBB)
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, indices);

    // if(depthTestEnabled) glEnable(GL_DEPTH_TEST);

    // 정리
    glBindVertexArray(0);
    glUniform1f(LocLightIntensity, LightIntensity); // 조명 복원
}








































// -------------------- 렌더링 (Draw Scene) --------------------
void DrawScene() {
    if (ShaderProgram == 0) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(ShaderProgram);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glm::vec3 CameraPos;
    glm::mat4 View;
    if(CameraMoveStart) {
        // 카메라 무빙 시작
        CameraPos = glm::vec3(CameraMoveTranslationX, CameraMoveTranslationY, CameraMoveTranslationZ);
        View = glm::lookAt(CameraPos, glm::vec3(CameraMoveAtX, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
	}
    else if (!CameraMoveStart) {
        // 카메라/뷰 설정
        CameraPos = glm::vec3(0.0f, 15.0f, CameraPosZ);
        View = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1.0f, 0));
    }
    // View 행렬 조작 없이 일반 View 행렬 그대로 사용
    
    if (CameraMoveStart) {
        // 카메라 무빙 중일 때 회전 적용
       /* View = glm::rotate(View, glm::radians(CameraMoveRotationY), glm::vec3(0.0f, 1.0f, 0.0f));*/
        
    }
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)1200 / 800, 0.1f, 1000.0f);

    glUniformMatrix4fv(LocView, 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(LocProjection, 1, GL_FALSE, glm::value_ptr(Projection));

    // 조명/카메라 위치 설정
    glUniform3fv(LocLightPos, 1, glm::value_ptr(LightPos));
    glUniform3f(LocLightColor, 1.0f, 1.0f, 1.0f);
    glUniform3fv(LocViewPos, 1, glm::value_ptr(CameraPos));

    // 기본 objectColor 설정
    glUniform3f(LocObjectColor, 1.0f, 1.0f, 1.0f);
    glUniform1f(LocLightIntensity, LightIntensity); // 일반 조명 세기 설정
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
    // 게임 제목 렌더링
    glUniform1f(LocLightIntensity, 1.0f); // 조명 세기 조절
	glm::mat4 RotateModel = glm::rotate(glm::mat4(1.0f), glm::radians(GameTitleAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 GameTitleModel = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 3.0f, 30.0f)) * RotateModel;
    GameTitleModel = GameTitleModel * GameTitleObject.ModelMatrix;
    Draw(GameTitleObject, GameTitleModel);

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
    glm::mat4 TreeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    TreeModel = TreeModel * TreeObject.ModelMatrix;
    Draw(TreeObject, TreeModel);

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

    // 드래곤 해골 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 DragonSkullModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    DragonSkullModel = DragonSkullModel * DragonSkullObject.ModelMatrix;
    Draw(DragonSkullObject, DragonSkullModel);

    // 물고기 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 FishModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    FishModel = FishModel * FishObject.ModelMatrix;
    Draw(FishObject, FishModel);

    // 수중 괴물 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 MonsterModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    MonsterModel = MonsterModel * MonsterObject.ModelMatrix;
    Draw(MonsterObject, MonsterModel);

    // 골렘 렌더링
    glUniform1f(LocLightIntensity, 1.0f);
    glm::mat4 GolemModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    GolemModel = GolemModel * GolemObject.ModelMatrix;
    Draw(GolemObject, GolemModel);
    // ---------------------------------------------------------------------

	// ---------------------------- UI 렌더링 ----------------------------
    // 하트 렌더링
    if (!CameraMoveStart) {
        glUniform1f(LocLightIntensity, 1.0f);
        int CurrentHp = Hp;
        std::vector<glm::mat4> HeartModel(CurrentHp, glm::mat4(1.0f));
        glm::mat4 RotateModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(HeartAngle), glm::vec3(0, 1, 0));

        for (int i = 0; i < CurrentHp; ++i) {
            HeartModel[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f + 2.0f * i, 5.0f, -10.0f)) * RotateModelMatrix;
            HeartModel[i] = HeartModel[i] * HeartObject.ModelMatrix;
            Draw(HeartObject, HeartModel[i]);
        }
    }

    glUniform1f(LocLightIntensity, 1.0f);
	// --------------------------------------------------------------------
    
    // ---------------------------- 망치 두더지 이펙트  렌더링 ----------------------------
    if (!CameraMoveStart) {
        // 오브젝트 렌더링 (Rotatable Object)
        // 망치 렌더링
        MultiTextureObject* HammerParts[2] = {
            &EmptyObject, &EmptyObject
        };

        if (HammerChoice == 1) {
            HammerParts[0] = &WoodenHammerObject;
            HammerParts[1] = &WoodenHammer2Object;
        }
        else if (HammerChoice == 2) {
            HammerParts[0] = &DemonHammerObject;
            HammerParts[1] = &DemonHammer2Object;
        }
        else if (HammerChoice == 3) {
            HammerParts[0] = &PickaxeObject;
            HammerParts[1] = &Pickaxe2Object;
        }


        // 두더지 렌더링
        

        MultiTextureObject* Mole[16] = {
            &EmptyObject,&EmptyObject,&EmptyObject,&EmptyObject,
            &EmptyObject,&EmptyObject,&EmptyObject,&EmptyObject,
            &EmptyObject,&EmptyObject,&EmptyObject,&EmptyObject,
            &EmptyObject,&EmptyObject,&EmptyObject,&EmptyObject
        };

        for (int i = 0; i < 16; ++i) {
            
            if (MoleCoordinates[i].MoleType == 1) Mole[i] = &MoleObject;
            if (MoleCoordinates[i].MoleType == 2) Mole[i] = &GoldenMoleObject;
            if (MoleCoordinates[i].MoleType == 3) Mole[i] = &BombMoleObject;

            MultiTextureObject* CurrentObject = Mole[i];
            glm::mat4 FinalModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(MoleCoordinates[i].X, MoleCoordinates[i].Y, MoleCoordinates[i].Z)) 
                * CurrentObject->ModelMatrix;

            // **1. 두더지 월드 OBB 업데이트**
            UpdateOBB(*CurrentObject, FinalModelMatrix);
            Draw(*CurrentObject, FinalModelMatrix);
        }
        

        // 이펙트 렌더링
        if (Effect) {
            MultiTextureObject* Effect = &EmptyObject;

            if (EffectChoice == 1) Effect = &CoinObject;

            MultiTextureObject* CurrentEffect = Effect;

            glm::mat4 RotateModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(CoinAngle), glm::vec3(0, 1, 0)) * CurrentEffect->ModelMatrix;
            glm::mat4 FinalModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, CoinY, 0.0f)) * RotateModelMatrix * CurrentEffect->ModelMatrix;

            Draw(*CurrentEffect, FinalModelMatrix);
        }
        // ---------------------------------------------------------------
        



        // ---------------------------- 충돌 감지 및 반응 ----------------------------
        // **충돌 감지 플래그 초기화**
        bool collisionOccurred = false;

        for (int i = 0; i < 2; ++i) {
            MultiTextureObject* CurrentObject = HammerParts[i];

            MultiTextureObject* OBBHammer = HammerParts[0]; // 망치의 중심 파트로 OBB 검사

            if (CurrentObject->Faces.empty()) continue;

            if (OBBHammer->Faces.empty()) continue;

            glm::mat4 RotatedModelMatrix = CurrentObject->ModelMatrix;

            RotatedModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(modelhammerRZ), glm::vec3(0, 0, 1)) * RotatedModelMatrix;
            RotatedModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(modelMoveTZ, 0.0f, modelMoveTX)) * RotatedModelMatrix;
            RotatedModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * RotatedModelMatrix;

            glm::mat4 FinalModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * RotatedModelMatrix;

            // **1. 망치의 월드 OBB 업데이트**
            UpdateOBB(*CurrentObject, FinalModelMatrix);

            //**1. 망치 중심 파트 OBB 업데이트
            UpdateOBB(*OBBHammer, FinalModelMatrix);

            // **2. 망치와 두더지 충돌 검사**
            if (CheckOBBCollision(OBBHammer->obbWorld, MoleObject.obbWorld)) {
                collisionOccurred = true;
            }

            Draw(*CurrentObject, FinalModelMatrix);
        }


        // ---------------------------- 충돌 횟수 카운트 ----------------------------
        if (collisionOccurred && !preCollision) {
            collisionCount++;
            std::cout << "Collision Detected! Total Collisions : " << collisionCount << std::endl;
        }

        // ---------------------------- 이전 충돌 상태 업데이트 ----------------------------
        preCollision = collisionOccurred;


        // **3. 충돌 반응 로직**
        if (collisionOccurred) {
            // 충돌 시 조명 세기 강조 및 객체 색상 변경 (빨간색)
            glUniform1f(LocLightIntensity, 2.0f);
            glUniform3f(LocObjectColor, 1.0f, 0.2f, 0.2f);

            // **충돌 시 OBB를 빨간색으로 그립니다.**
            DrawOBB(MoleObject.obbWorld, glm::vec3(1.0f, 0.0f, 0.0f), drawOBB); // 두더지 머리 OBB
            // 현재 망치 파트의 OBB도 빨간색으로 그립니다.
            // **주의: 망치는 여러 파트이므로 충돌한 파트만 빨간색으로 그리는 것이 정확하나, 
            // 여기서는 OBB가 업데이트된 마지막 파트의 OBB를 그립니다.**
            for (int i = 0; i < 4; ++i) {
                MultiTextureObject* CurrentObject = HammerParts[0];
                if (!CurrentObject->Faces.empty()) {
                    // 충돌 여부와 상관없이 OBB가 업데이트되었으므로 월드 OBB를 그립니다.
                    DrawOBB(CurrentObject->obbWorld, glm::vec3(1.0f, 0.0f, 0.0f), drawOBB);
                }
            }
        }
        else {
            // 충돌이 없으면 기본 조명/색상으로 복원
            glUniform1f(LocLightIntensity, 1.0f);
            glUniform3f(LocObjectColor, 1.0f, 1.0f, 1.0f);

            // **충돌이 없으면 OBB를 초록색(또는 다른 색)으로 그립니다.**
            DrawOBB(MoleObject.obbWorld, glm::vec3(0.0f, 1.0f, 0.0f), drawOBB); // 두더지 굴 OBB
            for (int i = 0; i < 4; ++i) {
                MultiTextureObject* CurrentObject = HammerParts[0];
                if (!CurrentObject->Faces.empty()) {
                    DrawOBB(CurrentObject->obbWorld, glm::vec3(0.0f, 1.0f, 0.0f), drawOBB);
                }
            }
        }
        glUniform1f(LocLightIntensity, 1.0f); // 일반 조명 세기 복원 (혹시 DrawOBB가 변경했다면)
    }
   
    // --------------------------------------------------------------------------------------------------------------


    


    glutSwapBuffers();
}

// -------------------- 타이머 & 키보드 (Timer & Keyboard) --------------------
void Timer(int) {
    
	// 망치 모션
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

	// 카메라 무빙
    if (CameraMoveStart) {
        CameraMoveTranslationZ -= 0.1f;
        CameraMoveTranslationX += 0.07941f;
        CameraMoveTranslationY += 0.016176f;
		CameraMoveAtX -= 0.0147f;
        if (CameraMoveAtX <= 0.0f) {
			CameraMoveAtX = 0.0f;
        }
        if(CameraMoveTranslationZ <= CameraPosZ) {
            CameraMoveTranslationZ = CameraPosZ;
		}
        if (CameraMoveTranslationY >= 15.0f) {
			CameraMoveTranslationY = 15.0f;
        }
        if(CameraMoveTranslationX >= 0.0f) {
            CameraMoveTranslationX = 0.0f;
		}
        if(CameraMoveAtX == 0.0f && CameraMoveTranslationX == 0.0f && 
            CameraMoveTranslationY == 15.0f && CameraMoveTranslationZ == CameraPosZ) {
            CameraMoveStart = false;
		}
    }

	// 코인 효과 움직임
    if (Effect) {
        if (EffectChoice == 1) {
            CoinY += 0.2f;
            CoinAngle += 10.0f;
            if (CoinY >= 5.0f)
            {
                CoinY = 0.0f;
                CoinAngle == 0.0f;
                Effect = false;
            }
        }
    }

    // 하트 UI 회전
    if (HeartRotate) HeartAngle += 5.0f;

    // 게임 제목 회전
    if (CameraMoveStart) GameTitleAngle += 1.0f;
	else GameTitleAngle = 0.0f;
    
    
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

void Keyboard(unsigned char key, int, int) {
    if (!CameraMoveStart) {
        switch (key) {
            // 프로젝트 내에서 실제로 구현하는 기능들
            // 망치 선택 기능
        case'1': HammerChoice = 1; break; // 나무 망치 선택
        case'2': HammerChoice = 2; break; // 악마 망치 선택 
        case'3': HammerChoice = 3; break; // 보석 박혀있는 망치
            // 이펙트 출력 - 임시
        case'e':
            Effect = true; 
            EffectChoice = 1;
            break;
		case'h':
            Hp -= 1;
            if (Hp <= 0) Hp = 0;
            break;




            // 디버그 용도 기능들
        case'w':
            // 카메라 앞 이동
            CameraPosZ -= 2.0f;
            std::cout << "CameraPosZ: " << CameraPosZ << std::endl;
            break;
        case's':
            // 카메라 뒤 이동x
            CameraPosZ += 2.0f;
            std::cout << "CameraPosZ: " << CameraPosZ << std::endl;
            break;
        case'b':
            // OBB 바운딩 박스 토글
            drawOBB = !drawOBB;
            break;
        case'z':
            // 카메라 무빙 시작
            CameraMoveStart = !CameraMoveStart;
            CameraMoveTranslationX = -27.0f;
            CameraMoveTranslationY = 9.5f;
            CameraMoveTranslationZ = 70.0f;

            CameraMoveAtX = 5.0f;
            break;
        case 'q':exit(0); break;
        }
        glutPostRedisplay();
    }
}

void DoMouse(GLint button, GLint state, GLint x, GLint y) {
    if (!CameraMoveStart) {
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
}

void DoPassiveMotion(GLint x, GLint y) {
    if (!CameraMoveStart) {
        GLfloat windowX, windowY, windowZ;
        GLdouble posX, posY, posZ;

        // ** 현재 매트릭스 정보 가져오기 **
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        // ** 윈도우 좌표 설정 **
        windowX = (float)x;
        windowY = (float)(viewport[3] - y);
        windowZ = 0.0f;

        // ** 역 투영 (Unproject) 수행 **
        gluUnProject(windowX, windowY, windowZ, modelview, projection, viewport, &posX, &posY, &posZ);

        // ** 객체 위치 업데이트 **
        modelMoveTX = (float)-posX * 15.0f;
        modelMoveTY = 0.0f;
        modelMoveTZ = (float)-posY * 25.0f;

        glutPostRedisplay();
    }
}
// ----------------------------------------------------

// -------------------- 메인 함수 (Main) --------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("CGProject");

    // GLEW 초기화 (OpenGL 함수 포인터 로드)
    glewInit();

    // 셰이더 생성 및 유효성 검사
    CreateShaderProgram();
    if (ShaderProgram == 0) {
        cerr << "프로그램 초기화 실패. 종료합니다." << endl;
        return -1;
    }

    // OBB 렌더링 초기화 함수 호출 추가
    InitOBBRenderer();

    
    // 텍스처 로드 
	// 게임 제목 - WHACK A MOLE(두더지 잡기)
    GameTitleTextureIds.push_back(LoadTexture("GameTitle.jpg"));
    
    // 환경 - (땅)
    GroundTextureIds.push_back(LoadTexture("Ground.jpg"));

    // 환경 - (강)
    RiverTextureIds.push_back(LoadTexture("River.jpg"));

    // 환경 - (구름)
    CloudTextureIds.push_back(LoadTexture("Cloud.jpg"));

    // 환경 - (나무)
    TreeTextureIds.push_back(LoadTexture("Tree.jpg"));

    // 환경 - (잔디)
    GrassTextureIds.push_back(LoadTexture("Grass.jpg"));

    // 환경 - (두더지 굴)
    HoleTextureIds.push_back(LoadTexture("Hole.jpg"));

    // 환경 - (돌)
    StoneTextureIds.push_back(LoadTexture("Stone.jpg"));

    // 환경 - (울타리)
    FenceTextureIds.push_back(LoadTexture("Fence.jpg"));

    // 환경 - (드래곤 해골)
    DragonSkullTextureIds.push_back(LoadTexture("DragonSkull.jpg"));

    // 환경 - (물고기)
    FishTextureIds.push_back(LoadTexture("Fish.jpg"));

    // 환경 - (수중 괴물)
    MonsterTextureIds.push_back(LoadTexture("Monster.jpg"));

    // 환경 - (골렘)
    GolemTextureIds.push_back(LoadTexture("Golem.jpg"));

    // 오브젝트 - (망치)
    // -- 나무 --
    WoodenHammerTextureIds.push_back(LoadTexture("WoodenHammer.jpg"));
    WoodenHammer2TextureIds.push_back(LoadTexture("WoodenHammer2.jpg"));

    // -- 악마 --
    DemonHammerTextureIds.push_back(LoadTexture("DemonHammer.jpg"));
    DemonHammer2TextureIds.push_back(LoadTexture("DemonHammer2.jpg"));

    // -- 보석 --
    PickaxeTextureIds.push_back(LoadTexture("Pickaxe.jpg"));
    Pickaxe2TextureIds.push_back(LoadTexture("Pickaxe2.jpg"));

    // 오브젝트 - (두더지)
    // -- 기본 --
    MoleTextureIds.push_back(LoadTexture("Mole.jpg"));

    // -- 황금 --
    GoldenMoleTextureIds.push_back(LoadTexture("GoldenMole.jpg"));

    // -- 폭탄 --
    BombMoleTextureIds.push_back(LoadTexture("BombMole.jpg"));

	// 오브젝트 - (효과)
	// -- 동전 --
	CoinTextureIds.push_back(LoadTexture("Coin.jpg"));

    // 오브젝트 - (UI)
    // -- 하트 --
    HeartTextureIds.push_back(LoadTexture("Heart.jpg"));

    // ---- 오브젝트 생성 ----
    // 게임 제목 - WHACK A MOLE(두더지 잡기)
    CreateMultiFaceObject(GameTitleObject, "GameTitle.obj", glm::vec3(1.0f, 1.0f, 1.0f), GameTitleTextureIds);

    // 환경 - (땅)
    CreateMultiFaceObject(GroundObject, "Ground.obj", glm::vec3(1.0f, 1.0f, 1.0f), GroundTextureIds);

    // 환경 - (강)
    CreateMultiFaceObject(RiverObject, "River.obj", glm::vec3(1.0f, 1.0f, 1.0f), RiverTextureIds);

    // 환경 - (구름)
    CreateMultiFaceObject(CloudObject, "Cloud.obj", glm::vec3(1.0f, 1.0f, 1.0f), CloudTextureIds);

    // 환경 - (나무)
    CreateMultiFaceObject(TreeObject, "Tree.obj", glm::vec3(1.0f), TreeTextureIds);

    // 환경 - (잔디)
    CreateMultiFaceObject(GrassObject, "Grass.obj", glm::vec3(1.0f), GrassTextureIds);

    // 환경 - (두더지 굴)
    CreateMultiFaceObject(HoleObject, "Hole.obj", glm::vec3(1.0f), HoleTextureIds);

    // 환경 - (돌)
    CreateMultiFaceObject(StoneObject, "Stone.obj", glm::vec3(1.0f), StoneTextureIds);

    // 환경 - (울타리)
    CreateMultiFaceObject(FenceObject, "Fence.obj", glm::vec3(1.0f), FenceTextureIds);

    // 환경 - (드래곤 해골)
    CreateMultiFaceObject(DragonSkullObject, "DragonSkull.obj", glm::vec3(1.0f), DragonSkullTextureIds);

    // 환경 - (물고기)
    CreateMultiFaceObject(FishObject, "Fish.obj", glm::vec3(1.0f), FishTextureIds);

    // 환경 - (수중 괴물)
    CreateMultiFaceObject(MonsterObject, "Monster.obj", glm::vec3(1.0f), MonsterTextureIds);

    // 환경 - (골렘)
    CreateMultiFaceObject(GolemObject, "Golem.obj", glm::vec3(1.0f), GolemTextureIds);
    // -----------------------------------------------------

    // 오브젝트 - (망치) 
    // -- 나무 --
    CreateMultiFaceObject(WoodenHammerObject, "WoodenHammer.obj", glm::vec3(1.0f), WoodenHammerTextureIds);
    CreateMultiFaceObject(WoodenHammer2Object, "WoodenHammer2.obj", glm::vec3(1.0f), WoodenHammer2TextureIds);

    // -- 악마 --
    CreateMultiFaceObject(DemonHammerObject, "DemonHammer.obj", glm::vec3(1.0f), DemonHammerTextureIds);
    CreateMultiFaceObject(DemonHammer2Object, "DemonHammer2.obj", glm::vec3(1.0f), DemonHammer2TextureIds);

    // -- 보석 박혀있는 망치 --
    CreateMultiFaceObject(PickaxeObject, "Pickaxe.obj", glm::vec3(1.0f), PickaxeTextureIds);
    CreateMultiFaceObject(Pickaxe2Object, "Pickaxe2.obj", glm::vec3(1.0f), Pickaxe2TextureIds);

    // 오브젝트 - (두더지)
    // -- 기본 --
    CreateMultiFaceObject(MoleObject, "Mole.obj", glm::vec3(0.5f), MoleTextureIds);

    // -- 황금 --
    CreateMultiFaceObject(GoldenMoleObject, "GoldenMole.obj", glm::vec3(0.5f), GoldenMoleTextureIds);

    // -- 폭탄 --
    CreateMultiFaceObject(BombMoleObject, "BombMole.obj", glm::vec3(0.5f), BombMoleTextureIds);


    // 오브젝트 - (효과)
    // -- 동전 --
    CreateMultiFaceObject(CoinObject, "Coin.obj", glm::vec3(1.0f), CoinTextureIds);

    // 오브젝트 - (UI)
    // -- 하트 --
    CreateMultiFaceObject(HeartObject, "Heart.obj", glm::vec3(1.5f), HeartTextureIds);

    // 렌더링 설정 및 메인 루프 시작
   
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutDisplayFunc(DrawScene);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(DoMouse);
    glutPassiveMotionFunc(DoPassiveMotion);
    glutTimerFunc(0, Timer, 0);
    glutMainLoop();
    return 0;
}