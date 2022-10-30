#include "levelEditorProperties.h"

levelEditorProperties& levelEditorProperties::getInstance()
{
	static levelEditorProperties instance;
	return instance;
}

void levelEditorProperties::ImGuiProperties(const EntityID& id)
{
	static int selectedNode = -1;
	static bool check = true;
	if (id >= 0)
	{
		std::vector<std::string>componentNames = ecs.getEntityComponents(id);
		for (int i = 0; i < componentNames.size(); ++i)
		{
			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, componentNames[i].c_str()))
			{
				auto component = rttr::type::get_by_name(componentNames[i]);
				rttr::instance componentInstance = GetComponentByName(component, id);

				for (rttr::property property : component.get_properties())
				{
					if (property.get_type() == rttr::type::get<vector2D::vec2D>())
					{
						vector2D::vec2D temp = property.get_value(componentInstance).get_value<vector2D::vec2D>();
						ImGui::DragFloat2(property.get_name().data(), temp.m, 1.0f, -500.0f, 500.0f);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<vector3D::vec3D>())
					{
						vector3D::vec3D temp = property.get_value(componentInstance).get_value<vector3D::vec3D>();
						ImGui::DragFloat3(property.get_name().data(), temp.m, 0.01f, 0.0f, 1.0f);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<std::string>())
					{
						char tempChar[100];
						std::string temp = property.get_value(componentInstance).to_string();

						strcpy_s(tempChar, temp.c_str());

						ImGui::InputText(property.get_name().data(), tempChar, 100);
						property.set_value(componentInstance, std::string(tempChar));
					}

					else if (property.get_type() == rttr::type::get<int>())
					{
						int temp = property.get_value(componentInstance).to_int();
						ImGui::DragInt(property.get_name().data(), &temp, 1.f, 0, 0);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<float>() || property.get_type() == rttr::type::get<double>())
					{
						float temp = property.get_value(componentInstance).to_float();
						ImGui::DragFloat(property.get_name().data(), &temp, 1.f, 0, 0);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<bool>())
					{
						bool temp = property.get_value(componentInstance).to_bool(); 
						ImGui::Checkbox(property.get_name().data(), &temp);
						property.set_value(componentInstance, temp);
					}
				}

				ImGui::TreePop();
			}
		}
	}
}
