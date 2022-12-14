/*!
@file    ECS.hpp
@author  p.jiankai@digipen.edu, ruoyan.go@digipen.edu
@date    10/9/2022

This file includes the entity component system classes and functions
definitions.

*//*__________________________________________________________________________*/

template<typename T>
template<typename U>
const IDType TypeIdGenerator<T>::GetNewID() {
    static const IDType idCounter = m_count++;
    return idCounter;
}

inline Entity::Entity(ECS& theecs, std::string name) : m_id(theecs.GetNewID()), m_ecs(theecs)
{
    m_ecs.RegisterEntity(m_id);
}
template<typename C, typename... Args>
inline C* Entity::Add(Args&&... args)
{
    return m_ecs.AddComponent<C>(m_id, std::forward<Args>(args)...);
}
template<typename C>
inline C* Entity::Add(C&& c)
{
    return m_ecs.AddComponent<C>(m_id, std::forward<C>(c));
}
inline EntityID Entity::GetID() const
{
    return m_id;
}

template<class C>
void Component<C>::SetName(std::string name) 
{
    componentName = name;
}
template<class C>
std::string Component<C>::GetName() const 
{
    return componentName;
}
// not 100% sure how this works but finds the correct place to delete memory
template<class C>
void Component<C>::DestroyData(unsigned char* data) const
{
    C* dataLocation = std::launder(reinterpret_cast<C*>(data));

    dataLocation->~C();
}
// just allocating new memory
template<class C>
void Component<C>::ConstructData(unsigned char* data) const
{
    new (&data[0]) C();
}
// same as above
template<class C>
void Component<C>::MoveData(unsigned char* source, unsigned char* destination) const
{
    new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
}
//get size of components to help ecs later
template<class C>
std::size_t Component<C>::GetSize() const
{
    return sizeof(C);
}
// check how this works
template<class C>
ComponentTypeID Component<C>::GetTypeID()
{
    return TypeIdGenerator<ComponentBase>::GetNewID<C>();
}

inline Archetype* ECS::GetArchetype(const ArchetypeID& id)
{
    for (Archetype* archetype : m_archetypes)
    {
        if (archetype->type == id)
            return archetype;
    }

    // archetype doesn't exist, so create a new one
    
    Archetype* newArchetype = new Archetype;
    newArchetype->type = id;
    m_archetypes.push_back(newArchetype);

    // add an empty array for each component in the type
    for (ArchetypeID::size_type i = 0; i < id.size(); ++i)
    {
        if (newArchetype->type[i])
        {
            newArchetype->componentData.push_back(new unsigned char[0]);
            newArchetype->componentDataSize.push_back(1);
        }
        else
        {
            newArchetype->componentData.push_back(nullptr);
            newArchetype->componentDataSize.push_back(0);
        }
    }

    ///for (ArchetypeID::size_type i = 0; i < id.size(); ++i)
    ///{
    ///    newArchetype->componentData.push_back(new unsigned char[0]);
    ///    newArchetype->componentDataSize.push_back(0);
    ///}

    return newArchetype;
}
inline ECS::ECS()
    :
    m_entityIdCounter(1)
{}

inline EntityID ECS::GetNewID()
{
    return m_entityIdCounter++;
}
inline int ECS::GetSystemCount()
{
    return systemCount;
}
//
template<class C>
void ECS::RegisterComponent(std::string name)
{
    ComponentTypeID componentTypeId = Component<C>::GetTypeID();

    if (m_componentMap.contains(componentTypeId))
    {
        return; // can't re-register a type
    }

    m_componentMap.emplace(componentTypeId, new Component<C>);
    m_componentMap[componentTypeId]->SetName(name);

    systemCount++;;
}

inline void ECS::RegisterSystem(const std::uint8_t& layer, SystemBase* system)
{
    m_systems[layer].push_back(system);
}

inline void ECS::RegisterEntity(const EntityID entityId)
{
    Record dummyRecord;
    dummyRecord.archetype = nullptr;
    dummyRecord.index = 0;
    m_entityArchetypeMap[entityId] = dummyRecord;
}

