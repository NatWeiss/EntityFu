///
/// [EntityFu](https://github.com/NatWeiss/EntityFu)
/// A simple, fast entity component system written in C++.
/// Under the MIT license.
///

#pragma once

typedef unsigned int Cid; // Component-ID

class Entity
{
	public:
		struct Component;

		/// Typedef for entity IDs. Use `Eid` for shorthand.
		typedef unsigned eidType;
	
		/// The maximum entity ID number. Increase this if you need more entities.
		enum {kMaxEid = 4096};

		/// Allocate the memory for entities and components. Can call this manually or let it allocate automatically.
		static void alloc();
	
		/// Deallocate the memory for entities and components. Only do this when you no longer need the ECS.
		static void dealloc();
	
		/// Create an entity and return the `Eid`.
		static eidType create();

		/// Return a count of all entities.
		static int count();
	
		/// Destroy an entity and all its components right now.
		static void destroyNow(eidType eid);
	
		/// Destroy all entities and components right now.
		static void destroyAll();
	
		/// Return the minimum `Eid` for the given component class.
		static eidType getMinEid(Cid componentClass);
	
		/// Return the maximum `Eid` for the given component class.
		static eidType getMaxEid(Cid componentClass);
	
		/// Add the given component to the given entity.
		/// Note that components must be allocated with new.
		template<class ComponentClass> inline static void addComponent(eidType eid, ComponentClass* c)
		{
			return Entity::addComponent(eid, c, ComponentClass::classId);
		}

		/// Remove a component from an entity.
		template<class ComponentClass> inline static void removeComponent(eidType eid)
		{
			return Entity::removeComponent(eid, ComponentClass::classId);
		}

		/// Return true if the given component exists for the entity.
		template<class ComponentClass> inline static bool hasComponent(eidType eid)
		{
			return Entity::getComponent(eid, ComponentClass::classId) != nullptr;
		}

		/// Get a pointer to a component.
		template<class ComponentClass> inline static ComponentClass* getPointer(eidType eid)
		{
			return static_cast<ComponentClass*>(Entity::getComponent(eid, ComponentClass::classId));
		}

		/// Get a reference to a component.
		template<class ComponentClass> inline static ComponentClass& get(eidType eid)
		{
			return Entity::ref(Entity::getPointer<ComponentClass>(eid));
		}
	
		/// Count all the entities with the given component class.
		template<class ComponentClass> inline static int count()
		{
			return Entity::count(ComponentClass::classId);
		}

		/// Create an entity with some components.
		/// Convenience method which calls `Entity::create()` and `Entity::addComponent`.
		template<class C1> static eidType create(C1* c1)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			return id;
		}

		template<class C1, class C2> static eidType create(C1* c1, C2* c2)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			return id;
		}

		template<class C1, class C2, class C3> static eidType create(C1* c1, C2* c2, C3* c3)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			Entity::addComponent(id, c3, C3::classId);
			return id;
		}

		template<class C1, class C2, class C3, class C4> static eidType create(C1* c1, C2* c2, C3* c3, C4* c4)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			Entity::addComponent(id, c3, C3::classId);
			Entity::addComponent(id, c4, C4::classId);
			return id;
		}

		template<class C1, class C2, class C3, class C4, class C5> static eidType create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			Entity::addComponent(id, c3, C3::classId);
			Entity::addComponent(id, c4, C4::classId);
			Entity::addComponent(id, c5, C5::classId);
			return id;
		}

		template<class C1, class C2, class C3, class C4, class C5, class C6> static eidType create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5, C6* c6)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			Entity::addComponent(id, c3, C3::classId);
			Entity::addComponent(id, c4, C4::classId);
			Entity::addComponent(id, c5, C5::classId);
			Entity::addComponent(id, c6, C6::classId);
			return id;
		}

		template<class C1, class C2, class C3, class C4, class C5, class C6, class C7> static eidType create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5, C6* c6, C7* c7)
		{
			auto id = Entity::create();
			Entity::addComponent(id, c1, C1::classId);
			Entity::addComponent(id, c2, C2::classId);
			Entity::addComponent(id, c3, C3::classId);
			Entity::addComponent(id, c4, C4::classId);
			Entity::addComponent(id, c5, C5::classId);
			Entity::addComponent(id, c6, C6::classId);
			Entity::addComponent(id, c7, C7::classId);
			return id;
		}

	private:
		/// Disallow construction. Entities are not classes!
		Entity() {}
		Entity(const Entity& rhs) {}
		~Entity() {}
		Entity& operator=(const Entity& rhs) {return *this;}

		static void addComponent(eidType eid, Component* c, Cid componentClass);
		static void removeComponent(eidType eid, Cid componentClass);
		static Component* getComponent(eidType eid, Cid cid);
		static int count(Cid componentClass);
		static void log(Cid componentClass);
		static void logAll();

		static bool* entities;
		static Component*** components;
		static eidType* minEids;
		static eidType* maxEids;

		/// Get a guaranteed reference to a component.
		template<class ComponentClass> static ComponentClass& ref(ComponentClass* p)
		{
			if (p) return *p;
			static ComponentClass s;
			return s;
		}
};

/// `Eid` is shorthand for `Entity::eidType`.
typedef Entity::eidType Eid;

/// Loop over all the entities of a given component class.
/// Use the variable `eid` to get the components.
/// Ensure that the `eid` actually has the given component class with `Entity::hasComponent`, `Entity::getPointer` or `Entity::get`.
#define forAllEntities(c) for (auto eid = Entity::getMinEid(c::classId), eidMax = Entity::getMaxEid(c::classId); eid <= eidMax; eid++)

///
/// Component
///
/// Inherit your game's components from this class.
/// Give your component some data and be sure to implement the `isEmpty` method.
/// Also give your component class a static `classId` integer and increment `Entity::Component::numClassIds`.
/// See the example code for more details.
///
struct Entity::Component
{
	virtual ~Component() {}
	virtual bool isEmpty() const = 0;
	static Cid numClassIds;
	// static Cid classId;
};

///
/// System
///
/// Just implement either `tick` or `animate` or both.
/// Feel free to change this per your timestep implementation.
/// `Ent` is a convenience struct which gets all the most often used components for an entity by reference,
/// hence its implementation is entirely up to your kind of systems.
///
class System
{
	public:
		struct Ent;
	
		static void tick(double fixedDelta) {}
		static void animate(double delta, double tickPercent) {}
};


