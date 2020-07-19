#pragma once
#ifndef _DEBUGMANAGER_H_
#define _DEBUGMANAGER_H_

#include <vector>
#include "SDL.h"

class DebugManager
{
public: // Public methods.
	static int s_debugMode;
	static void DrawLine(const SDL_Point start, const SDL_Point end, const SDL_Color col);
	static void QueueLine(const SDL_Point start, const SDL_Point end, const SDL_Color col);
	static void FlushLines();
	static void DrawRay(const SDL_Point start, const double angle, const double length, const SDL_Color col);
	static void Quit();
private: // Private properties.
	static std::vector<SDL_Point> s_points;
	static std::vector<SDL_Color> s_colors;
private: // Public methods.
	DebugManager() {}
};

typedef DebugManager DEMA;

#endif