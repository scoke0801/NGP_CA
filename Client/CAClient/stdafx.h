// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

// C++ 런타임 헤더 파일입니다.
#include <iostream>
#include <fstream>
#include <chrono>
#include <cassert>
#include <vector>
#include <map>

using namespace std;
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <atlimage.h>
#include <commdlg.h>
#include "Resource.h"

#include "Vector2D.h"

#define CLIENT_WIDTH 1040	//클라이언트 넓이를 지정합니다.
#define CLIENT_HEIGHT 780	//클라이언트 높이를 지정합니다.

#define	MAP_WIDTH 15
#define MAP_HEIGHT 13

// 위치 값을 받아서 맵 상의 좌표값으로 계산하여 반환
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