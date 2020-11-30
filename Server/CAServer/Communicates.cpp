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

int g_pIndex = 0; // �÷��̾� ��ȣ

DWORD __stdcall ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientAddr;
	int addrLen;
	int idx = Data.Thread_Num;
	int retval = 0;

	// Ŭ���̾�Ʈ ���� �ޱ�
	addrLen = sizeof(clientAddr);
	getpeername(client_sock, (SOCKADDR*)&clientAddr, &addrLen);

	Data.Thread_Num++;
	GameSceneProcessor::GetInstance()->IncreaseClientNum();
	cout << "Increased GameSceneProcessor::ClientNum : "
		<< GameSceneProcessor::GetInstance()->GetClientNum() << "\n";

	// �ӽ��ڵ�
	GameSceneProcessor::GetInstance()->InitPlayers();
	cout << Data.Thread_Num << "�� Ŭ�� �����߽��ϴ�." << endl;

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

		// 2 ���� ����ϴ� Ŭ���̾�Ʈ�� SceneŸ���� �޾ƿ´�.
		if (!RecvFrameData(client_sock, buffer, receivedSize)) break;
		//cout << "�� ��ȣ" << atoi(buffer);
		//if (strcmp(buffer, "Quit")) break;
		SceneType sceneType = SceneType(atoi(buffer));
		switch (sceneType)
		{
		case SceneType::TitleScene:
			if (ProcessTitleScene(client_sock, g_pIndex)) {
				cout << "������ �÷��̾� �ε���: " << g_pIndex << endl;
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

			 // 3 ���� ������ �÷��̾��� ���� �Ѱ��ش�.
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

	cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientAddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientAddr.sin_port) << endl;

	return 0;
}

bool ProcessTitleScene(SOCKET& sock, int idx)
{
	int retval;
	char buffer[BUFSIZE + 1];
	bool isLogin = FALSE;			//�α��� ���� ���ο� ���� ���ϰ�

	TitleSceneRecvData recvData;	//���Ź��� ������
	TitleSceneSendData sendData;	//�۽��� ������
	map<string, string> accounts;	//���Ͽ��� �о�� ����������
	string data;					//������ ���縦 ���� ����
	sendData.playerIndex = -1;		//�α����� �ƴ� ��쳪 �α��� ���н� -1�� ����, �α��� ������ 0~3

	// ���� ���� �о����
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

	//�޾ƿ� ���� ������ ������ recvData�� �� ������ �Ҵ�
	recvData.id = data.substr(data.find("<ID>")+4, data.find("<PW>") - (data.find("<ID>")+4));
	recvData.pw = data.substr(data.find("<PW>") + 4, data.find("<isNew>") - (data.find("<PW>") + 4));
	recvData.isNew = (bool)data[data.find("<isNew>") + 7];

	//Ȯ�ο� ���
	cout << "[ID]:" << recvData.id << " [PW]:" << recvData.pw << " [isNew]:" << recvData.isNew << endl;

	if (recvData.isNew == TRUE) {
	//�ű� ���� ����� ���
		ofstream out;
		out.open("data/Account.txt"s, ios::app);

		auto isAdded = accounts.insert(pair<string, string>(recvData.id, recvData.pw));
		if (isAdded.second == TRUE) {
		//���� ��� ����
			cout << ">> " << isAdded.second << endl;
			out << recvData.id << " " << recvData.pw << endl;
			sendData.text = "Account registration completed!";
			sendData.result = TRUE;
		}
		else {
		//���� ��� ����
			cout << ">> " << isAdded.second << endl;
			sendData.text = "This ID already exists.";
			sendData.result = FALSE;
		}

		out.close();
	}
	else {
	//���� ����� �ƴ� �α����� ���
		map<string, string>::iterator find;
		find = accounts.find(recvData.id);
		if (find == accounts.end()) {
		//���̵� ã�� ���� ���
			sendData.result = FALSE;
			sendData.text = "Wrong Account.";
		}
		else {
		//���̵� ã�� ���
			if (find->second == recvData.pw) {
			//��й�ȣ ��ġ = �α��� ����
				sendData.text = "Login Accept!";
				sendData.playerIndex = idx;
				sendData.result = TRUE;
				isLogin = TRUE;
			}
			else {
			//��й�ȣ ����ġ = �α��� ����
				sendData.text = "Login Fail...";
				sendData.result = FALSE;
			}
		}
	}

	//Ȯ�ο� ���
	cout << "[result]:" << sendData.result << " [TEXT]:" << sendData.text << endl;

	data.clear();
	
	//�۽��� ������ ����
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

	GameRecordSceneRecvData recvData;			//���Ź��� ������
	vector<GameRecordSceneSendData> ranking;	//���Ͽ��� �о�� ���� ������
	string data;								//������ ���縦 ���� ����

	RecvFrameData(socket, buffer, retval);
	data = buffer;

	//�޾ƿ� ���� ������ ������ recvData�� �� ������ �Ҵ�
	recvData.id = data.substr(data.find("<ID>") + 4, data.find("<ITS>") - (data.find("<ID>") + 4));
	recvData.itemScore = atoi(data.substr(data.find("<ITS>") + 5).c_str());
	recvData.survivedScore = atof(data.substr(data.find("<SVS>") + 5).c_str());

	//Ȯ�ο� ���
	cout << "[ID]: " << recvData.id << " [ITS]: " << recvData.itemScore << " [SVS]: " << recvData.survivedScore << endl;

	//���Ź��� ���� ���Ͽ� ����
	ofstream out;
	out.open("data/Score.txt"s, ios::app);
	out << recvData.id << " " << recvData.itemScore << " " << recvData.survivedScore << endl;
	out.close();

	//���� ���� �о����, �����ð��� int�� ��ȯ �� *10�� �ؼ� ������ ��ȯ
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

	//������ ���� + ���� ���� ������ �������� ����
	sort(ranking.begin(), ranking.end(),
		[](const GameRecordSceneSendData& a, const GameRecordSceneSendData& b) {
			return (a.itemScore + a.survivedScore) > (b.itemScore + b.survivedScore); });

	data.clear();

	//���� ���� 10�� ��� �۽�
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

	cout << "�κ��" << endl;
	
	// 3 ���� ������ �÷��̾��� ���� �Ѱ��ش�.
	string temp = to_string(Data_n);
	SendFrameData(sock, temp, retval);

	cout << "�������Ӽ�" << Data_n << endl;

	// ä�� �����͸� �޴´�.
	for (int i = 0; i < Data_n; ++i)
	{
		RecvFrameData(sock, buffer, retval);
		chat_Da.push_back(buffer);
		
	}

	cout << "ä�õ�����" << chat_Da[0] << endl;

	return 0;
}