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
extern MultiTextureObject DemonHammer3Object;

// -- 망치 -- (보석 박혀있는 컨셉)
extern MultiTextureObject PickaxeObject;
extern MultiTextureObject Pickaxe2Object;
extern MultiTextureObject Pickaxe3Object;
extern MultiTextureObject Pickaxe4Object;

// -- 두더지 -- (기본)
extern MultiTextureObject MoleBodyAndHandObject; // 몸통과 손
extern MultiTextureObject MoleEyeAndMustacheObject; // 눈과 콧수염
extern MultiTextureObject MoleNailObject; // 손톱
extern MultiTextureObject MoleNoseObject; // 코
extern MultiTextureObject MoleNoseTipObject; // 코 끝

// -- 두더지 -- (황금)
extern MultiTextureObject GoldenMoleBodyAndHandObject; // 몸통과 손
extern MultiTextureObject GoldenMoleEyeAndMustacheObject; // 눈과 콧수염
extern MultiTextureObject GoldenMoleNailObject; // 손톱
extern MultiTextureObject GoldenMoleNoseObject; // 코
extern MultiTextureObject GoldenMoleNoseTipObject; // 코 끝
extern MultiTextureObject GoldenMoleCoinObject; // 코인

// -- 두더지 -- (폭탄)
extern MultiTextureObject BombMoleBodyAndHandObject; // 몸통과 손
extern MultiTextureObject BombMoleEyeAndMustacheObject; // 눈과 콧수염
extern MultiTextureObject BombMoleNailObject; // 손톱
extern MultiTextureObject BombMoleNoseObject; // 코
extern MultiTextureObject BombMoleNoseTipObject; // 코 끝
extern MultiTextureObject BombMoleBombObject; // 폭탄
extern MultiTextureObject BombMoleFuseObject; // 도화선
extern MultiTextureObject BombMoleXmarkObject; // 폭탄 X 마크


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
extern std::vector<GLuint> DemonHammer3TextureIds;

// -- 망치 -- (보석 박혀있는 컨셉)
extern std::vector<GLuint> PickaxeTextureIds;
extern std::vector<GLuint> Pickaxe2TextureIds;
extern std::vector<GLuint> Pickaxe3TextureIds;
extern std::vector<GLuint> Pickaxe4TextureIds;

// -- 두더지 -- (기본)
extern std::vector<GLuint> MoleBodyAndHandTextureIds; // 몸통과 손
extern std::vector<GLuint> MoleNailTextureIds; // 손톱
extern std::vector<GLuint> MoleEyeAndMustacheTextureIds; // 눈과 콧수염
extern std::vector<GLuint> MoleNoseTextureIds; // 코
extern std::vector<GLuint> MoleNoseTipTextureIds; // 코 끝

// -- 두더지 -- (황금)
extern std::vector<GLuint> GoldenMoleBodyAndHandTextureIds; // 몸통과 손
extern std::vector<GLuint> GoldenMoleNailTextureIds; // 손톱
extern std::vector<GLuint> GoldenMoleEyeAndMustacheTextureIds; // 눈과 콧수염
extern std::vector<GLuint> GoldenMoleNoseTextureIds; // 코
extern std::vector<GLuint> GoldenMoleNoseTipTextureIds; // 코 끝
extern std::vector<GLuint> GoldenMoleCoinTextureIds; // 코인

// -- 두더지 -- (폭탄)
extern std::vector<GLuint> BombMoleBodyAndHandTextureIds; // 몸통과 손
extern std::vector<GLuint> BombMoleEyeAndMustacheTextureIds; // 눈과 콧수염
extern std::vector<GLuint> BombMoleNailTextureIds; // 손톱
extern std::vector<GLuint> BombMoleNoseTextureIds; // 코
extern std::vector<GLuint> BombMoleNoseTipTextureIds; // 코 끝
extern std::vector<GLuint> BombMoleBombTextureIds; // 폭탄
extern std::vector<GLuint> BombMoleFuseTextureIds; // 도화선
extern std::vector<GLuint> BombMoleXmarkTextureIds; // 폭탄 X 마크

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