template<typename C, typename... Args>
inline C* ECS::AddComponent(const EntityID& entityID, Args&&... args) 
{
    // no safety checks

    ComponentTypeID newCompTypeId = Component<C>::GetTypeID();

    const std::size_t& compDataSize = m_componentMap[newCompTypeId]->GetSize();

    // this ensures the entity is added to dummy archetype if needed
    Record& record = m_entityArchetypeMap[entityID];
    Archetype* oldArchetype = record.archetype;

    C* newComponent = nullptr; // will be returned

    Archetype* newArchetype = nullptr;

    if (oldArchetype)
    {
        if (oldArchetype->type[newCompTypeId])
        {
            return nullptr;
        }

        // Copy the ArchetypeID to add in the new component
        ArchetypeID newArchetypeId = oldArchetype->type;
        ///newArchetypeId.push_back(newCompTypeId);
        newArchetypeId[newCompTypeId] = 1;
        ///std::sort(newArchetypeId.begin(), newArchetypeId.end());

        newArchetype = GetArchetype(newArchetypeId);


        //----------------------------------------------------------------------------------REWRITE THIS
        for (std::size_t j = 0; j < newArchetypeId.size(); ++j)
        {
            if (!newArchetypeId[j])
            {
                continue;
            }

            ///const ComponentTypeID& newCompId = newArchetypeId[j];

            const ComponentBase* const newComp = m_componentMap[j];

            const std::size_t& newCompDataSize = newComp->GetSize();

            std::size_t currentSize = newArchetype->entityIds.size() * newCompDataSize;
            std::size_t newSize = currentSize + newCompDataSize;
            if (newSize > newArchetype->componentDataSize[j])
            {
                newArchetype->componentDataSize[j] *= 2;
                newArchetype->componentDataSize[j] += newCompDataSize;
                unsigned char* newData = new unsigned char[newArchetype->componentDataSize[j]];
                for (std::size_t e = 0; e < newArchetype->entityIds.size(); ++e)
                {
                    newComp->MoveData(&newArchetype->componentData[j][e * newCompDataSize],
                        &newData[e * newCompDataSize]);
                    newComp->DestroyData(&newArchetype->componentData[j][e * newCompDataSize]);
                }
                delete[] newArchetype->componentData[j];

                newArchetype->componentData[j] = newData;
            }

            ArchetypeID oldArchetypeId = oldArchetype->type;

            for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
            {
                if (!(oldArchetype->type[i]))
                {
                    continue;
                }

                if (i == j)
                {
                    const ComponentBase* const oldComp = m_componentMap[i];

                    const std::size_t& oldCompDataSize = oldComp->GetSize();

                    oldComp->MoveData(&oldArchetype->componentData[i][record.index * oldCompDataSize],
                        &newArchetype->componentData[j][currentSize]);
                    oldComp->DestroyData(&oldArchetype->componentData[i][record.index * oldCompDataSize]);

                    goto cnt;
                }
            }

            newComponent
                = new (&newArchetype->componentData[j][currentSize])
                C(std::forward<Args>(args)...);

        cnt:;
        }

        if (!oldArchetype->entityIds.empty())
        {
            for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
            {
                if (!(oldArchetype->type[i]))
                {
                    continue;
                }

                if (i == newCompTypeId)
                {
                    ComponentBase* removeWrapper = m_componentMap[newCompTypeId];
                    removeWrapper->DestroyData(
                        &oldArchetype->componentData[i][record.index * sizeof(C)]);
                }

                const ComponentBase* const oldComp = m_componentMap[i];

                const std::size_t& oldCompDataSize = oldComp->GetSize();

                //std::size_t currentSize = oldArchetype->entityIds.size() * oldCompDataSize;
                //std::size_t newSize = currentSize - oldCompDataSize;
                unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];
                oldArchetype->componentDataSize[i] -= oldCompDataSize;
                for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIds.size(); ++e)
                {
                    if (e == record.index)
                        continue;

                    oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                        &newData[ei * oldCompDataSize]);
                    oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

                    ++ei;
                }

                delete[] oldArchetype->componentData[i];

                oldArchetype->componentData[i] = newData;
            }
        }

        std::vector<EntityID>::iterator willBeRemoved
            = std::find(oldArchetype->entityIds.begin(),
                oldArchetype->entityIds.end(),
                entityID);

        std::for_each(willBeRemoved, oldArchetype->entityIds.end(),
            [this, &oldArchetype](const EntityID& eid)
            {
                Record& moveR = m_entityArchetypeMap[eid];
                --moveR.index;
            });

        oldArchetype->entityIds.erase(willBeRemoved);
    }
    // if "archetype" in records is not created yet (new entity)
    else
    {
        ///ArchetypeID newArchetypeId(1, newCompTypeId);
        ArchetypeID newArchetypeId(systemCount, 0);
        newArchetypeId[newCompTypeId] = 1;

        const ComponentBase* const newComp = m_componentMap[newCompTypeId];

        newArchetype = GetArchetype(newArchetypeId);

        std::size_t currentSize = newArchetype->entityIds.size() * compDataSize;
        std::size_t newSize = currentSize + compDataSize;
        if (newSize > newArchetype->componentDataSize[newCompTypeId])
        {
            newArchetype->componentDataSize[newCompTypeId] *= 2;
            newArchetype->componentDataSize[newCompTypeId] += compDataSize;
            unsigned char* newData = new unsigned char[newArchetype->componentDataSize[newCompTypeId]];
            for (std::size_t e = 0; e < newArchetype->entityIds.size(); ++e)
            {
                newComp->MoveData(&newArchetype->componentData[newCompTypeId][e * compDataSize],
                    &newData[e * compDataSize]);
                newComp->DestroyData(&newArchetype->componentData[newCompTypeId][e * compDataSize]);
            }
            delete[](newArchetype->componentData[newCompTypeId]);

            newArchetype->componentData[newCompTypeId] = newData;
        }

        newComponent
            = new (&newArchetype->componentData[newCompTypeId][currentSize])
            C(std::forward<Args>(args)...);
    }

    newArchetype->entityIds.push_back(entityID);
    record.index = newArchetype->entityIds.size() - 1;
    record.archetype = newArchetype;

    return newComponent;
}
/*
If i understand correctly each time you add a component you rebuild each component array from the old archetype and decrement all entity index after the
one you removed from the archetype index. i think it would be faster if you just swapped the last entity in the old archetype with the one you?re removing.
that way you only have to move data once per component and change only one index. and only rebuild the entire thing if too much memory is allocated.

*/

