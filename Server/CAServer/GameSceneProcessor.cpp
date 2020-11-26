#include "stdafx.h"
#include "GameSceneProcessor.h"
//#include "Communicates.h"
#pragma warning(disable  : 4996)    

bool GameSceneProcessor::ProcessGameScene(SOCKET& socket)
{
	EnterCriticalSection(&m_cs);

	int receivedSize;
	// +1, null value
	char buffer[BUFSIZE + 1];

	Vector2f prevPosition; 
	GameSceneRecvData recvedData;
	ZeroMemory(&recvedData, sizeof(recvedData));

	if (!RecvFrameData(socket, buffer, receivedSize))
	{
		LeaveCriticalSection(&m_cs);
		return 0;
	}
	//cout << buffer;
	char* token = strtok(buffer, "\n");
	bool bombCreateFlag = false;

	while (token != NULL)
	{
		if (strstr(token, "<PlayerIndex>:"))
		{
			recvedData.playerIndex = ConvertoIntFromText(token, "<PlayerIndex>:");
			if (m_Communicated[recvedData.playerIndex])
			{
				string temp = " \n";
				auto res = SendFrameData(socket, temp, receivedSize);
				LeaveCriticalSection(&m_cs);
				return true;
			}
			else
				m_Communicated[recvedData.playerIndex] = true;
			
			//cout << "<PlayerIndex>: " << recvedData.playerIndex << " \n";
		}
		else if (strstr(token, "<Position>:"))
		{
			recvedData.position = GetPositionFromText(token);
			prevPosition = recvedData.position;

			//cout << "x : " << recvedData.position.x << " y : " << recvedData.position.y << "\n";
		}						
		else if (strstr(token, "<Power>:"))
		{
			recvedData.power = ConvertoIntFromText(token, "<Power>:");
			//cout << "<Power>: " << recvedData.power << " \n";
		}
		else if (strstr(token, "<Speed>:"))
		{
			recvedData.speed = ConvertoIntFromText(token, "<Speed>:");
			//cout << "<Speed>: " << recvedData.speed << " \n";
		}
		else if (strstr(token, "<Direction>:"))
		{
			recvedData.direction = (Direction)ConvertoIntFromText(token, "<Direction>:");
			//cout << "<Direction>: " << (int)recvedData.direction << " \n";
		}
		else if (strstr(token, "<PlayerState>:"))
		{
			recvedData.state = (PlayerState)ConvertoIntFromText(token, "<PlayerState>:");
			//cout << "<PlayerState>: " << (int)recvedData.state << " \n";
		}
		else if (strstr(token, "<BombCreateFlag>:"))
		{
			bombCreateFlag = true;
			CreateBomb(recvedData.position, recvedData.power, recvedData.playerIndex);
		}
		token = strtok(NULL, "\n");
	}

	switch (recvedData.state)
	{
	case PlayerState::move:
		if (recvedData.playerIndex == 1)
		{
			int stop = 3;
		}
		prevPosition = recvedData.position;
		if (recvedData.direction == Direction::left)
			recvedData.position.x = recvedData.position.x - (recvedData.speed * PlAYER_SPEED * FPS);
		if (recvedData.direction == Direction::right)
			recvedData.position.x = recvedData.position.x + (recvedData.speed * PlAYER_SPEED * FPS);
		if (recvedData.direction == Direction::up)
			recvedData.position.y = recvedData.position.y - (recvedData.speed * PlAYER_SPEED * FPS);
		if (recvedData.direction == Direction::down)
			recvedData.position.y = recvedData.position.y + (recvedData.speed * PlAYER_SPEED * FPS);
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
	// 플레이어가 맵 밖으로 나가지않도록
	if (!IsInMap(recvedData.position)) recvedData.position = prevPosition;
	// Block - Player 충돌체크
	if (IsCollideToBlock(recvedData.position)) recvedData.position = prevPosition;

	// Bomb - Block,Item 충돌체크
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Bombs[i][j] == nullptr) continue;
			m_Bombs[i][j]->CheckPlayerOut(recvedData.position, recvedData.playerIndex);
			m_Bombs[i][j]->TimeUpdate();
			if (m_Bombs[i][j]->IsTimeToExplose()) m_Bombs[i][j]->ChangeState(BombState::Explosion);
			if (m_Bombs[i][j]->IsOnExplosion())
			{
				int power = m_Bombs[i][j]->GetPower();

				vector<Vector2i> coords;
				Vector2i coord = { j, i }; 

				for (int i = 1; i <= power; ++i)	// down
				{
					Vector2i temp = { coord.x, coord.y + i };
					if (IsDestroyedBlock(temp)) break;

					coords.push_back(temp);
					if (!IsInMapCoord(temp)) continue;
					// 아이템 처리
					if (m_Items[temp.y][temp.x])
					{
						m_DeletedItem.push_back(temp);
						delete m_Items[temp.y][temp.x];
						m_Items[temp.y][temp.x] = nullptr;
					}
					if (m_Map[temp.y][temp.x] == MapTileType::EMPTY) continue;
					
					auto iter = find(coords.begin(), coords.end(), coord);
					if (iter != coords.end()) coords.erase(iter);

					if (!m_Blocks[temp.y][temp.x]->GetIsCanDestroy()) break;

					m_DeletedBlock.push_back(temp);
					m_Map[temp.y][temp.x] = MapTileType::EMPTY;
					break;
				}
				for (int i = 1; i <= power; ++i)	// up
				{
					Vector2i temp = { coord.x, coord.y - i };
					if (IsDestroyedBlock(temp)) break;

					coords.push_back(temp);
					if (!IsInMapCoord(temp)) continue;
					if (m_Items[temp.y][temp.x])
					{
						m_DeletedItem.push_back(temp);
						delete m_Items[temp.y][temp.x];
						m_Items[temp.y][temp.x] = nullptr;
					}
					if (m_Map[temp.y][temp.x] == MapTileType::EMPTY) continue;

					auto iter = find(coords.begin(), coords.end(), coord);
					if (iter != coords.end()) coords.erase(iter);

					if (!m_Blocks[temp.y][temp.x]->GetIsCanDestroy()) break;

					m_DeletedBlock.push_back(temp);
					m_Map[temp.y][temp.x] = MapTileType::EMPTY;
					break;
				}
				for (int i = 1; i <= power; ++i)	// left
				{
					Vector2i temp = { coord.x - i, coord.y };
					if (IsDestroyedBlock(temp)) break;

					coords.push_back(temp);
					if (!IsInMapCoord(temp)) continue;
					if (m_Items[temp.y][temp.x])
					{
						m_DeletedItem.push_back(temp);
						delete m_Items[temp.y][temp.x];
						m_Items[temp.y][temp.x] = nullptr;
					}
					if (m_Map[temp.y][temp.x] == MapTileType::EMPTY) continue;

					auto iter = find(coords.begin(), coords.end(), coord);
					if (iter != coords.end()) coords.erase(iter);

					if (!m_Blocks[temp.y][temp.x]->GetIsCanDestroy()) break;

					m_DeletedBlock.push_back(temp);
					m_Map[temp.y][temp.x] = MapTileType::EMPTY;
					break;
				}
				for (int i = 1; i <= power; ++i)	// right
				{
					Vector2i temp = { coord.x + i, coord.y }; 
					if (IsDestroyedBlock(temp)) break;

					coords.push_back(temp);
					if (!IsInMapCoord(temp)) continue; 
					if (m_Items[temp.y][temp.x])
					{
						m_DeletedItem.push_back(temp);
						delete m_Items[temp.y][temp.x];
						m_Items[temp.y][temp.x] = nullptr;
					}
					if (m_Map[temp.y][temp.x] == MapTileType::EMPTY) continue;

					auto iter = find(coords.begin(), coords.end(), coord);
					if (iter != coords.end()) coords.erase(iter);

					if (!m_Blocks[temp.y][temp.x]->GetIsCanDestroy()) break;

					m_DeletedBlock.push_back(temp);
					m_Map[temp.y][temp.x] = MapTileType::EMPTY;

					break;
				}

				coords.push_back(coord); 

				for (auto coord_ : coords)
				{
					if (!IsInMapCoord(coord_)) continue;
					if (!m_Bombs[coord_.y][coord_.x]) continue;
					if (m_Bombs[coord_.y][coord_.x]->IsOnExplosion()) continue;
					m_Bombs[coord.y][coord.x]->SetLastBranchCoords(coords);
					
					m_Bombs[coord_.y][coord_.x]->ChangeState(BombState::Explosion);
					i = 0;
					j = 0;
				}
			} 
		}
	}

	// Bomb - Player 충돌체크
	if (IsCollideToBomb(recvedData.position, recvedData.state))
	{
		if (recvedData.state == PlayerState::die)
		{
			int stop = 3;
		}
		recvedData.position = prevPosition;
	}

	// 제거된 블록에서 아이템 생성
	for (auto coord : m_DeletedBlock)
	{
		delete m_Blocks[coord.y][coord.x];
		m_Blocks[coord.y][coord.x] = nullptr;
		int itemCreate = rand() % 10;
		int itemName = rand() % (int)ItemName::count;
		if (itemCreate <= 2)
		{
			m_CreatedItem.push_back(coord);
			m_Items[coord.y][coord.x] = new CItem((ItemName)itemName, coord);
		}
	}
	// 터진 물풍선 제거
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Bombs[i][j] == nullptr) continue;
			if (m_Bombs[i][j]->IsOnExplosion())
			{
				Vector2i coord = m_Bombs[i][j]->GetCoordinate();
				m_DeletedBomb.push_back(coord);
				delete m_Bombs[coord.y][coord.x];
				m_Bombs[coord.y][coord.x] = nullptr;
			}
		}
	}
	
	GameSceneSendData sendData;
	sendData.index = recvedData.playerIndex;
	sendData.position.x = recvedData.position.x;
	sendData.position.y = recvedData.position.y;
	sendData.speed = recvedData.speed;
	sendData.state = recvedData.state;
	sendData.isGameEnd = false;
	sendData.bombCreateFlag = bombCreateFlag; 

	// 갱신된 정보 반영
	if (m_Players[recvedData.playerIndex])
	{
		m_Players[recvedData.playerIndex]->SetPosition(recvedData.position);
		m_Players[recvedData.playerIndex]->SetState(recvedData.state);
	}

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

	toSendData += "<Players>:";
	toSendData += to_string(m_ClientNum);
	toSendData += "\n";
	for (int i = 0; i < MAX_CLIENT_NUM; ++i)
	{
		if (m_Players[i] != nullptr)
		{
			toSendData += to_string(m_Players[i]->GetIndex());
			toSendData += " ";
			toSendData += to_string((int)m_Players[i]->GetState());
			toSendData += "\n";
			toSendData += to_string(m_Players[i]->GetPosition().x);
			toSendData += " ";
			toSendData += to_string(m_Players[i]->GetPosition().y);
		}
		toSendData += "\n";
	}
	if (m_DeletedBlock.size() != 0) 
	{
		toSendData += "<DeletedBlock>:";
		toSendData += to_string(m_DeletedBlock.size());
		toSendData += "\n";
		cout << "DeletedBlock : ";
		for(int i = 0; i <m_DeletedBlock.size(); ++i)
		{
			toSendData += to_string(m_DeletedBlock[i].x);
			toSendData += " ";
			toSendData += to_string(m_DeletedBlock[i].y);	
			toSendData += "\n";
			cout << "x : " << m_DeletedBlock[i].x << " y : " << m_DeletedBlock[i].y << " ";
		} 
		cout << "\n";
		//m_DeletedBlock.clear();
	}
	if (m_CreatedBomb.size() != 0)
	{
		toSendData += "<BombCreateFlag>:";
		toSendData += to_string(m_CreatedBomb.size());
		toSendData += "\n";
		cout << "CreatededBomb(size:" << m_CreatedBomb.size() << ") ";
		for (int i = 0; i < m_CreatedBomb.size(); ++i)
		{
			toSendData += to_string(m_CreatedBomb[i].x);
			toSendData += " ";
			toSendData += to_string(m_CreatedBomb[i].y);
			toSendData += "\n";
			cout << "x : " << m_CreatedBomb[i].x << " y : " << m_CreatedBomb[i].y << " ";
		}
		cout << "\n"; 
		//m_CreatedBomb.clear();
	}
	if (m_DeletedBomb.size() != 0)
	{
		toSendData += "<DeletedBomb>:";
		toSendData += to_string(m_DeletedBomb.size());
		toSendData += "\n";
		cout << "DeletedBomb(size:" << m_DeletedBomb.size() << ") ";
		for (int i = 0; i < m_DeletedBomb.size(); ++i)
		{
			toSendData += to_string(m_DeletedBomb[i].x);
			toSendData += " ";
			toSendData += to_string(m_DeletedBomb[i].y);
			toSendData += "\n";
			cout << "x : " << m_DeletedBomb[i].x << " y : " << m_DeletedBomb[i].y << " ";
		}
		cout << "\n";
		//m_DeletedBomb.clear();
	}
	if (m_CreatedItem.size() != 0)
	{
		toSendData += "<CreatedItem>:";
		toSendData += to_string(m_CreatedItem.size());
		toSendData += "\n";
		cout << "CreatedItem(size:" << m_CreatedItem.size() << ") ";
		
		for (int i = 0; i < m_CreatedItem.size(); ++i)
		{
			ItemName itemName = m_Items[m_CreatedItem[i].y][m_CreatedItem[i].x]->GetName();
			toSendData += to_string((int)itemName);
			toSendData += " ";
			toSendData += to_string(m_CreatedItem[i].x);
			toSendData += " ";
			toSendData += to_string(m_CreatedItem[i].y);
			toSendData += "\n";
			cout << "x : " << m_CreatedItem[i].x << " y : " << m_CreatedItem[i].y << " ";
		}
		cout << "\n";
		//m_CreatedItem.clear();
	}
	if (m_DeletedItem.size() != 0)
	{
		toSendData += "<DeletedItem>:";
		toSendData += to_string(m_DeletedItem.size());
		toSendData += "\n";
		cout << "DeletedItem(size:" << m_DeletedItem.size() << ") ";
		for (int i = 0; i < m_DeletedItem.size(); ++i)
		{
			toSendData += to_string(m_DeletedItem[i].x);
			toSendData += " ";
			toSendData += to_string(m_DeletedItem[i].y);
			toSendData += "\n";
			cout << "x : " << m_DeletedItem[i].x << " y : " << m_DeletedItem[i].y << " ";
		}
		cout << "\n";
		//m_DeletedItem.clear(); 
	}
	
	m_SendCount = 0;
	for (int i = 0; i < m_ClientNum; ++i)
	{
		if (!m_Communicated[i]) break;
		++m_SendCount;
	}
	
	if (m_SendCount >= m_ClientNum)
	{
		m_DeletedBlock.clear();
		m_CreatedBomb.clear();
		m_DeletedBomb.clear();
		m_CreatedItem.clear();
		m_DeletedItem.clear();
		m_SendCount = 0;

		for (int i = 0; i < 4; ++i)
		{
			m_Communicated[i] = false;
		}
		//ZeroMemory(m_Communicated, sizeof(m_Communicated));
	}
	auto res = SendFrameData(socket, toSendData, receivedSize); 
	
	LeaveCriticalSection(&m_cs);
	
	return res;
}
// mbstowcs unsafe###
Vector2f GameSceneProcessor::GetPositionFromText(const char* text)
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

