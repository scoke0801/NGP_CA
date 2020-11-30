#pragma once
#include "stdafx.h"

#include "stdafx.h"

#include "Vector2D.h"

// 여기를 채워 주세요
#pragma region LobbySceneDatas
struct LobbySceneRecvData
{
	int chartype;
	string id;
};
struct LobbySceneSendData
{ 
	string id;
	int Num;
	int char_type;
	string playerIndex;
	bool isReady;
	string chatData;
	bool isGameStart; 
};

#pragma endregion

// 여기를 채워 주세요
#pragma region TitleSceneDatas;
struct TitleSceneRecvData
{
	int playerIndex;
	string text;
	bool result;
};
struct TitleSceneSendData
{
	string id;
	string pw;
	bool isNew;
};

struct TitleToLobbySceneData
{
	int playerindx;
	string id;
};
#pragma endregion 

// 여기를 채워 주세요
#pragma region GameRecordSceneDatas;
struct GameRecordSceneRecvData
{
	string id;
	int itemScore;
	int survivedScore;
};
struct GameRecordSceneSendData
{
	string id;
	int itemScore;
	double survivedScore;
};
#pragma endregion


#pragma region GameSceneDatas
struct LobbyToGameSceneData
{
	int playerNum;
	int idx_t[4];
	string id_t[4];
	int chName[4];

	int Client_idx;

	//int ClientIdx;
	//int playerNum;
	//int idx_[4];
	//string id_[4];
	//int chName[4]; 
	int idx;
	int id;
};

enum class MapDatas
{
	Empty = 0,
	BombCreated_0,
	BombCreated_1,
	BombCreated_2,
	BombCreated_3,
	BombCreated_4,
	BombCreated_5,
	BombCreated_6,
	BombCreated_7,
	BombCreated_8,
	BombCreated_9,
	BombDeleted,
	ItemUnChanged = 21,
	ItemDeleted,
	ItemCreated_Ballon,
	ItemCreated_Potion,
	ItemCreated_Nuclear,
	ItemCreated_Skate,
	BlockUnChanged = 31,
	BlockDeleted,
	BlockMoved,
	BlockCreated
};

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
	int bombNum;
	Direction direction;
	int state;
	int mapData[MAP_WIDTH][MAP_HEIGHT];
};

struct GameToGameRecordSceneData
{
	string id;
	double alivedTime;
	int itemObtainScore;
};
#pragma endregion