#include "levelEditor.h"
#include <filesystem>

class levelEditorContentBrowser
{
public:
	static levelEditorContentBrowser& getInstance();
	void ImGuiContentBrowser();

private:
	levelEditorContentBrowser();

	~levelEditorContentBrowser() { }

	std::filesystem::path currentPath;
};
