#include "stdafx.h"
#include "Button.h"

Button::Button()
{

}

Button::~Button()
{
}

void Button::Init(LPCTSTR fname, Vector2D<float> pos)
{
	image.Load(fname);
	m_Position = pos;
	m_Size = { image.GetWidth(), image.GetHeight() };
}

void Button::Draw(HDC hdc)
{
	image.TransparentBlt(
		hdc, m_Position.x, m_Position.y,
		m_Size.x, m_Size.y,
		0, 0, m_Size.x, m_Size.y,
		RGB(255, 0, 255));
}

void Button::Update(float timeElapsed)
{
}

bool Button::IsClicked(Vector2D<float> mpos)
{
	if ((m_Position.x < mpos.x) && (m_Position.x + m_Size.x > mpos.x) &&
		(m_Position.y < mpos.y) && (m_Position.y + m_Size.y > mpos.y))
		return TRUE;
	else
		return FALSE;
}
