#include <input.h>
#include <app.h>
#include <iostream>
#include <fstream>
#include "mainHeader.h"
#include "EventManager/eventManager.h"
#include "levelEditor.h"
#include "Scripts.h"

extern BehaviourTree behaviorTree;
extern EventManager eventManager;

void unitAIUpdate()
{
	//loopStart;

    //check to see if its enemy first CHANGE THIS
    for (auto& i : ecs.getEntities())
    {
        Unit* unit = ecs.GetComponent<Unit>(i);
        if (unit && ecs.GetComponent<BaseInfo>(i)->type != "Prefab")
        {
            unit->aiTree = &behaviorTree;
            unit->aiTree->run(i);
        }

        //check if its targeting some other unit then check range then attack (-hp) CHANGE THIS


    }
	//std::vector<Event>& eventQueue = eventManager.findQueue(Systems::AI);
}

void buildingsUpdate() {
    for (auto& i : ecs.getEntities())
    {
        Building* building = ecs.GetComponent<Building>(i);
        if (building && ecs.GetComponent<BaseInfo>(i)->type != "Prefab")
        {
            //spawn here CHANGE THIS

    }


}