int GameSceneProcessor::ConvertoIntFromText(const char* text, const char* token)
{
	char buf[256];
	ZeroMemory(buf, 256);
	int tokenLen = strlen(token);
	strncpy(buf, text + tokenLen, strlen(text) - tokenLen);
	return atoi(buf);
}

void GameSceneProcessor::InitMap()
{
	BlockName mapData[MAP_HEIGHT][MAP_WIDTH];
	ZeroMemory(mapData, sizeof(mapData));
	mapData[0][1] = mapData[0][3]
		= mapData[1][11]
		= mapData[2][2] = mapData[2][4]
		= mapData[3][10] = mapData[3][12] = mapData[3][14]
		= mapData[4][1] = mapData[4][3]
		= mapData[5][0] = mapData[5][3] = mapData[5][5] = mapData[5][11] = mapData[5][13]
		= mapData[7][1] = mapData[7][3]
		= mapData[8][10] = mapData[8][12] = mapData[8][14]
		= mapData[9][0] = mapData[9][2] = mapData[9][4]
		= mapData[10][11] = mapData[10][13]
		= mapData[11][3]
		= mapData[12][10] = mapData[12][12]
		= BlockName::BLOCK_YELLOW;

	mapData[0][2] = mapData[0][4] = mapData[0][11]
		= mapData[1][10]
		= mapData[2][3]
		= mapData[3][11] = mapData[3][13]
		= mapData[4][0] = mapData[4][2] = mapData[4][4]
		= mapData[5][10] = mapData[5][12] = mapData[5][14]
		= mapData[7][0] = mapData[7][2] = mapData[7][4] = mapData[7][10] = mapData[7][12] = mapData[7][14]
		= mapData[8][11] = mapData[8][13]
		= mapData[9][1] = mapData[9][3]
		= mapData[10][10] = mapData[10][12]
		= mapData[11][2] = mapData[11][4]
		= mapData[12][3] = mapData[12][11]
		= BlockName::BLOCK_RED;
	mapData[0][8]
		= mapData[1][2] = mapData[1][4] = mapData[1][6]
		= mapData[2][7] = mapData[2][8] = mapData[2][11] = mapData[2][13]
		= mapData[3][0] = mapData[3][2] = mapData[3][4] = mapData[3][6]
		= mapData[4][8] = mapData[4][11] = mapData[4][13]
		= mapData[5][6] = mapData[5][7]
		= mapData[6][8]
		= mapData[7][6]
		= mapData[8][1] = mapData[8][3] = mapData[8][7] = mapData[8][8]
		= mapData[9][6] = mapData[9][10] = mapData[9][12] = mapData[9][14]
		= mapData[10][3] = mapData[10][8]
		= mapData[11][6] = mapData[11][7] = mapData[11][10] = mapData[11][12]
		= mapData[12][8]
		= BlockName::BOX;
	mapData[1][5] = mapData[1][9]
		= mapData[3][5] = mapData[3][9]
		= mapData[5][5]
		= mapData[6][1] = mapData[6][3] = mapData[6][5] = mapData[6][9] = mapData[6][11] = mapData[6][13]
		= mapData[6][0] = mapData[6][2] = mapData[6][4] = mapData[6][10] = mapData[6][12] = mapData[6][14]
		= mapData[7][9] = mapData[9][9] = mapData[11][9]
		= mapData[9][5] = mapData[11][5]
		= BlockName::TREE;

	mapData[1][1] = mapData[1][3]
		= mapData[3][1] = mapData[3][3]
		= mapData[5][1] = mapData[5][3]
		= mapData[9][11] = mapData[9][13]
		= mapData[11][11] = mapData[11][13]
		= BlockName::HOUSE_RED;

	mapData[0][10] = mapData[0][12] = mapData[0][14]
		= mapData[2][10] = mapData[2][12] = mapData[2][14]
		= mapData[4][10] = mapData[4][12] = mapData[4][14]
		= BlockName::HOUSE_YELLOW;

	mapData[8][0] = mapData[8][2] = mapData[8][4]
		= mapData[10][0] = mapData[10][2] = mapData[10][4]
		= mapData[12][0] = mapData[12][2] = mapData[12][4]
		= BlockName::HOUSE_BLUE;
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (mapData[i][j] == BlockName::EMPTY) continue;

			bool isCanDestroy = true;
			if (mapData[i][j] == BlockName::HOUSE_BLUE) isCanDestroy = false;
			if (mapData[i][j] == BlockName::HOUSE_RED) isCanDestroy = false;
			if (mapData[i][j] == BlockName::HOUSE_YELLOW) isCanDestroy = false;
			if (mapData[i][j] == BlockName::TREE) isCanDestroy = false;

			CBlock* block = new CBlock({i,j}, isCanDestroy, true);
			 
			m_Map[i][j] = MapTileType::BLOCK;

			m_Blocks[i][j] = block;
		}
	}
}

