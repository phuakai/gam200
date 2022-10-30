#include "levelEditor.h"

class levelEditorProperties
{
public:
	static levelEditorProperties& getInstance();
	void ImGuiProperties(const EntityID& id);

private:
	levelEditorProperties() { }
	~levelEditorProperties() { }
};
