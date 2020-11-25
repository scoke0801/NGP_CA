#pragma once

#include "Datas.h"
#include "Communicates.h"
#include "MapObjects.h"

class GameSceneProcessor
{
private:
	BlockName		m_MapData[MAP_HEIGHT][MAP_WIDTH];
	MapTileType		m_Map[MAP_HEIGHT][MAP_WIDTH];

	vector<CBomb*>  m_Bombs;

	GameSceneProcessor() { InitMap(); }

	GameSceneProcessor(const GameSceneProcessor& other) = delete;
	void operator=(const GameSceneProcessor& other) = delete;

public:
	static GameSceneProcessor* GetInstancec()
	{
		static GameSceneProcessor self;
		return &self;
	}

	bool ProcessGameScene(SOCKET& socket);

public:

	Vector2f GetPositionFromText(const char* text);
	int ConvertoIntFromText(const char* text, const char* token);
	void InitMap();

	// 충돌 처리 계산을 위한 함수들
	bool IsCollideInMap(Vector2f playerPosition);
	bool IsInMap(Vector2f position); // 좌표가 맵 안에 있는지 확인

	// 위치 값을 받아서 맵 상의 좌표값으로 계산하여 반환
	static Vector2D<int> GetCoordinates(Vector2D<float> position, Vector2D<int> size);
	static Vector2D<float> GetPositionCoord(Vector2D<int> coord);
	
	bool IsCollide(Vector2f pos, Vector2i targetIdx);
	RECT GetCollisionRect(Vector2f pos);

	void CreateBomb(Vector2f pos, int power);
};