void GameSceneProcessor::InitPlayers()
{
	Vector2i TileStartPosition = { 26, 53 };
	const Vector2f Positions[5] =
	{
		{(float)TileStartPosition.x + OBJECT_SIZE * 13,
		 (float)TileStartPosition.y + OBJECT_SIZE * 1},
		{(float)TileStartPosition.x + OBJECT_SIZE * 0,
		 (float)TileStartPosition.y + OBJECT_SIZE * 1},
		{(float)TileStartPosition.x + OBJECT_SIZE * 1,
		 (float)TileStartPosition.y + OBJECT_SIZE * 11},
		{(float)TileStartPosition.x + OBJECT_SIZE * 14,
		 (float)TileStartPosition.y + OBJECT_SIZE * 11},
		{ -1000.0f, -1000.0f }
	};
	for (int i = 0; i < m_ClientNum; ++i)
	{
		m_Players[i] = new CPlayer(Positions[i], i);
	}
}

Vector2D<int> GameSceneProcessor::GetCoordinates(Vector2D<float> position, Vector2D<int> size)
{
	Vector2D<int> coordinate;
	Vector2D<int> TileStartPosition = { 26, 53 };
	coordinate.x = (position.x + size.x * 0.5f - TileStartPosition.x);// / OBJECT_SIZE;
	coordinate.y = (position.y + size.y * 0.5f - TileStartPosition.y);// / OBJECT_SIZE;
	if (coordinate.x > 0) coordinate.x /= OBJECT_SIZE;
	if (coordinate.y > 0) coordinate.y /= OBJECT_SIZE;

	return coordinate;
}

