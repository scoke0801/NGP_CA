#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"
#include "Sound.h"
#include "Item.h"
#include "Block.h"
#include "Bomb.h"
#include "Datas.h"

#define BUFSIZE 4096

CGameScene::CGameScene()
{
	ZeroMemory(m_Map, sizeof(m_Map));
	//ZeroMemory(m_Players, sizeof(m_Players));

	m_SoundManager = new CSoundManager();
	m_SoundManager->AddStream("assets/sound/village.mp3", Sound_Name::BGM_MAIN_GAME);
	m_SoundManager->AddSound("assets/sound/bomb_set.mp3", Sound_Name::EFFECT_BOMB_SET);
	m_SoundManager->AddSound("assets/sound/bomb_pop.mp3", Sound_Name::EFFECT_BOMB_POP);
	m_SoundManager->AddSound("assets/sound/wave.mp3", Sound_Name::EFFECT_BOMB_WAVE);
	//m_SoundManager->PlayBgm(Sound_Name::BGM_MAIN_GAME);
	//m_SoundManager->SetVolume(0.0f);

	ZeroMemory(m_Blocks, sizeof(m_Blocks));

	LoadImages();
	InitMap();
	
	m_Type = SceneType::GameScene;
}

CGameScene::~CGameScene()
{
	m_SoundManager->Stop();
}

void CGameScene::SendDataToNextScene(void* pContext)
{
	LobbyToGameSceneData* data = (LobbyToGameSceneData*)pContext;
	m_ClientIdx = data->idx;
	m_ClientID = data->id;

	const Vector2f Positions[5] =
	{
		{(float)m_TileStartPosition.x + OBJECT_SIZE * 13,
		 (float)m_TileStartPosition.y + OBJECT_SIZE * 1},
		{(float)m_TileStartPosition.x + OBJECT_SIZE * 0,
		 (float)m_TileStartPosition.y + OBJECT_SIZE * 1},
		{(float)m_TileStartPosition.x + OBJECT_SIZE * 1,
		 (float)m_TileStartPosition.y + OBJECT_SIZE * 11},
		{(float)m_TileStartPosition.x + OBJECT_SIZE * 14,
		 (float)m_TileStartPosition.y + OBJECT_SIZE * 11},
		{ -1000.0f, -1000.0f }
	};

	for (int i = 0; i < 10; ++i)
	{
		m_Players[i] = new CPlayer(Positions[4]);
	}
	if (m_Players[m_ClientIdx])
	{
		SAFE_DELETE(m_Players[m_ClientIdx]);	
	}
	m_Players[m_ClientIdx] = new CPlayer(Positions[m_ClientIdx]);
	m_Players[m_ClientIdx]->SetIndex(m_ClientIdx);
	m_Player = m_Players[m_ClientIdx];

	string id = "Player";
	m_Player->SetID(id + to_string(m_ClientID)); 
}

void CGameScene::Update(float timeElapsed)
{
	m_SoundManager->OnUpdate();

	for (auto player : m_Players)
	{
		player->Update(timeElapsed);
	}

	// 물풍선 업데이트
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Bombs[i][j] == nullptr) continue;

			m_Bombs[i][j]->Update(timeElapsed);

			if (m_Bombs[i][j]->CheckDelete())
			{
				Vector2D<int> coord = GetCoordinates(m_Bombs[i][j]->GetPosition(), m_Bombs[i][j]->GetSize());
				m_Map[coord.y][coord.x] = MAP_TILE_TYPE::EMPTY;

				CPlayer* player = m_Bombs[i][j]->GetPlayer();
				player->EraseBomb(m_Bombs[i][j]);
				delete m_Bombs[i][j];
				m_Bombs[i][j] = nullptr;
			}
		}
	}
	// 블록 업데이트
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (!m_Blocks[i][j]) continue;
			m_Blocks[i][j]->Update(timeElapsed);

			if (m_Blocks[i][j]->CheckDelete())
			{
				Vector2D<int> coord = GetCoordinates(m_Blocks[i][j]->GetPosition(), m_Blocks[i][j]->GetSize());
				m_Map[coord.y][coord.x] = MAP_TILE_TYPE::EMPTY;

				delete m_Blocks[i][j];
				m_Blocks[i][j] = nullptr; 
			}
		}
	}

	// 아이템 업데이트
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (!m_Items[i][j]) continue;
			m_Items[i][j]->Update(timeElapsed);
		}
	}

	for (auto player : m_Players)
	{
		//// 플레이어 - 블록 충돌 처리
		//for (int i = 0; i < MAP_HEIGHT; ++i)
		//{
		//	for (int j = 0; j < MAP_WIDTH; ++j)
		//	{
		//		if (m_Blocks[i][j] == nullptr) continue;
		//
		//		bool bCollide = player->IsCollide(m_Blocks[i][j]);
		//		bool bMovable = m_Blocks[i][j]->IsCanMove();
		//		
		//		if (bCollide)
		//		{
		//			//player->FixCollision();
		//		}
		//		else continue;;
		//		if (m_Blocks[i][j]->GetIsOnMove()) continue;
		//		if (bMovable)
		//		{ 
		//			Direction dir = player->GetDirection();
		//			Vector2i coord(j, i);
		//			if (!CalcNextCoordinates(coord, dir)) continue;
		//			m_Blocks[i][j]->SetIsOnMove(true, coord);
		//			m_Blocks[i][j]->SetDirection(dir);
		//			m_Blocks[coord.y][coord.x] = m_Blocks[i][j];
		//			m_Map[i][j] = MAP_TILE_TYPE::EMPTY;
		//			m_Blocks[i][j] = nullptr;
		//		}
		//	}
		//}
		// 플레이어 - 아이템 충돌 처리
		for (int i = 0; i < MAP_HEIGHT; ++i)
		{
			for (int j = 0; j < MAP_WIDTH; ++j)
			{
				if (!m_Items[i][j]) continue;
				if (!player->IsCollide(m_Items[i][j])) continue;
				ItemName itemName = m_Items[i][j]->GetItemName();
				switch (itemName)
				{
				case ItemName::ballon:
					player->MoreBomb();
					break;
				case ItemName::nuclear:
					player->PowerUp(999);
					break;
				case ItemName::skate:
					player->SpeedUp();
					break;
				case ItemName::potion:
					player->PowerUp(1);
					break;
				}

				delete m_Items[i][j];
				m_Items[i][j] = nullptr;
			}
		} 
	}
}

