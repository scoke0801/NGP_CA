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
enum class Direction
{
	down = 0,
	up,
	left,
	right
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
#define PLAYER_SPEED_LIMIT PlAYER_SPEED * 7
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

	Direction m_Dir;		// 플레이어 방향
	PlayerState	m_State;	// 플레이어 상태
	
	UINT m_Power;			// 물줄기의 길이
	UINT m_MaxBomb;			// 물풍선 최대 개수

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

	void SpeedUp() { m_Vel.x = min(m_Vel.x + PlAYER_SPEED, PLAYER_SPEED_LIMIT), m_Vel.y = min(m_Vel.y + PlAYER_SPEED, PLAYER_SPEED_LIMIT); }
	void PowerUp(int power) { m_Power = min(m_Power + power, POWER_LIMIT); }
	void MoreBomb() { m_MaxBomb += 1; }

	int GetPower() { return m_Power; }
	int GetMaxBomb() { return m_MaxBomb; }
private:
	void LoadImages();
	void LoadSounds();
	void Animate(float timeElapsed);
};

