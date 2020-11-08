// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

// C++ ��Ÿ�� ��� �����Դϴ�.
#include <iostream>
#include <fstream>
#include <chrono>
#include <cassert>
#include <vector>
#include <map>

using namespace std;
// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#include <atlimage.h>
#include <commdlg.h>
#include "Resource.h"

#include "Vector2D.h"

#define CLIENT_WIDTH 1040	//Ŭ���̾�Ʈ ���̸� �����մϴ�.
#define CLIENT_HEIGHT 780	//Ŭ���̾�Ʈ ���̸� �����մϴ�.

#define	MAP_WIDTH 15
#define MAP_HEIGHT 13

// ��ġ ���� �޾Ƽ� �� ���� ��ǥ������ ����Ͽ� ��ȯ
Vector2D<int> GetCoordinates(Vector2D<float> position, Vector2D<int> size);
Vector2D<float> GetPositionCoord(Vector2D<int> coord);

bool IsInMapCoord(Vector2D<int> coord);
#define SAFE_DELETE(p) { if(p) {delete p;} (p) = nullptr ;}

enum class Direction
{
	down = 0,
	up,
	left,
	right
};