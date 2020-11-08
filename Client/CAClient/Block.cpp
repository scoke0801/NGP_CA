#include "stdafx.h"
#include "Block.h"

CBlock::CBlock(BlockName name, Vector2D<float> position, bool isCanDestroy)
{
	m_Name = name;
	m_Position = position;
	m_Size = { OBJECT_SIZE,  OBJECT_SIZE };
	m_IsCanCollide = true;	
	m_IsCanDestroy = isCanDestroy;
	m_State = BlockState::Wait;

	m_AnimationSizes = {40, 52};
	m_AnimationIdx = 0;
	m_AnimationLen = 6;
	switch (name)
	{
	case BlockName::EMPTY:
		break;
	case BlockName::BLOCK_YELLOW:
		m_Image.Load(_T("assets/map/TownBlockYellow.bmp"));
		break;
	case BlockName::BLOCK_RED:
		m_Image.Load(_T("assets/map/TownBlockRed.bmp"));
		break;
	case BlockName::HOUSE_BLUE:
		m_Image.Load(_T("assets/map/TownHouseBlue.png"));
		break;
	case BlockName::HOUSE_RED:
		m_Image.Load(_T("assets/map/TownHouseRed.png"));
		break;
	case BlockName::HOUSE_YELLOW:
		m_Image.Load(_T("assets/map/TownHouseYellow.png"));
		break;
	case BlockName::BOX:
		m_Image.Load(_T("assets/map/TownBox.bmp"));
		break;
	case BlockName::TREE:
		m_Image.Load(_T("assets/map/TownTree.png"));
		m_AnimationSizes.y = 67;
		break; 
	default:
		assert(!"잘못된 블록 형식!");
		break;
	}
}

CBlock::~CBlock()
{
}

void CBlock::Draw(HDC hdc)
{
	if (m_Name == BlockName::EMPTY) return;
	if (m_State == BlockState::End) return;
	//DrawRect(hdc); return;
	m_Image.TransparentBlt(hdc,
		m_Position.x, m_Position.y, 
		m_Size.x , m_Size.y,
		m_AnimationIdx * m_AnimationSizes.x, 0,
		m_AnimationSizes.x, m_AnimationSizes.y,
		RGB(255, 0, 255));
}

void CBlock::Update(float timeElapsed)
{
	if (m_Name == BlockName::EMPTY) return;
	if (m_State == BlockState::Wait) return;

	Animate(timeElapsed);
}

bool CBlock::IsCanDestroy()
{
	if (m_State != BlockState::Wait) return false;
	return m_IsCanDestroy;
}

void CBlock::Animate(float timeElapsed)
{
	if (m_State == BlockState::Wait) return;

	m_TimeSum += timeElapsed;

	if (m_TimeSum < 0.1f) return;
	m_TimeSum = 0.0f;

	++m_AnimationIdx;
	if (m_AnimationIdx >= m_AnimationLen)
	{
		ChangeState(BlockState::End);
	}
}