void CGameScene::Draw(HDC hdc)
{ 
	m_UIImage.StretchBlt(hdc,
		0, 0,
		m_rtClient.right, m_rtClient.bottom,
		0, 0, m_UIImage.GetWidth(), m_UIImage.GetHeight()); 

	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Bombs[i][j] == nullptr) continue;
			m_Bombs[i][j]->Draw(hdc);
			m_Bombs[i][j]->Draw(hdc, m_Blocks);
		}
	}
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Items[i][j] == nullptr) continue;
			m_Items[i][j]->Draw(hdc);
		}
	}
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (m_Blocks[i][j] == nullptr) continue;
			m_Blocks[i][j]->Draw(hdc);
		}
	}
	
	for (auto player : m_Players)
	{
		player->Draw(hdc);
	}	
}

void CGameScene::Communicate(SOCKET& sock)
{
	int retVal;
	string toSendData = to_string((int)m_Type);
	SendFrameData(sock, toSendData, retVal);
	 
	GameSceneSendData data;
	data.playerIndex = m_Player->GetIndex();
	data.position = m_Player->GetPosition();
	data.waterRange = m_Player->GetPower();
	data.speed = m_Player->GetSpeed();
	data.direction = m_Player->GetDirection();
	data.state = (int)m_Player->GetState();
	//data.mapData = m_Map;

	toSendData.clear();

	toSendData = "<Position>:";
	toSendData += to_string(data.position.x);
	toSendData += " ";
	toSendData += to_string(data.position.y);
	toSendData += "\n";

	toSendData += "<PlayerIndex>:";
	toSendData += to_string(data.playerIndex);
	toSendData += "\n";

	toSendData += "<Power>:";
	toSendData += to_string(data.waterRange);
	toSendData += "\n";

	toSendData += "<Speed>:";
	toSendData += to_string(data.speed);
	toSendData += "\n";

	toSendData += "<Direction>:";
	toSendData += to_string((int)data.direction);
	toSendData += "\n";

	toSendData += "<PlayerState>:";
	toSendData += to_string(data.state);
	toSendData += "\n";

	if (m_Player->GetCreateBombFlag())
	{
		toSendData += "<BombCreateFlag>:";
		toSendData += m_Player->GetCreateBombFlag();
		toSendData += "\n";
		m_Player->SetCreateBombFlag(false);
	} 
	SendFrameData(sock, toSendData, retVal);

	char buffer[BUFSIZE + 1];
	int receivedSize = 0;

	RecvFrameData(sock, buffer, retVal);
	//cout << "받은 값" << buffer << "\n";
	char* token = strtok(buffer, "\n");
	int index = 0;
	Vector2f position;
	Vector2f prevPosition;
	if (m_Player) prevPosition = m_Player->GetPosition();

	while (token != NULL)
	{
		if (strstr(token, "<PlayerIndex>:"))
		{
			index = ConvertoIntFromText(token, "<PlayerIndex>:");
			//cout << "<PlayerIndex>: " << index << " \n";
		}
		else if (strstr(token, "<Position>:"))       //(token, "<position>:"))
		{
			m_ClientIdx;
			position = GetPositionFromText(token);
			//cout << "x : " << position.x << " y : " << position.y << "\n";
			m_Players[index]->SetPosition(position);
		}
		else if (strstr(token, "<IsGameEnd>:"))
		{
			//cout << "<IsGameEnd>: " << boolalpha << (bool)ConvertoIntFromText(token, "<IsGameEnd>:") << " \n";
		}
		else if (strstr(token, "<Speed>:"))
		{
			//cout << "<Speed>: " << ConvertoIntFromText(token, "<Speed>:") << " \n";
		}
		else if (strstr(token, "<PlayerState>:"))
		{
			//cout << "<PlayerState>: " << ConvertoIntFromText(token, "<PlayerState>:") << " \n";
		}
		else if (strstr(token, "<BombCreateFlag>:"))
		{
			vector<Vector2i> createdBlock;
			int num = ConvertoIntFromText(token, "<BombCreateFlag>:");
			GetCoordsFromText(token, num, createdBlock);
			cout << "CreatedBomb : [index : "  << index << "] ";
			for (auto coord : createdBlock)
			{
				CreateBomb(coord);
				cout << "x : " << coord.x << " y : " << coord.y << " ";
			}
			cout << "\n";
			////m_SoundManager->PlayEffect(Sound_Name::EFFECT_BOMB_SET);
			m_Player->SetCreateBombFlag(false);
		}
		else if (strstr(token, "<DeletedBlock>:"))
		{
			vector<Vector2i> deletedBlock;
			int num = ConvertoIntFromText(token, "<DeletedBlock>:");
			GetCoordsFromText(token, num, deletedBlock);
			cout << "DeletedBlock : [index : " << index << "] ";
			for (auto coord : deletedBlock)
			{
				if (m_Blocks[coord.y][coord.x])
					m_Blocks[coord.y][coord.x]->ChangeState(BlockState::Destroyed);
				cout << "x : " << coord.x << " y : " << coord.y << " ";
			}
			cout << "\n";
		}
		else if (strstr(token, "<DeletedBomb>:"))
		{
			vector<Vector2i> deletedBomb;
			int num = ConvertoIntFromText(token, "<DeletedBomb>:");
			GetCoordsFromText(token, num, deletedBomb);
			cout << "DeletedBomb : [index : " << index << "] ";
			for (auto coord : deletedBomb)
			{
				if (!m_Bombs[coord.y][coord.x]) continue;
				m_Bombs[coord.y][coord.x]->ChangeState(BombState::Explosion);
				--num;
				cout << "x : " << coord.x << " y : " << coord.y << " ";
			}
			cout << "num : [" << num << "]\n";
		}
		else if (strstr(token, "<CreatedItem>:"))
		{
			vector<Vector2i> createdItem;
			vector<int> itemNames;
			int num = ConvertoIntFromText(token, "<CreatedItem>:");
			GetCoordsFromText(token, num, itemNames, createdItem);
			cout << "CreatedItem : [index : " << index << "] ";

			int idx = 0;
			for (auto coord : createdItem)
			{
				m_Items[coord.y][coord.x] = new CItem((ItemName)itemNames[idx++], GetPositionCoord(coord));

				cout << "x : " << coord.x << " y : " << coord.y << " ";
			}
			cout << "\n";
		}
		else if (strstr(token, "<DeletedItem>:"))
		{
			vector<Vector2i> deletedItem;
			int num = ConvertoIntFromText(token, "<DeletedItem>:");
			GetCoordsFromText(token, num, deletedItem);
			cout << "DeletedItem : [index : " << index << "] ";

			int idx = 0;
			for (auto coord : deletedItem)
			{
				delete m_Items[coord.y][coord.x];
				m_Items[coord.y][coord.x] = nullptr;
				cout << "x : " << coord.x << " y : " << coord.y << " ";
			}
			cout << "\n";
		}
		else if (strstr(token, "<Players>:"))
		{
			vector<int> states;
			vector<int> indices;
			vector<Vector2f> positions;
			int num = ConvertoIntFromText(token, "<Players>:");
			GetPlayerInfoFromText(token, num, indices, states, positions);
			//cout << "Players : ";
			
			int i = 0;
			 
			for (int i = 0; i < indices.size(); ++i)
			{ //
				//if (m_Players[indices[i]] == m_Player) continue;
				if (m_Players[indices[i]]->GetState() != (PlayerState)states[indices[i]])
					m_Players[indices[i]]->ChangeState((PlayerState)states[indices[i]]);
				m_Players[indices[i]]->SetPosition(positions[indices[i]]);
			}
			//cout << "\n";
		}
		token = strtok(NULL, "\n");
	}
}

