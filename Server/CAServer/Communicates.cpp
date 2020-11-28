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

	
	int retval = 0;
	// 클라이언트 정보 받기
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	Data.Thread_Num++;

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
		if (!RecvFrameData(client_sock, buffer, receivedSize)) return 0;
			cout << "씬 번호 " << atoi(buffer) << endl;

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
			//ProcessGameScene(client_sock, Data.Thread_Num);
			break;

		case SceneType::GameRecordScene:
			//ProcessGameRecordScene();
			break;
		}
		
	}

	Data.Thread_Num--;

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

bool ProcessGameScene(SOCKET& sock, int clientNum)
{
	int receivedSize;
	// +1, null value
	char buffer[BUFSIZE + 1];
	 
	Vector2f position;
	PlayerState playerState;
	int speed;
	Direction direction;
	Vector2f prevPosition;

	for(int i = 0; i < clientNum; ++i)
	{
		if (!RecvFrameData(sock, buffer, receivedSize)) return 0;
		//cout << buffer;
		char* token = strtok(buffer, "\n");
		int playerIndex = 0;
		while (token != NULL)
		{
			if (strstr(token, "<PlayerIndex>:"))
			{
				playerIndex = ConvertoIntFromText(token, "<PlayerIndex>:");
				cout << "<PlayerIndex>: " << playerIndex << " \n";
			}
			else if (strstr(token, "<Position>:"))       
			{
				position = GetPositionFromText(token);
				cout << "x : " << position.x << " y : " <<
					position.y << "\n";
			}
			else if (strstr(token, "<Power>:"))
			{
				cout << "<Power>: " << ConvertoIntFromText(token, "<Power>:") << " \n";
			}
			else if (strstr(token, "<Speed>:"))
			{
				speed = ConvertoIntFromText(token, "<Speed>:");
				cout << "<Speed>: " << speed << " \n";
			}
			else if (strstr(token, "<Direction>:"))
			{
				direction = (Direction)ConvertoIntFromText(token, "<Direction>:");
				cout << "<Direction>: " << (int)direction << " \n";
			}
			else if (strstr(token, "<PlayerState>:"))
			{
				playerState = (PlayerState)ConvertoIntFromText(token, "<PlayerState>:");
				cout << "<PlayerState>: " << (int)playerState << " \n";
			}
			token = strtok(NULL, "\n");
		}

		switch (playerState)
		{
		case PlayerState::move:
			prevPosition = position;
			if (direction == Direction::left)
				position.x = position.x - (speed * PlAYER_SPEED * FPS);
			if (direction == Direction::right)
				position.x = position.x + (speed * PlAYER_SPEED * FPS);
			if (direction == Direction::up)
				position.y = position.y - (speed * PlAYER_SPEED * FPS);
			if (direction == Direction::down)
				position.y = position.y + (speed * PlAYER_SPEED * FPS);

			break;
		case PlayerState::wait:
			break;
		case PlayerState::die:
			break;
		case PlayerState::trap:
			break;
		case PlayerState::live:
			break;
		default:
			break;
		}
		GameSceneSendData sendData;
		sendData.index = playerIndex;
		sendData.position.x = position.x;
		sendData.position.y = position.y;
		sendData.speed = 1;
		sendData.state = playerState;
		sendData.isGameEnd = false;
		//int mapData[width][height]; 

		string toSendData;
		toSendData = "<Position>:";
		toSendData += to_string(sendData.position.x);
		toSendData += " ";
		toSendData += to_string(sendData.position.y);
		toSendData += "\n";

		toSendData += "<PlayerIndex>:";
		toSendData += to_string(sendData.index);

		toSendData += "<Speed>:";
		toSendData += to_string(sendData.speed);
		toSendData += "\n";

		toSendData += "<PlayerState>:";
		toSendData += to_string((int)sendData.state);
		toSendData += "\n";

		toSendData += "<IsGameEnd>:";
		toSendData += to_string(sendData.isGameEnd);
		toSendData += "\n";
		auto res = SendFrameData(sock, toSendData, receivedSize);
	}
}

bool ProcessLobbyScene(SOCKET& sock, int Data_n)
{
	int count = 0;
	int retval = 0;
	char buffer[BUFSIZE + 1];
	string a;
	vector <string> chat_Da(10);
	vector <string> char_Da(10);

	cout << "로비씬" << endl;
	
	// 3 현재 접속한 플레이어의 수를 넘겨준다.
	SendFrameData(sock,to_string(Data_n), retval);

	cout << "현재접속수 " << Data_n << endl;
	
	for (int i = 0; i < Data_n; i++)
	{
		RecvFrameData(sock, buffer, retval);
		
		char_Da.push_back(buffer);
	
		cout << "캐릭터타입_" << Data_n <<":: "<< buffer << endl;

		SendFrameData(sock, char_Da[i], retval);
	}
	
	//// 채팅 데이터를 받는다.
	//for (int i = 0; i < Data_n; ++i)
	//{
	//	RecvFrameData(sock, buffer, retval);
	//	chat_Da.push_back(buffer);
	//	
	//}

	//cout << "채팅데이터" << chat_Da[0] << endl;

	return -1;
}

Vector2f GetPositionFromText(const char* text)
{
	if (strstr(text, "<Position>:"))       //(token, "<position>:"))
	{
		Vector2f position;
		int count = 0;
		for (int i = 11; i < strlen(text); ++i, ++count)
		{
			if (text[i] == ' ')
			{
				char temp[20] = {};
				strncpy(temp, text + 11, count);
				position.x = atof(temp);
				strncpy(temp, text + i, strlen(text) - i);
				position.y = atof(temp);

				return position;
			}
		}
	}
	return { -1,-1 };
}

int ConvertoIntFromText(const char* text, const char* token)
{
	char buf[256];
	ZeroMemory(buf, 256);
	int tokenLen = strlen(token);
	strncpy(buf, text + tokenLen, strlen(text) - tokenLen);
	return atoi(buf);
}

