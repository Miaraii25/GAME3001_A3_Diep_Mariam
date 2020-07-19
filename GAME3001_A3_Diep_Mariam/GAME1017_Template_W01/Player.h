#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Sprite.h"

class Player : public AnimatedSprite
{
public:
	Player(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, SDL_FRect* goal = nullptr);
	void Update();
	void Render();
	void Start();
private:
	enum state { idle, running } m_state;
	bool m_dir;
	SDL_FRect* m_pGoal;
	void SetState(int s);
};
#endif

