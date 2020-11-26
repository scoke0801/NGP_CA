#include "stdafx.h"
#include "Communicates.h"
#include "GameSceneProcessor.h"
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
int recvn(SOCKET s, char * buf, int len, int flags)
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
bool SendFrameData(SOCKET& sock, string& str, int& retval)
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

void saveFile(string filename, vector<string> fileData)
{
}

LobbySceneSendData Data;

DWORD __stdcall ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;
	int idx = Data.Thread_Num;
	int retval = 0;

	// 클라이언트 정보 받기
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	Data.Thread_Num++;
	GameSceneProcessor::GetInstance()->IncreaseClientNum();
	cout << "Increased GameSceneProcessor::ClientNum : "
		<< GameSceneProcessor::GetInstance()->GetClientNum() << "\n";

	// 임시코드
	GameSceneProcessor::GetInstance()->InitPlayers();
	cout << Data.Thread_Num << "번 클라가 접속했습니다." << endl;

	// +1, null value
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	int count = 0;

	// 계정정보 파일 읽어오기
	  map<string, string> accounts;
	  ifstream in("data/Account.txt"s);
	  if (in) {
		  string id, pw;
		  while (!in.eof()) {
			  in >> id >> pw;
			  accounts.insert(pair<string, string>(id, pw));
		  }
	  }
	  in.close();

	while (1) {
		// 2 현재 통신하는 클라이언트의 Scene타입을 받아온다.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) break;
		//cout << "씬 번호" << atoi(buffer);
		//if (strcmp(buffer, "Quit")) break;
		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::TitleScene:
			ProcessTitleScene(client_sock, accounts); 
			break;

		case SceneType::LobbyScene:
			ProcessLobbyScene(client_sock,Data.Thread_Num);
			break;

		case SceneType::GameScene:
			GameSceneProcessor::GetInstance()->ProcessGameScene(client_sock);
			break;

		case SceneType::GameRecordScene:
			//ProcessGameRecordScene();
			break;
		default:
		{ 
			int retval = 0;  

			 // 3 현재 접속한 플레이어의 수를 넘겨준다.
			string temp = to_string(idx);
			temp += " ";
			temp += to_string((int)GetCurrentThreadId()); 
			
			int res = GetCurrentThreadId();
			SendFrameData(client_sock, temp, retval);
		}
			break;
		}
	}

	Data.Thread_Num--;
	GameSceneProcessor::GetInstance()->DecreaseClientNum();
	cout << "Decreased GameSceneProcessor::ClientNum : "
		<< GameSceneProcessor::GetInstance()->GetClientNum() << "\n";
	// closesocket()
	closesocket(client_sock);

	cout << "[TCP 서버] 클라이언트 종료 : IP 주소 = " << inet_ntoa(clientAddr.sin_addr)
		<< ", 포트 번호 = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}

bool ProcessTitleScene(SOCKET& sock, map<string, string> filedata)
{
	int retval;
	int receivedSize;

	char buffer[BUFSIZE + 1];

	TitleSceneRecvData player;
	TitleSceneSendData check;

	map<string, string> accounts = filedata;

	if (!RecvFrameData(sock, buffer, receivedSize)) return 0;
	cout << "플레이어 ID : " << buffer;
	player.id = buffer;

	if (!RecvFrameData(sock, buffer, receivedSize)) return 0;
	cout << ", 플레이어 PW : " << buffer;
	player.pw = buffer;

	if (!RecvFrameData(sock, buffer, receivedSize)) return 0;
	cout << ", IsNew : " << buffer << "\n";
	player.isNew = (bool)buffer;

	if (player.isNew == TRUE) {
		ofstream out;
		out.open("data/Account.txt"s, ios::app);

		auto isAdded = accounts.insert(pair<string, string>(player.id, player.pw));
		if (isAdded.second == TRUE) {
			out << player.id << " " << player.pw << endl;
			cout << "Account registration completed!\n";
			check.result = TRUE;
		}
		else {
			cout << "This ID already exists.\n";
			check.result = FALSE;
		}

		out.close();
	}
	else {
		map<string, string>::iterator data;
		data = accounts.find(player.id);
		if (data == accounts.end())
			check.result = FALSE;
		else {
			if (data->second == player.pw)
				check.result = TRUE;
			else
				check.result = FALSE;
		}
	}

	string data = to_string(check.result);
	SendFrameData(sock, data, retval);
}

bool ProcessLobbyScene(SOCKET& sock, int Data_n)
{
	int count = 0;
	int retval = 0;
	char buffer[BUFSIZE + 1];
	vector <string> chat_Da;

	cout << "로비씬" << endl;
	
	// 3 현재 접속한 플레이어의 수를 넘겨준다.
	string temp = to_string(Data_n);
	SendFrameData(sock, temp, retval);

	cout << "현재접속수" << Data_n << endl;

	// 채팅 데이터를 받는다.
	for (int i = 0; i < Data_n; ++i)
	{
		RecvFrameData(sock, buffer, retval);
		chat_Da.push_back(buffer);
		
	}

	cout << "채팅데이터" << chat_Da[0] << endl;

	return 0;
}