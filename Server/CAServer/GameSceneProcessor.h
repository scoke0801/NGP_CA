#pragma once

#include "Datas.h"
#include "Communicates.h"
#include "MapObjects.h"

#define MAX_CLIENT_NUM 4

class GameSceneProcessor
{
private:
	MapTileType			m_Map[MAP_HEIGHT][MAP_WIDTH];

	CItem* m_Items[MAP_HEIGHT][MAP_WIDTH];
	CBlock* m_Blocks[MAP_HEIGHT][MAP_WIDTH];
	CBomb* m_Bombs[MAP_HEIGHT][MAP_WIDTH];
	CPlayer* m_Players[MAX_CLIENT_NUM];

	vector<Vector2i>	m_DeletedBlock;
	vector<Vector2i>	m_DeletedBomb;
	vector<Vector2i>	m_CreatedBomb;
	vector<Vector2i>	m_DeletedItem;
	vector<Vector2i>	m_CreatedItem;
	bool				m_Communicated[4];

	int					m_ClientNum = 0;
	int					m_SendCount = 0;

	CRITICAL_SECTION	m_cs;
	GameSceneProcessor()
	{
		InitializeCriticalSection(&m_cs);
			ZeroMemory(m_Players, sizeof(m_Players));
		ZeroMemory(m_Communicated, sizeof(m_Communicated));
		InitMap();
	}
	~GameSceneProcessor() { DeleteCriticalSection(&m_cs); }

	GameSceneProcessor(const GameSceneProcessor& other) = delete;
	void operator=(const GameSceneProcessor& other) = delete;

public:
	static GameSceneProcessor* GetInstance()
	{
		static GameSceneProcessor self;
		return &self;
	}

	bool ProcessGameScene(SOCKET& socket);

public:
	Vector2f GetPositionFromText(const char* text);
	int ConvertoIntFromText(const char* text, const char* token);
	
	void InitMap();
	void InitPlayers();

	// 충돌 처리 계산을 위한 함수들
	bool IsCollideToBlock(Vector2f playerPosition);
	bool IsCollideToBomb(Vector2f playerPosition, PlayerState& state);
	bool IsCollideToItem(Vector2f playerPosition);
	bool IsInMap(Vector2f position); // 좌표가 맵 안에 있는지 확인

	void CreateBomb(Vector2f pos, int power, int index);
	bool IsDestroyedBlock(Vector2i coord);
public:
	// 위치 값을 받아서 맵 상의 좌표값으로 계산하여 반환
	static bool IsCollide(Vector2f pos, Vector2i targetIdx);
	static RECT GetCollisionRect(Vector2f pos);

	static Vector2D<int> GetCoordinates(Vector2D<float> position, Vector2D<int> size);
	static Vector2D<float> GetPositionCoord(Vector2D<int> coord);
	static bool IsInMapCoord(Vector2D<int> coord);

public:
	int GetClientNum() const { return m_ClientNum; }
	void SetClientNum(int num) { m_ClientNum = num; }
	void IncreaseClientNum() { m_ClientNum++; }
	void DecreaseClientNum() { m_ClientNum--; }
};

