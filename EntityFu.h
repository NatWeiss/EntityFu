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

namespace Entity
{
	struct Component;

	/// The maximum number of entities. Increase this if you need more.
	enum {kMaxEntities = 8192};

	/// Allocate the memory for entities and components. Can call this manually or let it allocate automatically.
	void alloc();

	/// Deallocate the memory for entities and components. Only do this when you no longer need the ECS.
	void dealloc();

	/// Create an entity and return the `Eid`.
	Eid create();

	/// Return a count of all entities.
	unsigned count();

	/// Return true if the entity has been created.
	bool exists(Eid eid);

	/// Destroy an entity and all its components right now.
	void destroyNow(Eid eid);

	/// Destroy all entities and components right now.
	void destroyAll();

	/// Component-related methods that require a `Cid`.
	/// The templated versions of these methods do not require a `Cid`, yet incur an extra function call of overhead.
	void addComponent(Cid cid, Eid eid, Component* c);
	void removeComponent(Cid cid, Eid eid);
	Component* getComponent(Cid cid, Eid eid);
	const std::vector<Eid>& getAll(Cid cid);
	unsigned count(Cid cid);

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

	/// Get a reference to a component.
	/// Warning, if the Eid does not exist a blank static component will be returned,
	/// so be sure to check if it is empty before writing any data to it or else
	/// the blank static component will no longer be empty and the Earth will implode.
	template<class ComponentClass> inline static ComponentClass& get(Eid eid)
	{
		auto p = static_cast<ComponentClass*>(Entity::getComponent(ComponentClass::cid, eid));
		if (p != nullptr)
			return *p;
		static ComponentClass s;
		return s;
	}

	/// Get a pointer to a component.
	template<class ComponentClass> inline static ComponentClass* getPointer(Eid eid)
	{
		return static_cast<ComponentClass*>(Entity::getComponent(ComponentClass::cid, eid));
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

	/// A utility method for `Entity::create(...)`.
	/// The final call to `addComponents`.
	template <class C> static void addComponents(Eid eid, C* c)
	{
		Entity::addComponent(C::cid, eid, c);
	}

	/// A utility method for `Entity::create(...)`.
	/// The variadic template version of `addComponents`.
	template <class C, typename ...Args> static void addComponents(Eid eid, C* c, Args... args)
	{
		Entity::addComponent(C::cid, eid, c);
		Entity::addComponents(eid, args...);
	}

	/// Create an entity with some components.
	/// Convenience method which calls `Entity::create()` and `Entity::addComponent`.
	template <typename ...Args> static Eid create(Args... args)
	{
		auto eid = Entity::create();
		Entity::addComponents(eid, args...);
		return eid;
	}

};

///
/// Component
///
/// Inherit your game's components from this class.
/// Give your component some data and be sure to implement the `empty` method.
/// Also give your component class a static member `cid` of type `Cid` and increment `Entity::Component::numCids`.
/// See the example code for details.
///
struct Entity::Component
{
	virtual ~Component();
	virtual bool empty() const = 0;
	virtual bool full() const;
	static Cid numCids;
	// static Cid cid;
};

///
/// Convenience macro to get a reference to a component or else run some code.
/// Example: Entity__get(eid, health, HealthComponent, continue);
///
#define Entity__get(componentClass, eid, varName, orElseCode) \
	auto& (varName) = Entity::get<componentClass>((eid)); \
	if ((varName).empty()) orElseCode

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
	
		static void tick(double fixedDelta);
		static void animate(double delta, double tickPercent);
};


