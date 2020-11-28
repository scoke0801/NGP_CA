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
	for (int i = 0; i < 4; ++i) SAFE_DELETE(m_Players[i]);
	for (int i = 0; i < MAP_HEIGHT; ++i)
	{
		for (int j = 0; j < MAP_WIDTH; ++j)
		{
			SAFE_DELETE(m_Blocks[i][j]);
			SAFE_DELETE(m_Bombs[i][j]);
			SAFE_DELETE(m_Items[i][j]);
		}
	} 
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
		if (i == m_ClientIdx) continue;
		if (i % 2 != 0)
			m_Players[i] = new CPlayer(Positions[4], PlayerName::Dao);
		else
			m_Players[i] = new CPlayer(Positions[4]);
	}
	if (m_Players[m_ClientIdx])
	{
		SAFE_DELETE(m_Players[m_ClientIdx]);	
	}
	//m_Players[m_ClientIdx] = new CPlayer(Positions[m_ClientIdx], PlayerName::Dao);
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

				//CPlayer* player = m_Bombs[i][j]->GetPlayer();
				//player->EraseBomb(m_Bombs[i][j]);
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
	if (m_Player->IsAlive())
	{
		static float gap = 0;
		static bool up = true;
		Vector2f pos = m_Player->GetPosition();
		m_PlayerArrow.TransparentBlt(hdc, 
			pos.x + 14, pos.y - 28 - gap, 24, 28, 
			0, 0, 24, 28, 
			RGB(255, 0, 255));

		if (up)
		{
			gap += 0.1;
			if (gap > 7.0f)  up = false;
		}
		else
		{
			gap -= 0.1;
			if (gap < 0.0f) up = true;
		}
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
	data.bombNum = m_Player->GetMaxBomb();
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

	toSendData += "<BombNum>:";
	toSendData += to_string(data.bombNum);
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
		if (strstr(token, "<IsGameEnd>:"))
		{
			//cout << "<IsGameEnd>: " << boolalpha << (bool)ConvertoIntFromText(token, "<IsGameEnd>:") << " \n";
		} 
		else if (strstr(token, "<Players>:"))
		{
			vector<int> states;
			vector<int> indices;
			vector<Vector2f> positions;
			int num = ConvertoIntFromText(token, "<Players>:");
			
			for (int i = 0; i < num; ++i)
			{
				token = strtok(NULL, "\n");
				char temp[30] = {};
				strcpy(temp, token);
				int index = atoi(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				int state = atoi(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				float posX = atof(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				float posY = atof(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				int speed = atoi(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				int power = atoi(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				int bombNum = atoi(temp);

				token = strtok(NULL, "\n");
				strcpy(temp, token);
				int direction = atoi(temp);
				if (m_Players[index]->GetState() != (PlayerState)state)
				{
					if((PlayerState)state != PlayerState::die)
						m_Players[index]->ChangeState((PlayerState)state);
				}
					
				m_Players[index]->SetPosition({ posX, posY });
				m_Players[index]->SetPower(power);
				m_Players[index]->SetSpeed(speed);
				m_Players[index]->SetBombNum(bombNum);
				m_Players[index]->SetDirection((Direction)direction);
			}
			int stop = 3; 
		}
		else if (strstr(token, "<Map>:"))
		{
			token = strtok(NULL, "\n");
			MapDatas m_MapToDatas[MAP_HEIGHT][MAP_WIDTH];
			for (int i = 0; i < MAP_HEIGHT; ++i)
			{
				//token = strtok(NULL, "\n");
				char temp[30] = {};
				int k = 0;
				for (int j = 0; j < MAP_WIDTH; ++j)
				{
					MapDatas mapData  = (MapDatas)atoi(strncpy(temp, token, 2));
					m_MapToDatas[i][j] = mapData;

					token = strtok(NULL, "\n");
				} 
			}
			for (int i = 0; i < MAP_HEIGHT; ++i)
			{
				for (int j = 0; j < MAP_WIDTH; ++j)
				{
					switch (m_MapToDatas[i][j])
					{
					case MapDatas::BlockDeleted:
						if (!m_Blocks[i][j]) break; 
						m_Blocks[i][j]->ChangeState(BlockState::Destroyed);
						break;
					case MapDatas::ItemCreated_Ballon:
					case MapDatas::ItemCreated_Nuclear:
					case MapDatas::ItemCreated_Potion:
					case MapDatas::ItemCreated_Skate:
					{
						if (m_Items[i][j])break;
						if (m_Blocks[i][j]) 
							m_Blocks[i][j]->ChangeState(BlockState::Destroyed);
						int itemName = (int)m_MapToDatas[i][j];
						itemName -= (int)MapDatas::ItemCreated_Ballon;
						m_Items[i][j] = new CItem((ItemName)itemName, GetPositionCoord({ j, i }));
						break;
					}
					case MapDatas::ItemDeleted:
						if (!m_Items[i][j])break;
						SAFE_DELETE(m_Items[i][j]);
						break;
					case MapDatas::BombCreated_0:
					case MapDatas::BombCreated_1:
					case MapDatas::BombCreated_2:
					case MapDatas::BombCreated_3:
					case MapDatas::BombCreated_4:
					case MapDatas::BombCreated_5:
					case MapDatas::BombCreated_6:
					case MapDatas::BombCreated_7:
					case MapDatas::BombCreated_8:
					case MapDatas::BombCreated_9:
					{
						if (m_Bombs[i][j]) break;
						int power = (int)m_MapToDatas[i][j];
						power -= (int)MapDatas::BombCreated_0;
						Vector2i mapCoord = { j,i };
						Vector2i playerCoord = m_Player->GetCoordinate();
						mapCoord = { j, i };
						m_Bombs[i][j] = new CBomb(GetPositionCoord(mapCoord), power);
						if (mapCoord == playerCoord)
						{
							m_Bombs[i][j]->SetPlayer(m_Player);
							m_Player->IncreaseCurrentBombNum();
						}
						break;
					}
					case MapDatas::BombDeleted:
						if (!m_Bombs[i][j])break;
						if (m_Bombs[i][j]->IsOnExplosion())break;
						if (m_Bombs[i][j]->GetPlayer() == m_Player)
						{
							m_Player->DecreaseCurrentBombNum();
						}
						m_Bombs[i][j]->ChangeState(BombState::Explosion);
						break;
					}
				}
			}
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
	m_PlayerArrow.Load(_T("assets/player/solo_player.bmp"));
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
