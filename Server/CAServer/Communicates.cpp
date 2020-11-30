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
	cout << Data.Thread_Num << "번 클라가 접속했습니다." << endl;

	// +1, null value
	char buffer[BUFSIZE + 1];
	int receivedSize = 0;
	int count = 0;

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
			ProcessLobbyScene(client_sock,Data.Thread_Num);
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
			temp += to_string((int)GetCurrentThreadId()); 
			
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

bool ProcessTitleScene(SOCKET& sock, int idx)
{
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
				sendData.playerIndex = idx;
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

	//수신받은 점수 파일에 저장
	ofstream out;
	out.open("data/Score.txt"s, ios::app);
	out << recvData.id << " " << recvData.itemScore << " " << recvData.survivedScore << endl;
	out.close();

	//점수 정보 읽어오기, 생존시간은 int로 변환 후 *10을 해서 점수로 변환
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
	in.close();

	//아이템 점수 + 생존 점수 합으로 내림차순 정렬
	sort(ranking.begin(), ranking.end(),
		[](const GameRecordSceneSendData& a, const GameRecordSceneSendData& b) {
			return (a.itemScore + a.survivedScore) > (b.itemScore + b.survivedScore); });

	data.clear();

	//상위 점수 10개 목록 송신
	for (int i = 1; i < 11; i++) {
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
	return 0;
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