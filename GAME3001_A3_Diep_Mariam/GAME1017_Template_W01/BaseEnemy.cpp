#include "BaseEnemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "DebugManager.h"
#define SPEED 2

BaseEnemy::BaseEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0), m_baseAngle(baseAngle), m_dirX(dirX), m_dirY(dirY), m_playerDest(playerDest)
{
	m_dx = m_dy = m_accel = m_vel = 0.0;
	m_velMax = 5.0;
	m_rotMax = 2.5;
	SetState(idle);
}

void BaseEnemy::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_P))// Toggle enemies idle/patrol mode.
	{
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
	}
}

void BaseEnemy::Render()
{
	// enable debug mode
	if (m_isDebugEnable) {
		double circle_x = GetDstP()->x + GetDstP()->w/2;
		double circle_y = GetDstP()->y + GetDstP()->h/2;
		double circle_radius = 100; // Increase this later
		SDL_SetRenderDrawColor(m_pRend, 250, 0, 0, 255);

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

		// Draw line
		point1_x = m_playerDest->x + m_playerDest->w / 2;
		point1_y = m_playerDest->y + m_playerDest->h / 2;
		SDL_RenderDrawLineF(m_pRend, point1_x, point1_y, circle_x, circle_y);
	}

	SetAngle(m_baseAngle);
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
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
	if (m_state == idle)
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
		SetState(running);
	}
	else
	{
		SetState(idle);
	}
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
