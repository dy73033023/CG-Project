// globals.h의 extern 변수들에 메모리를 할당합니다.
#include "Globals.h"

// -------------------- 전역 변수 초기화 --------------------
GLuint ShaderProgram = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> Mole(1, 3);

// Uniform 위치 캐시 (Loc: Location)
GLint LocModel = -1, LocView = -1, LocProjection = -1;
GLint LocObjectColor = -1, LocHasTexture = -1, LocLightPos = -1, LocLightColor = -1, LocLightIntensity = -1, LocViewPos = -1;
GLint LocMainTexture = -1;

// 객체 렌더링 정보

// -- 게임 제목 --
MultiTextureObject GameTitleObject;

// -- 땅 --
MultiTextureObject GroundObject;

// -- 강 --
MultiTextureObject RiverObject;

// -- 구름 --
MultiTextureObject CloudObject;

// -- 나무 --
MultiTextureObject TreeObject;

// -- 잔디 --
MultiTextureObject GrassObject;

// -- 두더지 굴 --
MultiTextureObject HoleObject;

// -- 돌 --
MultiTextureObject StoneObject;

// -- 울타리 --
MultiTextureObject FenceObject;

// -- 드래곤 해골 --
MultiTextureObject DragonSkullObject;

// -- 물고기 --
MultiTextureObject FishObject;

// -- 수중 괴물 --
MultiTextureObject MonsterObject;

// -- 골렘 --
MultiTextureObject GolemObject;

// -- 망치 -- (초기 상태 : 아무 정보 없음)
MultiTextureObject EmptyObject;
MultiTextureObject EmptyObject2;

// -- 망치 -- (나무 컨셉)
MultiTextureObject WoodenHammerObject;
MultiTextureObject WoodenHammer2Object;

// -- 망치 -- (악마 컨셉)
MultiTextureObject DemonHammerObject;
MultiTextureObject DemonHammer2Object;

// -- 망치 -- (보석 박혀있는 컨셉)
MultiTextureObject PickaxeObject;
MultiTextureObject Pickaxe2Object;

// -- 두더지 -- (기본)
MultiTextureObject MoleObject;

// -- 두더지 -- (황금)
MultiTextureObject GoldenMoleObject;

// -- 두더지 -- (폭탄)
MultiTextureObject BombMoleObject;

// -- 효과 -- 
// -- 동전 --
MultiTextureObject CoinObject;

// -- UI -- 
MultiTextureObject HeartObject;

// 텍스처 ID들을 저장할 벡터
// -- 게임 제목 --
std::vector<GLuint> GameTitleTextureIds;

// -- 땅 --
std::vector<GLuint> GroundTextureIds;

// -- 강 --
std::vector<GLuint> RiverTextureIds;

// -- 구름 --
std::vector<GLuint> CloudTextureIds;

// --나무 --
std::vector<GLuint> TreeTextureIds;

// -- 잔디 --
std::vector<GLuint> GrassTextureIds;

// -- 두더지 굴 --
std::vector<GLuint> HoleTextureIds;

// -- 돌 --
std::vector<GLuint> StoneTextureIds;

// -- 울타리 --
std::vector<GLuint> FenceTextureIds;

// -- 드래곤 해골 --
std::vector<GLuint> DragonSkullTextureIds;

// -- 물고기 --
std::vector<GLuint> FishTextureIds;

// -- 수중 괴물 --
std::vector<GLuint> MonsterTextureIds;

// -- 골렘 --
std::vector<GLuint> GolemTextureIds;

// -- 망치 --
// -- 나무 --
std::vector<GLuint> WoodenHammerTextureIds;
std::vector<GLuint> WoodenHammer2TextureIds;

// -- 악마 --
std::vector<GLuint> DemonHammerTextureIds;
std::vector<GLuint> DemonHammer2TextureIds;

// -- 보석 박혀있는 망치 --
std::vector<GLuint> PickaxeTextureIds;
std::vector<GLuint> Pickaxe2TextureIds;

// -- 두더지 -- (기본)
std::vector<GLuint> MoleTextureIds;

