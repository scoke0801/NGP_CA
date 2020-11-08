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

#define CLIENT_WIDTH 1040	//클라이언트 넓이를 지정합니다.
#define CLIENT_HEIGHT 780	//클라이언트 높이를 지정합니다.

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

 //최대 FPS
#define MAX_FPS 1.0 / 60.0
//#if _DEBUG
//#define MAX_FPS 0.0
//#else
//#define MAX_FPS 1.0 / 60.0
//#endif
//프레임을 따라잡기까지 최대 몇번 루프를 돌 것인지를 지정합니다.
#define MAX_LOOP_TIME 50
