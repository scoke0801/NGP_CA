#pragma once
class Button
{
private:
	CImage image;

	Vector2D<float> m_Position;	// 위치
	Vector2D<int>	m_Size;		// 크기

public:
	Button();
	~Button();

public:
	void Init(LPCTSTR fname, Vector2D<float> pos);
	void Draw(HDC hdc);
	void Update(float timeElapsed);

	virtual bool IsClicked(Vector2D<float> mpos);
};

