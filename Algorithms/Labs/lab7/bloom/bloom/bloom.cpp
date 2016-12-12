// bloom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <cstdlib>

using namespace std;

typedef unsigned int(*hash_function)(const void *data);
typedef struct bloom_filter * bloom_t;

/* Creates a new bloom filter with no hash functions and size * 8 bits. */
bloom_t bloom_create(size_t size);
/* Frees a bloom filter. */
void bloom_free(bloom_t filter);
/* Adds a hashing function to the bloom filter. You should add all of the
* functions you intend to use before you add any items. */
void bloom_add_hash(bloom_t filter, hash_function func);
/* Adds an item to the bloom filter. */
void bloom_add(bloom_t filter, const void *item);
/* Tests if an item is in the bloom filter.
*
* Returns false if the item has definitely not been added before. Returns true
* if the item was probably added before. */
bool bloom_contains(bloom_t filter, const void *item);

struct bloom_hash 
{
	hash_function func;
	struct bloom_hash *next;
};

struct bloom_filter 
{
	struct bloom_hash *func;
	void *bits;
	size_t size;
};

unsigned int djb2(const void *_str);
unsigned int jenkins(const void *_str);


int main()
{
	bloom_t bloom = bloom_create(100);

	bloom_add_hash(bloom, djb2);
	bloom_add_hash(bloom, jenkins);

	printf("Should be 0: %d\n", bloom_contains(bloom, "hello world"));

	bloom_add(bloom, "hello world");

	printf("Should be 1: %d\n", bloom_contains(bloom, "hello world"));

	printf("Should (probably) be 0: %d\n", bloom_contains(bloom, "world hello"));

	bloom_free(bloom);

	getchar();
    return 0;
}


bloom_t bloom_create(size_t size) 
{
	bloom_t res = (bloom_t)calloc(1, sizeof(struct bloom_filter));
	res->size = size;
	res->bits = malloc(size);
	return res;
}

void bloom_free(bloom_t filter) 
{
	if (filter)
	{
		while (filter->func)
		{
			struct bloom_hash *h = filter->func;
			filter->func = h->next;
			free(h);
		}
		free(filter->bits);
		free(filter);
	}
}

void bloom_add_hash(bloom_t filter, hash_function func) 
{
	struct bloom_hash *h = (bloom_hash *)calloc(1, sizeof(struct bloom_hash));
	h->func = func;
	struct bloom_hash *last = filter->func;
	while (last && last->next) 
	{
		last = last->next;
	}
	if (last) 
	{
		last->next = h;
	}
	else 
	{
		filter->func = h;
	}
}
void bloom_add(bloom_t filter, const void *item)
{
	struct bloom_hash *h = filter->func;
	uint8_t *bits = (uint8_t *)filter->bits;
	while (h)
	{
		unsigned int hash = h->func(item);
		hash %= filter->size * 8;
		bits[hash / 8] |= 1 << hash % 8;
		h = h->next;
	}
}

bool bloom_contains(bloom_t filter, const void *item)
{
	struct bloom_hash *h = filter->func;
	uint8_t *bits = (uint8_t *)filter->bits;
	while (h) 
	{
		unsigned int hash = h->func(item);
		hash %= filter->size * 8;
		if (!(bits[hash / 8] & 1 << hash % 8)) 
		{
			return false;
		}
		h = h->next;
	}
	return true;
}

unsigned int djb2(const void *_str) 
{
	const char *str = (char *)_str;
	unsigned int hash = 5381;
	char c;
	while ((c = *str++)) 
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

unsigned int jenkins(const void *_str) 
{
	const char *key = (char *)_str;
	unsigned int hash = 5381, i;
	while (*key)
	{
		hash += *key;
		hash += (hash << 10);
		hash ^= (hash >> 6);
		key++;
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}