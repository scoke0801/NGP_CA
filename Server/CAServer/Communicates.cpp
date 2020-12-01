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
void saveFile(string filename, map<string, string> fileData)
{
	ofstream out;
	map<string, string> data = fileData;

	out.open(filename, ios::app);

	for (auto player : data) {
		out << player.first << " " << player.second << endl;
	}

	out.close();
}

LobbySceneSendData Data;

int g_pIndex = 0; // 플레이어 번호

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
	
	// +1, null value
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	
	while (1) {
		static std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
		static std::chrono::duration<double> timeElapsed;

		timeElapsed = std::chrono::system_clock::now() - currentTime;
		currentTime = std::chrono::system_clock::now();
		//cout << "TimeElapsed: " << timeElapsed.count() << " \n";

		// 2 현재 통신하는 클라이언트의 Scene타입을 받아온다.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) break;
		//cout << "씬 번호" << atoi(buffer);
		//if (strcmp(buffer, "Quit")) break;
		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::TitleScene:
			if (ProcessTitleScene(client_sock, g_pIndex)) {
				cout << "접속한 플레이어 인덱스: " << g_pIndex << endl;
				g_pIndex++;
			}
			break;

		case SceneType::LobbyScene:
			ProcessLobbyScene(client_sock, idx, GameSceneProcessor::GetInstance()->GetClientNum());
			break;

		case SceneType::GameScene:
			GameSceneProcessor::GetInstance()->ProcessGameScene(client_sock);
			break;

		case SceneType::GameRecordScene:
			ProcessGameRecordScene(client_sock, idx);
			break;
		default:
		{ 
			int retval = 0;  

			 // 3 현재 접속한 플레이어의 수를 넘겨준다.
			string temp = to_string(idx);
			temp += " ";

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

bool ProcessTitleScene(SOCKET& sock,int idx)
{
	static int idx_ = 0;
	int retval;
	char buffer[BUFSIZE + 1];
	bool isLogin = FALSE;			//로그인 성공 여부에 따른 리턴값

	TitleSceneRecvData recvData;	//수신받은 데이터
	TitleSceneSendData sendData;	//송신할 데이터
	map<string, string> accounts;	//파일에서 읽어올 계정정보들
	string data;					//데이터 복사를 위한 공간
	sendData.playerIndex = -1;		//로그인이 아닌 경우나 로그인 실패시 -1로 전송, 로그인 성공시 0~3

	// 계정 정보 읽어오기
	ifstream in("data/Account.txt"s);
	if (in) {
		string id, pw;
		while (!in.eof()) {
			in >> id >> pw;
			accounts.insert(pair<string, string>(id, pw));
		}
	}
	in.close();

	RecvFrameData(sock, buffer, retval);
	data = buffer;

	//받아온 계정 정보를 나누어 recvData의 각 변수에 할당
	recvData.id = data.substr(data.find("<ID>")+4, data.find("<PW>") - (data.find("<ID>")+4));
	recvData.pw = data.substr(data.find("<PW>") + 4, data.find("<isNew>") - (data.find("<PW>") + 4));
	recvData.isNew = (bool)data[data.find("<isNew>") + 7];

	//확인용 출력
	cout << "[ID]:" << recvData.id << " [PW]:" << recvData.pw << " [isNew]:" << recvData.isNew << endl;

	if (recvData.isNew == TRUE) {
	//신규 계정 등록인 경우
		ofstream out;
		out.open("data/Account.txt"s, ios::app);

		auto isAdded = accounts.insert(pair<string, string>(recvData.id, recvData.pw));
		if (isAdded.second == TRUE) {
		//계정 등록 성공
			cout << ">> " << isAdded.second << endl;
			out << recvData.id << " " << recvData.pw << endl;
			sendData.text = "Account registration completed!";
			sendData.result = TRUE;
		}
		else {
		//계정 등록 실패
			cout << ">> " << isAdded.second << endl;
			sendData.text = "This ID already exists.";
			sendData.result = FALSE;
		}

		out.close();
	}
	else {
	//계정 등록이 아닌 로그인일 경우
		map<string, string>::iterator find;
		find = accounts.find(recvData.id);
		if (find == accounts.end()) {
		//아이디를 찾지 못한 경우
			sendData.result = FALSE;
			sendData.text = "Wrong Account.";
		}
		else {
		//아이디를 찾은 경우
			if (find->second == recvData.pw) {
			//비밀번호 일치 = 로그인 성공
				sendData.text = "Login Accept!";
				sendData.playerIndex = idx_++;
				sendData.result = TRUE;
				isLogin = TRUE;
			}
			else {
			//비밀번호 불일치 = 로그인 실패
				sendData.text = "Login Fail...";
				sendData.result = FALSE;
			}
		}
	}

	//확인용 출력
	cout << "[result]:" << sendData.result << " [TEXT]:" << sendData.text << endl;

	data.clear();
	
	//송신할 데이터 생성
	data = "<INDEX>";
	data += to_string(sendData.playerIndex);
	data += "<TEXT>";
	data += sendData.text;
	data += "<result>";
	data += sendData.result;

	SendFrameData(sock, data, retval);

	return isLogin;
}

bool ProcessGameRecordScene(SOCKET& socket, int idx)
{
	int retval;
	char buffer[BUFSIZE + 1];

	GameRecordSceneRecvData recvData;			//수신받은 데이터
	vector<GameRecordSceneSendData> ranking;	//파일에서 읽어올 점수 정보들
	string data;								//데이터 복사를 위한 공간

	RecvFrameData(socket, buffer, retval);
	data = buffer;

	//받아온 계정 정보를 나누어 recvData의 각 변수에 할당
	recvData.id = data.substr(data.find("<ID>") + 4, data.find("<ITS>") - (data.find("<ID>") + 4));
	recvData.itemScore = atoi(data.substr(data.find("<ITS>") + 5).c_str());
	recvData.survivedScore = atof(data.substr(data.find("<SVS>") + 5).c_str());

	//확인용 출력
	cout << "[ID]: " << recvData.id << " [ITS]: " << recvData.itemScore << " [SVS]: " << recvData.survivedScore << endl;

	EnterCriticalSection(&(GameSceneProcessor::GetInstance()->m_cs));
	ofstream out;
	out.open("data/Score.txt"s, ios::app);
	out << recvData.id << " " << recvData.itemScore << " " << recvData.survivedScore << endl;
	out.close();
	LeaveCriticalSection(&(GameSceneProcessor::GetInstance()->m_cs));

	ranking.clear();
	ifstream in("data/Score.txt"s);
	if (in) {
		string id;
		int itscore;
		double svscore;
		while (!in.eof()) {
			in >> id >> itscore >> svscore;
			ranking.push_back({ id, itscore, (int)svscore * 10 });
		}
	}
	ranking.pop_back();

	in.close();
	//아이템 점수 + 생존 점수 합으로 내림차순 정렬
	sort(ranking.begin(), ranking.end(),
		[](const GameRecordSceneSendData& a, const GameRecordSceneSendData& b) {
			return (a.itemScore + a.survivedScore) > (b.itemScore + b.survivedScore); });

	data.clear();

	//상위 점수 10개 목록 송신
	for (int i = 0; i < 10; i++) {
		data = "<ID>";
		data += ranking[i].id;
		data += "<ITS>";
		data += to_string(ranking[i].itemScore);
		data += "<SVS>";
		data += to_string(ranking[i].survivedScore);
		SendFrameData(socket, data, retval);
		cout << ranking[i].id << " " << ranking[i].itemScore << " " << ranking[i].survivedScore << endl;
		data.clear();
	}
	cout << "송신 끝=============" << endl;

	return 0;
}

class LobbyScene
{
public:
	int playerTypes[3]; // size 3으로 고정
	int index[3];
	int Ready[3];
	string id[3];
	string chatting[3];
	int gamestart;
};

LobbyScene lobbyScene;

bool ProcessLobbyScene(SOCKET& sock, int Client_Idx,int Client_Num)
{
	int retval = 0;
	char buffer[BUFSIZE + 1];
	string toSendData;

	LobbySceneSendData Data_e;
	
	toSendData = " ";					
	toSendData += to_string(Client_Idx);		// 플레이어가 각자 저장한 인덱스
	toSendData += to_string(Client_Num);		// 클라이언트의 총갯수
	
	// 인덱스,총갯수 보내기
	SendFrameData(sock, toSendData, retval);
	
	RecvFrameData(sock, buffer, retval);

	// 캐릭터 변경 확인 변수
	int Character_Change = 0;

	Character_Change = atoi(buffer);

	if (Character_Change==1 && Client_Idx==0)
	{
		lobbyScene.playerTypes[Client_Idx] = 1;
	}
	if (Character_Change == 0 && Client_Idx == 0)
	{
		lobbyScene.playerTypes[Client_Idx] = 0;
	}
	if (Character_Change == 1 && Client_Idx == 1)
	{
		lobbyScene.playerTypes[Client_Idx] = 1;
	}
	if (Character_Change == 0 && Client_Idx == 1)
	{
		lobbyScene.playerTypes[Client_Idx] = 0;
	}
	if (Character_Change == 1 && Client_Idx == 2)
	{
		lobbyScene.playerTypes[Client_Idx] = 1;
	}
	if (Character_Change == 0 && Client_Idx == 2)
	{
		lobbyScene.playerTypes[Client_Idx] = 0;
	}
	
	// 캐릭터 변경 타입 보내고 받기
	toSendData.clear();

	toSendData += to_string(lobbyScene.playerTypes[0]);
	toSendData += to_string(lobbyScene.playerTypes[1]);
	toSendData += to_string(lobbyScene.playerTypes[2]);
	

	SendFrameData(sock, toSendData, retval);

	
	// 아이디 보내고 받기
	RecvFrameData(sock, buffer, retval);

	string ID_Change;

	if (Client_Idx == 0)
	{
		lobbyScene.id[Client_Idx] = buffer;
	}
	if (Client_Idx == 1)
	{
		lobbyScene.id[Client_Idx] = buffer;
	}
	if (Client_Idx == 2)
	{
		lobbyScene.id[Client_Idx] = buffer;
	}


	toSendData.clear();
	toSendData += "ID";
	toSendData += to_string(Client_Num);
	//toSendData += "\n";
	for (int i = 0; i < Client_Num; ++i)
	{
		if (lobbyScene.id[i] != "")
		{
			toSendData += lobbyScene.id[i];
		}
		else
			toSendData += "\n";
		
	}
	toSendData += "\n";

	cout << toSendData;

	SendFrameData(sock, toSendData, retval);

	
	////////// 클라인덱스
	RecvFrameData(sock, buffer, retval);

	if (Client_Idx == 0)
	{
		lobbyScene.index[Client_Idx] = 0;
	}
	
	if (Client_Idx == 1)
	{
		lobbyScene.index[Client_Idx] = 1;
	}
	
	if (Client_Idx == 2)
	{
		lobbyScene.index[Client_Idx] = 2;
	}
	 
	toSendData.clear();

	for (int i = 0; i < 3; i++)
	{
		toSendData += to_string(lobbyScene.index[i]);
	}

	SendFrameData(sock, toSendData, retval);

	// 클라 준비
	RecvFrameData(sock, buffer, retval);

	int Character_Ready = 0;
	
	Character_Ready = atoi(buffer);

	bool isReady = false;
	for (int i = 1; i < Client_Num; ++i)
	{
		if (lobbyScene.id[i] == "") continue;
		isReady = (lobbyScene.Ready[1]);
		if (!isReady)break;
	}
	lobbyScene.gamestart = isReady;
	if (lobbyScene.gamestart)
	{
		int stop = 3;
	}
	if (Character_Ready == 0 && Client_Idx == 0)
	{
		
		//if (lobbyScene.Ready[1] == 1 && lobbyScene.Ready[2] == 1)
		//{
		//	lobbyScene.gamestart = 1;
		//}
	}
	if (Character_Ready == 0 && Client_Idx == 0)
	{
		lobbyScene.Ready[Client_Idx] = 0;
	}

	if (Character_Ready == 1 && Client_Idx == 1)
	{
		lobbyScene.Ready[Client_Idx] = 1;
	}
	if (Character_Ready == 0 && Client_Idx == 1)
	{
		lobbyScene.Ready[Client_Idx] = 0;
	}
	if (Character_Ready == 1 && Client_Idx == 2)
	{
		lobbyScene.Ready[Client_Idx] = 1;
	}
	if (Character_Ready == 0 && Client_Idx == 2)
	{
		lobbyScene.Ready[Client_Idx] = 0;
	}



	toSendData.clear();

	toSendData += to_string(lobbyScene.Ready[0]);
	toSendData += to_string(lobbyScene.Ready[1]);
	toSendData += to_string(lobbyScene.Ready[2]);
	toSendData += to_string(lobbyScene.gamestart);

	SendFrameData(sock, toSendData, retval);

	// 채팅 부분

	//RecvFrameData(sock, buffer, retval);

	//if (Client_Idx == 0)
	//{
	//	lobbyScene.chatting[Client_Idx] = buffer;
	//}
	//if (Client_Idx == 1)
	//{
	//	lobbyScene.chatting[Client_Idx] = buffer;
	//}
	//if (Client_Idx == 2)
	//{
	//	lobbyScene.chatting[Client_Idx] = buffer;
	//}

	//toSendData.clear();
	//toSendData += "Chat";
	//toSendData += to_string(Data);
	//toSendData += "\n";
	//for (int i = 0; i < Data; ++i)
	//{
	//	toSendData += lobbyScene.chatting[i];
	//	toSendData += "\n";
	//}

	//cout << toSendData << endl;

	//SendFrameData(sock, toSendData, retval);


	

	return 0;
}


