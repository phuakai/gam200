#include <input.h>
#include <app.h>
#include <iostream>
#include <fstream>
#include "mainHeader.h"
#include "EventManager/eventManager.h"
#include "levelEditor.h"
#include "Scripts.h"
#include "physicsPartition.h"

extern BehaviourTree behaviorTree;
extern EventManager eventManager;

void unitAIUpdate()
{
	//loopStart;

    for (auto& i : ecs.getEntities())
    {
        Script* unit = ecs.GetComponent<Script>(i);
        if (unit && ecs.GetComponent<Script>(i)->faction == 2)
        {
            unit->aiTree = &behaviorTree;
            unit->aiTree->run(i);
        }

        std::list<EntityID*> inRange;
        AABB range(
            ecs.GetComponent<BaseInfo>(i)->position.x - ecs.GetComponent<BaseInfo>(i)->dimension.x / 2,
            ecs.GetComponent<BaseInfo>(i)->position.y - ecs.GetComponent<BaseInfo>(i)->dimension.y / 2,
            ecs.GetComponent<BaseInfo>(i)->position.x + ecs.GetComponent<BaseInfo>(i)->dimension.x / 2,
            ecs.GetComponent<BaseInfo>(i)->position.y + ecs.GetComponent<BaseInfo>(i)->dimension.y / 2);
        mainTree.query(range, inRange);

        //need to check if its an enemy
        if (inRange.size() != 0) 
        {
            for (auto const& j : inRange) 
            {
                if (ecs.GetComponent<BaseInfo>(*j)->type == EntityType::PLAYER)
                {
                    if (ecs.GetComponent<Attack>(i))
                    {
                        ecs.GetComponent<Attack>(i)->attackTimer -= (float)Graphics::Input::delta_time;

                        if (ecs.GetComponent<HealthBar>(*j) && ecs.GetComponent<Attack>(i)->attackTimer <= 0)
                        {
                            ecs.GetComponent<HealthBar>(*j)->health -= 1;
                        }
                    }
                }
            }
        }
        if (ecs.GetComponent<Attack>(i) && ecs.GetComponent<Attack>(i)->attackTimer <= 0)
        {
            ecs.GetComponent<Attack>(i)->attackTimer = 15;
        }

    }
	//std::vector<Event>& eventQueue = eventManager.findQueue(Systems::AI);
}

void buildingsUpdate() {
    for (auto& i : ecs.getEntities())
    {
        Building* building = ecs.GetComponent<Building>(i);
        if (building && ecs.GetComponent<BaseInfo>(i)->type == EntityType::ENEMY)
        {
            //spawn here CHANGE THIS

        }


    }
}