
#include "mainHeader.h"
#include "EventManager/eventManager.h"
#include "physicsPartition.h"

extern AIStats testStats;
BehaviourTree behaviorTree;
extern BehaviourTree behaviorTree;
extern EventManager eventManager;
extern quadTree mainTree;

State Task::run(EntityID ID)
{
    State endState = State::True;

    return endState;
}



    const std::vector<std::shared_ptr<Task>>Composite::getchildren() const { return children; }
    void Composite::addChild(std::shared_ptr<Task> child) { children.push_back((child)); }

    MoveEvent enemyMove;


    State TargetInRange::run(EntityID ID)
    {
        
        std::list<EntityID*> inRange;
        
        //float rangebox = ((50 +50));

        //just shoot me
        AABB range(ecs.GetComponent<BaseInfo>(ID)->position.x - 50, ecs.GetComponent<BaseInfo>(ID)->position.y - 50, ecs.GetComponent<BaseInfo>(ID)->position.x + 50, ecs.GetComponent<BaseInfo>(ID)->position.y + 50);
        mainTree.query(range, inRange);


        //need to check if its an enemy
        if (inRange.size() != 0) {
          //  std::cout << "things in range";
            for (auto const& i : inRange) {
                std::cout << *i << std::endl;
                if (ecs.GetComponent<Unit>(*i)->faction == PLAYER) {
                    //is this inefficent
                  //  ecs.GetComponent<Unit>(*i)->target = *i;
                    
                    ecs.GetComponent<Physics>(ID)->target = ecs.GetComponent<BaseInfo>(*i)->position;

                    enemyMove.id = ID;
                    enemyMove.message = (1UL << Systems::Physics);

                    eventManager.post(enemyMove);
                    std::cout << "chasing" << i << std::endl;
              }
            }
            
            return State::True; //need to return the enemy in the range?
        }
        else {
            //std::cout << "no target here " << std::endl;
            return State::False;
        }
    }

    State Selector::run(EntityID ID) 
    {

        //this only works if i dont want loop to break, 
        // std::for_each(Tasklist.begin(), Tasklist.end(), run(EntityID ID));
        for (auto i : children)
        {

            if (i->run(ID) == State::True)
            {

                return State::True;
            } // idk if i have to public the virtual run, i think can remove that once the rest are in CHANGE THIS PLS

        }
        return State::False;
    }



    State Sequence::run(EntityID ID) 
    {

        for (auto i : children)
        {

            if (i->run(ID) == State::False)
            {

                return State::False;
            } // idk if i have to public the virtual run, i think can remove that once the rest are in  CHANGE THIS PLS

        }
        return State::True;
    }

// add random versions CHANGE THIS PLEASE


    State AggressionState::run(EntityID ID) 
    {
        std::cout << "State change to aggression";
        return State::True;
    }

    State PlaySound::run(EntityID ID) 
    {
     //   std::cout << "play a sound\n";
        return State::True;
    }


    State PlayAnimation::run(EntityID ID) 
    {
     //   std::cout << "animate?\n";
        return State::True;
    }


    State PayResource::run(EntityID ID)  {
        //should i put the if here or ?
        if (testStats.resourceOne >= 50) {
            testStats.resourceOne -= 10;
        //    std::cout << "Burning 10 / " << testStats.resourceOne << std::endl;
            return State::True;
        }
        else {
       //     std::cout << "only have " << testStats.resourceOne << " /50 left " << std::endl;
            return State::False;
        }
    }


    State noResource::run(EntityID ID)  {
        if (1) {
            AIEvent theEvent;
            theEvent.id = ID;

            eventManager.post(theEvent);
         //   std::cout << "no resource so cry " << std::endl;
            return State::True;
        }
    }

    State YesResource::run(EntityID ID)  {
       // std::cout << "boi moving" << std::endl;

        return State::True;

    }



    State IdleState::run(EntityID ID)
    {
      //  std::cout << "State change to idle";
        return State::True;
    }


    std::shared_ptr<Task> Decorator::getChild()const { return child; }

    void Decorator::setChild(std::shared_ptr<Task> c) { child = c; }

//----------------------------------------------------------------------------------------------------------------

    State testFail::run(EntityID ID) 
    {
        return State::False;
     //   std::cout << "fail here\n";
    }

//----------------------------------------------------------------------------------------------------------------




    std::shared_ptr<Task> child;
    void RootNode::setChild(std::shared_ptr<Task> newChild) { child = newChild; }
     State RootNode::run(EntityID ID)  { return child->run(ID); }


    BehaviourTree::BehaviourTree() : root(new RootNode) {}
    BehaviourTree::~BehaviourTree() { delete root; }
    void BehaviourTree::setRootChild(std::shared_ptr<Task> rootChild) const { root->setChild(rootChild); }
    State BehaviourTree::run(EntityID ID) const { return root->run(ID); }



void behaviorTreeInit()
{
    //auto test = std::make_shared<PlaySound>;
    std::shared_ptr<Task> sequen = std::make_shared<Sequence>();
    std::shared_ptr<Task> select = std::make_shared<Selector>();
    behaviorTree.setRootChild(sequen);
    std::shared_ptr<Task> gotResource = std::make_shared<YesResource>();

    std::shared_ptr<Task> playAni = std::make_shared<PlayAnimation>();
    std::shared_ptr<Task> fail = std::make_shared<testFail>();
    std::shared_ptr<Task> testSound = std::make_shared<PlaySound>();
    std::shared_ptr<Task> booResource = std::make_shared<noResource>();
    std::shared_ptr<Task> payUp = std::make_shared<PayResource>();
    std::shared_ptr<Task> testRange = std::make_shared<TargetInRange>();
   

 
    sequen->children.push_back(testRange);
    sequen->children.push_back(payUp);
    sequen->children.push_back(gotResource);
    sequen->children.push_back(playAni);
    sequen->children.push_back(testSound);


   // select->children.push_back(fail);

    

    select->children.push_back(booResource);
  //  select->children.push_back(booResource);
   // select->children.push_back(playAni);


}