// -- 두더지 -- (황금)
std::vector<GLuint> GoldenMoleTextureIds;

// -- 두더지 -- (폭탄)
std::vector<GLuint> BombMoleTextureIds;

// -- 효과 --
// -- 동전 --
std::vector<GLuint> CoinTextureIds;

// -- UI --
std::vector<GLuint> HeartTextureIds;

// 축 VAO
GLuint AxesVao = 0;

// OBB 렌더링용
GLuint OBBVao = 0;
GLuint OBBVbo = 0;
bool drawOBB = true; // 기본적으로 OBB를 그리도록 설정

// ----------------- 변환 관련 변수 (Transformation Variables) -------------------
bool RotateObject = true;
float RotationAngle = 0.0f;
float LightIntensity = 1.0f;
glm::vec3 LightPos = glm::vec3(0.0f, 50.0f, 50.0f);
float CameraPosZ = 36.0f;

// 카메라 무빙
bool CameraMoveStart = true;
float CameraMoveTranslationX = -27.0f;
float CameraMoveTranslationY = 9.5f;
float CameraMoveTranslationZ = 70.0f;

float CameraMoveAtX = 5.0f;


// 회전 축
float Rx = 0, Ry = 1;

// Arcball / Mouse Variables
GLboolean bArcball = GL_FALSE;
GLboolean bParallel = GL_FALSE;
GLfloat cameraRX = 0.0f;
GLfloat cameraRY = 0.0f;
GLfloat cameraTX = 0.0f;
GLfloat cameraTY = 0.0f;
GLint preCursorX = 0;
GLint preCursorY = 0;
GLint nowCursorX = 0;
GLint nowCursorY = 0;
GLfloat modelMoveTX = 0.0f;
GLfloat modelMoveTY = 0.0f;
GLfloat modelMoveTZ = 0.0f;

// 마우스 입력을 3차원 월드공간으로 바꾸기 위한 변수
GLint screenWidth = 1200;
GLint screenHeight = 800;
GLdouble modelview[16];
GLdouble projection[16];
GLint viewport[4];

// 망치 움직임 상태
bool hammerDown = false;
bool hammerUp = false;

// 망치 회전 각도
GLfloat modelhammerRZ = 0.0f;

// ----------------- 객체 선택 기능 변수 (Object Selection Variables) -------------------
// 망치 선택 변수
int HammerChoice = 0;
// 두더지 선택 변수
int MoleChoice = 0; 

// ----------------- 효과 관련 변수 (Effect Variables) -------------------
// UI 이펙트 렌더링 변수
bool Effect = false;
bool HeartRotate = true;
int Hp = 5; // 하트 개수

// 이펙트 선택 변수
int EffectChoice = 0;

// ------------------ UI 이펙트 애니메이션 ------------------
float CoinY = 0.0f;
float CoinAngle = 0.0f;
float HeartAngle = 0.0f;
float GameTitleAngle = 0.0f;

// 충돌 체크 변수
// 충돌이 발생했는지 여부
bool preCollision;
// 충돌 횟수
int collisionCount;
//	-------------------- 두더지 위치 -----------------------
MoleXYZT MoleCoordinates[16] = {
    {0.4f, 0.0f, 0.2f, 1}, {9.8f, 0.0f, -7.5f, 1}, {-7.3f, 0.0f, 4.5f, 1}, {2.25f, 0.0f, 10.6f, 1},
    {-3.7f, 0.0f, -7.2f, 1}, {8.8f, 0.0f, 4.1f, 1}, {-4.7f, 0.0f, -2.0f, 2}, {3.5f, 0.0f, 5.0f, 2},
    {-8.8f, 0.0f, 9.0f, 2}, {8.8f, 0.0f, -0.8f, 2}, {9.2f, 0.0f, 10.0f, 2}, {-10.2f, 0.0f, -1.3f, 3},
    {-8.6f, 0.0f, -8.7f, 3}, {2.2f, 0.0f, -9.2f, 3}, {5.1f, 0.0f, -5.0f, 3}, {-2.6f, 0.0f, 6.9f, 3},
};