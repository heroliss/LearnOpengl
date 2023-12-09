#include "Camera.h"
#include "imgui.h"

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
		SameLine();
		//正交投影参数
		if (orthoGraphic) {
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
		else {
			SetNextItemWidth(60);
			dirty |= DragFloat("Field of View (vertical)", &fovy, 0.1f);
		}
		//修改投影矩阵
		if (dirty) {
			UpdateProjectionMatrix();
		}
	}
}