Vector2D<float> GameSceneProcessor::GetPositionCoord(Vector2D<int> coord)
{
	Vector2D<float> position;
	Vector2D<int> TileStartPosition = { 26, 53 };
	position.x = coord.x * OBJECT_SIZE + TileStartPosition.x;
	position.y = coord.y * OBJECT_SIZE + TileStartPosition.y;
	return position;
}

bool GameSceneProcessor::IsInMapCoord(Vector2D<int> coord)
{
	if (coord.x < 0) return false;
	if (coord.x >= MAP_WIDTH) return false;
	if (coord.y < 0) return false;
	if (coord.y >= MAP_HEIGHT) return false;
	return true;
}

bool GameSceneProcessor::IsCollide(Vector2f position, Vector2i targetIdx)
{
	Vector2f targetPos = GetPositionCoord(targetIdx);

	RECT rtThis = GetCollisionRect(position);
	RECT rtOther = GetCollisionRect(targetPos);

	bool leftCondition = rtThis.left < rtOther.right;
	bool rightCondition = rtThis.right > rtOther.left;
	bool topCondition = rtThis.top < rtOther.bottom;
	bool bottomCondition = rtThis.bottom > rtOther.top;

	return (leftCondition &&
		rightCondition &&
		topCondition &&
		bottomCondition);
}

