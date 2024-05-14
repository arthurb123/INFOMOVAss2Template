#pragma once

namespace Tmpl8 {

// single-level fully associative cache

#define CACHELINEWIDTH	64
#define CACHESIZE		4096				// in bytes
#define DRAMSIZE		3276800				// 3.125MB; 1024x800 pixels

struct CacheLine
{
	uchar bytes[CACHELINEWIDTH] = {};
	uint tag;
	bool dirty = false;
};

class Level // abstract base class for a level in the memory hierarchy
{
public:
	virtual void WriteLine( uint address, CacheLine line ) = 0;
	virtual CacheLine ReadLine( uint address ) = 0;
	Level* nextLevel = 0;
	uint r_hit = 0, r_miss = 0, w_hit = 0, w_miss = 0;
};

class Cache : public Level // cache level for the memory hierarchy
{
public:
	void WriteLine( uint address, CacheLine line );
	CacheLine ReadLine( uint address );
	CacheLine& backdoor( int i ) { return slot[i]; } /* for visualization without side effects */
private:
	CacheLine slot[CACHESIZE / CACHELINEWIDTH];
};

class Memory : public Level // DRAM level for the memory hierarchy
{
public:
	Memory()
	{
		mem = new uchar[DRAMSIZE];
		memset( mem, 0, DRAMSIZE ); 
	}
	void WriteLine( uint address, CacheLine line );
	CacheLine ReadLine( uint address );
	uchar* backdoor() { return mem; } /* for visualization without side effects */
private:
	uchar* mem = 0;
};

class MemHierarchy // memory hierarchy
{
public:
	MemHierarchy()
	{
		l1 = new Cache();
		l1->nextLevel = l2 = new Memory();
	}
	void WriteByte( uint address, uchar value );
	uchar ReadByte( uint address );
	void WriteUint( uint address, uint value );
	uint ReadUint( uint address );
	void ResetCounters()
	{
		l1->r_hit = l1->w_hit = l1->r_miss = l1->r_hit = 0;
		l2->r_hit = l2->w_hit = l2->r_miss = l2->r_hit = 0;
	}
	Level* l1, *l2; 
};

} // namespace Tmpl8