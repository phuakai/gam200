/*!
@file    ECS.h
@author  p.jiankai@digipen.edu, ruoyan.go@digipen.edu
@date    10/9/2022

This file includes the entity component system classes and functions 
declaration.

*//*__________________________________________________________________________*/

#pragma once
#ifndef __ECS__
#define __ECS__

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
#include <registration>
#include <registration_friend>
//#include <instance.h>
#include <type>

//hang me https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/

// just making it look good
typedef std::uint32_t IDType;
typedef IDType EntityID;
typedef IDType ComponentTypeID;
const IDType NULL_ENTITY = 0;

typedef std::vector<ComponentTypeID> ArchetypeID;

//template<class C>
class ECS;
class Entity;

// class to generate IDs, will generate new IDs for each type of component etc
template<typename T>
class TypeIdGenerator{
private:

    static IDType m_count;
  

public:

    template<typename U>
    static const IDType GetNewID();
};


template<typename T> IDType TypeIdGenerator<T>::m_count = 0;

typedef unsigned char* ComponentData;

struct Archetype
{
    // archtype ID is just a vector of all components in it
    ArchetypeID type;
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
    virtual std::string GetName() const = 0;
    virtual void SetName(std::string name) = 0;
    virtual std::size_t GetSize() const = 0;


};



//templated child class



// i dont even know whats going on


template<class C>
class Component : public ComponentBase 
{

public:
    virtual void DestroyData(unsigned char* data) const override;

    virtual void MoveData(unsigned char* source, unsigned char* destination) const override;

    virtual void ConstructData(unsigned char* data) const override;

    virtual std::size_t GetSize() const override;

    virtual std::string GetName() const override;

    static ComponentTypeID GetTypeID();

    virtual void SetName(std::string name) override;


//private:
    std::string componentName;
    RTTR_ENABLE();
};

 class ECS {
private:
    typedef std::unordered_map<ComponentTypeID, ComponentBase*> ComponentTypeIDBaseMap;

    //track which entity is which archetype
    struct Record 
    {
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

    int GetSystemCount();

    template<class C>
    void RegisterComponent(std::string name);

    //template<class C>
    //bool IsComponentRegistered();

    void RegisterSystem(const std::uint8_t& layer, SystemBase* system);

    void RegisterEntity(const EntityID entityId);

    void RunSystems(const std::uint8_t& layer, const float elapsedMilliseconds);

    Archetype* GetArchetype(const ArchetypeID& id);

    template<typename C, typename... Args>
    C* AddComponent(const EntityID& entityId, Args&&... args);

    template<typename C>
    void RemoveComponent(const EntityID& entityId);

    void setEntityName(const EntityID& entityId, std::string name);

    template<typename C>
    C* GetComponent(const EntityID& entityId);

    //template<class C>
  //  bool HasComponent(const EntityID& entityId);

    void RemoveEntity(const EntityID& entityId);

    //template<class... Cs>
   // std::vector<EntityID> GetEntitiesWith();


    std::vector<std::string> getAllRegisteredComponents();
    std::vector<std::string> getEntityComponents(const EntityID& entityId);
    
    std::vector<EntityID> getEntities();
    std::vector<std::string> getEntityNames();

    std::string* getEntityName(const EntityID& entityId);

    //change back to priv 
private:

    int systemCount = 0;

    EntityArchetypeMap m_entityArchetypeMap;

    ArchetypesArray m_archetypes;

    EntityID m_entityIdCounter;

    SystemsArrayMap m_systems;

    ComponentTypeIDBaseMap m_componentMap;
};

 extern ECS ecs;

//static ECS ecs;
//------------------------------------------------------------------------------------------------------
//extern ECS ecs;

//wrapper for entity ID
class Entity 
{
public:

    explicit Entity(ECS& ecs = ecs, std::string name = "nil");

    template<typename C, typename... Args>
    C* Add(Args&&... args);

    template<typename C>
    C* Add(C&& c);

    EntityID GetID() const;


private:
    EntityID m_id;
    ECS& m_ecs;

    // maybe a  ecs.CreateEntity()
    //should have a ecs.RemoveEntity(m_id);.


        //alternate implementation
        // EntityID myEntity = ecs.CreateEntity();
        //ecs.AddComponent<Physics>(myEntity, { 2.f, 5.f, .34f, .1f });
};



#include "ECS.hpp"

#endif //__ECS__
