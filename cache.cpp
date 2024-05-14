#include "precomp.h"
#include "cache.h"

void Memory::WriteLine( uint address, CacheLine line )
{
	// verify that the address is a multiple of the cacheline width
	assert( (address & CACHELINEWIDTH - 1) == 0 );

	// verify that the provided cacheline has the right tag
	assert( (address / CACHELINEWIDTH) == line.tag );

	// write the line to simulated DRAM
	memcpy( mem + address, line.bytes, CACHELINEWIDTH );
	w_hit++; // writes to mem always 'hit'
}

CacheLine Memory::ReadLine( uint address )
{
	// verify that the address is a multiple of the cacheline width
	assert( (address & CACHELINEWIDTH - 1) == 0 );

	// read the line from simulated RAM
	CacheLine retVal;
	memcpy( retVal.bytes, mem + address, CACHELINEWIDTH );
	retVal.tag = address / CACHELINEWIDTH;
	retVal.dirty = false;
	
	// return the data
	r_hit++; // reads from mem always 'hit'
	return retVal;
}

void Cache::WriteLine( uint address, CacheLine line )
{
	// verify that the address is a multiple of the cacheline width
	assert( (address & CACHELINEWIDTH - 1) == 0 );

	// verify that the provided cacheline has the right tag
	assert( (address / CACHELINEWIDTH) == line.tag );

	// fully associative: see if any of the slots match our address
	int slotsInCache = CACHESIZE / CACHELINEWIDTH;
	for (int i = 0; i < slotsInCache; i++) if (slot[i].tag == line.tag)
	{
		// cacheline is already in the cache; overwrite
		slot[i] = line;
		w_hit++;
		return;
	}

	// address not found; evict a line
	int slotToEvict = RandomUInt() % slotsInCache;
	if (slot[slotToEvict].dirty)
	{
		// evicted line is dirty; write to next level
		nextLevel->WriteLine( slot[slotToEvict].tag * CACHELINEWIDTH, slot[slotToEvict] );
	}
	slot[slotToEvict] = line;
	w_miss++;
}

CacheLine Cache::ReadLine( uint address )
{
	// verify that the address is a multiple of the cacheline width
	assert( (address & CACHELINEWIDTH - 1) == 0 );

	// fully associative: see if any of the slots match our address
	int slotsInCache = CACHESIZE / CACHELINEWIDTH;
	uint addressTag = address / CACHELINEWIDTH;
	for (int i = 0; i < slotsInCache; i++)
	{
		if (slot[i].tag == addressTag)
		{
			// cacheline is in the cache; return data
			r_hit++;
			return slot[i]; // by value
		}
	}

	// data is not in this cache; ask the next level
	CacheLine line = nextLevel->ReadLine( address );

	// store the retrieved line in this cache
	WriteLine( address, line );

	// return the requested data
	r_miss++;
	return line;
}

void MemHierarchy::WriteByte( uint address, uchar value )
{
	// fetch the cacheline for the specified address
	int offsetInLine = address & (CACHELINEWIDTH - 1);
	int lineAddress = address - offsetInLine;
	CacheLine line = l1->ReadLine( lineAddress );
	line.bytes[offsetInLine] = value;
	line.dirty = true;
	l1->WriteLine( lineAddress, line );
}

uchar MemHierarchy::ReadByte( uint address )
{
	// fetch the cacheline for the specified address
	int offsetInLine = address & (CACHELINEWIDTH - 1);
	int lineAddress = address - offsetInLine;
	CacheLine line = l1->ReadLine( lineAddress );
	return line.bytes[offsetInLine];
}

void MemHierarchy::WriteUint( uint address, uint value )
{
	// fetch the cacheline for the specified address
	int offsetInLine = address & (CACHELINEWIDTH - 1);
	int lineAddress = address - offsetInLine;
	CacheLine line = l1->ReadLine( lineAddress );
	memcpy( line.bytes + offsetInLine, &value, sizeof( uint ) );
	line.dirty = true;
	l1->WriteLine( lineAddress, line );
}

uint MemHierarchy::ReadUint( uint address )
{
	// fetch the cacheline for the specified address
	int offsetInLine = address & (CACHELINEWIDTH - 1);
	assert( (offsetInLine & 3) == 0 ); // we will not support straddlers
	int lineAddress = address - offsetInLine;
	CacheLine line = l1->ReadLine( lineAddress );
	return ((uint*)line.bytes)[offsetInLine / 4];
}