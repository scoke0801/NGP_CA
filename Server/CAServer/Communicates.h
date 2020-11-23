#pragma once

#include "Datas.h"

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void charToWchar(const char* msg, wchar_t* out);

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);

// ���� �Լ� ���� ���
void err_display(const char* msg);

// ���� ���� �Լ�
void saveFile(string filename, vector<string> fileData);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char* buf, int len, int flags);
bool SendFrameData(SOCKET& sock, string& str, int& retval);
bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);

DWORD WINAPI ClientThread(LPVOID arg);

bool ProcessGameScene(SOCKET& socket);
Vector2f GetPositionFromText(const char* text);
int ConvertoIntFromText(const char* text, const char* token);