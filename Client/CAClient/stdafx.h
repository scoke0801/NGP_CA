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

#define CLIENT_WIDTH 1040	//Ŭ���̾�Ʈ ���̸� �����մϴ�.
#define CLIENT_HEIGHT 780	//Ŭ���̾�Ʈ ���̸� �����մϴ�.

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

 //�ִ� FPS
#define MAX_FPS 1.0 / 60.0
//#if _DEBUG
//#define MAX_FPS 0.0
//#else
//#define MAX_FPS 1.0 / 60.0
//#endif
//�������� ���������� �ִ� ��� ������ �� �������� �����մϴ�.
#define MAX_LOOP_TIME 50
