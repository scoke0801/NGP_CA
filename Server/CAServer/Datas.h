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
	string cr_data;
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
	int playerIndex;
	string text;
	bool result;
};
#pragma endregion 

// 여기를 채워 주세요
#pragma region GameRecordSceneDatas;
struct GameRecordSceneRecvData
{
	string id;
	int itemScore;
	double survivedScore;
};
struct GameRecordSceneSendData
{
	string id;
	int itemScore;
	int survivedScore;
};
#pragma endregion


#pragma region GameSceneDatas

#define PlAYER_SPEED 26.0f
#define OBJECT_SIZE 52
#define MAX_BOMB_NUM 8
#define POWER_LIMIT 6
#define PLAYER_SPEED_LIMIT 10
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
enum class BlockName
{
	EMPTY = 0,
	BLOCK_YELLOW = 1,
	BLOCK_RED,
	HOUSE_YELLOW,
	HOUSE_RED,
	HOUSE_BLUE,
	BOX,
	TREE,
};

enum class ItemName
{
	ballon = 0,
	potion,
	nuclear,
	skate,
	count
};
enum class CharacterName
{
	Bazzi = 0,
	Dao
};
enum class MapTileType
{
	EMPTY = 0,
	BLOCK,
	BOMB,
	ITEM
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
	int playerIndex;
	Vector2f position;
	int power;
	int speed; 
	int bombNum;
	Direction direction;
	PlayerState state;
};
struct GameSceneSendData
{ 
	UINT index;
	Vector2f position;
	int speed;
	PlayerState state;
	int mapData[MAP_HEIGHT][MAP_WIDTH];
	bool isGameEnd; 
	bool bombCreateFlag;
};
#pragma endregion