///
/// [EntityFu](https://github.com/NatWeiss/EntityFu)
/// A simple, fast entity component system written in C++.
/// Under the MIT license.
///

#include "EntityFu.h"
#include <algorithm>
using namespace std;

/// Turn this on to have a faster yet riskier ECS.
#define kTrustPointers 0

/// Log macro.
#ifndef LogV
#ifndef NDEBUG
	#include <stdio.h>
	#define LogV(verbosity, minVerbosity, ...) do {if ((verbosity) >= (minVerbosity)) {printf(__VA_ARGS__); printf("\n");}} while (0)
#else
	#define LogV(...) do {} while (0)
#endif
#endif

/// Assert macro.
#ifndef Assert
#ifndef NDEBUG
	#define Assert(condition, format, ...) {if(!condition) throw format;}
#else
	#define Assert(...) do {} while (0)
#endif
#endif

/// Turn this to 1 or 2 to debug the ECS.
/// 1 == log creation, 2 == also log deletion, 3 == also log component add/remove, 4 == also log component totals.
static int verbosity = 0;

/// Static pointers to the ECS data.
static bool* entities = nullptr;
static Entity::Component*** components = nullptr;
static vector<Eid>* componentEids = nullptr;

static void log(Cid cid)
{
	auto n = Entity::count(cid);
	auto& eids = Entity::getAll(cid);
	if (eids.size() > 0)
		LogV(verbosity, 4, "  Cid %u has %d entities ranging from %u to %u", cid, n, eids.front(), eids.back());
}

void Entity::alloc()
{
	if (components != nullptr)
		return;
	LogV(verbosity, 1, "Allocing entities");

	// allocate entities
	entities = new bool[kMaxEntities];
	for (Eid eid = 0; eid < kMaxEntities; ++eid)
		entities[eid] = false;

	// allocate components
	auto max = Component::numCids;
	components = new Component**[max];
	componentEids = new vector<Eid>[Component::numCids];
	for (Cid cid = 0; cid < max; cid++)
	{
		// allocate component array
		components[cid] = new Component*[kMaxEntities];
		
		// zero component pointers
		for (Eid eid = 0; eid < kMaxEntities; eid++)
			components[cid][eid] = nullptr;
	}
}

void Entity::dealloc()
{
	LogV(verbosity, 1, "Deallocing entities");
	
	if (components != nullptr)
	{
		Entity::destroyAll();
		for (Cid cid = 0; cid < Component::numCids; cid++)
			if (components[cid] != nullptr)
				delete [] components[cid];
		delete [] components;
	}

	if (componentEids != nullptr)
		delete [] componentEids;

	if (entities != nullptr)
		delete [] entities;
	
	entities = nullptr;
	components = nullptr;
	componentEids = nullptr;
}

Eid Entity::create()
{
	// auto allocate
	Entity::alloc();
	
	Eid eid = 1;
	for (; eid < kMaxEntities && entities[eid]; ++eid)
	{
	}

	if (eid < 1 || eid >= kMaxEntities)
	{
		Assert(false, "Maximum number of entities reached!");
		eid = 0;
	}
	else
	{
		entities[eid] = true;
		LogV(verbosity, 1, "Entity %u created", eid);
	}
	
	return eid;
}

void Entity::destroyNow(Eid eid)
{
	if (eid == 0)
		return;
	LogV(verbosity, 1, "Entity %u being destroyed", eid);

	for (Cid cid = 0; cid < Component::numCids; cid++)
		Entity::removeComponent(cid, eid);
	entities[eid] = false;
}

void Entity::destroyAll()
{
	unsigned count = 0;
	auto oldVerbosity = verbosity;
	verbosity = 0;
	for (Eid eid = 1; eid < kMaxEntities; ++eid)
	{
		if (entities[eid])
		{
			Entity::destroyNow(eid);
			count++;
		}
	}
	verbosity = oldVerbosity;
	LogV(verbosity, 1, "%u entities destroyed", count);
}

void Entity::addComponent(Cid cid, Eid eid, Component* c)
{
	if (c == nullptr)
		return;
	if (eid >= kMaxEntities || !entities[eid] || cid >= Component::numCids)
	{
		Assert(false, "Invalid eid %u or cid %u", eid, cid);
		return;
	}

	if (verbosity >= 4)
		log(cid);
	LogV(verbosity, 3, "    Adding component cid %u eid %u (%x)", cid, eid, (int)(long)c);
	
	// if component already added, delete old one
	if (components[cid][eid] != nullptr)
		Entity::removeComponent(cid, eid);
	
	// pointers to components are stored in the map
	// (components must be allocated with new, not stack objects)
	components[cid][eid] = c;
	
	// store component eids
	componentEids[cid].push_back(eid);

	if (verbosity >= 4)
		log(cid);
}

void Entity::removeComponent(Cid cid, Eid eid)
{
	if (eid >= kMaxEntities || !entities[eid] || cid >= Component::numCids)
	{
		Assert(false, "Invalid eid %u or cid %u", eid, cid);
		return;
	}

	// get pointer
	auto ptr = components[cid][eid];
	if (ptr == nullptr)
		return;

	if (verbosity >= 4)
		log(cid);
	LogV(verbosity, 3, "    Removing component cid %u eid %u (%x)", cid, eid, (int)(long)ptr);

	// pointers to components are deleted
	delete ptr;
	
	// erase the component pointer
	components[cid][eid] = nullptr;

	// update component eids
	auto& eids = componentEids[cid];
	auto it = find(eids.begin(), eids.end(), eid);
	if (it != eids.end())
		it = eids.erase(it);
	
	if (verbosity >= 4)
		log(cid);
}

Entity::Component* Entity::getComponent(Cid cid, Eid eid)
{
#if (kTrustPointers == 0)
	if (eid < kMaxEntities && cid < Component::numCids)
	{
#endif
		return components[cid][eid];

#if (kTrustPointers == 0)
	}
	return nullptr;
#endif
}

const vector<Eid>& Entity::getAll(Cid cid)
{
	if (componentEids != nullptr && cid < Component::numCids)
		return componentEids[cid];
	static vector<Eid> blankEids;
	return blankEids;
}

unsigned Entity::count()
{
	int ret = 0;
	if (entities != nullptr)
	{
		for (Eid eid = 1; eid < kMaxEntities; ++eid)
			if (entities[eid])
				++ret;
	}
	return ret;
}

unsigned Entity::count(Cid cid)
{
	return (unsigned)Entity::getAll(cid).size();
}

bool Entity::exists(Eid eid)
{
	return entities != nullptr && entities[eid];
}

//
// Entity::Component
//

Entity::Component::~Component()
{
}

bool Entity::Component::full() const
{
	return !this->empty();
}

//
// System
//

void System::tick(double fixedDelta)
{
}

void System::animate(double delta, double tickPercent)
{
}


