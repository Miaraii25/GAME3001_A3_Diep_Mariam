#pragma once
#ifndef _BASE_ENEMY_H_
#define _BASE_ENEMY_H_

#include "Sprite.h"

class BaseEnemy : public AnimatedSprite
{
public:
	BaseEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest);
	void Update();
	void Render();
	void Start();
	void SetDebugMode(bool enable);
private:
	enum state { idle, running } m_state;
	bool m_dir;
	void Move2Stop(const double angle);
	void SetState(int s);
	void ToggleState();
	void Stop();
	void SetVs(const double angle);
	double m_dx, m_dy,
		m_accel,
		m_vel,
		m_velMax,
		m_rotMax;
	double m_baseAngle;
	int m_dirX;
	int m_dirY;
	bool m_isDebugEnable;
	SDL_FRect* m_playerDest;
};

#endif

