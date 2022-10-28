#include "levelEditor.h"

class levelEditorHierarchy
{
public:
	static levelEditorHierarchy& getInstance();
	void ImGuiHierarchy();

	int getSelected();

private:
	levelEditorHierarchy() 
	{ 
		selected = -1; 
		parent.push_back(0);
		latestParentID = 0;
		std::vector<EntityID> entities = ecs.getEntities();
		listOfEntities.push_back(entities);
	}
	~levelEditorHierarchy() { }

	std::vector<int> parent;
	int latestParentID;
	std::vector<std::vector<EntityID>> listOfEntities;

	int selected;
};
