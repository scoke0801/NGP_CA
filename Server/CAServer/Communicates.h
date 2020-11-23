#pragma once

#include "Datas.h"

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void charToWchar(const char* msg, wchar_t* out);

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg);

// 소켓 함수 오류 출력
void err_display(const char* msg);

// 파일 저장 함수
void saveFile(string filename, vector<string> fileData);

// recv를 이용한 사용자 정의함수
int recvn(SOCKET s, char* buf, int len, int flags);
bool SendFrameData(SOCKET& sock, string& str, int& retval);
bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);

DWORD WINAPI ClientThread(LPVOID arg);

bool ProcessGameScene(SOCKET& socket);
Vector2f GetPositionFromText(const char* text);
int ConvertoIntFromText(const char* text, const char* token);