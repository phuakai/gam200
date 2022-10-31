#include "levelEditorContentBrowser.h"
#include "texture.h"

// change to asset file location next time
static const std::filesystem::path assetPath = "../asset";

levelEditorContentBrowser& levelEditorContentBrowser::getInstance()
{
	static levelEditorContentBrowser instance;
	return instance;
}

levelEditorContentBrowser::levelEditorContentBrowser() : currentPath(assetPath) { }

void levelEditorContentBrowser::ImGuiContentBrowser(const EntityID& id)
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
		std::string pathChanged = p.path().string();

		for (int i = 0; i < pathChanged.size(); ++i)
		{
			if (pathChanged[i] == '\\')
			{
				pathChanged[i] = '/';
			}
		}

		if (p.is_directory())
		{
			if (ImGui::Button(path.c_str(), { (ImGui::GetWindowSize().x - 10) / 5, (ImGui::GetWindowSize().x - 10) / 5 }))
			{
				currentPath /= p.path().filename();
			}
		}
		else if(ImGui::Button(path.c_str(), { (ImGui::GetWindowSize().x - 10) / 5, (ImGui::GetWindowSize().x - 10) / 5 }) && id != -1)
		{
			for (int i = 0; i < textureobjects.paths.size(); ++i)
			{
				std::string texture(textureobjects.paths[i]);
				if (pathChanged == texture)
				{
					ecs.GetComponent<Texture>(id)->textureID = i + 1;
					break;
				}
			}
			std::cout << std::endl;
		}

		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}
