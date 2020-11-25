#pragma once

class CBomb
{
private:
	Vector2i m_Idx;
	Vector2f m_Position;
	int m_Power;

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
};

