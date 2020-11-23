#pragma once

#define	MAP_WIDTH 15
#define MAP_HEIGHT 13
 
#include "Vector2D.h"

enum class SceneType
{
	TitleScene=0,
	LobbyScene,
	GameScene,
	GameRecordScene
};

// ���⸦ ä�� �ּ���
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{ 
	int Thread_Num=0;
	bool is_All_Ready;
	bool Player2_Ready;
	bool Player3_Ready; 
	string chatData;
	bool isGameStart;
};
struct LobbySceneSendData
{
	int Thread_Num;

	bool is_All_Ready;
	string chatData;
	bool isGameStart; 
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