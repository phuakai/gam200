#include "levelEditor.h"

class levelEditorProperties
{
public:
	static levelEditorProperties& getInstance();
	void ImGuiProperties(EntityID id);

private:
	levelEditorProperties() { }
	~levelEditorProperties() { }
};
