#include <unordered_map>
#include <queue>
#include <vector>
#include "ECS.h"

namespace Systems
{
	enum Systems
	{
		Collision = 0,
		Physics,
		Graphics,
		Sound
	};
}

class Event
{
public:
	EntityID id;
	char message;		// example: a mixture of systemIDs
};

class MoveEvent : public Event
{
public:
	bool check;
};

class EventManager
{
public:
	EventManager();
	~EventManager();

	// called during initiailise to create a new queue
	void subscribe(int systemID);
	// called in RunSystems to queue the events, splits the events into different queues here
	void post(Event& event);

	std::vector<Event>& findQueue(int systemID);
	// called in the systems to read the queue
	Event dequeue(int systemID);

private:
	std::unordered_map<char, std::vector<Event>> masterQueue;
};
