// 전역 변수 및 구조체 정의
#pragma once

#include <gl/glew.h>
#include <glm.hpp>
#include <ext.hpp>
#include <vector>
#include <string>
#include <random>

// -------------------- 구조체 (Structs) --------------------

// 면 하나의 렌더링 정보 (FaceRenderObject) - 단일 메쉬를 위해 하나만 사용됨
struct FaceRenderObject {
    GLuint Vao = 0;
    GLuint Ebo = 0;
    size_t IndexCount = 0;
    GLuint TextureId = 0;
};

// OBB 구조체
struct OBB {
    glm::vec3 Center;        // 중심점
    glm::vec3 HalfSizes;    // 반 크기 (각 축에 대한 절반 길이)
    glm::mat4 Orientation;   // 회전 행렬
};

// 다중 텍스처 오브젝트 정보 (MultiTextureObject)
struct MultiTextureObject {
    GLuint SharedVbo = 0;
    std::vector<FaceRenderObject> Faces;
    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    // OBB 충돌처리를 위한 추가 필드
	OBB obbLocal;   // 로컬 OBB
	OBB obbWorld;   // 월드 OBB
};




// -------------------- 전역 변수 (Global Variables) --------------------
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_int_distribution<> Mole;


extern GLuint ShaderProgram;

// Uniform 위치 캐시 (Loc: Location)
extern GLint LocModel, LocView, LocProjection;
extern GLint LocObjectColor, LocHasTexture, LocLightPos, LocLightColor, LocLightIntensity, LocViewPos;
extern GLint LocMainTexture;

// 객체 렌더링 정보
// -- 땅 --
extern MultiTextureObject GroundObject;

// -- 강 --
extern MultiTextureObject RiverObject;

// -- 구름 --
extern MultiTextureObject CloudObject;

// -- 나무 --
extern MultiTextureObject TreeObject;

// -- 잔디 --
extern MultiTextureObject GrassObject;

// -- 두더지 굴 --
extern MultiTextureObject HoleObject;

// -- 돌 --
extern MultiTextureObject StoneObject;

// -- 울타리 --
extern MultiTextureObject FenceObject;

// -- 드래곤 해골 --
extern MultiTextureObject DragonSkullObject;

// -- 망치 -- (초기상태 : 아무 정보 없는 상태)
extern MultiTextureObject EmptyObject;
extern MultiTextureObject EmptyOject2;
extern MultiTextureObject EmptyOject3;
extern MultiTextureObject EmptyOject4;

// -- 망치 -- (나무 컨셉)
extern MultiTextureObject WoodenHammerObject;
extern MultiTextureObject WoodenHammer2Object;

// -- 망치 -- (악마 컨셉)
extern MultiTextureObject DemonHammerObject;
extern MultiTextureObject DemonHammer2Object;

// -- 망치 -- (보석 박혀있는 컨셉)
extern MultiTextureObject PickaxeObject;
extern MultiTextureObject Pickaxe2Object;

// -- 두더지 -- (기본)
extern MultiTextureObject MoleObject;

// -- 두더지 -- (황금)
extern MultiTextureObject GoldenMoleObject;

// -- 두더지 -- (폭탄)
extern MultiTextureObject BombMoleObject;


// 텍스처 ID들을 저장할 벡터

// -- 땅 --
extern std::vector<GLuint> GroundTextureIds;

// -- 강 --
extern std::vector<GLuint> RiverTextureIds;

// -- 구름 --
extern std::vector<GLuint> CloudTextureIds;

// -- 나무 --
extern std::vector<GLuint> TreeTextureIds;

// -- 잔디 --
extern std::vector<GLuint> GrassTextureIds;

// -- 두더지 굴 --
extern std::vector<GLuint> HoleTextureIds;

// -- 돌 --
extern std::vector<GLuint> StoneTextureIds;

// -- 울타리 --
extern std::vector<GLuint> FenceTextureIds;

// -- 드래곤 해골 --
extern std::vector<GLuint> DragonSkullTextureIds;

// -- 망치 -- (나무 컨셉)
extern std::vector<GLuint> WoodenHammerTextureIds;
extern std::vector<GLuint> WoodenHammer2TextureIds;

// -- 망치 -- (악마 컨셉)
extern std::vector<GLuint> DemonHammerTextureIds;
extern std::vector<GLuint> DemonHammer2TextureIds;

// -- 망치 -- (보석 박혀있는 컨셉)
extern std::vector<GLuint> PickaxeTextureIds;
extern std::vector<GLuint> Pickaxe2TextureIds;

// -- 두더지 -- (기본)
extern std::vector<GLuint> MoleTextureIds;

// -- 두더지 -- (황금)
extern std::vector<GLuint> GoldenMoleTextureIds;

// -- 두더지 -- (폭탄)
extern std::vector<GLuint> BombMoleTextureIds;

// 축 VAO
extern GLuint AxesVao;

// OBB 렌더링용
extern GLuint OBBVao;
extern GLuint OBBVbo;
extern bool drawOBB; // OBB 그리기 활성화/비활성화 플래그

// ----------------- 변환 관련 변수 (Transformation Variables) -------------------
extern bool RotateObject;
extern float RotationAngle;
extern float LightIntensity;
extern glm::vec3 LightPos;
extern float CameraPosZ;

// 회전 축
extern float Rx, Ry;

// Arcball / Mouse Variables
extern GLboolean bArcball;
extern GLboolean bParallel;
extern GLfloat cameraRX, cameraRY, cameraTX, cameraTY;
extern GLint preCursorX, preCursorY, nowCursorX, nowCursorY;
extern GLfloat modelMoveTX;
extern GLfloat modelMoveTY;

// -- 망치 움직임 상태 --
extern bool hammerDown;
extern bool hammerUp;

// -- 망치 회전 각도 -- 
extern GLfloat modelhammerRZ;

// --망치 선택 변수 --
extern int HammerChoice;
// 1. 악마 망치, 2. 보석 박혀있는 망치, 3.~

// --두더지 선택 변수--
extern int MoleChoice;
// 1. 기본 두더지, 2.~