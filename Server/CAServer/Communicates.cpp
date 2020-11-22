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
	// 데이터 받기(고정 길이)
	int len;
	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);

	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return false;
	}
	else if (retval == 0) return false;

	// 데이터 받기(가변 길이)
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

	// 데이터 보내기(고정 길이)
	retval = send(sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return false;
	}

	// 데이터 보내기(가변 길이)
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
	// 클라이언트 정보 받기
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	Data.Thread_Num++;

	cout << Data.Thread_Num << "번 클라가 접속했습니다." << endl;

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
		// 현재 통신하는 클라이언트의 Scene타입을 받아온다.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
		cout << "받은 값 " << atoi(buffer) << "\n";

=======
		
>>>>>>> Stashed changes
		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::TitleScene:
			//PrcoessTitleScene();
			if (count == 0)
			{
				cout << "게임 시작" << endl;


				// 데이터 받기
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
			
			cout << "게임 시작 가능" << endl;

			// 데이터 보내기
			retval = recv(client_sock, buffer, retval, 0);
			if (retval == SOCKET_ERROR)
=======
			if (count == 1)
>>>>>>> Stashed changes
			{
				cout << "게임 시작 가능" << endl;
				// 데이터 받기
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
			cout << "게임 시작" << endl;

			break;
=======
			
		
>>>>>>> Stashed changes
		case SceneType::GameScene:
			if (count == 2)
			{
				ProcessGameScene();
				// 현재 통신하는 클라이언트의 Scene타입을 받아온다.
				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;

				cout << "씬 번호" << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", x 위치 : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", y 위치 : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", 물풍선 길이 : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", 속도 : " << atoi(buffer);

				if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
				cout << ", 상태 : " << atoi(buffer) << "\n";
				

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

	cout << "[TCP 서버] 클라이언트 종료 : IP 주소 = " << inet_ntoa(clientAddr.sin_addr)
		<< ", 포트 번호 = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}

void ProcessGameScene()
{
}
