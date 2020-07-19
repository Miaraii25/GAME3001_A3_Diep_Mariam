#include "Player.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "PathManager.h"
#define SPEED 2

Player::Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, SDL_FRect* goal)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(idle), m_dir(0), m_pGoal(goal) 
{}

void Player::Update()
{
	switch (m_state)
	{
	case idle:
		if (EVMA::KeyHeld(SDL_SCANCODE_W) || EVMA::KeyHeld(SDL_SCANCODE_S) ||
			EVMA::KeyHeld(SDL_SCANCODE_A) || EVMA::KeyHeld(SDL_SCANCODE_D) || 
			(EVMA::KeyHeld(SDL_SCANCODE_M) && !PAMA::getPath().empty()))
		{
			SetState(running);
		}
		break;
	case running:
		if (EVMA::KeyReleased(SDL_SCANCODE_W) || EVMA::KeyReleased(SDL_SCANCODE_S) ||
			EVMA::KeyReleased(SDL_SCANCODE_A) || EVMA::KeyReleased(SDL_SCANCODE_D) || EVMA::KeyReleased(SDL_SCANCODE_M))
		{
			SetState(idle);
			break; // Skip movement parsing below.
		}
		if (EVMA::KeyHeld(SDL_SCANCODE_M) && !PAMA::getPath().empty()) //M key, march
		{
			const std::vector<PathConnection*>& path = PAMA::getPath();
			for (int i = 0; i < path.size(); ++i)
			{
				PathConnection* conn = path[i];

				if ((int)(m_dst.x / 32) == (int)(conn->GetFromNode()->x / 32) && (int)(m_dst.y / 32) == (int)(conn->GetFromNode()->y / 32))
				{
					float toXDiff = conn->GetToNode()->x - m_dst.x;
					float toYDiff = conn->GetToNode()->y - m_dst.y;
					float fromXDiff = conn->GetFromNode()->x - m_dst.x;
					float fromYDiff = conn->GetFromNode()->y - m_dst.y;

					if ((toXDiff > 0.0f && fromYDiff == 0.0f) || (toYDiff != 0.0f && fromXDiff > 0.0f))
					{
						m_dst.x++;
						if (m_dst.x > conn->GetToNode()->x) m_dst.x = conn->GetToNode()->x;
						m_dir = 0;
					}
					else if ((toXDiff < 0.0f && fromYDiff == 0.0f) || (toYDiff != 0.0f && fromXDiff < 0.0f))
					{
						m_dst.x--;
						if (m_dst.x < conn->GetToNode()->x) m_dst.x = conn->GetToNode()->x;
						m_dir = 1;
					}
					else if ((toYDiff > 0.0f && fromXDiff == 0.0f) || (toXDiff != 0.0f && fromYDiff > 0.0f))
					{
						m_dst.y++;
						if (m_dst.y > conn->GetToNode()->y) m_dst.y = conn->GetToNode()->y;
					}
					else if ((toYDiff < 0.0f && fromXDiff == 0.0f) || (toXDiff != 0.0f && fromYDiff < 0.0f))
					{
						m_dst.y--;
						if (m_dst.y < conn->GetToNode()->y) m_dst.y = conn->GetToNode()->y;
					}
					break;
				}
				else if (i == path.size() - 1 && (int)(m_dst.x / 32) == (int)(conn->GetToNode()->x / 32) && (int)(m_dst.y / 32) == (int)(conn->GetToNode()->y / 32))
				{
					if (GetDstP()->x < conn->GetToNode()->x)
					{
						m_dst.x++;
						if (m_dst.x > conn->GetToNode()->x) m_dst.x = conn->GetToNode()->x;
						m_dir = 0;
					}
					else if (GetDstP()->x > conn->GetToNode()->x)
					{
						m_dst.x--;
						if (m_dst.x < conn->GetToNode()->x) m_dst.x = conn->GetToNode()->x;
						m_dir = 1;
					}
					else if (GetDstP()->y < conn->GetToNode()->y)
					{
						m_dst.y++;
						if (m_dst.y > conn->GetToNode()->y) m_dst.y = conn->GetToNode()->y;
					}
					else if (GetDstP()->y > conn->GetToNode()->y)
					{
						m_dst.y--;
						if (m_dst.y < conn->GetToNode()->y) m_dst.y = conn->GetToNode()->y;
					}
				}
			}
		}
		else
		{

			if (EVMA::KeyHeld(SDL_SCANCODE_W))
			{
				if (m_dst.y > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
				{
					m_dst.y += -SPEED;
				}
			}
			else if (EVMA::KeyHeld(SDL_SCANCODE_S))
			{
				if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
				{
					m_dst.y += SPEED;
				}
			}
			if (EVMA::KeyHeld(SDL_SCANCODE_A))
			{
				if (m_dst.x > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
				{
					m_dst.x += -SPEED;
					m_dir = 1;
				}
			}
			else if (EVMA::KeyHeld(SDL_SCANCODE_D))
			{
				if (m_dst.x < 1024 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
				{
					m_dst.x += SPEED;
					m_dir = 0;
				}
			}
		}
		break;
	}
	Animate();
}

void Player::Render()
{
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));
}

void Player::Start()
{
	m_sprite = 0;
}

void Player::SetState(int s)
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


