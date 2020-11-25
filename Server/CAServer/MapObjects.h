#pragma once

class CBomb
{
private:
	Vector2i m_Idx;
	Vector2f m_Position;
	int m_Power;

	vector<Vector2i> m_LastBranchCoords;	// 물풍선 폭발 범위 좌표

	// 물풍선 초기 생성 이후, 플레이어가 물풍선 위에 있는 문제 해결 위한 변수
	bool			m_PlayerIsOn;
	Vector2f		m_PlayerPosition;

	// 시간 처리를 위한 변수입니다. 
	std::chrono::system_clock::time_point m_CreatedTime;
	std::chrono::duration<double> m_timeElapsed; // 시간이 얼마나 지났나

public:
	CBomb(Vector2D<float> position, int power);

	void TimeUpdate();
};

