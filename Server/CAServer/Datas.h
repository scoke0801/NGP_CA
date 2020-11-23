#pragma once

#define	MAP_WIDTH 15
#define MAP_HEIGHT 13
 
#include "Vector2D.h"

enum class SceneType
{
	LobbyScene = 0,
	TitleScene,
	GameScene,
	GameRecordScene
};

// ���⸦ ä�� �ּ���
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{
};
struct LobbySceneSendData
{
};
#pragma endregion

// ���⸦ ä�� �ּ���
#pragma region TitleSceneDatas;
struct TitleSceneRecvData
{
};
struct TitleSceneSendData
{
};
#pragma endregion 

// ���⸦ ä�� �ּ���
#pragma region GameRecordSceneDatas;
struct GameRecordSceneRecvData
{
};
struct GameRecordSceneSendData
{
};
#pragma endregion


#pragma region GameSceneDatas

#define PlAYER_SPEED 52.0f
enum class PlayerState
{
	wait = 0,
	move,
	trap = 4,
	die,
	live,
};
enum class Direction
{
	down = 0,
	up,
	left,
	right
};
struct GameSceneRecvData
{
	int playerIndex;
	Vector2f position;
	int waterRange;
	int speed;
	Direction direction;
	int state;
	int mapData[MAP_HEIGHT][MAP_WIDTH];
};
struct GameSceneSendData
{
	Vector2f position;
	int speed;
	PlayerState state;
	int mapData[MAP_HEIGHT][MAP_WIDTH];
	bool isGameEnd;
};
#pragma endregion