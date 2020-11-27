#pragma once
#include "GameObject.h"
enum class PlayerImages
{
	down = 0,
	up,
	left,
	right,
	trap,
	die,
	live,
	count,
};
enum class PlayerState
{
	wait = 0,
	move,
	trap = 4,
	die,
	live,
};

#define PlAYER_SPEED 52.0f
#define POWER_LIMIT 6
#define PLAYER_SPEED_LIMIT 7

enum class PlayerName
{
	Bazzi,
	Dao
};
class CBomb;
class CPlayer : public CGameObject
{
private:
	Vector2D<float>	m_PrevPosition;

	CImage m_Images[(int)PlayerImages::count];
	UINT m_ImageIndex[(int)PlayerImages::count];
	CImage m_PortraitImage;		// uiâ�� ����� �̹���

	Vector2D<int> m_AnimationSizes[(int)PlayerImages::count];
	PlayerImages m_currentAnimation;
	UINT m_AnimationLen[(int)PlayerImages::count];
	int m_AnimationIdx;

	Direction m_Dir;		// �÷��̾� ����
	PlayerState	m_State;	// �÷��̾� ����
	
	UINT m_Power;			// ���ٱ��� ����
	UINT m_MaxBomb;			// ��ǳ�� �ִ� ����
	UINT m_Speed;			// �÷��̾� ���ǵ�
	UINT m_Idx;				// ���° �÷��̾�����
	string m_ID;			// �÷��̾� ID

	int m_CurrentBombNum; // ���� �÷��̾ ���� ��ǳ�� ��

	float m_TimeSum;
	bool m_BoomCreateFlag; // �������� ��ǳ�� ���� ����
public:
	CPlayer(Vector2D<float> position, PlayerName name = PlayerName::Bazzi);
	~CPlayer();

public:
	virtual void Draw(HDC hdc);
	void DrawPlayerInfo(HDC hdc);
	virtual void Update(float timeElapsed);

	virtual bool IsCollide(const CGameObject& other);
	virtual bool IsCollide(CGameObject* other);

	void FixCollision();

public:
	void ChangeState(PlayerState nextState);

	void Move(Direction dir);
	void Stop();
	
	//void ConnectBomb(CBomb* bomb) { m_Bombs.push_back(bomb); }
	//void EraseBomb(CBomb* bomb) { auto iter = std::find(m_Bombs.begin(), m_Bombs.end(), bomb); m_Bombs.erase(iter); }

	Vector2i GetCoordinate() const { return GetCoordinates(m_Position, {OBJECT_SIZE, OBJECT_SIZE }); }
	
	void SpeedUp(); 
	void PowerUp(int power) { m_Power = min(m_Power + power, POWER_LIMIT); }
	void MoreBomb() { m_MaxBomb += 1; }

	int GetPower() { return m_Power; }
	void SetPower(int power) { m_Power = power; }

	int GetMaxBomb() { return m_MaxBomb; }
	void SetBombNum(int bombNum) { m_MaxBomb = bombNum; }
	void IncreaseCurrentBombNum() { m_CurrentBombNum++; }
	void DecreaseCurrentBombNum() { m_CurrentBombNum--; }

	int GetSpeed() { return m_Speed; }
	void SetSpeed(int speed) { m_Speed = speed; }

	PlayerState GetState() { return m_State; }
	
	Direction GetDirection() const {return m_Dir;}
	void SetDirection(Direction dir);// { m_Dir = dir; }
	
	UINT GetIndex() const { return m_Idx; }
	string GetID() const { return m_ID; }

	void SetIndex(UINT idx) { m_Idx = idx; }
	void SetID(string id) { m_ID = id; }

	bool IsAlive() const { return m_State != PlayerState::die; }

	bool CanCreateBomb();
	void SetCreateBombFlag(bool flag) { m_BoomCreateFlag = flag; }
	bool GetCreateBombFlag() { return m_BoomCreateFlag; }

private:
	void LoadImages(PlayerName name);
	void LoadSounds();
	void Animate(float timeElapsed);
};