RECT GameSceneProcessor::GetCollisionRect(Vector2f pos)
{
	Vector2i size = { OBJECT_SIZE, OBJECT_SIZE };
	int correction = 4;

	RECT rt;
	rt.top = pos.y + correction;
	rt.left = pos.x + correction;
	rt.bottom = pos.y + size.y - correction;
	rt.right = pos.x + size.x - correction;
	return rt;
}

void GameSceneProcessor::CreateBomb(Vector2f pos, int power, int index)
{
	Vector2i coord = GetCoordinates(pos, { OBJECT_SIZE, OBJECT_SIZE });
	if (!m_Bombs[coord.y][coord.x])
	{ 
		m_Bombs[coord.y][coord.x] = new CBomb(pos, power, index);
		m_CreatedBomb.push_back(coord); 
		//cout << "CreateBomb : x - " << coord.x << " y - " << coord.y << "\n";
	}
}

bool GameSceneProcessor::IsDestroyedBlock(Vector2i coord)
{
	auto result = find(m_DeletedBlock.begin(), m_DeletedBlock.end(), coord);
	return result != m_DeletedBlock.end();
}

bool GameSceneProcessor::IsCollideToBlock(Vector2f playerPosition)
{
	// with block
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Map[i][j] == MapTileType::EMPTY) continue;

			if (IsCollide(playerPosition, { j, i }))
			{
				//cout << "충돌확인 i : " << i << " j : " << j << "\n";
				return true;
			}
		}
	}

	return false;
}

