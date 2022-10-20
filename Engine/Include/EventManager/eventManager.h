#include <unordered_map>
#include <vector>
#include "ECS.h"

class Event
{
	EntityID id;
	char message;
};

class EventManager
{
public:
	EventManager();
	~EventManager();

	// called during initiailise to create a new queue
	void subscribe(char systemID);
	// called in RunSystems to queue the events, splits the events into different queues here
	void post(char systemIDs, Event event);

	// helper function called in post to enqueue the event
	void enqueue(char systemID, Event event);
	// called in the systems to read the queue
	void dequeue(char systemID);

private:
	std::unordered_map<char, std::vector<Event>> masterQueue;
};
