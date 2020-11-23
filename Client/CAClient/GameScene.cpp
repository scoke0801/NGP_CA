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

	Vector2D<float> PlayerPos(m_TileStartPosition.x + OBJECT_SIZE * 13,
		m_TileStartPosition.y + OBJECT_SIZE * 1); 
	m_Players.push_back(new CPlayer(PlayerPos));

	PlayerPos.x = m_TileStartPosition.x + OBJECT_SIZE * 0;
	PlayerPos.y = m_TileStartPosition.y + OBJECT_SIZE * 1;
	m_Players.push_back(new CPlayer(PlayerPos));

	PlayerPos.x = m_TileStartPosition.x + OBJECT_SIZE * 1;
	PlayerPos.y = m_TileStartPosition.y + OBJECT_SIZE * 11;
	m_Players.push_back(new CPlayer(PlayerPos));

	PlayerPos.x = m_TileStartPosition.x + OBJECT_SIZE * 14;
	PlayerPos.y = m_TileStartPosition.y + OBJECT_SIZE * 11;
	m_Players.push_back(new CPlayer(PlayerPos));

	string id = "Player ";
	for (int i = 0; i < m_Players.size(); ++i)
	{
		m_Players[i]->SetIndex(i);
		m_Players[i]->SetID(id + to_string(i));
	}
	m_Type = SceneType::GameScene;
}

