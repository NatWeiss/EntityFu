///
/// [EntityFu](https://github.com/NatWeiss/EntityFu)
/// A simple, fast entity component system written in C++.
/// Under the MIT license.
///

#include "EntityFu.h"
#include <iostream>
#include <unistd.h>
using namespace std;

/// An example component.
struct HealthComponent : Entity::Component
{
	int hp, maxHP;
	
	HealthComponent(int _hp, int _maxHP)
	{
		hp = _hp;
		maxHP = _maxHP;
	}
	
	HealthComponent() : HealthComponent(0, 0) {}

	virtual bool isEmpty() const {return maxHP == 0;}

	static Cid cid;
};

/// Assign unique class IDs from 0 to `numClassIds` - 1
static Cid _id = 0;
Cid HealthComponent::cid = _id++;
Cid Entity::Component::numCids = _id; // also critical to assign `numClassIds`

/// An example `Ent`.
struct System::Ent
{
	Eid id;
	HealthComponent& health;

	Ent(Eid _id) :
		health(Entity::get<HealthComponent>(_id)),
		id(_id)
	{}
};

/// An example system.
struct HealthSystem : System
{
	static void tick(double fixedDelta)
	{
		// for this example, just decrement all health components each tick
	
		// always use reference for list to avoid copy!!!
		auto const & list = Entity::getEidList<HealthComponent>();
		for (auto && eid : list)
		{
			Ent e(eid);
			
			// decrement
			e.health.hp--;
			if (e.health.hp < 0)
				e.health.hp = 0;
			cout << "Entity " << (int)e.id << " has " << e.health.hp << "/" << e.health.maxHP << " hit points." << endl;

			// destroy entity if zero health
			if (e.health.hp <= 0)
				Entity::destroyNow(eid);
		}
	}
};

int main(int argc, const char * argv[])
{
	// create an entity with one component
	Entity::create(new HealthComponent(100, 100));
	Entity::create(new HealthComponent(7, 7));
	
	// run the system
	while (Entity::count())
	{
		HealthSystem::tick(0.1);
		usleep(1000 * 100);
	}
	
	Entity::dealloc();
	cout << "Goodbye, World!\n";
    return 0;
}


