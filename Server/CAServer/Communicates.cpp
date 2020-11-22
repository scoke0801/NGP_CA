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

bool const SendFrameData(SOCKET& sock, string& str, int& retval)
{
	int len = str.length();

	// ������ ������(���� ����)
	retval = send(sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return false;
	}

	// ������ ������(���� ����)
	retval = send(sock, str.c_str(), len, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return false;
	}
	return true;
}

void saveFile(string filename, vector<string> fileData)
{

}
LobbySceneSendData Data;

DWORD __stdcall ClientThread(LPVOID arg)
{
	
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;
<<<<<<< Updated upstream

	int retval=0;

=======
	 
	vector<string> toSendData;
	int retval=0; 
>>>>>>> Stashed changes
	// Ŭ���̾�Ʈ ���� �ޱ�
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	Data.Thread_Num++;

	cout << Data.Thread_Num << "�� Ŭ�� �����߽��ϴ�." << endl;

	toSendData.emplace_back(to_string(Data.Thread_Num));

	for (int i = 0; i < toSendData.size(); ++i)
	{
		SendFrameData(client_sock, toSendData[i], retval);
	}

	// +1, null value
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	int count = 0;
	while (1) {
<<<<<<< Updated upstream
		// ���� ����ϴ� Ŭ���̾�Ʈ�� SceneŸ���� �޾ƿ´�.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
		cout << "���� �� " << atoi(buffer) << "\n";

=======
		
>>>>>>> Stashed changes
		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::TitleScene:
			//PrcoessTitleScene();
			if (count == 0)
			{
				cout << "���� ����" << endl;


				// ������ �ޱ�
				retval = recv(client_sock, buffer, retval, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					break;
				}
				count++;
				break;
			}

		case SceneType::LobbyScene:
			//ProcessLobbyScene();
<<<<<<< Updated upstream
			
			cout << "���� ���� ����" << endl;

			// ������ ������
			retval = recv(client_sock, buffer, retval, 0);
			if (retval == SOCKET_ERROR)
=======
			if (count == 1)
>>>>>>> Stashed changes
			{
				cout << "���� ���� ����" << endl;
				// ������ �ޱ�
				retval = recv(client_sock, buffer, retval, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send()");
					break;
				}
				cout << retval << endl;
				count++;

				
				break;
			}
<<<<<<< Updated upstream

			break;
		case SceneType::TitleScene:
			//PrcoessTitleScene();
			cout << "���� ����" << endl;

			break;
=======
			
		
>>>>>>> Stashed changes
		case SceneType::GameScene:
			if (count == 2)
			{
				ProcessGameScene();
				// ���� ����ϴ� Ŭ���̾�Ʈ�� SceneŸ���� �޾ƿ´�.
				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;

				cout << "�� ��ȣ" << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", x ��ġ : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", y ��ġ : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", ��ǳ�� ���� : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", �ӵ� : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", ���� : " << atoi(buffer) << "\n";
				

				break;
			}
		case SceneType::GameRecordScene:
			//ProcessGameRecordScene();
			break;
		}
		
	
		break;
	}

	Data.Thread_Num--;
	// closesocket()
	closesocket(client_sock);

	cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}

void ProcessGameScene()
{
}
