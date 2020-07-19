#pragma once
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "SDL.h"
#include <iostream>

class Sprite // Inline class.
{
public: // Inherited and public.
	Sprite(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t)
		:m_src(s), m_dst(d), m_pRend(r), m_pText(t), m_angle(0.0) {}
	virtual void Render() {	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, SDL_FLIP_NONE); }
	SDL_Rect* GetSrcP() { return &m_src; }
	SDL_FRect* GetDstP() { return &m_dst; }
	double& GetAngle() { return m_angle; }
	void SetAngle(double a) { m_angle = a; }
protected: // Private BUT inherited.
	double m_angle;
	SDL_Rect m_src;
	SDL_FRect m_dst;
	SDL_Renderer* m_pRend;
	SDL_Texture* m_pText;
private: // Private NOT inherited.
};

class AnimatedSprite : public Sprite// Also inline.
{
public:
	AnimatedSprite(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf)
		:Sprite(s, d, r, t), m_sprite(sstart), m_spriteMin(smin), m_spriteMax(smax), m_frameMax(nf) {}
	void Animate()
	{
		if (m_frame++ == m_frameMax) // Post-increment ensures m_frame starts at 0.
		{
			m_frame = 0;
			if (m_sprite++ == m_spriteMax)
				m_sprite = m_spriteMin;
		}
		m_src.x = m_src.w * m_sprite;
	}
protected:
	int m_sprite,		// The current sprite index in row.
		m_spriteMin,	// The sprite index to start animation on.
		m_spriteMax,	// The sprite index to end animation on.
		m_frame = 0,	// Frame counter.
		m_frameMax;		// Number of frames to display each sprite.
};

/* Note: A lot of animation methods use a modulus operation to wrap the frames
   around, but I am not. I may have different cycles on the same row and so I
   want total control over my animations without having it look too complex. */

#endif