bool CGameScene::ProcessInput(UCHAR* pKeysBuffer)
{
	if (m_Player == nullptr) return false;
	if (pKeysBuffer[VK_UP] & 0xF0)
		m_Player->Move(Direction::up);
	if (pKeysBuffer[VK_DOWN] & 0xF0)
		m_Player->Move(Direction::down);
	if (pKeysBuffer[VK_LEFT] & 0xF0)
		m_Player->Move(Direction::left);
	if (pKeysBuffer[VK_RIGHT] & 0xF0)
		m_Player->Move(Direction::right);

	return false;
}

LRESULT CGameScene::ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

void CGameScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void CGameScene::ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
		if (!m_Player)break;
		m_Player->Stop();
		break;
	}
}
void CGameScene::ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_SPACE:
	{
		if (!m_Player) break;
		Vector2D<int> coord = GetCoordinates(m_Player->GetPosition(), m_Player->GetSize());
		if (m_Map[coord.y][coord.x] == MAP_TILE_TYPE::EMPTY)
		{
			if (m_Player->CanCreateBomb()) {
				//CreateBomb(coord);
				//m_SoundManager->PlayEffect(Sound_Name::EFFECT_BOMB_SET);
				m_Player->SetCreateBombFlag(true);
			}
		}
		break;
	}
	case VK_F1:	// 0
		m_Player->ChangeState(PlayerState::wait);
		break;
	case VK_F2:	// 1
		m_Player->ChangeState(PlayerState::trap);
		break;
	case VK_F3:	// 2
		m_Player->ChangeState(PlayerState::die);
		break;
	case VK_F4:	// 3
		m_Player->ChangeState(PlayerState::live);
		break;
	}
}

