#pragma once
#include "stdafx.h"

#include "stdafx.h"

#include "Vector2D.h"

// 여기를 채워 주세요
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{
};
struct LobbySceneSendData
{ 
	string Char_Type;
	int Num;
	bool isReady;
	string chatData;
	bool isGameStart; 
};
#pragma endregion

// 여기를 채워 주세요
#pragma region TitleSceneDatas;
struct TitleSceneRecvData
{
	bool result;
};
struct TitleSceneSendData
{
	string id;
	string pw;
	bool isNew;
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
	Vector2f position;
	int speed;
	//PlayerState state;
	int mapData[MAP_WIDTH][MAP_HEIGHT];
	bool isGameEnd;
};

struct GameSceneSendData
{
	int playerIndex;
	Vector2f position;
	int waterRange;
	int speed;
	Direction direction;
	int state;
	int mapData[MAP_WIDTH][MAP_HEIGHT];
};

#pragma endregion