template<class C>
inline void ECS::RemoveComponent(const EntityID& entityId)
{
    //if (!IsComponentRegistered<C>())
      //  return;

    ComponentTypeID compTypeId = Component<C>::GetTypeID();

    // if (!m_entityArchetypeMap.contains(entityId))
      //   return; // it doesn't exist

    Record& record = m_entityArchetypeMap[entityId];

    Archetype* oldArchetype = record.archetype;

    if (!oldArchetype)
        return; // there's no components anyway

    ///if (std::find(oldArchetype->type.begin(),
    ///    oldArchetype->type.end(),
    ///    compTypeId)
    ///    == oldArchetype->type.end())
    ///{
    ///    return;
    ///}
    if (!oldArchetype->type[compTypeId])
    {
        // this entity doesn't have this component
        return;
    }

    // find the new archetypeId by removing the old ComponentTypeId
    ArchetypeID newArchetypeId = oldArchetype->type;
    newArchetypeId[compTypeId] = 0;
    ///newArchetypeId.erase(std::remove(newArchetypeId.begin(),
    ///    newArchetypeId.end(),
    ///    compTypeId),
    ///    newArchetypeId.end());
    ///std::sort(newArchetypeId.begin(), newArchetypeId.end());

    Archetype* newArchetype = GetArchetype(newArchetypeId);

    for (std::size_t j = 0; j < newArchetypeId.size(); ++j)
    {
        if (!newArchetypeId[j])
        {
            continue;
        }

        const ComponentBase* const newComp = m_componentMap[(unsigned int)j];

        const std::size_t& newCompDataSize = newComp->GetSize();

        std::size_t currentSize = newArchetype->entityIds.size() * newCompDataSize;
        std::size_t newSize = currentSize + newCompDataSize;
        if (newSize > newArchetype->componentDataSize[j])
        {
            newArchetype->componentDataSize[j] *= 2;
            newArchetype->componentDataSize[j] += newCompDataSize;
            unsigned char* newData = new unsigned char[newSize];
            for (std::size_t e = 0; e < newArchetype->entityIds.size(); ++e)
            {
                newComp->MoveData(&newArchetype->componentData[j][e * newCompDataSize],
                    &newData[e * newCompDataSize]);
                newComp->DestroyData(&newArchetype->componentData[j][e * newCompDataSize]);
            }
            delete[] newArchetype->componentData[j];

            newArchetype->componentData[j] = newData;
        }

        newComp->ConstructData(&newArchetype->componentData[j][currentSize]);

        ArchetypeID oldArchetypeId = oldArchetype->type;

        for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
        {
            if (!(oldArchetype->type[i]))
            {
                continue;
            }

            if (i == j)
            {
                const std::size_t& oldCompDataSize
                    = m_componentMap[(unsigned int)i]->GetSize();

                ComponentBase* removeWrapper = m_componentMap[(unsigned int)i];
                removeWrapper->MoveData(&oldArchetype->componentData[i][record.index * oldCompDataSize],
                    &newArchetype->componentData[j][currentSize]);

                removeWrapper->DestroyData(&oldArchetype->componentData[i][record.index * oldCompDataSize]);

                break;
            }
        }
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        if (!(oldArchetype->type[i]))
        {
            continue;
        }

        // if this is the component being removed, we should also destruct it
        if (i == compTypeId)
        {
            ComponentBase* removeWrapper = m_componentMap[compTypeId];
            removeWrapper->DestroyData(
                &oldArchetype->componentData[i][record.index * sizeof(C)]);
        }

        const ComponentBase* const oldComp = m_componentMap[(unsigned int)i];

        const std::size_t& oldCompDataSize = oldComp->GetSize();

        //std::size_t currentSize = oldArchetype->entityIds.size() * oldCompDataSize;
        //std::size_t newSize = currentSize - oldCompDataSize;
        unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];
        oldArchetype->componentDataSize[i] -= oldCompDataSize;
        for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIds.size(); ++e)
        {
            if (e == record.index)
                continue;

            oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                &newData[ei * oldCompDataSize]);
            oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

            ++ei;
        }

        delete[] oldArchetype->componentData[i];

        oldArchetype->componentData[i] = newData;
    }

    // each entity in the old archetypes entityIds after this one now
    // has an index 1 less
    std::vector<EntityID>::iterator willBeRemoved
        = std::find(oldArchetype->entityIds.begin(),
            oldArchetype->entityIds.end(),
            entityId);

    std::for_each(willBeRemoved, oldArchetype->entityIds.end(),
        [this, &oldArchetype](const EntityID& eid)
        {
            Record& moveR = m_entityArchetypeMap[eid];
            --moveR.index;
        });

    oldArchetype->entityIds.erase(
        std::remove(oldArchetype->entityIds.begin(),
            oldArchetype->entityIds.end(),
            entityId), oldArchetype->entityIds.end());

    newArchetype->entityIds.push_back(entityId);
    record.index = newArchetype->entityIds.size() - 1;
    record.archetype = newArchetype;
}

