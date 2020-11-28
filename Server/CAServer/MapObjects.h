#pragma once
#include "Datas.h"
enum class BombState
{
	Wait = 0,
	Explosion,
	End,
	count
};
enum class Explosion
{
	inPlace = 1,	// ���ڸ�����
	down,
	up,
	left,
	right,
};
#define EXPLOSION_TIME 1.0f 
class CBomb
{
private:
	Vector2i m_Coordinates;
	Vector2f m_Position;
	int m_Power;
	BombState m_State;

	vector<Vector2i> m_LastBranchCoords;	// ��ǳ�� ���� ���� ��ǥ

	// ��ǳ�� �ʱ� ���� ����, �÷��̾ ��ǳ�� ���� �ִ� ���� �ذ� ���� ����
	bool			m_PlayerIsOn;
	Vector2f		m_PlayerPosition;
	int				m_PlayerIndex;

	// �ð� ó���� ���� �����Դϴ�. 
	std::chrono::system_clock::time_point m_CreatedTime;
	std::chrono::duration<double> m_timeElapsed; // �ð��� �󸶳� ������

public:
	CBomb(Vector2D<float> position, int power, int index);

	void TimeUpdate();
	bool IsTimeToExplose();
	bool IsOnExplosion() const { return m_State == BombState::Explosion; }

	void CheckPlayerOut(Vector2f playerPos, int index);

	void ChangeState(BombState state) { m_State = state; }
	BombState GetState() const { return m_State; }

	int GetPower() const { return m_Power; }

	Vector2i GetCoordinate() const { return m_Coordinates; }

	bool CheckDelete() { return m_State == BombState::End; } 

	void SetLastBranchCoords(vector<Vector2i>& coords);
	vector<Vector2i>& GetLastBranchCoords() { return m_LastBranchCoords; }

	bool GetIsPlayerOn()const { return m_PlayerIsOn; }
};

class CBlock
{
private:
	Vector2i m_Coordinates;

	bool m_IsCanDestroy;	// �ı����� ������� ����
	bool m_IsCanMove;	// �̵����� ������� ���� 

public:
	CBlock(Vector2i coord, bool isCanDestroy, bool isCanMove);

	bool GetIsCanDestroy() const { return m_IsCanDestroy; }
	bool GetIsCanMove() const { return m_IsCanMove; }
};

class CItem
{
private:
	Vector2i m_Coordinates;
	Vector2f m_Position;

	ItemName m_Name;
public:
	CItem(ItemName name, Vector2i coord);

	ItemName GetName() const { return m_Name; }
};

class CPlayer
{
private:
	Vector2f m_Position;
	int m_Index;
	PlayerState m_State;
	Direction m_Direction;
	int m_Power;
	int m_Speed;
	int m_BombNum;

	// �ð� ó���� ���� �����Դϴ�. 
	std::chrono::system_clock::time_point m_PrevUpdateTime;
	std::chrono::duration<double> m_TimeElapsed; // �ð��� �󸶳� ������

public:
	CPlayer(Vector2f position, int index, PlayerState state = PlayerState::wait);

	void SetState(PlayerState state) { m_State = state; }
	PlayerState GetState() const { return m_State; }

	void SetPosition(Vector2f position) { m_Position = position; }
	Vector2f GetPosition() const { return m_Position; }

	int GetIndex() const { return m_Index; }

	void SetDirection(Direction dir) { m_Direction = dir; }
	Direction GetDirection() const { return m_Direction; }

	void SetPower(int power) { m_Power = power;}
	int GetPower() const { return m_Power; }
	
	void SetSpeed(int speed) { m_Speed = speed; }
	int GetSpeed() const { return m_Speed; }

	void SetBombNum(int bombNum) { m_BombNum = bombNum; }
	int GetBombNum() const { return m_BombNum; }

	void UpdateElapsedTime();
	double GetElapsedTime() { return m_TimeElapsed.count();  }
};