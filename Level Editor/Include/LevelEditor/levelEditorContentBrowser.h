#include "levelEditor.h"

class levelEditorContentBrowser
{
public:
	static levelEditorContentBrowser& getInstance();
	void ImGuiContentBrowser(const EntityID& id);

private:
	levelEditorContentBrowser();

	~levelEditorContentBrowser() { }

	std::filesystem::path currentPath;
};
