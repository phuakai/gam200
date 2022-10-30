#include "levelEditor.h"

class levelEditorProperties
{
public:
	static levelEditorProperties& getInstance();
	void ImGuiProperties(const int& id);

private:
	levelEditorProperties() { }
	~levelEditorProperties() { }
};