void CGameScene::InitMap()
{
	//0, Y, R, Y, R, 0, 0, 0, B, 0, YH, R, YH, 0, YH
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
		= BlockName::HOUSE_BLUE	;
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			if (mapData[i][j] == BlockName::EMPTY) continue;

			Vector2D<float> position( OBJECT_SIZE * j + m_TileStartPosition.x, OBJECT_SIZE * i + m_TileStartPosition.y );
			//Vector2D<float> position(BLOCK_WIDTH * j +26, BLOCK_HEIGHT * i + 52);
			
			bool isCanDestroy = true;
			if (mapData[i][j] == BlockName::HOUSE_BLUE) isCanDestroy = false;
			if (mapData[i][j] == BlockName::HOUSE_RED) isCanDestroy = false;
			if (mapData[i][j] == BlockName::HOUSE_YELLOW) isCanDestroy = false;
			if (mapData[i][j] == BlockName::TREE) isCanDestroy = false;

			CBlock* block = new CBlock(mapData[i][j], position, isCanDestroy);
			
			Vector2D<int> coord = GetCoordinates(block->GetPosition(), block->GetSize());
			m_Map[coord.y][coord.x] = MAP_TILE_TYPE::BLOCK;

			m_Blocks[i][j] = block;
		}
	}
}

void CGameScene::LoadImages()
{
	// 52, 52
	m_TileStartPosition = { 26, 53 };  
	m_UIImage.Load(_T("assets/ui_bg.png"));
}

void CGameScene::CreateBomb(Vector2D<int> coordinate)
{
	Vector2D<float> position = GetPositionCoord(coordinate);

	m_Bombs[coordinate.y][coordinate.x] = new CBomb(position, m_Player->GetPower());
	m_Map[coordinate.y][coordinate.x] = MAP_TILE_TYPE::BOMB;

	m_Bombs[coordinate.y][coordinate.x]->SetPlayer(m_Player);
	m_Player->ConnectBomb(m_Bombs[coordinate.y][coordinate.x]);
	//cout << "CreateBomb : x - " << coordinate.x << " y - " << coordinate.y << "\n";
}

void CGameScene::CreateBomb()
{
	m_Player->SetCreateBombFlag(true);
}

bool CGameScene::CalcNextCoordinates(Vector2D<int>& coord, Direction dir)
{
	switch (dir)
	{
	case Direction::down:
		coord.y += 1;
		break;
	case Direction::up:
		coord.y -= 1;
		break;
	case Direction::left:
		coord.x -= 1;
		break;
	case Direction::right:
		coord.x += 1;
		break;
	default:
		break;
	}
	if (!IsInMapCoord(coord)) return false;

	if (m_Blocks[coord.y][coord.x] || m_Bombs[coord.y][coord.x]) return false;
	if (m_Items[coord.y][coord.x]) 
	{
		m_Map[coord.y][coord.x] = MAP_TILE_TYPE::EMPTY;
		SAFE_DELETE(m_Items[coord.y][coord.x]);
	}
	return true;
}