CGameScene::~CGameScene()
{
	m_SoundManager->Stop();
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

			if (m_Bombs[i][j]->IsTimeToExplose())
			{

				//m_SoundManager->PlayEffect(Sound_Name::EFFECT_BOMB_WAVE);
				m_Bombs[i][j]->ChangeState(BombState::Explosion);
			}

			if (m_Bombs[i][j]->IsOnExplosion())
			{
				const int power = m_Bombs[i][j]->GetPower();

				vector<Vector2i> coords;
				Vector2i coord;

				for (int z = 1; z < power; ++z)//down
				{
					coord = { j, i + z };	
					coords.push_back(coord);
					if (!IsInMapCoord(coord)) continue;
					if (m_Items[coord.y][coord.x]) SAFE_DELETE(m_Items[coord.y][coord.x]);
					if (!m_Blocks[coord.y][coord.x]) continue;
					if (!m_Blocks[coord.y][coord.x]->IsCanDestroy()) break;
					m_Blocks[coord.y][coord.x]->ChangeState(BlockState::Destroyed);
					break;
				}
					
				for (int z = 1; z < power; ++z) //up
				{
					coord = { j, i - z }; 
					coords.push_back(coord);
					if (!IsInMapCoord(coord)) continue; 
					if (m_Items[coord.y][coord.x])  SAFE_DELETE(m_Items[coord.y][coord.x]);
					if (!m_Blocks[coord.y][coord.x]) continue;
					if (!m_Blocks[coord.y][coord.x]->IsCanDestroy())
					{
						auto iter = find(coords.begin(), coords.end(), coord);
						if(iter != coords.end()) coords.erase(iter);
						break;
					}
					m_Blocks[coord.y][coord.x]->ChangeState(BlockState::Destroyed);
					{
						auto iter = find(coords.begin(), coords.end(), coord);
						if (iter != coords.end()) coords.erase(iter);
						break;
					} 
				}
				for (int z = 1; z < power; ++z)//left
				{
					coord = { j - z, i };
					coords.push_back(coord);
					if (!IsInMapCoord(coord)) continue;
					if (m_Items[coord.y][coord.x]) SAFE_DELETE(m_Items[coord.y][coord.x]);
					if (!m_Blocks[coord.y][coord.x]) continue;
					if (!m_Blocks[coord.y][coord.x]->IsCanDestroy()) {
						auto iter = find(coords.begin(), coords.end(), coord);
						if (iter != coords.end()) coords.erase(iter);
						break;
					}
					m_Blocks[coord.y][coord.x]->ChangeState(BlockState::Destroyed);
					{
						auto iter = find(coords.begin(), coords.end(), coord);
						if (iter != coords.end()) coords.erase(iter);
						break;
					}
				}
				for (int z = 1; z < power; ++z) //right
				{
					coord = { j + z, i };
					coords.push_back(coord);
					if (!IsInMapCoord(coord)) continue;
					if (m_Items[coord.y][coord.x]) SAFE_DELETE(m_Items[coord.y][coord.x]);
					if (!m_Blocks[coord.y][coord.x]) continue;
					if (!m_Blocks[coord.y][coord.x]->IsCanDestroy()) {
						auto iter = find(coords.begin(), coords.end(), coord);
						if (iter != coords.end()) coords.erase(iter);
						break;
					}
					m_Blocks[coord.y][coord.x]->ChangeState(BlockState::Destroyed);
					{
						auto iter = find(coords.begin(), coords.end(), coord);
						if (iter != coords.end()) coords.erase(iter);
						break;
					}
				}

				coords.push_back({ j, i });
				for (auto coord : coords)
				{
					if (!IsInMapCoord(coord)) continue;
					if (!m_Bombs[coord.y][coord.x]) continue; 
					m_Bombs[coord.y][coord.x]->SetLastBranchCoords(coords);

					if (m_Bombs[coord.y][coord.x]->GetState() != BombState::Wait) continue; 
					m_Bombs[coord.y][coord.x]->ChangeState(BombState::Explosion);
					//m_SoundManager->PlayEffect(Sound_Name::EFFECT_BOMB_WAVE);
				}
				m_Bombs[i][j]->SetLastBranchCoords(coords);
			}

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

				int itemCreate = rand() % 10;
				int itemName = rand() % (int)ItemName::count;
				if (itemCreate <= 2)
					m_Items[i][j] = new CItem((ItemName)itemName, GetPositionCoord(coord));

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
		// 플레이어 - 블록 충돌 처리
		for (int i = 0; i < MAP_HEIGHT; ++i)
		{
			for (int j = 0; j < MAP_WIDTH; ++j)
			{
				if (m_Blocks[i][j] == nullptr) continue;

				bool bCollide = player->IsCollide(m_Blocks[i][j]);
				bool bMovable = m_Blocks[i][j]->IsCanMove();
				
				if (bCollide)
				{
					//player->FixCollision();
				}
				else continue;;
				if (m_Blocks[i][j]->GetIsOnMove()) continue;
				if (bMovable)
				{ 
					Direction dir = player->GetDirection();
					Vector2i coord(j, i);
					if (!CalcNextCoordinates(coord, dir)) continue;
					m_Blocks[i][j]->SetIsOnMove(true, coord);
					m_Blocks[i][j]->SetDirection(dir);
					m_Blocks[coord.y][coord.x] = m_Blocks[i][j];
					m_Map[i][j] = MAP_TILE_TYPE::EMPTY;
					m_Blocks[i][j] = nullptr;
				}
			}
		}
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

		// 플레이어 - 물풍선 충돌 처리
		for (int i = 0; i < MAP_HEIGHT; ++i)
		{
			for (int j = 0; j < MAP_WIDTH; ++j)
			{
				if (!m_Bombs[i][j]) continue; 
				// if (m_Bombs[i][j]->GetPlayer() == player) continue;
				Vector2i playerCoord = GetCoordinates(player->GetPosition(), player->GetSize()); 
				bool bCollide = m_Bombs[i][j]->IsCollide(player);
				
				if (bCollide)	// 일반 충돌 처리
				{
					//player->FixCollision();
				}

#ifndef _DEBUG
				if (m_Bombs[i][j]->IsOnExplosion())	// 물줄기와 플레이어 충돌처리
				{
					vector<Vector2i> branchCoords = m_Bombs[i][j]->GetLastBranchCoords();
					
					auto iter = find(branchCoords.begin(), branchCoords.end(), playerCoord);
					if (iter == branchCoords.end()) continue;

					player->ChangeState(PlayerState::die);
				}
#endif
			}
		}
		if (!player->IsInMap())
		{
			//player->FixCollision();
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
			if (m_Blocks[i][j] == nullptr) continue;
			m_Blocks[i][j]->Draw(hdc);
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
	data.playerIndex = m_Players[0]->GetIndex();
	data.position = m_Players[0]->GetPosition();
	data.waterRange = m_Players[0]->GetPower();
	data.speed = m_Players[0]->GetSpeed();
	data.direction = m_Players[0]->GetDirection();
	data.state = (int)m_Players[0]->GetState();
	//data.mapData = m_Map;

	toSendData.clear();

	toSendData = "<Position>:";
	toSendData += to_string(data.position.x);
	toSendData += " ";
	toSendData += to_string(data.position.y);
	toSendData += "\n";

	toSendData += "<PlayerIndex>:";
	toSendData += to_string(data.playerIndex);

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
	SendFrameData(sock, toSendData, retVal);

	char buffer[BUFSIZE + 1];
	int receivedSize = 0;

	RecvFrameData(sock, buffer, retVal);
	//cout << "받은 값" << buffer << "\n";
	char* token = strtok(buffer, "\n");
	int index = 0;
	Vector2f position;
	while (token != NULL)
	{
		if (strstr(token, "<PlayerIndex>:"))
		{
			index = ConvertoIntFromText(token, "<PlayerIndex>:");
			cout << "<PlayerIndex>: " << index << " \n";
		}
		else if (strstr(token, "<Position>:"))       //(token, "<position>:"))
		{
			position = GetPositionFromText(token);
			cout << "x : " << position.x << " y : " <<
				position.y << "\n";
			m_Players[index]->SetPosition(position);
		}
		else if (strstr(token, "<IsGameEnd>:"))
		{
			cout << "<IsGameEnd>: " << boolalpha << (bool)ConvertoIntFromText(token, "<IsGameEnd>:") << " \n";
		}
		else if (strstr(token, "<Speed>:"))
		{
			cout << "<Speed>: " << ConvertoIntFromText(token, "<Speed>:") << " \n";
		}
		else if (strstr(token, "<PlayerState>:"))
		{
			cout << "<PlayerState>: " << ConvertoIntFromText(token, "<PlayerState>:") << " \n";
		}
		token = strtok(NULL, "\n");
	}
}

bool CGameScene::ProcessInput(UCHAR* pKeysBuffer)
{
	if (pKeysBuffer[VK_UP] & 0xF0)
		m_Players[0]->Move(Direction::up);
	if (pKeysBuffer[VK_DOWN] & 0xF0)
		m_Players[0]->Move(Direction::down);
	if (pKeysBuffer[VK_LEFT] & 0xF0)
		m_Players[0]->Move(Direction::left);
	if (pKeysBuffer[VK_RIGHT] & 0xF0)
		m_Players[0]->Move(Direction::right);

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
		m_Players[0]->Stop();
		break;
	}
}
void CGameScene::ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_SPACE:
	{
		Vector2D<int> coord = GetCoordinates(m_Players[0]->GetPosition(), m_Players[0]->GetSize());
		if (m_Map[coord.y][coord.x] == MAP_TILE_TYPE::EMPTY)
		{
			if (m_Players[0]->CanCreateBomb()) {
				CreateBomb(coord);
				//m_SoundManager->PlayEffect(Sound_Name::EFFECT_BOMB_SET);
			}
		}
		break;
	}
	case VK_F1:	// 0
		m_Players[0]->ChangeState(PlayerState::wait);
		break;
	case VK_F2:	// 1
		m_Players[0]->ChangeState(PlayerState::trap);
		break;
	case VK_F3:	// 2
		m_Players[0]->ChangeState(PlayerState::die);
		break;
	case VK_F4:	// 3
		m_Players[0]->ChangeState(PlayerState::live);
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

	m_Bombs[coordinate.y][coordinate.x] = new CBomb(position, m_Players[0]->GetPower());
	m_Map[coordinate.y][coordinate.x] = MAP_TILE_TYPE::BOMB;

	m_Bombs[coordinate.y][coordinate.x]->SetPlayer(m_Players[0]);
	m_Players[0]->ConnectBomb(m_Bombs[coordinate.y][coordinate.x]);
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
