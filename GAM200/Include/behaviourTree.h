#include <input.h>
#include <app.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <fstream>
#include "gameStats.h"

//#include "ECS.h"
//#include "../../Engine/Include/eventManager/eventManager.h"

//#include <Scripts.h>
//#include "../../GAM200/Include/Scripts.h"


extern AIStats testStats;
//extern EventManager eventmanager;
//time for ellie to implode

//behavior tree shit
//i put a behavior tree component in the ecs?


enum class State
{  //in case i need more States than just true false  CHANGE THIS PLS
    True,
    False,

};


class Task
{ //base class
public:
    virtual	State run(EntityID ID);
   std::vector<std::shared_ptr<Task>> children;
};

class TargetInRange : Task
{ //condition

    int unit;
public:
    State run(EntityID ID) override;

};
class Composite :public  Task
{
protected:


public:
    const std::vector<std::shared_ptr<Task>>getchildren() const;
    void addChild(std::shared_ptr<Task> child);

};

class Selector :public Composite 
{ //Composite

public:
    State run(EntityID ID) override;
};

class Sequence :public Composite 
{ //Composite

public:
    State run(EntityID ID) override;
};
// add random versions CHANGE THIS PLEASE

class AggressionState :public Task
{ // action 
    //State changes
public:
    State run(EntityID ID) override;
};
class PlaySound :public Task 
{ // action 

    //probably send a flag here
public:
    State run(EntityID ID) override;
};

class PlayAnimation : public Task 
{
public:
    State run(EntityID ID) override;
};

class PayResource : public Task {
    //kinda want this to take in a pointer to some resource or smth smth
    public: 
        State run(EntityID ID) override;
};
//test for selector
class noResource :public Task {
public : 
    State run(EntityID ID) override;
};
class YesResource : public Task {
public: 
    State run(EntityID ID) override;
};


class IdleState : Task 
{//action
public:
    State run(EntityID ID) override;
};


class Decorator : public Task
{
    std::shared_ptr<Task> child;

    std::shared_ptr<Task> getChild()const;

    void setChild(std::shared_ptr<Task> c);

};

//----------------------------------------------------------------------------------------------------------------
class testFail : public Task 
{
public:
    State run(EntityID ID) override;
};
//----------------------------------------------------------------------------------------------------------------

class MaxRange : public Decorator
{

    //meh
};

class RootNode : public Task 
{
public:
    std::shared_ptr<Task> child;
    void setChild(std::shared_ptr<Task> newChild);
    virtual State run(EntityID ID) override;
};

class BehaviourTree 
{
public:
    RootNode* root;
    BehaviourTree();// : root(new RootNode) {}
    
    ~BehaviourTree();
    void setRootChild(std::shared_ptr<Task> rootChild) const;
    State run(EntityID ID) const;
};

void behaviorTreeInit();