template<class C>
inline C* ECS::GetComponent(const EntityID& entityId)
{
    ComponentTypeID compTypeId = Component<C>::GetTypeID();
    if (!m_entityArchetypeMap.contains(entityId))
        return nullptr; // it doesn't exist

    Record& record = m_entityArchetypeMap[entityId];

    if (!record.archetype)
        return nullptr; // there's no components anyway

    if (!record.archetype->type[compTypeId])
        return nullptr; // this entity doesn't have this component

    return reinterpret_cast<C*>(record.archetype->componentData[compTypeId] + record.index * sizeof(C));
}

inline void ECS::RemoveEntity(const EntityID& entityId)
{
    // if (!m_entityArchetypeMap.contains(entityId))
      //   return; // it doesn't exist

    Record& record = m_entityArchetypeMap[entityId];

    Archetype* oldArchetype = record.archetype;

    if (!oldArchetype)
    {
        m_entityArchetypeMap.erase(entityId);
        return; // we wouldn't know where to delete
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        if (!oldArchetype->type[i])
        {
            continue;
        }

        const ComponentBase* const comp = m_componentMap[(unsigned int)i];

        const std::size_t& compSize = comp->GetSize();

        comp->DestroyData
        (&oldArchetype->componentData[i][record.index * compSize]);
    }

    for (std::size_t i = 0; i < oldArchetype->type.size(); ++i)
    {
        if (!oldArchetype->type[i])
        {
            continue;
        }

        const ComponentBase* const oldComp = m_componentMap[(unsigned int)i];

        const std::size_t& oldCompDataSize = oldComp->GetSize();

        //std::size_t currentSize = oldArchetype->entityIds.size() * oldCompDataSize;
        //std::size_t newSize = currentSize - oldCompDataSize;
        unsigned char* newData = new unsigned char[oldArchetype->componentDataSize[i] - oldCompDataSize];
        oldArchetype->componentDataSize[i] -= oldCompDataSize;
        for (std::size_t e = 0, ei = 0; e < oldArchetype->entityIds.size(); ++e)
        {
            if (e == record.index)
                continue;

            oldComp->MoveData(&oldArchetype->componentData[i][e * oldCompDataSize],
                &newData[ei * oldCompDataSize]);

            oldComp->DestroyData(&oldArchetype->componentData[i][e * oldCompDataSize]);

            ++ei;
        }

        delete[] oldArchetype->componentData[i];

        oldArchetype->componentData[i] = newData;
    }

    m_entityArchetypeMap.erase(entityId);

    std::vector<EntityID>::iterator willBeRemoved
        = std::find(oldArchetype->entityIds.begin(),
            oldArchetype->entityIds.end(),
            entityId);

    std::for_each(willBeRemoved, oldArchetype->entityIds.end(),
        [this, &oldArchetype, &entityId](const EntityID& eid)
        {
            if (eid == entityId)
                return; // no need to adjust our removing one
            Record& moveR = m_entityArchetypeMap[eid];
            moveR.index -= 1;
        });

    oldArchetype->entityIds.erase(willBeRemoved);
}

