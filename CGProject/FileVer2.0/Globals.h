// 전역 변수 및 구조체 정의
#pragma once

#include <gl/glew.h>
#include <glm.hpp>
#include <ext.hpp>
#include <gl/glu.h> //gluUnproject 사용
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

// 두더지 위치 정보
struct MoleXYZT {
    float X;
    float Y;
    float Z;
    int MoleType;
};

extern MoleXYZT MoleCoordinates[16];

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
// -- 게임 제목 --
extern MultiTextureObject GameTitleObject;

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

// -- 물고기 --
extern MultiTextureObject FishObject;

// -- 수중 괴물 --
extern MultiTextureObject MonsterObject;

// -- 골렘 --
extern MultiTextureObject GolemObject;

// -- 망치 -- (초기상태 : 아무 정보 없는 상태)
extern MultiTextureObject EmptyObject;
extern MultiTextureObject EmptyOject2;

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

// -- 효과 -- 
// -- 동전 --
extern MultiTextureObject CoinObject;

// -- UI --
extern MultiTextureObject HeartObject;

// 텍스처 ID들을 저장할 벡터
// -- 게임 제목 --
extern std::vector<GLuint> GameTitleTextureIds;

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

// -- 물고기 --
extern std::vector<GLuint> FishTextureIds;

// -- 수중 괴물 --
extern std::vector<GLuint> MonsterTextureIds;

// -- 골렘 --
extern std::vector<GLuint> GolemTextureIds;

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

// -- 효과 -- 
// -- 동전 --
extern std::vector<GLuint> CoinTextureIds;

// -- UI --
extern std::vector<GLuint> HeartTextureIds;

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

// 카메라 무빙
extern bool CameraMoveStart;
// 카메라 이동 거리
extern float CameraMoveTranslationX;
extern float CameraMoveTranslationY;
extern float CameraMoveTranslationZ;

// 카메라 보는 방향
extern float CameraMoveAtX;


// 회전 축
extern float Rx, Ry;

// Arcball / Mouse Variables
extern GLboolean bArcball;
extern GLboolean bParallel;
extern GLfloat cameraRX, cameraRY, cameraTX, cameraTY;
extern GLint preCursorX, preCursorY, nowCursorX, nowCursorY;
extern GLfloat modelMoveTX;
extern GLfloat modelMoveTY;
extern GLfloat modelMoveTZ;


// 마우스 입력을 3차원 월드공간으로 바꾸기 위한 변수
extern GLint screenWidth;
extern GLint screenHeight;
extern GLdouble modelview[16];
extern GLdouble projection[16];
extern GLint viewport[4];

// -- 망치 움직임 상태 --
extern bool hammerDown;
extern bool hammerUp;

// -- 망치 회전 각도 -- 
extern GLfloat modelhammerRZ;

// ----------------- 객체 선택 기능 변수 (Object Selection Variables) -------------------
// 망치 선택 변수
extern int HammerChoice;
// 두더지 선택 변수
extern int MoleChoice;

// ----------------- 효과 관련 변수 (Effect Variables) -------------------
// UI 이펙트 렌더링 변수
extern bool Effect;
extern bool HeartRotate;
extern int Hp; // 하트 개수
// 이펙트 선택 변수
extern int EffectChoice;


// ------------------ UI, 이펙트 애니메이션 ------------------
extern float CoinY;
extern float CoinAngle;
extern float HeartAngle;
extern float GameTitleAngle;

// 충돌 체크 변수
// 충돌이 발생했는지 여부
extern bool preCollision;
// 충돌 횟수
extern int collisionCount;