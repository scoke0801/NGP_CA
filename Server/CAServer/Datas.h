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

// 여기를 채워 주세요
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{
};
struct LobbySceneSendData
{
};
#pragma endregion

// 여기를 채워 주세요
#pragma region TitleSceneDatas;
struct TitleSceneRecvData
{
};
struct TitleSceneSendData
{
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
struct GameSceneRecvData
{
	int playerIndex;
	Vector2f position;
	int waterRange;
	int speed;
	int state;
	int mapData[width][height];
};

enum class PlayerState
{

};


struct GameSceneSendData
{
	Vector2f position;
	int speed;
	PlayerState state;
	int mapData[width][height];
	bool isGameEnd;
};
#pragma endregion