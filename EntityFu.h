///
/// [EntityFu](https://github.com/NatWeiss/EntityFu)
/// A simple, fast entity component system written in C++.
/// Under the MIT license.
///

#pragma once
#include <vector>

/// An `Eid` is an entity ID.
typedef unsigned Eid;

/// A `Cid` is a component ID.
typedef unsigned Cid;

class Entity
{
	public:
		struct Component;

		/// The maximum number of entities. Increase this if you need more.
		enum {kMaxEntities = 4096};

		/// Allocate the memory for entities and components. Can call this manually or let it allocate automatically.
		static void alloc();
	
		/// Deallocate the memory for entities and components. Only do this when you no longer need the ECS.
		static void dealloc();
	
		/// Create an entity and return the `Eid`.
		static Eid create();

		/// Return a count of all entities.
		static unsigned count();
	
		/// Destroy an entity and all its components right now.
		static void destroyNow(Eid eid);
	
		/// Destroy all entities and components right now.
		static void destroyAll();
	
		/// Add the given component to the given entity.
		/// Note that components must be allocated with new.
		template<class ComponentClass> inline static void addComponent(Eid eid, ComponentClass* c)
		{
			return Entity::addComponent(ComponentClass::cid, eid, c);
		}

		/// Remove a component from an entity.
		template<class ComponentClass> inline static void removeComponent(Eid eid)
		{
			return Entity::removeComponent(ComponentClass::cid, eid);
		}

		/// Get a pointer to a component.
		template<class ComponentClass> inline static ComponentClass* getPointer(Eid eid)
		{
			return static_cast<ComponentClass*>(Entity::getComponent(ComponentClass::cid, eid));
		}

		/// Get a reference to a component.
		/// Warning, if the Eid does not exist a blank static component will be returned,
		/// so be sure to check if it is empty before writing any data to it or else
		/// the blank static component will no longer be empty and the Earth will implode.
		template<class ComponentClass> inline static ComponentClass& get(Eid eid)
		{
			return Entity::ref(Entity::getPointer<ComponentClass>(eid));
		}
	
		/// Get a vector of all Eids for the given component class.
		template<class ComponentClass> inline static const std::vector<Eid>& getAll()
		{
			return Entity::getAll(ComponentClass::cid);
		}
	
		/// Count all the entities with the given component class.
		template<class ComponentClass> inline static unsigned count()
		{
			return Entity::count(ComponentClass::cid);
		}

		/// Create an entity with some components.
		/// Convenience method which calls `Entity::create()` and `Entity::addComponent`.
		template<class C1> static Eid create(C1* c1)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			return eid;
		}

		template<class C1, class C2> static Eid create(C1* c1, C2* c2)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			return eid;
		}

		template<class C1, class C2, class C3> static Eid create(C1* c1, C2* c2, C3* c3)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			Entity::addComponent(C3::cid, eid, c3);
			return eid;
		}

		template<class C1, class C2, class C3, class C4> static Eid create(C1* c1, C2* c2, C3* c3, C4* c4)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			Entity::addComponent(C3::cid, eid, c3);
			Entity::addComponent(C4::cid, eid, c4);
			return eid;
		}

		template<class C1, class C2, class C3, class C4, class C5> static Eid create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			Entity::addComponent(C3::cid, eid, c3);
			Entity::addComponent(C4::cid, eid, c4);
			Entity::addComponent(C5::cid, eid, c5);
			return eid;
		}

		template<class C1, class C2, class C3, class C4, class C5, class C6> static Eid create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5, C6* c6)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			Entity::addComponent(C3::cid, eid, c3);
			Entity::addComponent(C4::cid, eid, c4);
			Entity::addComponent(C5::cid, eid, c5);
			Entity::addComponent(C6::cid, eid, c6);
			return eid;
		}

		template<class C1, class C2, class C3, class C4, class C5, class C6, class C7> static Eid create(C1* c1, C2* c2, C3* c3, C4* c4, C5* c5, C6* c6, C7* c7)
		{
			auto eid = Entity::create();
			Entity::addComponent(C1::cid, eid, c1);
			Entity::addComponent(C2::cid, eid, c2);
			Entity::addComponent(C3::cid, eid, c3);
			Entity::addComponent(C4::cid, eid, c4);
			Entity::addComponent(C5::cid, eid, c5);
			Entity::addComponent(C6::cid, eid, c6);
			Entity::addComponent(C7::cid, eid, c7);
			return eid;
		}

	private:
		/// Disallow construction. Entities are not classes!
		Entity() {}
		Entity(const Entity& rhs) {}
		~Entity() {}
		Entity& operator=(const Entity& rhs) {return *this;}

		static void addComponent(Cid cid, Eid eid, Component* c);
		static void removeComponent(Cid cid, Eid eid);
		static Component* getComponent(Cid cid, Eid eid);
		static const std::vector<Eid>& getAll(Cid cid);
		static unsigned count(Cid cid);
		static void log(Cid cid);
		static void logAll();

		static bool* entities;
		static Component*** components;
		static std::vector<Eid>* componentEids;

		/// Get a guaranteed reference to a component.
		template<class ComponentClass> static ComponentClass& ref(ComponentClass* p)
		{
			if (p) return *p;
			static ComponentClass s;
			return s;
		}
};

///
/// Component
///
/// Inherit your game's components from this class.
/// Give your component some data and be sure to implement the `isEmpty` method.
/// Also give your component class a static member `cid` of type `Cid` and increment `Entity::Component::numCids`.
/// See the example code for details.
///
struct Entity::Component
{
	virtual ~Component() {}
	virtual bool isEmpty() const = 0;
	static Cid numCids;
	// static Cid cid;
};

///
/// System
///
/// Just implement either `tick` or `animate` or both.
/// Feel free to change this per your timestep implementation.
/// `Ent` is a convenience struct which gets all the most often used components for an entity by reference,
/// hence its implementation is based on your system design.
///
class System
{
	public:
		struct Ent;
	
		static void tick(double fixedDelta) {}
		static void animate(double delta, double tickPercent) {}
};


