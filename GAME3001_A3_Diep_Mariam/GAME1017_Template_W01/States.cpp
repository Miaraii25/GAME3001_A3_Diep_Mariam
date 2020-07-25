#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "Button.h"
#include <iostream>
#include <fstream>

#include "CollisionManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "PathManager.h"
#include <ctime>

using namespace std;
// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.


// Begin GameState
GameState::GameState() {}

void GameState::Enter()
{
	std::cout << "Entering GameState..." << std::endl;
	srand((unsigned)time(NULL));
	m_pTileText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Tiles.png");
	m_pPlayerText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Maga.png");
	m_pEnemyText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Enemies.png");
	m_pCircleText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Circle.png");
	m_pBulletText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Enemies.png");

	SOMA::Load("Aud/adventure.wav", "adventure", SOUND_MUSIC);
	SOMA::Load("Aud/boom.wav", "Boom", SOUND_SFX);
	SOMA::Load("Aud/death.wav", "Death", SOUND_SFX);
	SOMA::Load("Aud/PressM.wav", "PressM", SOUND_SFX);
	SOMA::Load("Aud/PressR.wav", "PressR", SOUND_SFX);
	SOMA::Load("Aud/PressF.wav", "PressF", SOUND_SFX);
	SOMA::Load("Aud/PressH.mp3", "PressH", SOUND_SFX);
	SOMA::Load("Aud/Laser2.wav", "Shoot", SOUND_SFX);

	SOMA::SetMusicVolume(15);
	SOMA::PlayMusic("adventure", -1, 3000);

	SDL_Color black = { 0, 0, 0, 0 };

	m_pInstruct[0] = new Label("standard", 35, 40, "Press R to restart the play scene", black);
	m_pInstruct[1] = new Label("standard", 35, 55, "Press H to toggle the Debug view", black);
	m_pInstruct[2] = new Label("standard", 35, 70, "Press P to toggle idle/patrol mode of enemies ", black);
	m_pInstruct[3] = new Label("standard", 35, 85, "Press K to take damage", black);
	m_pInstruct[4] = new Label("standard", 35, 100, "Press M to move actor", black); 
	m_pInstruct[5] = new Label("standard", 35, 115, "Right click to fire projectile", black);
	///m_pInstruct[6] = new Label("standard", 35, 130, "Left click on the mouse to have a close contact", black);
	//m_pInstruct[3] = new Label("standard", 35, 85, "Press F to find the shortest path (in debug view)", black);

	// Add player 
	m_pPlayer = new Player({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pPlayerText, 0, 0, 0, 4);

	// Add enemies
	m_enemies.push_back(new BaseEnemy({ 0,0,40,57 }, { (float)(17) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 4, 4, 4, 4, 0.0f, 0, -1, m_pPlayer->GetDstP()));
	m_enemies.push_back(new BaseEnemy({ 0,0,40,57 }, { (float)(6) * 32, (float)(10) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 4, 4, 4, 4, 90.0f, 1, 0, m_pPlayer->GetDstP()));
	m_enemies.push_back(new BaseEnemy({ 0,0,40,57 }, { (float)(22) * 32, (float)(11) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 4, 4, 4, 4, 180.0f, 0, 1, m_pPlayer->GetDstP()));
	m_enemies.push_back(new BaseEnemy({ 0,0,40,57 }, { (float)(23) * 32, (float)(15) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 4, 4, 4, 4, 270.0f, -1, 0, m_pPlayer->GetDstP()));

	// Add bullet
	m_pBullet.reserve(10);
	

	ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			m_tiles.emplace(key, new Tile({ x * 32, y * 32, 32, 32 }, { 0,0,32,32 }, Engine::Instance().GetRenderer(), m_pTileText, o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				inFile >> key;
				Engine::Instance().GetLevel()[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				Engine::Instance().GetLevel()[row][col]->GetDstP()->x = (float)(32 * col);
				Engine::Instance().GetLevel()[row][col]->GetDstP()->y = (float)(32 * row);
				// Instantiate the labels for each tile.
				Engine::Instance().GetLevel()[row][col]->m_lCost = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 4, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 18, " ", { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lX = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 18, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, to_string(col).c_str(), { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lY = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 2, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, to_string(row).c_str(), { 0,0,0,255 });
				// Construct the Node for a valid tile.
				if (!Engine::Instance().GetLevel()[row][col]->IsObstacle() && !Engine::Instance().GetLevel()[row][col]->IsHazard())
					Engine::Instance().GetLevel()[row][col]->m_node = new PathNode((int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->x), (int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->y));
			}
		}
	}
	inFile.close();
	// Now build the graph from ALL the non-obstacle and non-hazard tiles. Only N-E-W-S compass points.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			if (Engine::Instance().GetLevel()[row][col]->Node() == nullptr) // Now we can test for nullptr.
				continue; // An obstacle or hazard tile has no connections.
			// Make valid connections. Inside map and a valid tile.
			if (row - 1 != -1 && Engine::Instance().GetLevel()[row - 1][col]->Node() != nullptr) // Tile above. 
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row - 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row - 1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row - 1][col]->Node()->y)));
			if (row + 1 != ROWS && Engine::Instance().GetLevel()[row + 1][col]->Node() != nullptr) // Tile below.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row + 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row + 1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row + 1][col]->Node()->y)));
			if (col - 1 != -1 && Engine::Instance().GetLevel()[row][col - 1]->Node() != nullptr) // Tile to Left.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col - 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col - 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col - 1]->Node()->y)));
			if (col + 1 != COLS && Engine::Instance().GetLevel()[row][col + 1]->Node() != nullptr) // Tile to right.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col + 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col + 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col + 1]->Node()->y)));
		}
	}
}

