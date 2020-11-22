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

class CBomb;
class CPlayer : public CGameObject
{
private:
	Vector2D<float>	m_PrevPosition;

	CImage m_Images[(int)PlayerImages::count];
	UINT m_ImageIndex[(int)PlayerImages::count];

	Vector2D<int> m_AnimationSizes[(int)PlayerImages::count];
	PlayerImages m_currentAnimation;
	UINT m_AnimationLen[(int)PlayerImages::count];
	int m_AnimationIdx;

	Direction m_Dir;		// �÷��̾� ����
	PlayerState	m_State;	// �÷��̾� ����
	
	UINT m_Power;			// ���ٱ��� ����
	UINT m_MaxBomb;			// ��ǳ�� �ִ� ����
	UINT m_Speed;			// �÷��̾� ���ǵ�

	vector<CBomb*> m_Bombs;	// ���� �÷��̾ ���� ��ǳ��

	float m_TimeSum;

public:
	CPlayer(Vector2D<float> position);
	~CPlayer();

public:
	virtual void Draw(HDC hdc);
	virtual void Update(float timeElapsed);

	virtual bool IsCollide(const CGameObject& other);
	virtual bool IsCollide(CGameObject* other);

	void FixCollision();

public:
	void ChangeState(PlayerState nextState);

	void Move(Direction dir);
	void Stop();
	
	void ConnectBomb(CBomb* bomb) { m_Bombs.push_back(bomb); }
	void EraseBomb(CBomb* bomb) { auto iter = std::find(m_Bombs.begin(), m_Bombs.end(), bomb); m_Bombs.erase(iter); }

	void SpeedUp(); 
	void PowerUp(int power) { m_Power = min(m_Power + power, POWER_LIMIT); }
	void MoreBomb() { m_MaxBomb += 1; }

	int GetPower() { return m_Power; }
	int GetMaxBomb() { return m_MaxBomb; }
	int GetSpeed() { return m_Speed; }
	PlayerState GetState() { return m_State; }
	Direction GetDirection() const {return m_Dir;}

	bool CanCreateBomb();
private:
	void LoadImages();
	void LoadSounds();
	void Animate(float timeElapsed);
};

