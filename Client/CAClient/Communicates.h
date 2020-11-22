#pragma once
#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#pragma comment(lib, "ws2_32")
#include <WinSock2.h> 

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define MAX_FILE_NAME_LAN 256

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void textConvert(const char* msg, wchar_t* ret);

void error_quit(const char* msg);

void error_display(const char* msg);

int recvn(SOCKET s, char* buf, int len, int flag);
<<<<<<< Updated upstream

bool SendFrameData(SOCKET& sock, string& str, int& retval);

=======
 
bool SendFrameData(SOCKET& sock, string& str, int& retval); 
bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
>>>>>>> Stashed changes
