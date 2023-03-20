#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	m_player(400, 540, 0, 0, 0)	
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	// Run and Stand

	
	if (m_state != STANDING || m_state != RUNNING || m_state !=CLIMBING)
	{
		m_player.SetVelocity(0, -200);
	}

	for (CSprite* pSprite : m_sprites)
	{
		if ((string)pSprite->GetProperty("tag") == "platform")
		{
			if (m_player.HitTest(pSprite))
			{

				if (m_player.GetY() >= pSprite->GetY() + 10)
				{
					int y = (pSprite->GetY() + 25);
					m_player.SetY(y);
				}

				if ((m_player.GetX() <= pSprite->GetX() + (pSprite->GetWidth() / 2)) && m_player.GetY() >= pSprite->GetWidth() - 10 && m_player.GetY() <= pSprite->GetWidth() + 10)
				{
					int x = (pSprite->GetX() + (pSprite->GetWidth() / 2) + 15);
					m_player.SetX(x);
				}
				else if (m_player.GetX() >= pSprite->GetX() - (pSprite->GetWidth() / 2) && m_player.GetY() >= pSprite->GetWidth() - 10 && m_player.GetY() <= pSprite->GetWidth() + 10)
				{
					int x = (pSprite->GetX() - (pSprite->GetWidth() / 2) - 15);
					m_player.SetX(x);
				}
			}
		}

		if ((string)pSprite->GetProperty("tag") == "rope")
		{
			if (m_player.HitTest(pSprite))
			{

				if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP) || IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN))
				{
					m_state = CLIMBING;
				}
			}
		}

	}

	if (m_state == CLIMBING)
	{
		for (CSprite* pSprite : m_sprites)
		{
			if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP))
			{
				m_player.Accelerate(0, 50);
				m_player.SetAnimation("climb");
			}
			else if (IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN))
			{
				m_player.Accelerate(0, -50);
				m_player.SetAnimation("climb");
			}
		}
	}

	if (m_player.GetX() >= GetWidth())
	{
		m_player.SetX(GetWidth()-2);
	}
	if (m_player.GetX() <= 0)
	{
		m_player.SetX(2);
	}
	if (m_player.GetY() < 0)
	{
		GameOver();
	}
	if (m_player.GetY() >= GetHeight())
	{
		m_player.SetY(GetHeight() - 2);
	}

	if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP) && (m_state == STANDING || m_state == RUNNING || m_state != CLIMBING))
	{
		m_player.Accelerate(0, 700);

		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
		}

	if (m_state == AIRBORNE)
	{
		m_player.Accelerate(0, -200);
	}


	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT) && m_state != CLIMBING)
	{
		m_player.Accelerate(-300, 0);
		if (m_state != RUNNING || m_side != LEFT)
			m_player.SetAnimation("run_left");
		m_state = RUNNING;
		m_side = LEFT;
	}
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT) && m_state != CLIMBING)
	{
		m_player.Accelerate(300, 0);
		if (m_state != RUNNING || m_side != RIGHT)
			m_player.SetAnimation("run_right");
		m_state = RUNNING;
		m_side = RIGHT;
	}
	else if (m_state == RUNNING)
	{

		m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
		m_state = STANDING;
	}

	for (CSprite* pEnemy : m_sprites)
	{
		if ((string)pEnemy->GetProperty("tag") == "enemy" || (string)pEnemy->GetProperty("tag") == "hazard")
		{
			if (m_player.HitTest(pEnemy))
			{
				GameOver();
			}
		}
	}

	// Pre-Update Position
	CVector v0 = m_player.GetPos();

	// Updates
	for (CSprite* pSprite : m_sprites)
		pSprite->Update(t);
	m_player.Update(t);
}

void CMyGame::OnDraw(CGraphics* g)
{
	for (CSprite *pSprite : m_sprites)
		pSprite->Draw(g);
	m_player.Draw(g);
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadImage("player.png", "stand_right", 11, 6, 0, 0, CColor::White());
	m_player.LoadImage("player.png", "stand_left", 11, 6, 0, 1, CColor::White());
	m_player.AddImage("player.png", "run_right",     11, 6, 0, 0, 10, 0, CColor::White());
	m_player.AddImage("player.png", "run_left",      11, 6, 0, 1, 10, 1, CColor::White());
	m_player.LoadImage("player.png", "jump_right",   11, 6, 3, 2, CColor::White());
	m_player.LoadImage("player.png", "jump_left",    11, 6, 3, 3, CColor::White());
	m_player.LoadImage("player.png", "crouch_right", 11, 6, 2, 4, CColor::White());
	m_player.LoadImage("player.png", "crouch_left",  11, 6, 2, 5, CColor::White());
	m_player.LoadImage("player.png", "hang",		 11, 6, 10, 2, CColor::White());
	m_player.AddImage("player.png", "climb",		 11, 6, 9, 2, 10, 2, CColor::White());
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CSprite *pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();

	CSprite *pSprite;

	switch (nLevel)
	{
	case 1:// build Level 1 sprites

		// spawn the player
		m_player.SetPos(20, 530);
		m_player.SetImage("stand_right");

		// spawn all other sprites here ...

		// platforms
		pSprite = new CSpriteRect(400, 10, 800, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		pSprite = new CSpriteRect(200, 500, 400, 20, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "platform");
		m_sprites.push_back(pSprite);

		// Enemies
		pSprite = new CSprite(20, 30, "skull20.png", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "enemy");
		pSprite->SetVelocity(100, 0);
		pSprite->SetOmega(3.82 * 100);
		m_sprites.push_back(pSprite);

		//Ropes
		pSprite = new CSpriteRect(400, 300, 5, 400, CColor::Black(), CColor::White(), GetTime());
		pSprite->SetProperty("tag", "rope");
		m_sprites.push_back(pSprite);


		break;

	case 2:// Level 2 (EMPTY)

		break;
	}

	// any initialisation common to all levels
	m_player.SetImage("stand_right");
	m_player.SetVelocity(0, 0);
	m_state = STANDING;
	m_side = RIGHT;
}

// called when the game is over
void CMyGame::OnGameOver()
{
	PauseGame();
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
