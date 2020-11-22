#include "stdafx.h"
#include "Communicates.h"

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

bool RecvFrameData(SOCKET& client_sock, char* buf, int& retval)
{
	// ������ �ޱ�(���� ����)
	int len;
	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);

	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return false;
	}
	else if (retval == 0) return false;

	// ������ �ޱ�(���� ����)
	retval = recvn(client_sock, buf, len, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return false;
	}

	buf[retval] = '\0';
	return true;
}

void saveFile(string filename, vector<string> fileData)
{

}

DWORD __stdcall ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;

	int retval=0;

	// Ŭ���̾�Ʈ ���� �ޱ�
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	// +1, null value
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	while (1) {
		// ���� ����ϴ� Ŭ���̾�Ʈ�� SceneŸ���� �޾ƿ´�.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
		cout << "���� �� " << atoi(buffer) << "\n";

		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::LobbyScene:
			//ProcessLobbyScene();
			
			cout << "���� ���� ����" << endl;

			// ������ ������
			retval = recv(client_sock, buffer, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			break;
		case SceneType::TitleScene:
			//PrcoessTitleScene();
			cout << "���� ����" << endl;

			break;
		case SceneType::GameScene:
			ProcessGameScene();
			break;
		case SceneType::GameRecordScene:
			//ProcessGameRecordScene();
			break;
		}
	}


	// closesocket()
	closesocket(client_sock);
	cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}

void ProcessGameScene()
{
}
