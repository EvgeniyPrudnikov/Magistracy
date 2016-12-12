// bloom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <array>

using namespace std;

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline

#include <stdlib.h>

#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32(uint32_t x, int8_t r)
{
	return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r)
{
	return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint64_t getblock64(const uint64_t * p, int i)
{
	return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

//----------

FORCE_INLINE uint64_t fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128(const void * key, const int len, const uint32_t seed, void * out)
{
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	const uint64_t * blocks = (const uint64_t *)(data);

	for (int i = 0; i < nblocks; i++)
	{
		uint64_t k1 = getblock64(blocks, i * 2 + 0);
		uint64_t k2 = getblock64(blocks, i * 2 + 1);

		k1 *= c1; k1 = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;

		h1 = ROTL64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;

		k2 *= c2; k2 = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

		h2 = ROTL64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks * 16);

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch (len & 15)
	{
	case 15: k2 ^= ((uint64_t)tail[14]) << 48;
	case 14: k2 ^= ((uint64_t)tail[13]) << 40;
	case 13: k2 ^= ((uint64_t)tail[12]) << 32;
	case 12: k2 ^= ((uint64_t)tail[11]) << 24;
	case 11: k2 ^= ((uint64_t)tail[10]) << 16;
	case 10: k2 ^= ((uint64_t)tail[9]) << 8;
	case  9: k2 ^= ((uint64_t)tail[8]) << 0;
		k2 *= c2; k2 = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

	case  8: k1 ^= ((uint64_t)tail[7]) << 56;
	case  7: k1 ^= ((uint64_t)tail[6]) << 48;
	case  6: k1 ^= ((uint64_t)tail[5]) << 40;
	case  5: k1 ^= ((uint64_t)tail[4]) << 32;
	case  4: k1 ^= ((uint64_t)tail[3]) << 24;
	case  3: k1 ^= ((uint64_t)tail[2]) << 16;
	case  2: k1 ^= ((uint64_t)tail[1]) << 8;
	case  1: k1 ^= ((uint64_t)tail[0]) << 0;
		k1 *= c1; k1 = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	h2 += h1;

	((uint64_t*)out)[0] = h1;
	((uint64_t*)out)[1] = h2;
}

struct BloomFilter 
{
	BloomFilter(uint64_t size, uint8_t numHashes);
	void add(const char *data);
	bool possiblyContains(const char *data) const;

private:
	uint8_t m_numHashes;
	vector<bool> m_bits;
};

BloomFilter::BloomFilter(uint64_t size, uint8_t numHashes) : m_bits(size), m_numHashes(numHashes) {}

array<uint64_t, 2> myhash(const char *data) 
{
	auto len = strlen(data) + 1;
	std::array<uint64_t, 2> hashValue;
	MurmurHash3_x64_128(data, len, 0, hashValue.data());
	return hashValue;
}

inline uint64_t nthHash(uint8_t n, uint64_t hashA, uint64_t hashB, uint64_t filterSize)
{
	return (hashA + n * hashB) % filterSize;
}

void BloomFilter::add(const char *data)
{
	auto hashValues = myhash(data);

	for (int n = 0; n < m_numHashes; n++)
	{
		m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())] = true;
	}
}

bool BloomFilter::possiblyContains(const char *data) const
{
	auto hashValues = myhash(data);

	for (int n = 0; n < m_numHashes; n++) {
		if (!m_bits[nthHash(n, hashValues[0], hashValues[1], m_bits.size())]) 
		{
			return false;
		}
	}
	return true;
}

int main()
{
	BloomFilter bloom = BloomFilter(15, 2);

	bloom.add("10");

	cout << bloom.possiblyContains("10") << endl;
	cout << bloom.possiblyContains("11") << endl;
	cout << bloom.possiblyContains("dfds") << endl;
	cout << bloom.possiblyContains("13") << endl;


	getchar();
	return 0;
}
