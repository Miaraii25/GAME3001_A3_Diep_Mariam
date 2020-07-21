#pragma once
#ifndef _BULLET_H_
#define _BULLET_H_

#include "Sprite.h"

class Bullet : public AnimatedSprite
{
public:
	Bullet(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf);
	void Update();
	void Render();
	void Start();
	bool isDied() { return (m_state == died); }
	void Shoot(const SDL_Point start, int direction);
	void Collision();
	bool IsCollisioned() { return m_isCollisioned; }
private:
	enum state { shooting, died } m_state;
	bool m_dir;
	void SetState(int s);
	void Stop();
	double m_baseAngle;
	double m_turnAngle;
	int m_dirX;
	int m_dirY;
	bool m_isDebugEnable;
	SDL_FRect* m_playerDest;
	bool m_isLOS;
	bool m_inRange;
	bool m_isCollisioned;
	int m_direction;
	int m_distance;
};

#endif

