// globals.h의 extern 변수들에 메모리를 할당합니다.
#include "Globals.h"

// -------------------- 전역 변수 초기화 --------------------
GLuint ShaderProgram = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> Mole(1, 2);

// Uniform 위치 캐시 (Loc: Location)
GLint LocModel = -1, LocView = -1, LocProjection = -1;
GLint LocObjectColor = -1, LocHasTexture = -1, LocLightPos = -1, LocLightColor = -1, LocLightIntensity = -1, LocViewPos = -1;
GLint LocMainTexture = -1;

// 객체 렌더링 정보

// -- 땅 --
MultiTextureObject GroundObject;

// -- 강 --
MultiTextureObject RiverObject;

// -- 구름 --
MultiTextureObject CloudObject;

// -- 나무 --
MultiTextureObject TreeTrunkObject;
MultiTextureObject TreeLeavesObject;

// -- 잔디 --
MultiTextureObject GrassObject;

// -- 두더지 굴 --
MultiTextureObject HoleObject;

// -- 돌 --
MultiTextureObject StoneObject;

// -- 울타리 --
MultiTextureObject FenceObject;

// -- 망치 -- (초기 상태 : 아무 정보 없음)
MultiTextureObject EmptyObject;
MultiTextureObject EmptyObject2;
MultiTextureObject EmptyObject3;
MultiTextureObject EmptyObject4;

// -- 망치 -- (나무 컨셉)
MultiTextureObject WoodenHammerObject;
MultiTextureObject WoodenHammer2Object;

// -- 망치 -- (악마 컨셉)
MultiTextureObject DemonHammerObject;
MultiTextureObject DemonHammer2Object;
MultiTextureObject DemonHammer3Object;

// -- 망치 -- (보석 박혀있는 컨셉)
MultiTextureObject PickaxeObject;
MultiTextureObject Pickaxe2Object;
MultiTextureObject Pickaxe3Object;
MultiTextureObject Pickaxe4Object;

// -- 두더지 -- (기본)
MultiTextureObject MoleBodyAndHandObject; // 몸통과 손
MultiTextureObject MoleEyeAndMustacheObject; // 눈과 콧수염
MultiTextureObject MoleNailObject; // 손톱
MultiTextureObject MoleNoseObject; // 코
MultiTextureObject MoleNoseTipObject; // 코 끝

// -- 두더지 -- (황금)
MultiTextureObject GoldenMoleBodyAndHandObject; // 몸통과 손
MultiTextureObject GoldenMoleEyeAndMustacheObject; // 눈과 콧수염
MultiTextureObject GoldenMoleNailObject; // 손톱
MultiTextureObject GoldenMoleNoseObject; // 코
MultiTextureObject GoldenMoleNoseTipObject; // 코 끝
MultiTextureObject GoldenMoleCoinObject; // 코인

// 텍스처 ID들을 저장할 벡터
// -- 땅 --
std::vector<GLuint> GroundTextureIds;

// -- 강 --
std::vector<GLuint> RiverTextureIds;

// -- 구름 --
std::vector<GLuint> CloudTextureIds;

// --나무 --
std::vector<GLuint> TreeTrunkTextureIds;
std::vector<GLuint> TreeLeavesTextureIds;

// -- 잔디 --
std::vector<GLuint> GrassTextureIds;

// -- 두더지 굴 --
std::vector<GLuint> HoleTextureIds;

// -- 돌 --
std::vector<GLuint> StoneTextureIds;

// -- 울타리 --
std::vector<GLuint> FenceTextureIds;

// -- 망치 --
// -- 나무 --
std::vector<GLuint> WoodenHammerTextureIds;
std::vector<GLuint> WoodenHammer2TextureIds;

// -- 악마 --
std::vector<GLuint> DemonHammerTextureIds;
std::vector<GLuint> DemonHammer2TextureIds;
std::vector<GLuint> DemonHammer3TextureIds;

// -- 보석 박혀있는 망치 --
std::vector<GLuint> PickaxeTextureIds;
std::vector<GLuint> Pickaxe2TextureIds;
std::vector<GLuint> Pickaxe3TextureIds;
std::vector<GLuint> Pickaxe4TextureIds;

// -- 두더지 -- (기본)
std::vector<GLuint> MoleBodyAndHandTextureIds; // 몸통과 손
std::vector<GLuint> MoleNailTextureIds; // 손톱
std::vector<GLuint> MoleEyeAndMustacheTextureIds; // 눈과 콧수염
std::vector<GLuint> MoleNoseTextureIds; // 코
std::vector<GLuint> MoleNoseTipTextureIds; // 코 끝

// -- 두더지 -- (황금)
std::vector<GLuint> GoldenMoleBodyAndHandTextureIds; // 몸통과 손
std::vector<GLuint> GoldenMoleNailTextureIds; // 손톱
std::vector<GLuint> GoldenMoleEyeAndMustacheTextureIds; // 눈과 콧수염
std::vector<GLuint> GoldenMoleNoseTextureIds; // 코
std::vector<GLuint> GoldenMoleNoseTipTextureIds; // 코 끝
std::vector<GLuint> GoldenMoleCoinTextureIds; // 코인

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
float CameraPosZ = 60.0f;

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

// 망치 움직임 상태
bool hammerDown = false;
bool hammerUp = false;

// 망치 회전 각도
GLfloat modelhammerRZ = 0.0f;

// 망치 선택 변수
int HammerChoice = 0; // 0.초기 선택 x 1. 악마 망치, 2. 보석 박혀있는 망치, 3~
int MoleChoice = 0; // 0. 초기 선택x  1.기본 두더지  2.~