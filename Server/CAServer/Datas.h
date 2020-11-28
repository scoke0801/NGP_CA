#pragma once

#define	MAP_WIDTH 15
#define MAP_HEIGHT 13

#include "Vector2D.h"

enum class SceneType
{
	TitleScene = 0,
	LobbyScene,
	GameScene,
	GameRecordScene
};

// 여기를 채워 주세요
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{
	int Thread_Num = 0;
	bool is_All_Ready;
	bool Player2_Ready;
	bool Player3_Ready;
	string chatData;
	bool isGameStart;
};
struct LobbySceneSendData
{
	int Thread_Num;
	string Char_Type[3];
	bool is_All_Ready;
	vector<string> chatData;
	bool isGameStart;
};
#pragma endregion

// 여기를 채워 주세요
#pragma region TitleSceneDatas;
struct TitleSceneRecvData
{
	string id;
	string pw;
	bool isNew;
};
struct TitleSceneSendData
{
	bool result;
};
#pragma endregion 

// 여기를 채워 주세요
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
	UINT index;
	Vector2f position;
	int speed;
	PlayerState state;
	int mapData[MAP_HEIGHT][MAP_WIDTH];
	bool isGameEnd;
};
#pragma endregion