inline void ECS::RemoveAllEntities()
{
    std::vector<EntityID> entities = ecs.getEntities();

    for (auto& i : entities)
    {
        RemoveEntity(i);
    }
    m_entityIdCounter = 1;
}


inline void ECS::RunSystems(const std::uint8_t& layer, const float elapsedMilliseconds)
{
    for (SystemBase* system : m_systems[layer])
    {
        const ArchetypeID& key = system->GetKey();

        for (Archetype* archetype : m_archetypes)
        {
            bool check = true;
            for (auto i : key)
            {
                if (!(archetype->type[i]))
                {
                    check = false;
                }
            }

            if (check)
            {
                // this archetype has all the types required by the system
                // so we can pull it's relevant data, reinterpret them as
                // their correct types, and call the Func in the system
                system->DoAction(elapsedMilliseconds, archetype);
            }
        }
    }
}

inline ECS::~ECS()
{
    for (Archetype* archetype : m_archetypes)
    {
        for (std::size_t i = 0; i < archetype->type.size(); ++i)
        {
            if (!archetype->type[i])
            {
                continue;
            }

            const ComponentBase* const comp = m_componentMap[(unsigned int)i];
            const std::size_t& dataSize = comp->GetSize();
            for (std::size_t e = 0; e < archetype->entityIds.size(); ++e)
            {
                comp->DestroyData(&archetype->componentData[i][e * dataSize]);
            }
            delete[] archetype->componentData[i];
        }
        delete archetype;
    }
    for (ComponentTypeIDBaseMap::value_type& p : m_componentMap)
        delete p.second;
}
// 

