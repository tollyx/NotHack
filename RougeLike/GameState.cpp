#include "stdafx.h"
#include "GameState.h"
#include "MainmenuState.h"
#include "TileManager.h"
#include "TileMap.h"
#include "Player.h"
#include "AudioManager.h"
#include "Sprite.h"
#include "DungeonGenerator.h"
#include "InputManager.h"
#include <chrono>

GameState::GameState(System& p_xSystem)
{
	m_xSystem = p_xSystem;
	m_pxMap = nullptr;
}

GameState::~GameState()
{
	
}

void GameState::Enter()
{
	m_xCamera.x = 0;
	m_xCamera.y = 0;
	m_xCamera.w = m_xSystem.m_pxTileManager->GetWindowTileWidth();
	m_xCamera.h = m_xSystem.m_pxTileManager->GetWindowTileHeight() - 2;

	m_iLevelDepth = 0;
	m_iTurns = 0;

	m_pxPlayer = new Player(0, 0);
	m_apxEntities.push_back(m_pxPlayer);
	NewMap();
}

bool GameState::Update(float p_fDeltaTime)
{
	bool update = false;
	int dx = 0, dy = 0;
	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_UP) || 
		m_xSystem.m_pxInputManager->IsKeyDown(SDLK_KP_8))
	{
		dy--;
		update = true;
	}
	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_LEFT) ||
		m_xSystem.m_pxInputManager->IsKeyDown(SDLK_KP_4))
	{
		dx--;
		update = true;
	}
	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_DOWN) ||
		m_xSystem.m_pxInputManager->IsKeyDown(SDLK_KP_2))
	{
		dy++;
		update = true;
	}
	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_RIGHT) ||
		m_xSystem.m_pxInputManager->IsKeyDown(SDLK_KP_6))
	{
		dx++;
		update = true;
	}
	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_PERIOD) ||
		m_xSystem.m_pxInputManager->IsKeyDown(SDLK_KP_5))
	{
		update = true;
	}

	if (m_xSystem.m_pxInputManager->IsKeyDown(SDLK_ESCAPE))
	{
		return false;
	}
	
	if (update) {
		if (m_pxMap->GetTile(m_pxPlayer->GetX() + dx, m_pxPlayer->GetY() + dy).isSolid == false)
		{
			m_pxPlayer->Move(dx, dy);

			m_xCamera.x = m_pxPlayer->GetX() - m_xCamera.w / 2;
			m_xCamera.y = m_pxPlayer->GetY() - m_xCamera.h / 2;
		}

		SDL_Point exitPos = m_pxMap->GetExit();
		if ( exitPos.x == m_pxPlayer->GetX() && exitPos.y == m_pxPlayer->GetY())
		{
			m_iLevelDepth++;
			NewMap();
		}
		else
		{
			auto it = m_apxEntities.begin();
			while (it != m_apxEntities.end())
			{
				(*it)->Update();
				it++;
			}
		}
		m_iTurns++;
	}

	if (m_pxPlayer->GetHP() <= 0)
	{
		return false;
	}

	return true;
}

void GameState::Exit()
{
	delete m_pxMap;
	m_pxMap = nullptr;
	{
		auto it = m_apxEntities.begin();
		while (it != m_apxEntities.end())
		{
			delete (*it);
			it++;
		}
	}
}

void GameState::Draw()
{
	m_xSystem.m_pxTileManager->DrawTileMap(m_pxMap, 0, 1, 
		m_xCamera.x, m_xCamera.y, 
		m_xCamera.w, m_xCamera.h);

	auto it = m_apxEntities.begin();
	while (it != m_apxEntities.end())
	{
		m_xSystem.m_pxTileManager->DrawTile((*it)->GetTile(), (*it)->GetX() - m_xCamera.x, (*it)->GetY() - m_xCamera.y + 1);

		it++;
	}

	// Health and max health
	m_xSystem.m_pxTileManager->DrawText(
		"HP:" + std::to_string(m_pxPlayer->GetHP()) + "/" + std::to_string(m_pxPlayer->GetMaxHP()),
		2, m_xSystem.m_pxTileManager->GetWindowTileHeight() - 1);

	// Level
	m_xSystem.m_pxTileManager->DrawText("LVL:" + std::to_string(m_pxPlayer->GetLvl()),
		11, m_xSystem.m_pxTileManager->GetWindowTileHeight() - 1);

	// Strength
	m_xSystem.m_pxTileManager->DrawText("STR:" + std::to_string(m_pxPlayer->GetSTR()),
		18, m_xSystem.m_pxTileManager->GetWindowTileHeight() - 1);

	// Defence
	m_xSystem.m_pxTileManager->DrawText("DEF:" + std::to_string(m_pxPlayer->GetDEF()),
		25, m_xSystem.m_pxTileManager->GetWindowTileHeight() - 1);

	// Experience to next level
	m_xSystem.m_pxTileManager->DrawText("NXT:" + std::to_string(m_pxPlayer->GetXpToNextLvl()),
		32, m_xSystem.m_pxTileManager->GetWindowTileHeight() - 1);
}

IState * GameState::NextState()
{
	return new MainmenuState(m_xSystem); // TODO: gameover state
}

void GameState::NewMap()
{
	if (m_pxMap != nullptr)
	{
		delete m_pxMap;
	}
	m_pxMap = DungeonGenerator::GenerateMap(
		m_xCamera.w / 2 + m_iLevelDepth * 2, 
		m_xCamera.h / 2 + m_iLevelDepth * 2, 
		16 + m_iLevelDepth * 2, 
		std::chrono::system_clock::now().time_since_epoch().count());
	SDL_Point entrancePos = m_pxMap->GetEntrance();
	m_pxPlayer->SetPos(entrancePos.x, entrancePos.y);
	m_xCamera.x = m_pxPlayer->GetX() - m_xCamera.w / 2;
	m_xCamera.y = m_pxPlayer->GetY() - m_xCamera.h / 2;
}
