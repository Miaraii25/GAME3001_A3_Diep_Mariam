#include "BaseEnemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "DebugManager.h"
#include "Engine.h"
#define SPEED 2
#define RADIUS 100

BaseEnemy::BaseEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0), m_baseAngle(baseAngle), m_dirX(dirX), m_dirY(dirY), m_playerDest(playerDest)
{
	m_dx = m_dy = m_accel = m_vel = 0.0;
	m_velMax = 5.0;
	m_rotMax = 2.5;
	SetState(idle);
	m_isLOS = false;
	m_inRange = false;
	m_turnAngle = 0;
	m_isCollisioned = false;
	SOMA::Load("Aud/engines.wav", "patrol", SOUND_SFX);
	SOMA::Load("Aud/Click.wav", "Click", SOUND_SFX);

}

void BaseEnemy::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_P))// Toggle enemies idle/patrol mode.
	{
		SOMA::PlaySound("Click", 0, 7);
		ToggleState();
	}
	if (m_state == running) {
		if (m_dirY == -1)
		{
			if (m_dst.y > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
			{
				m_dst.y += -SPEED;
			}
			else
			{
				m_dirY = 1;
				m_baseAngle += 180.0;
			}
		}
		else if (m_dirY == 1)
		{
			if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
			{
				m_dst.y += SPEED;
			}
			else
			{
				m_dirY = -1;
				m_baseAngle -= 180.0;
			}
		}
		if (m_dirX == -1)
		{
			if (m_dst.x > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
			{
				m_dst.x += -SPEED;
				m_dir = 1;
			}
			else
			{
				m_dirX = 1;
				m_baseAngle += 180.0;
			}
		}
		else if (m_dirX == 1)
		{
			if (m_dst.x < 1024 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
			{
				m_dst.x += SPEED;
				m_dir = 0;
			}
			else
			{
				m_dirX = -1;
				m_baseAngle -= 180.0;
			}
		}
		SetAngle(m_baseAngle);
	}
	else
	{
		m_turnAngle += 4.0f;
		if (m_turnAngle >= 360.0f)
		{
			m_turnAngle = m_turnAngle - 360.0f;
		}
		SetAngle(m_baseAngle + m_turnAngle);
		Animate();
	}

	if (m_isDebugEnable)
	{
		int enemyX = GetDstP()->x + GetDstP()->w / 2;
		int enemyY = GetDstP()->y + GetDstP()->h / 2;

		int playerX = m_playerDest->x + m_playerDest->w / 2;
		int playerY = m_playerDest->y + m_playerDest->h / 2;

		m_inRange = (MAMA::Distance(enemyX, playerX, enemyY, playerY) < (double)RADIUS) ? true : false;

		m_isLOS = true;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				auto tile = Engine::Instance().GetLevel()[row][col];
				if (tile->IsObstacle())
				{
					SDL_Rect rect = { (int)tile->GetDstP()->x, (int)tile->GetDstP()->y, (int)tile->GetDstP()->w, (int)tile->GetDstP()->h };
					if (SDL_IntersectRectAndLine((const SDL_Rect*)&rect, &enemyX, &enemyY, &playerX, &playerY))
					{
						m_isLOS = false;
						break;
					}
				}

			}

			if (!m_isLOS)
			{
				break;
			}
		}
	}
}

void BaseEnemy::Render()
{
	// enable debug mode
	if (m_isDebugEnable) {
		double circle_x = GetDstP()->x + GetDstP()->w / 2;
		double circle_y = GetDstP()->y + GetDstP()->h / 2;
		double circle_radius = RADIUS; // Increase this later
		if (m_inRange)
		{
			SDL_SetRenderDrawColor(m_pRend, 250, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(m_pRend, 0, 0, 250, 255);
		}

		double point1_x;
		double point1_y;
		double point2_x;
		double point2_y;

		// Draw circle
		for (int t = 1; t < 360; t++)
		{
			point1_x = circle_x + circle_radius * cos(t - 1);
			point1_y = circle_y + circle_radius * sin(t - 1);

			point2_x = circle_x + circle_radius * cos(t);
			point2_y = circle_y + circle_radius * sin(t);
			SDL_RenderDrawLineF(m_pRend, point1_x, point1_y, point2_x, point2_y);
		}

		// Draw LOS
		point1_x = m_playerDest->x + m_playerDest->w / 2;
		point1_y = m_playerDest->y + m_playerDest->h / 2;
		if (m_isLOS)
		{
			SDL_SetRenderDrawColor(m_pRend, 250, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(m_pRend, 0, 0, 250, 255);
		}
		SDL_RenderDrawLineF(m_pRend, point1_x, point1_y, circle_x, circle_y);
	}

	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));

	if (m_isCollisioned)
	{
		m_eslapsedFromCollision += 10;
		if (m_eslapsedFromCollision > RADIUS)
		{
			m_isCollisioned = false;
		}
		SDL_Rect src = { 0, 343, 96,96 };
		SDL_FRect dest = { GetDstP()->x - 32, GetDstP()->y - 32, GetDstP()->w + 64, GetDstP()->h + 64 };
		SDL_RenderCopyExF(m_pRend, m_pText, &src, &dest, 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	}
}

void BaseEnemy::Start()
{
	m_sprite = 0;
	m_accel = 0.2;
}

void BaseEnemy::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle || m_state == destroyed)
	{
		m_sprite = m_spriteMin = m_spriteMax = 0;
	}
	else // Only other is running for now...
	{
		Start();
		m_sprite = m_spriteMin = 1;
		m_spriteMax = 4;
	}
	
}

void BaseEnemy::ToggleState()
{
	if (m_state == idle)
	{
		SOMA::SetSoundVolume(14, 2);
		SOMA::PlaySound("patrol", -1, 2);
		SetState(running);
	}
	else
	{
		SetState(idle);
		SOMA::StopSound(2);
	}
	m_turnAngle = 0;
}

void BaseEnemy::SetDebugMode(bool enable) {
	m_isDebugEnable = enable;
}

void BaseEnemy::Stop()
{
	m_dx = m_dy = 0.0;
	m_vel = 0;
	m_frame = 0;
	m_frameMax = 4;
	m_sprite = 4;
}

void BaseEnemy::SetVs(const double angle)
{
	double destAngle = MAMA::Rad2Deg(angle) + 90;
	m_angle += std::min(std::max(MAMA::Angle180(destAngle - m_angle), -m_rotMax), m_rotMax); // Only rotate slightly towards the destination angle.
	// Now use the new slight rotation to generate dx and dy as normal.
	m_vel += m_accel;
	m_vel = std::min(m_vel, m_velMax);
}

void BaseEnemy::Move2Stop(const double angle)
{

}

void BaseEnemy::Collision()
{
	if (m_healthLevel <= 0) {
		// Do nothing
	}
	else
	{
		m_healthLevel -= 20;
		if (m_healthLevel <= 0) {
			SOMA::PlaySound("Death", 0, 6);
			//TODO: Death Animation here
			SetState(destroyed);
			m_healthLevel = 0;
		}
		else {
			SOMA::PlaySound("Boom", 0, 5);
		}
		m_isCollisioned = true;
		m_eslapsedFromCollision = 0;
	}

}