template<class... Cs>
void System<Cs...>::Action(Func func)
{
    m_func = func;
    m_funcSet = true;
}

//template<class... Cs>
//System<Cs...>::System(ECS& ecs, const std::uint8_t& layer) : m_ecs(ecs), m_funcSet(false)
//{
//    m_ecs.RegisterSystem(layer, this);
//}


//helper function to ensure a component with pos and vel is the same as a component with vel and pos
template<class... Ts>
ArchetypeID sort_keys(ArchetypeID types)
{
    std::sort(types.begin(), types.end());
    return types;
}

template<class... Cs>
ArchetypeID System<Cs...>::GetKey() const
{
    return sort_keys({ {Component<Cs>::GetTypeID()...} });
}

template<class... Cs>
void System<Cs...>::DoAction(const float elapsedMilliseconds, Archetype* archetype)
{
    if (m_funcSet)
        DoAction<0>(elapsedMilliseconds,
            archetype->type,
            archetype->entityIds,
            archetype->componentData);
}

template<class... Cs>
template<std::size_t Index, typename T, typename... Ts>
std::enable_if_t<Index != sizeof...(Cs)>
System<Cs...>::DoAction(const float elapsedMilliseconds,
    const ArchetypeID& archeTypeIds,
    const std::vector<EntityID>& entityIDs,
    T& t,
    Ts... ts)
{
    typedef std::tuple_element<Index, std::tuple<Cs...>>::type IthT;

    ComponentTypeID thisTypeCS = Component<IthT>::GetTypeID();

    if (!archeTypeIds[thisTypeCS])
    {
        throw std::runtime_error
        ("System was executed against an incorrect Archetype");
    }

    DoAction<Index + 1>(elapsedMilliseconds,
        archeTypeIds,
        entityIDs,
        t,
        ts...,
        reinterpret_cast<IthT*>(&t[thisTypeCS][0]));
}

template<class... Cs>
template<std::size_t Index, typename T, typename... Ts>
std::enable_if_t<Index == sizeof...(Cs)>
System<Cs...>::DoAction(const float elapsedMilliseconds,
    const ArchetypeID& archeTypeIds,
    const std::vector<EntityID>& entityIDs,
    T& t,
    Ts... ts)
{
    m_func(elapsedMilliseconds, entityIDs, ts...);
}

inline std::vector<std::string> ECS::getAllRegisteredComponents() 
{
    std::vector<std::string> componentNames;
    for (auto i : m_componentMap) {
        componentNames.push_back((i.second)->GetName());
    }
    return componentNames;
}
inline std::vector<EntityID> ECS::getEntities() 
{
    std::vector<EntityID> entityIDs;
    for (auto i : m_entityArchetypeMap) 
    {
        entityIDs.push_back(i.first);
    }
    return entityIDs;
}

//inline std::vector<std::string> ECS::getEntityNames() 
//{
//    std::vector<std::string> entitynames;
//    for (auto i : m_entityArchetypeMap) 
//    {
//        entitynames.push_back(*getEntityName(i.first));
//    }
//    return entitynames;
//}
//
//inline std::string* ECS::getEntityName(const EntityID& entityId) 
//{
//    Record& record = m_entityArchetypeMap[entityId];
//}

inline std::vector<std::string> ECS::getEntityComponents(const EntityID& entityId)
{
    std::vector<std::string> components;

    Record& record = m_entityArchetypeMap[entityId];

    if (!record.archetype)
        return components; // there's no components anyway    

    for (int i = 0; i < record.archetype->type.size(); ++i) 
    {
        if (!(record.archetype->type[i]))
        {
            continue;
        }

        auto it = m_componentMap.find(i);
        components.push_back(it->second->GetName());
    }
    return components;
}
