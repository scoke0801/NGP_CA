#pragma once
#include "Scene.h"

class CPlayer;
class CSoundManager;
class CItem;
class CBlock;
class CBomb;

enum class MAP_TILE_TYPE
{
	EMPTY = 0,
	BLOCK,
	BOMB,
};
 
class CGameScene : public CScene
{
private:
	int m_ClientIdx = 0;
	string m_ClientID ;
	CPlayer*			m_Players[4];
	//vector<CPlayer*>	m_Players;	// 전체 플레이어
	CPlayer*			m_Player;	// 클라이언트에 조종하는 플레이어

	CItem				*m_Items[MAP_HEIGHT][MAP_WIDTH];
	CBomb				*m_Bombs[MAP_HEIGHT][MAP_WIDTH];
	CBlock				*m_Blocks[MAP_HEIGHT][MAP_WIDTH];

	CSoundManager		*m_SoundManager;

	MAP_TILE_TYPE		m_Map[MAP_HEIGHT][MAP_WIDTH];

	CImage				m_UIImage;
	CImage				m_PlayerArrow;

	Vector2D<int>		m_TileStartPosition;

	bool				m_InitFlag = true;
public:
	CGameScene();
	~CGameScene();

	virtual void SendDataToNextScene(void* pContext);

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock);

	virtual bool ProcessInput(UCHAR* pKeysBuffer);

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void InitMap();
	void LoadImages();

	void CreateBomb();	// 서버에서 처리

	bool CalcNextCoordinates(Vector2D<int>& coord, Direction dir);
};

