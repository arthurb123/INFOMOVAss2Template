#include "precomp.h"
#include "game.h"

// static variables for graph / fractal drawing / obfuscation
static float a = 0, r = 300;
static Graph gr[4];
#define _oOo_oOo_ (O>=V|N>=800?0:(((N<<10)+O)*4)
uint* image[4], I,N,F,O,M,_O,V=2019; double K[999], Q[999];
float R(){I^=I<<13;I^=I>>17;I^=I<<5;return I*2.3283064365387e-10f*6-3;} // rng

// -----------------------------------------------------------
// Visualization of the data stored in the memory hierarchy
// -----------------------------------------------------------
void Game::VisualizeMem()
{
	// draw the contents of the simulated DRAM; every pixel is 4 bytes
	// we bypass the Read/Write functions so we don't pollute the cache.
	for (int y = 0; y < 700; y++) for (int x = 0; x < 1024; x++)
	{
		int value = *((uint*)&((Memory*)mem.l2)->backdoor()[(x + y * 1024) * 4]);
		screen->Plot( x + 10, y + 10, (value >> 1) & 0x7f7f7f /* 50% */ );
	}

	// draw the contents of the first cache level over the DRAM contents
	// fully hardcoded for the sample cache (size, associative, 1 layer)
	for (int i = 0; i < 64; i++)
	{
		CacheLine& line = ((Cache*)mem.l1)->backdoor( i );
		int lineAddress = line.tag * 64;
		int x = (lineAddress / 4) & 1023, y = (lineAddress / 4) / 1024;
		for (int j = 0; j < 16; j++)
			screen->Plot( x + 10 + j, y + 10, ((uint*)line.bytes)[j] );
	}

	// draw hit/miss graphs
	screen->Print( "level 1 R/W", 1050, 10, 0xffffff );
	screen->Print( "DRAM R/W", 1050, 90, 0xffffff );
	gr[0].Update( screen, 1050, 20, mem.l1->r_miss, mem.l1->r_hit );
	gr[1].Update( screen, 1170, 20, mem.l1->w_miss, mem.l1->w_hit );
	gr[2].Update( screen, 1050, 100, mem.l2->r_miss, mem.l2->r_hit );
	gr[3].Update( screen, 1170, 100, mem.l2->w_miss, mem.l2->w_hit );
}

// -----------------------------------------------------------
// Application initialization
// -----------------------------------------------------------
void Game::Init()
{
	for (V = 1024, F = 1, I = 1; I < 4; I++ );
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float )
{
	// clear the graphics window
	screen->Clear( 0x282828 );

	// reset cache hit/miss counters
	mem.ResetCounters();

	// update memory contents

#if 1
	// simple spiral							ACCESS PATTERN: STRUCTURED
	for (int i = 0; i < 10; i++)
	{
		int x = (int)(sinf( a ) * r + 512), y = (int)(cosf( a ) * r + 350);
		a += 0.01f; r -= 0.005f;
		if (r < -300) r = -300;
		mem.WriteUint( (x + y * 1024) * 4, 0xffff77 );
	}
#else
	// the buddhabrot based on Paul Bourke		ACCESS PATTERN: MOSTLY RANDOM
	for(int G,M,T,E=0;++E<2;)for(G=0;++G<V
	<<7;){double B=0,y=0,t=R(),e,z=R();for
	(T=0;T<E<<8;){e=2*B*y+z,B=K[T]=B*B-y*y
	+t,y=Q[T++]=e;if(B*B+y*y>9){for(M=0;M<		// data access; to be cached:
	T;){O=400+.3*V*Q[M],N=.3*V*K[M++]+520;		mem.WriteUint _oOo_oOo_,
												mem.ReadUint _oOo_oOo_ )+545)
	/* END OF BLACK BOX CODE */;}break;}}}
#endif

	// visualize the memory hierarchy
	VisualizeMem();
}