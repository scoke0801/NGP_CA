#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

#define SERVERPORT 9000
#define BUFSIZE 4096

#define width 480 
#define height 960

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);

// ���� �Լ� ���� ���
void err_display(const char* msg);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags);

