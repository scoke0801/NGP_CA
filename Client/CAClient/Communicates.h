#pragma once
#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#pragma comment(lib, "ws2_32")
#include <WinSock2.h> 

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void textConvert(const char* msg, wchar_t* ret);

void error_quit(const char* msg);

void error_display(const char* msg);

int recvn(SOCKET s, char* buf, int len, int flag);