void GameState::Update()
{

	// Reset the scene
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
	{
		SOMA::PlaySound("PressR", 0, 1);
		cout << "Reset Game State" << endl;
		STMA::PushState(new GameState);
	}

	if (EVMA::KeyPressed(SDL_SCANCODE_H))// Toggle debug mode.
	{
		m_showCosts = !m_showCosts; //=> Not support grisp now
		for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
			(*it)->SetDebugMode(m_showCosts);
		}
		SOMA::PlaySound("PressH", 0, 3);
	}

	if (1)
	{
		if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
		{
			m_hEuclid = !m_hEuclid;
			std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
		}
		if (EVMA::MousePressed(1) || EVMA::MousePressed(3)) // If user has clicked.
		{

			if (EVMA::MousePressed(1)) // Move the player with left-click.
			{
				//m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
				//m_pPlayer->GetDstP()->y = (float)(yIdx * 32);
				//m_pBullet->Shoot({ (int)(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w / 2) , (int)(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h / 2) }, m_pPlayer->GetDirection());
				m_pBullet.push_back(new Bullet({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pBulletText, 0, 0, 0, 4,
							 (float)(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w * 0.1), (float)(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h * 0.3), m_pPlayer->GetDirection()));
			
				SOMA::SetSoundVolume(5, 4);
				SOMA::PlaySound("Shoot", 0, 4);
			}

		}
	}
	m_pPlayer->Update(); // Just stops MagaMan from moving.

	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
		(*it)->Update();
	}

	for (int i = 0; i < m_pBullet.size(); i++)
	{
		m_pBullet[i]->Update();
	}

	for (int i = 0; i < m_pBullet.size(); i++)
	{
		if (!m_pBullet[i]->isDied())
		{
			for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
				if (COMA::AABBCheck(*m_pBullet[i]->GetDstP(), *((*it)->GetDstP())))
				{
					(*it)->Collision();
					m_pBullet[i]->Collision();
				}

			}
		}
	}
}

void GameState::Render()
{
	// Draw anew.

	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			Engine::Instance().GetLevel()[row][col]->Render(); // Render each tile.
			// Render the debug data...
			if (m_showCosts && Engine::Instance().GetLevel()[row][col]->Node() != nullptr)
			{
				Engine::Instance().GetLevel()[row][col]->m_lCost->Render();
				Engine::Instance().GetLevel()[row][col]->m_lX->Render();
				Engine::Instance().GetLevel()[row][col]->m_lY->Render();
				// I am also rendering out each connection in blue. If this is a bit much for you, comment out the for loop below.
				for (unsigned i = 0; i < Engine::Instance().GetLevel()[row][col]->Node()->GetConnections().size(); i++)
				{
					DEMA::QueueLine({ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->x + 16,Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->y + 16 },
						{ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->x + 16, Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->y + 16 }, { 0,0,255,255 });
				}
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		m_pInstruct[i]->Render();
	}

	m_pPlayer->Render();
	DrawHealthBar({ (int)m_pPlayer->GetDstP()->x - 16, (int)m_pPlayer->GetDstP()->y - 16, 64,16 }, m_pPlayer->getHealthLevel());

	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
		(*it)->Render();
		DrawHealthBar({ (int)(*it)->GetDstP()->x - 16, (int)(*it)->GetDstP()->y - 16, 64,16 }, (*it)->getHealthLevel());
	}

	for (int i = 0; i < m_pBullet.size(); i++)
	{
		m_pBullet[i]->Render();
	}

	PAMA::DrawPath(); // I save the path in a static vector to be drawn here.
	DEMA::FlushLines(); // And... render ALL the queued lines. Phew.
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void GameState::DrawHealthBar(SDL_Rect rect, int level)
{
	SDL_Rect liveRect;
	liveRect.x = rect.x;
	liveRect.y = rect.y;
	liveRect.w = (rect.w * level) / 100;
	liveRect.h = rect.h;

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 250, 0, 0, 255);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 255);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &liveRect);
}

void GameState::Exit()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete Engine::Instance().GetLevel()[row][col];
			Engine::Instance().GetLevel()[row][col] = nullptr; // Wrangle your dangle.
		}
	}
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
}

void GameState::Resume() {}
//End GameState

// Begin TitleState
TitleState::TitleState() {}

void TitleState::Enter()
{
	SDL_Color black = { 0, 0, 0, 0 };
	
	m_pNameLabel1 = new Label("UI", 240, 90, "DIEP, LY BAO LONG (ID: 101277290)", black);
	m_pNameLabel2 = new Label("UI", 240, 120, "MARIAM OGUNLESI (ID: 101285729)", black);
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 320,320,400,80 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("playBut"));
	m_pGameStart = new Sprite({ 0,0, 700, 600 }, { 0,0,1024, 768 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("StartScene"));
	
	SOMA::Load("Aud/opening.mp3", "Opening", SOUND_MUSIC);
	SOMA::SetMusicVolume(15);
	SOMA::PlayMusic("Opening", -1, 3000);
}
void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
}

void TitleState::Render()
{

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 115, 90, 90);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	//m_pStartLabel->Render();
	m_pGameStart->Render();
	m_pNameLabel1->Render();
	m_pNameLabel2->Render();
	m_playBtn->Render();
	State::Render();
}


void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}
// End GameState