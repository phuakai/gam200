
#include <cstdint>
#include <new>
#include <string>
#include <cstddef>
#include <vector>
#include <algorithm> 
#include <functional>
#include <unordered_map>
#include <iostream>
#include <chrono>

//hang me https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/

// just making it look good
typedef std::uint32_t IDType;
typedef IDType EntityID;
typedef IDType ComponentTypeID;
const IDType NULL_ENTITY = 0;

typedef std::vector<ComponentTypeID> ArchetypeID;

class ECS;
class Entity;

// class to generate IDs, will generate new IDs for each type of component etc
template<class T>
class TypeIdGenerator
{
private:

    static IDType m_count;

public:

    template<class U>
    static const IDType GetNewID();
};


template<class T> IDType TypeIdGenerator<T>::m_count = 0;

typedef unsigned char* ComponentData;

struct Archetype
{
    ArchetypeID type;
    // archtype ID is just a vector of all components in it
    std::vector<ComponentData> componentData;
    std::vector<EntityID> entityIds;
    std::vector<std::size_t> componentDataSize;
};



//base class helps us by allowing child class to store any type of component
//by having a non-template base-class we can store any templated version of the child class using a pointer and polymorphism.
class SystemBase
{
public:
    virtual ~SystemBase() {}

    virtual ArchetypeID GetKey() const = 0;

    virtual void DoAction(const float elapsedTime, Archetype* archetype) = 0;
};


template<class... Cs>
class System : public SystemBase
{
public:

    //friend class ECS;

    // sus ? probably have to change this

    virtual ArchetypeID GetKey() const override;

    typedef std::function<void(const float, const std::vector<EntityID>&, Cs*...)> Func;

    void Action(Func func);

    //System(ECS& ecs, const std::uint8_t& layer);


protected:

    template<std::size_t Index1, typename T, typename... Ts>
    std::enable_if_t<Index1 == sizeof...(Cs)>
        DoAction(const float elapsedMilliseconds,
            const ArchetypeID& archeTypeIds,
            const std::vector<EntityID>& entityIDs,
            T& t,
            Ts... ts);

    template<std::size_t Index1, typename T, typename... Ts>
    std::enable_if_t<Index1 != sizeof...(Cs)>
        DoAction(const float elapsedMilliseconds,
            const ArchetypeID& archeTypeIds,
            const std::vector<EntityID>& entityIDs,
            T& t,
            Ts... ts);

    virtual void DoAction(const float elapsedMilliseconds,
        Archetype* archetype) override;

    ECS& m_ecs;
    Func m_func;
    bool m_funcSet;

public:
    System(ECS& ecs, const std::uint8_t& layer) : m_ecs(ecs), m_funcSet(false)
    {
        m_ecs.RegisterSystem(layer, this);
    }

};



class ComponentBase
{
public:
    virtual ~ComponentBase() {}
    virtual void ConstructData(unsigned char* data) const = 0;
    virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;
    virtual void DestroyData(unsigned char* data) const = 0;


    virtual std::size_t GetSize() const = 0;
};



//templated child class

template<class C>


// i dont even know whats going on


class Component : public ComponentBase {

public:
    virtual void DestroyData(unsigned char* data) const override;

    virtual void MoveData(unsigned char* source, unsigned char* destination) const override;

    virtual void ConstructData(unsigned char* data) const override;

    virtual std::size_t GetSize() const override;

    static ComponentTypeID GetTypeID();
};

class ECS {
private:
    typedef std::unordered_map<ComponentTypeID, ComponentBase*> ComponentTypeIDBaseMap;



    //track which entity is which archetype 
    //idk how this works tbh
    struct Record {
        Archetype* archetype;
        std::size_t index;
    };

    typedef std::unordered_map<EntityID, Record> EntityArchetypeMap;


    //array of all archetypes
    typedef std::vector<Archetype*> ArchetypesArray;


    // iterate over systems to provide a way to order system calls
    typedef std::unordered_map<std::uint8_t, std::vector<SystemBase*>>
        SystemsArrayMap;

public:

    ECS();
    ~ECS();

    EntityID GetNewID();

    template<class C>
    void RegisterComponent();

    template<class C>
    bool IsComponentRegistered();

    void RegisterSystem(const std::uint8_t& layer, SystemBase* system);

    void RegisterEntity(const EntityID entityId);

    void RunSystems(const std::uint8_t& layer, const float elapsedMilliseconds);

    Archetype* GetArchetype(const ArchetypeID& id);

    template<class C, typename... Args>
    C* AddComponent(const EntityID& entityId, Args&&... args);

    template<class C>
    void RemoveComponent(const EntityID& entityId);

    //template<class C>
   // C* GetComponent(const EntityID& entityId);

    //template<class C>
  //  bool HasComponent(const EntityID& entityId);

    void RemoveEntity(const EntityID& entityId);

    //template<class... Cs>
   // std::vector<EntityID> GetEntitiesWith();

private:

    EntityArchetypeMap m_entityArchetypeMap;

    ArchetypesArray m_archetypes;

    EntityID m_entityIdCounter;

    SystemsArrayMap m_systems;

    ComponentTypeIDBaseMap m_componentMap;
};
//------------------------------------------------------------------------------------------------------
extern ECS ecs;

//wrapper for entity ID
class Entity {
public:

    explicit Entity(ECS& ecs = ecs) : m_id(ecs.GetNewID()), m_ecs(ecs) {
        m_ecs.RegisterEntity(m_id);
    }
    template<class C, typename... Args>
    C* Add(Args&&... args)
    {
        return m_ecs.AddComponent<C>(m_id, std::forward<Args>(args)...);
    }
    template<class C>
    C* Add(C&& c)
    {
        return m_ecs.AddComponent<C>(m_id, std::forward<C>(c));
    }
    EntityID GetID() const
    {
        return m_id;
    }

private:
    EntityID m_id;
    ECS& m_ecs;

    // maybe a  ecs.CreateEntity()
    //should have a ecs.RemoveEntity(m_id);.


        //alternate implementation
        // EntityID myEntity = ecs.CreateEntity();
        //ecs.AddComponent<Physics>(myEntity, { 2.f, 5.f, .34f, .1f });
};


struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float x;
    float y;
};

struct Randomness
{
    float a;
};

struct he {
    int a;
};

template<class C>
void registerComponent();