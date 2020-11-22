// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// SRPG�����.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.
#include "GameObject.h"

Vector2D<int> GetCoordinates(Vector2D<float> position, Vector2D<int> size)
{
	Vector2D<int> coordinate;
	Vector2D<int> TileStartPosition = { 26, 53 };
	coordinate.x = (position.x + size.x * 0.5f - TileStartPosition.x);// / OBJECT_SIZE;
	coordinate.y = (position.y + size.y * 0.5f - TileStartPosition.y);// / OBJECT_SIZE;
	if(coordinate.x > 0)  coordinate.x /= OBJECT_SIZE;
	if (coordinate.y > 0) coordinate.y /= OBJECT_SIZE;

	return coordinate;
}

Vector2D<float> GetPositionCoord(Vector2D<int> coord)
{
	Vector2D<float> position; 
	Vector2D<int> TileStartPosition = { 26, 53 };
	position.x = coord.x * OBJECT_SIZE + TileStartPosition.x;
	position.y = coord.y * OBJECT_SIZE + TileStartPosition.y;
	return position;
}

bool IsInMapCoord(Vector2D<int> coord)
{
	if (coord.x < 0) return false;
	if (coord.x >= MAP_WIDTH) return false;
	if (coord.y < 0) return false;
	if (coord.y >= MAP_HEIGHT) return false;
	return true;
}
