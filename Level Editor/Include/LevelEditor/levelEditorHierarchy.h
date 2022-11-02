#include "levelEditor.h"

class levelEditorHierarchy
{
public:
	static levelEditorHierarchy& getInstance();
	void ImGuiHierarchy();
	void clear();
	int getSelected();

private:
	levelEditorHierarchy()
	{
		selected = -1;
		hierarchyList[std::make_pair(100000, 1)] = ecs.getEntities();
	}
	~levelEditorHierarchy() { }

	std::map<std::pair<int, int>, std::vector<EntityID>> hierarchyList;
	int selected;
};
