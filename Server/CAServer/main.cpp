#include "stdafx.h"
 
#pragma warning(disable  : 4996)    // mbstowcs unsafe### 
// ���� ���� �Լ�
void saveFile(string filename, vector<string> fileData);

// Ŭ�� �۽��� ���� ������ �÷��̾��� ��ġ ������ �̿��Ͽ� �÷��̾��� ��ġ������ ����
void playerUpdate();

// �浹ó�� ����
void collisionCheck();

enum class Scenes
{
	TitleScene = 0,
	LobbyScene,
	GameRecordScene,

};

class Point2D
{

};

DWORD WINAPI GameSceneThread(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	int retVal;
	SOCKADDR_IN clientAddr;
	int addrLen;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� �ޱ�
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	// �۽��� ������ ����ü
	Point2D Player_position;			// �÷��̾��� ��ġ����
	int Player_waterRange;				// �÷��̾��� ���ݰ��ɹ���
	int Player_speed;					// �÷��̾��� �ӵ�
	int Player_state;					// �÷��̾��� ����
	int mapData[width][height];			// ������
	bool isGameEnd;						// ������ �������� ����

	// ���� ����..


	// closesocket()
	closesocket(client_sock);
	cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;


	return 0;
}

int main(int argc, char* argv[]) {
	int retVal;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { err_quit("socket()"); }

	// bind()
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	retVal = bind(listen_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retVal == SOCKET_ERROR) { err_quit("bind()"); }

	// listen() 
	retVal = listen(listen_sock, SOMAXCONN);
	if (retVal == SOCKET_ERROR) { err_quit("listen()"); }

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientAddr;
	int addrLen;

	HANDLE hThread;

	while (1) {
		// accept()
		addrLen = sizeof(clientAddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientAddr, &addrLen);

		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		cout << "\n[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
			<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

		// ������ ����
		hThread = CreateThread(NULL, 0, GameSceneThread, (LPVOID)client_sock, 0, NULL);


		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);


	WSACleanup();
	return 0;
}  
void charToWchar(const char* msg, wchar_t* out)
{
	mbstowcs(out, msg, strlen(msg) + 1);//Plus null
}
void err_quit(const char* msg)
{
	wchar_t wMsg[20]; 
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);  

	charToWchar(msg, wMsg);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, wMsg, MB_ICONERROR); 
	LocalFree(lpMsgBuf);
	exit(1);

}

void err_display(const char* msg)
{

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);

}

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR) { return SOCKET_ERROR; }
		else if (received == 0) { break; }

		left -= received;
		ptr += received;
	}

	return (len - left);
}

void saveFile(string filename, vector<string> fileData)
{

}

void playerUpdate()
{

}

void collisionCheck()
{

}
