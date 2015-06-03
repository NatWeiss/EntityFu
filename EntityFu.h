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
		template <typename ...Args> static Eid create(Args... args)
		{
			auto eid = Entity::create();
			Entity::addComponents(eid, args...);
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

		/// The variadic template version of `addComponents`.
		template <class C, typename ...Args> static void addComponents(Eid eid, C* c, Args... args)
		{
			Entity::addComponent(C::cid, eid, c);
			Entity::addComponents(eid, args...);
		}

		/// The final call to `addComponents`.
		template <class C> static void addComponents(Eid eid, C* c)
		{
			Entity::addComponent(C::cid, eid, c);
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


