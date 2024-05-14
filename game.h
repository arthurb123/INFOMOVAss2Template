// Template, IGAD version 3
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2023

#pragma once
#include "cache.h"

namespace Tmpl8
{

class Graph
{
public:
	void Update( Surface* screen, float x, float y, int hit, int miss )
	{
		memcpy( &ratio[0], &ratio[1], 99 * sizeof( float ) );
		ratio[99] = min( 1.0f, miss / max( 0.001f, (float)(hit + miss) ) );
		screen->Bar( (int)x, (int)y, (int)x + 99, (int)y + 50, 0xff0000 );
		for (int i = 0; i < 100; i++) if (ratio[i] > 0)
			screen->Line( i + x, y, i + x, y + ratio[i] * 50, 0xff00 );
	}
	float ratio[100] = {};
};

class Game : public TheApp
{
public:
	// game flow methods
	void Init();
	void Tick( float deltaTime );
	void VisualizeMem();
	void Shutdown() { /* implement if you want to do something on exit */ }
	// input handling
	void MouseUp( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { mousePos.x = x, mousePos.y = y; }
	void MouseWheel( float ) { /* implement if you want to handle the mouse wheel */ }
	void KeyUp( int ) { /* implement if you want to handle keys */ }
	void KeyDown( int ) { /* implement if you want to handle keys */ }
	// data members
	int2 mousePos;
	MemHierarchy mem;
};

} // namespace Tmpl8