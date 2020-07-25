#include "Bullet.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "DebugManager.h"
#include "Engine.h"
#define SPEED 5
#define RADIUS 200

Bullet::Bullet(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, /*const SDL_Point start*/float x, float y, int direction)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(died), m_dir(0) 
{
	m_isCollisioned = false;
	Shoot(x, y, direction);
}

void Bullet::Update()
{
	if (m_state != died)
	{
		if (!m_isCollisioned)
		{
			if (m_direction == 0)
			{
				m_dst.y -= SPEED;
			}
			else if (m_direction == 1)
			{
				m_dst.x += SPEED;
			}
			else if (m_direction == 2)
			{
				m_dst.y += SPEED;
			}
			else if (m_direction == 3)
			{
				m_dst.x -= SPEED;
			}

			m_distance += SPEED;
			if (m_distance == RADIUS)
			{
				m_state = died;
			}
		}
		else
		{

		}
	}
}

void Bullet::Render()
{
	if (m_state != died)
	{
		SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
	}
}

void Bullet::Start()
{

}

void Bullet::SetState(int s)
{
	m_state = static_cast<state>(s);
}

void Bullet::Stop()
{
	m_frame = 0;
	m_frameMax = 4;
	m_sprite = 4;
}

void Bullet::Shoot(/*const SDL_Point start*/float x , float y, int direction)
{
	m_state = shooting;
	m_isCollisioned = false;
	
	m_src.x = 0;
	m_src.y = 439;
	m_src.w = 9;
	m_src.h = 29;

	//m_dst.x = start.x;
	//m_dst.y = start.y;
	
	m_dst.x = x;
	m_dst.y = y;
	m_dst.w = 19;
	m_dst.h = 39;

	m_direction = direction;
	m_distance = 0;

	if (m_direction == 0)
	{
		SetAngle(0.0f);
	}
	else if (m_direction == 1)
	{
		SetAngle(90.0f);
	}
	else if (m_direction == 2)
	{
		SetAngle(180.0f);
	}
	else if (m_direction == 3)
	{
		SetAngle(270.0f);
	}
}

void Bullet::Collision()
{
	//m_isCollisioned = true;
	m_state = died;
}