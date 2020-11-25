#pragma once

#include "Datas.h"
#include "Communicates.h"
#include "MapObjects.h"

class GameSceneProcessor
{
private:
	MapTileType		m_Map[MAP_HEIGHT][MAP_WIDTH];

	CItem*  m_Items[MAP_HEIGHT][MAP_WIDTH];
	CBlock* m_Blocks[MAP_HEIGHT][MAP_WIDTH];
	CBomb*  m_Bombs[MAP_HEIGHT][MAP_WIDTH];

	vector<Vector2i> m_DeletedBlock;
	vector<Vector2i> m_DeletedBomb;
	vector<Vector2i> m_CreatedBomb;

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

	// �浹 ó�� ����� ���� �Լ���
	bool IsCollideToBlock(Vector2f playerPosition);
	bool IsCollideToBomb(Vector2f playerPosition, PlayerState& state);
	bool IsCollideToItem(Vector2f playerPosition);
	bool IsInMap(Vector2f position); // ��ǥ�� �� �ȿ� �ִ��� Ȯ��

	void CreateBomb(Vector2f pos, int power);
	bool IsDestroyedBlock(Vector2i coord);
public:
	// ��ġ ���� �޾Ƽ� �� ���� ��ǥ������ ����Ͽ� ��ȯ
	static bool IsCollide(Vector2f pos, Vector2i targetIdx);
	static RECT GetCollisionRect(Vector2f pos);

	static Vector2D<int> GetCoordinates(Vector2D<float> position, Vector2D<int> size);
	static Vector2D<float> GetPositionCoord(Vector2D<int> coord);
	static bool IsInMapCoord(Vector2D<int> coord);
};

