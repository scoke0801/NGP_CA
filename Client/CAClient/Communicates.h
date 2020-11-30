#pragma once
#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지

#pragma comment(lib, "ws2_32")
#include <WinSock2.h> 
  
//#define SERVERIP   "127.0.0.1"		 // 루프백
//#define SERVERIP   "192.168.35.118"
 #define SERVERIP   "211.207.24.227"// 종현
//#define SERVERIP	 "192.168.35.229"
//#define SERVERIP   "192.168.0.15"  // 영진 
#define SERVERPORT 9000
#define MAX_FILE_NAME_LAN 256

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void textConvert(const char* msg, wchar_t* ret);

void error_quit(const char* msg);

void error_display(const char* msg);

int recvn(SOCKET s, char* buf, int len, int flag);

bool SendFrameData(SOCKET& sock, string& str, int& retval);
bool RecvFrameData(SOCKET& sock, char* buf, int& retval);

Vector2f GetPositionFromText(const char* text);
void GetCoordsFromText(const char* token, int num, vector<Vector2i>& coords);
void GetCoordsFromText(const char* token, int num, vector<int>& names, vector<Vector2i>& coords);
void GetPlayerInfoFromText(const char* token, int num, vector<int>& indices, vector<int>& states, vector<Vector2f>& positions);
int ConvertoIntFromText(const char* text, const char* token);
