#include "levelEditorProperties.h"
#include "pathfinding.h"

void* checkComponentByName(rttr::type& componentName, const EntityID& entityID)
{
	if (componentName == rttr::type::get<Render>())
		return ecs.GetComponent<Render>(entityID);
	else if (componentName == rttr::type::get<BaseInfo>())
		return ecs.GetComponent<BaseInfo>(entityID);
	else if (componentName == rttr::type::get<Texture>())
		return ecs.GetComponent<Texture>(entityID);
	else if (componentName == rttr::type::get<Physics>())
		return ecs.GetComponent<Physics>(entityID);
	else if (componentName == rttr::type::get<Building>())
		return ecs.GetComponent<Building>(entityID);
	else if (componentName == rttr::type::get<Unit>())
		return ecs.GetComponent<Unit>(entityID);
	else if (componentName == rttr::type::get<Stats>())
		return ecs.GetComponent<Stats>(entityID);
	return nullptr;
}

levelEditorProperties& levelEditorProperties::getInstance()
{
	static levelEditorProperties instance;
	return instance;
}

void levelEditorProperties::ImGuiProperties(const int& id)
{
	static int selectedNode = -1;
	static bool check = true;
	static std::vector<int> componentCheck(ecs.getAllRegisteredComponents().size(), 0);

	//ImGui::IsMouseClicked(ImGuiMouseButton_Left);

	if (id != -1)
	{
		std::vector<std::string>allComponents = ecs.getAllRegisteredComponents();

		for (int i = 0; i < allComponents.size(); ++i)
		{
			if (allComponents[i] == "Render")
			{
				continue;
			}

			auto component = rttr::type::get_by_name(allComponents[i]);

			void* componentExistCheck = checkComponentByName(component, id);
			bool boolCheck = componentExistCheck == nullptr ? false : true;

			if (allComponents[i] == "BaseInfo")
			{
				ImGui::Text(allComponents[i].c_str());

				ImGui::Indent();

				BaseInfo* baseInfo = ecs.GetComponent<BaseInfo>(id);

				char tempChar[100];
				std::string tempName = baseInfo->name;

				strcpy_s(tempChar, tempName.c_str());

				ImGui::InputText("name", tempChar, 100);
				baseInfo->name = std::string(tempChar);

				vector2D::vec2D tempPosition = baseInfo->position;
				ImGui::DragFloat2("position", tempPosition.m, 1.0f, -500.0f, 500.0f);
				baseInfo->position = tempPosition;

				vector2D::vec2D tempDimension = baseInfo->dimension;
				ImGui::DragFloat2("dimension", tempDimension.m, 1.0f, -500.0f, 500.0f);
				baseInfo->dimension = tempDimension;

				ImGui::Unindent();

				continue;
			}

			ImGui::Checkbox(allComponents[i].c_str(), &boolCheck);

			if (boolCheck)
			{
				if (componentExistCheck == nullptr)
				{
					if (allComponents[i] == "Texture")
						ecs.AddComponent<Texture>(id);
					else if (allComponents[i] == "Physics")
					{
						ecs.AddComponent<Physics>(id);
						ecs.GetComponent<Physics>(id)->formationManagerID = -1;
					}
					else if (allComponents[i] == "Building")
						ecs.AddComponent<Building>(id);
					else if (allComponents[i] == "Unit")
						ecs.AddComponent<Unit>(id);
					else if (allComponents[i] == "Stats")
						ecs.AddComponent<Stats>(id);
				}

				rttr::instance componentInstance = GetComponentByName(component, id);

				ImGui::Indent();
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

				ImGui::Unindent();
			}

			else if (componentExistCheck != nullptr)
			{
				if (allComponents[i] == "Texture")
					ecs.RemoveComponent<Texture>(id);
				else if (allComponents[i] == "Physics")
				{
					int formationManagerID = ecs.GetComponent<Physics>(id)->formationManagerID;
					if (formationManagerID != -1)
					{
						formationManagers[formationManagerID].slotAssignment.erase(std::find(formationManagers[formationManagerID].slotAssignment.begin(), formationManagers[formationManagerID].slotAssignment.end(), id));
					}
					ecs.RemoveComponent<Physics>(id);
				}
				else if (allComponents[i] == "Building")
					ecs.RemoveComponent<Building>(id);
				else if (allComponents[i] == "Stats")
					ecs.RemoveComponent<Stats>(id);
				else if (allComponents[i] == "Unit")
					ecs.RemoveComponent<Unit>(id);
			}
		}
	}
}
