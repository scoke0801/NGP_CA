#pragma once

#include "Datas.h"

#pragma warning(disable  : 4996)    // mbstowcs unsafe###
void charToWchar(const char* msg, wchar_t* out);

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg);

// ���� �Լ� ���� ���
void err_display(const char* msg);

// ���� ���� �Լ�
void loadFile(string filename, map<string, string> fileData);
void saveFile(string filename, map<string, string> fileData);

// recv�� �̿��� ����� �����Լ�
int recvn(SOCKET s, char * buf, int len, int flags);

bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval);
bool SendFrameData(SOCKET& sock, string& str, int& retval);

DWORD WINAPI ClientThread(LPVOID arg);

bool ProcessTitleScene(SOCKET& socket, int idx);
bool ProcessGameRecordScene(SOCKET& socket, int idx);
bool ProcessLobbyScene(SOCKET& socket, int Data_n);