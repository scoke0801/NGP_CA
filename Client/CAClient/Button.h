#pragma once
class Button
{
private:
	CImage image;

	Vector2D<float> m_Position;	// ��ġ
	Vector2D<int>	m_Size;		// ũ��

public:
	Button();
	~Button();

public:
	void Init(LPCTSTR fname, Vector2D<float> pos);
	void Draw(HDC hdc);
	void Update(float timeElapsed);

	virtual bool IsClicked(Vector2D<float> mpos);
};

