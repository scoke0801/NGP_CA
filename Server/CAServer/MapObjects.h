#pragma once

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

	// �ð� ó���� ���� �����Դϴ�. 
	std::chrono::system_clock::time_point m_CreatedTime;
	std::chrono::duration<double> m_timeElapsed; // �ð��� �󸶳� ������

public:
	CBomb(Vector2D<float> position, int power);

	void TimeUpdate();
	bool IsTimeToExplose();
	bool IsOnExplosion() const { return m_State == BombState::Explosion; }

	void ChangeState(BombState state) { m_State = state; }
	BombState GetState() const { return m_State; }

	int GetPower() const { return m_Power; }

	Vector2i GetCoordinate() const { return m_Coordinates; }

	bool CheckDelete() { return m_State == BombState::End; } 
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

public:
	CItem();

};