bool GameSceneProcessor::IsCollideToBomb(Vector2f playerPosition, PlayerState& state)
{
	// with bomb
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (!m_Bombs[i][j]) continue;
			
			Vector2i playerCoord = GetCoordinates(playerPosition, { OBJECT_SIZE, OBJECT_SIZE });

			if (IsCollide(playerPosition, { j, i }))
			{
				if (m_Bombs[i][j]->GetIsPlayerOn()) continue;
				//cout << "충돌확인 i : " << i << " j : " << j << "\n";
				return true;
			}

			if (m_Bombs[i][j]->IsOnExplosion())	// 물줄기와 플레이어 충돌처리
			{
				vector<Vector2i> branchCoords = m_Bombs[i][j]->GetLastBranchCoords();

				auto iter = find(branchCoords.begin(), branchCoords.end(), playerCoord);
				if (iter == branchCoords.end()) continue;

				state = PlayerState::die;
			}
		}
	}
	return false;
}

bool GameSceneProcessor::IsCollideToItem(Vector2f playerPosition)
{
	return false;
}

bool GameSceneProcessor::IsInMap(Vector2f position)
{
	RECT rtMap;
	rtMap.left = 24;
	rtMap.right = 807;
	rtMap.top = 52;
	rtMap.bottom = 732;

	if (position.x < rtMap.left) return false;
	if (position.x + OBJECT_SIZE > rtMap.right) return false;
	if (position.y < rtMap.top) return false;
	if (position.y + OBJECT_SIZE > rtMap.bottom) return false;
	return true;
}
