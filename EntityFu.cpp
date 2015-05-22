///
/// [EntityFu](https://github.com/NatWeiss/EntityFu)
/// A simple, fast entity component system written in C++.
/// Under the MIT license.
///

#include "EntityFu.h"

/// Turn this on to have a faster yet riskier ECS.
#define kTrustPointers 0

/// Auto-define a log method.
#ifndef Log
	#include <stdio.h>
	#define Log(...) {printf(__VA_ARGS__); printf("\n");}
#endif

/// Auto-define an assert method.
#ifndef Assert2
	#define Assert2(...) do {} while (0)
#endif

/// Turn this to 1 or 2 to debug the ECS.
static int verbosity = 0;

bool* Entity::entities = nullptr;
Entity::Component*** Entity::components = nullptr;
Eid* Entity::minEids = nullptr;
Eid* Entity::maxEids = nullptr;

void Entity::alloc()
{
	if (components != nullptr)
		return;
	if (verbosity > 0)
		{Log("Allocing entities");}

	// allocate entities
	entities = new bool[kMaxEid];
	for (Eid eid = 0; eid < kMaxEid; ++eid)
		entities[eid] = false;

	// allocate components
	auto max = Component::numCids;
	components = new Component**[max];
	minEids = new Eid[max];
	maxEids = new Eid[max];
	for (Cid i = 0; i < max; i++)
	{
		// allocate component array
		components[i] = new Component*[kMaxEid];
		minEids[i] = kMaxEid - 1;
		maxEids[i] = 0;
		
		// zero component pointers
		for (Eid j = 0; j < kMaxEid; j++)
			components[i][j] = nullptr;
	}
}

void Entity::dealloc()
{
	if (verbosity > 0)
		{Log("Deallocing entities");}
	
	if (entities != nullptr)
		delete [] entities;
	
	if (components != nullptr)
	{
		for (Cid i = 0; i < Component::numCids; i++)
			if (components[i] != nullptr)
				delete [] components[i]; // note this doesn't destroy the components, just the array
		delete [] components;
	}

	if (minEids != nullptr)
		delete [] minEids;
	if (maxEids != nullptr)
		delete [] maxEids;

	entities = nullptr;
	components = nullptr;
	minEids = nullptr;
	maxEids = nullptr;
}

Eid Entity::create()
{
	// auto allocate
	Entity::alloc();
	
	Eid eid = 1;
	for (; eid < kMaxEid && entities[eid]; ++eid)
	{
	}

	if (eid < 1 || eid >= kMaxEid)
		{Log("Maximum number of entities reached!");} // this should probably be an assertion
	else
		entities[eid] = true;

	if (verbosity > 0)
		{Log("Entity %u created", eid);}
	
	return eid;
}

int Entity::count()
{
	int ret = 0;
	for (Eid eid = 1; eid < kMaxEid; ++eid)
		if (entities[eid])
			++ret;
	return ret;
}

void Entity::destroyNow(Eid eid)
{
	if (eid == 0)
		return;
	if (verbosity > 0)
		{Log("Entity %u being destroyed", eid);}

	for (Cid cid = 0; cid < Component::numCids; cid++)
		Entity::removeComponent(eid, cid);
	entities[eid] = false;
}

void Entity::destroyAll()
{
	for (Eid eid = 1; eid < kMaxEid; ++eid)
		if (entities[eid])
			Entity::destroyNow(eid);
}

Eid Entity::getMinEid(Cid cid)
{
	if (cid < Component::numCids)
		return minEids[cid];
	return kMaxEid - 1;
}

Eid Entity::getMaxEid(Cid cid)
{
	if (cid < Component::numCids)
		return maxEids[cid];
	return 1;
}

void Entity::addComponent(Eid eid, Component* c, Cid cid)
{
	if (c == nullptr)
		return;
	if (eid >= kMaxEid && cid >= Component::numCids)
	{
		Assert2(false, "Invalid eid %u or cid %u", eid, cid);
		return;
	}

	if (verbosity > 0)
	{
		Log("");
		Entity::log(cid);
		Log("Adding component cid %u eid %u (%x)", cid, eid, (int)(long)c);
	}

	// pointers to components are stored in the map
	// (components must be allocated with new, not stack objects)
	components[cid][eid] = c;
	
	// store min / max
	if (eid < minEids[cid])
		minEids[cid] = eid;
	if (eid > maxEids[cid])
		maxEids[cid] = eid;

	if (verbosity > 0)
		Entity::log(cid);
}

void Entity::removeComponent(Eid eid, Cid cid)
{
	if (eid >= kMaxEid && cid >= Component::numCids)
	{
		Assert2(false, "Invalid eid %u or cid %u", eid, cid);
		return;
	}

	// get pointer
	auto ptr = components[cid][eid];
	if (ptr == nullptr)
		return;

	if (verbosity > 1)
	{
		Log("");
		Entity::log(cid);
		Log("Removing component cid %u eid %u (%x)", cid, eid, (int)(long)ptr);
	}

	// pointers to components are deleted
	if (ptr != nullptr)
		delete ptr;
	
	// erase the component pointer
	components[cid][eid] = nullptr;

	// update min / max
	if (eid == minEids[cid])
	{
		auto i = eid;
		for (; i < kMaxEid - 1 && components[cid][i] == nullptr; i++) {}
		minEids[cid] = i;
		if (verbosity > 1)
			{Log("New min eid %u for cid %u component %x", i, cid, (int)(long)components[cid][i]);}
	}

	if (eid == maxEids[cid])
	{
		auto i = eid;
		for (; i > 1 && i < kMaxEid - 1 && components[cid][i] == nullptr; i--) {}
		maxEids[cid] = i;
		if (verbosity > 1)
			{Log("New max eid %u for cid %u component %x", i, cid, (int)(long)components[cid][i]);}
	}

	if (verbosity > 1)
		Entity::log(cid);
}

Entity::Component* Entity::getComponent(Eid eid, Cid cid)
{
#if (kTrustPointers == 0)
	if (eid < kMaxEid && cid < Component::numCids)
	{
#endif
		return components[cid][eid];

#if (kTrustPointers == 0)
	}
	return nullptr;
#endif
}

int Entity::count(Cid cid)
{
	int n = 0;
	for (auto eid = Entity::getMinEid(cid), eidMax = Entity::getMaxEid(cid); eid <= eidMax; eid++)
		if (components[cid][eid] != nullptr)
			n++;
	return n;
}

void Entity::log(Cid cid)
{
	auto n = Entity::count(cid);
	Log("Cid %u has %d entities ranging from %u to %u", cid, n, minEids[cid], maxEids[cid]);
}

void Entity::logAll()
{
	for (Cid cid = 0, max = Component::numCids; cid < max; cid++)
		Entity::log(cid);
}


