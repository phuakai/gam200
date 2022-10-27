#include "eventManager.h"

EventManager::EventManager() { }

EventManager::~EventManager() { }


void EventManager::subscribe(int systemID)
{
	char systemIDMask = 1UL << systemID;
	// check if systemID is used
	if (masterQueue.contains(systemIDMask))
	{
		return;	// can't re-register a type
	}

	std::vector<Event> eventQueue;
	masterQueue.emplace(systemIDMask, eventQueue);
}

void EventManager::post(Event& event)
{
	for (int i = 0; i < 8; ++i)
	{
		// message is directed to system with systemID (i)
		char systemID = 1UL << i;
		if (systemID & event.message)
		{
			//masterQueue[systemID].push_back(event);
			std::vector<Event>& currentMasterQueue = masterQueue[systemID];
			currentMasterQueue.emplace(currentMasterQueue.begin(), event);
		}
	}
}

std::vector<Event>& EventManager::findQueue(int systemID)
{
	return masterQueue[1UL << systemID];
}

Event EventManager::dequeue(int systemID)
{
	char id = 1UL << systemID;
	Event eventPop = masterQueue[id][masterQueue[id].size() - 1];
	masterQueue[id].pop_back();
	return eventPop;
}
