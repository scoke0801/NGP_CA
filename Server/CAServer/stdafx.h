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

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg);

// 소켓 함수 오류 출력
void err_display(const char* msg);

// recv를 이용한 사용자 정의함수
int recvn(SOCKET s, char* buf, int len, int flags);

