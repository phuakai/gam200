#include "levelEditorContentBrowser.h"

// change to asset file location next time
static const std::filesystem::path assetPath = "../asset";

levelEditorContentBrowser& levelEditorContentBrowser::getInstance()
{
	static levelEditorContentBrowser instance;
	return instance;
}

levelEditorContentBrowser::levelEditorContentBrowser() : currentPath(assetPath) { }

void levelEditorContentBrowser::ImGuiContentBrowser()
{
	//ImGui::Text("%s", std::filesystem::current_path().c_str());
	if (currentPath != assetPath)
	{
		if (ImGui::Button("<<"))
		{
			currentPath = currentPath.parent_path();
		}
	}

	ImGui::Columns(5);

	for (auto& p : std::filesystem::directory_iterator(currentPath))
	{
		auto relativePath = std::filesystem::relative(p.path(), assetPath);
		std::string path = relativePath.string();

		if (p.is_directory())
		{
			if (ImGui::Button(path.c_str(), { 256, 256 }))
			{
				currentPath /= p.path().filename();
			}
		}
		else
		{
			ImGui::Button(path.c_str(), { 256, 256 });
		}

		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}
