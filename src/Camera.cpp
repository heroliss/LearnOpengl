#include "Camera.h"
#include "imgui.h"
#include "Application.h"

using namespace ImGui;

void Camera::OnInspectorGUI()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
	{
		bool dirty = false;
		//共有参数
		SetNextItemWidth(50);
		if (DragFloat("Aspect", &aspect, 0.01f))
		{
			orthoRect.x = orthoRect.z * aspect;
			orthoRect.y = orthoRect.w * aspect;
			dirty = true;
		}
		SameLine();
		SetNextItemWidth(60);
		dirty |= DragFloat("Near", &near, 0.1f);
		SameLine();
		SetNextItemWidth(60);
		dirty |= DragFloat("Fear", &far, 0.1f);

		dirty |= Checkbox("OrthoGraphic", &orthoGraphic);

		//SameLine();
		//SetNextItemWidth(60);
		//dirty |= DragFloat("Ortho ratio", &orthoRatio, 0.001, 0, 1);

		SameLine();
		//正交投影参数
		if (orthoGraphic)
		{
			if (orthoRatio < 1)
			{
				orthoRatio = glm::min(1.0f, orthoRatio + Application::GetInstance()->DeltaTime * 3.0f);
				dirty = true;
			}

			Checkbox("Set Rect", &orthoGraphicIndividualSetting);
			SameLine();
			if (orthoGraphicIndividualSetting)
			{
				SetNextItemWidth(260);
				dirty |= DragFloat4("Rect", &orthoRect.x);
			}
			else
			{
				float orthoGraphicRectHeight = orthoRect.w - orthoRect.z;
				SetNextItemWidth(60);
				if (DragFloat("Height", &orthoGraphicRectHeight))
				{
					orthoRect.z = -orthoGraphicRectHeight / 2;
					orthoRect.w = orthoGraphicRectHeight / 2;
					orthoRect.x = orthoRect.z * aspect;
					orthoRect.y = orthoRect.w * aspect;
					dirty = true;
				}
			}
		}
		//透视投影参数
		else
		{
			if (orthoRatio > 0)
			{
				orthoRatio = glm::max(0.0f, orthoRatio - Application::GetInstance()->DeltaTime * 3.0f);
				dirty = true;
			}

			SetNextItemWidth(60);
			dirty |= DragFloat("Field of View (vertical)", &fovy, 0.1f);
		}

		//修改投影矩阵
		if (dirty) {
			UpdateProjectionMatrix();
		}
	}
}
