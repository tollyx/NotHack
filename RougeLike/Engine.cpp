#include "stdafx.h"
#include "Engine.h"
#include "DrawManager.h"
#include "SpriteManager.h"
#include "AudioManager.h"
#include "StateManager.h"
#include "IState.h"
#include "MainmenuState.h"
#include "GameState.h"
#include "Sprite.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "InputManager.h"
#include <iostream>

Engine::Engine()
{
	m_bRunning = false;
	m_pxDrawManager = nullptr;
	m_pxSpriteManager = nullptr;
	m_pxStateManager = nullptr;
	m_pxMouse = nullptr;
}

Engine::~Engine()
{

}


bool Engine::Initialize()
{
	// The initialize function will intialize libraries the program depends on and all manager we will create.
	 
	// Initializes the SDL library
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	// Creates a new DrawManager and calls Initialize with width / height parameters.
	m_pxDrawManager = new DrawManager();
	if (m_pxDrawManager->Initialize(60 * 12, 40 * 12) == false)
	{
		return false;
	}

	m_pxAudioManager = new AudioManager();
	if (m_pxAudioManager->Initialize() == false)
	{
		return false;
	}

	m_pxMouse = new Mouse();
	m_pxKeyboard = new Keyboard();

	m_pxInputManager = new InputManager(m_pxMouse, m_pxKeyboard);

	m_pxSpriteManager = new SpriteManager(m_pxDrawManager->GetRenderer());

	m_pxStateManager = new StateManager();

	System system;
	system.m_iScreenWidth = 60 * 12;
	system.m_iScreenHeight = 40 * 12;
	system.m_pxDrawManager = m_pxDrawManager;
	system.m_pxSpriteManager = m_pxSpriteManager;
	system.m_pxAudioManager = m_pxAudioManager;
	system.m_pxInputManager = m_pxInputManager;
	m_pxStateManager->SetState(new MainmenuState(system));

	m_bRunning = true;

	return true;
}

void Engine::Shutdown()
{
	// The shutdown function will quit, delete and shutdown everything we have started up or created in initialize (In reverse order of creation)
	delete m_pxSpriteManager;
	m_pxSpriteManager = nullptr;

	delete m_pxStateManager;
	m_pxStateManager = nullptr;
	
	delete m_pxMouse;
	m_pxMouse = nullptr;

	delete m_pxKeyboard;
	m_pxKeyboard = nullptr;

	m_pxAudioManager->Shutdown();
	delete m_pxAudioManager;
	m_pxAudioManager = nullptr;

	// Shuts down the drawmanager before deleting the object and nulling the pointer.
	m_pxDrawManager->Shutdown();
	delete m_pxDrawManager;
	m_pxDrawManager = nullptr;

	SDL_Quit();
}

void Engine::Update()
{
	// Our engines core loop
	while (m_bRunning)
	{
		if (HandleEvents()) // Only updates if needed, in other words, if a keypress happened.
		{
			if (!m_pxStateManager->Update())
			{
				m_bRunning = false;
			}
		}
		m_pxDrawManager->Clear();
		m_pxStateManager->Draw();

		m_pxDrawManager->Present();
		SDL_Delay(1000 / 60);
	}
}

bool Engine::HandleEvents()
{
	bool needsUpdate = false;
	SDL_Event xEvent;
	while (SDL_PollEvent(&xEvent))
	{
		if (xEvent.type == SDL_QUIT)
		{
			m_bRunning = false;
		}
		else if (xEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			m_pxMouse->SetButton(xEvent.button.button, true);
			//needsUpdate = true;
		}
		else if (xEvent.type == SDL_MOUSEBUTTONUP)
		{
			m_pxMouse->SetButton(xEvent.button.button, false);
		}
		else if (xEvent.type == SDL_MOUSEMOTION)
		{
			m_pxMouse->SetPosition(xEvent.motion.x, xEvent.motion.y);
		}
		else if (xEvent.type == SDL_KEYDOWN)
		{
			printf("Keydown: %i\n", xEvent.key.keysym.sym);
			m_pxKeyboard->SetKey(xEvent.key.keysym.sym, true);
			needsUpdate = true;
		}
		else if (xEvent.type == SDL_KEYUP)
		{
			printf("Keyup: %i\n", xEvent.key.keysym.sym);
			m_pxKeyboard->SetKey(xEvent.key.keysym.sym, false);
			//needsUpdate = true;
		}
	}
	return needsUpdate;
}