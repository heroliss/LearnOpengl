#pragma once

#include <string>
#include "PostProcessingMaterial.h"
#include "Texture.h"
#include "imgui.h"

struct Kernel
{
	std::string name;
	float data[9];
	void Copy(const Kernel& source)
	{
		name = source.name;
		std::copy(source.data, source.data + 9, data);
	}
};

class KernelEffect : public PostProcessingMaterial
{
public:
	KernelEffect() {
		CurrentKernel.Copy(kernels[0]);
	}
	std::string GetName() const override { return "Kernel Effect"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/KernelEffect.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
		Shader& shader = *GetShader();

		//给卷积核乘以倍数
		Kernel kernel;
		kernel.Copy(CurrentKernel);
		for (int i = 0; i < 9; i++)
		{
			kernel.data[i] *= kernelScale;
		}
		shader.SetUniform1fv("kernel", 9, kernel.data);
		shader.SetUniform1f("offset", offset);
	}

	void DrawImgui(int id) override
	{
		std::string idText = "##Kernel_" + std::to_string(id);

		ImGui::SameLine(120);

		auto currentPosX = ImGui::GetCursorPosX();

		if (ImGui::BeginCombo((idText + "_combo").c_str(), CurrentKernel.name.c_str(), ImGuiComboFlags_WidthFitPreview))
		{
			for (int i = 0; i < kernels.size(); i++)
			{
				auto kernel = kernels[i];
				bool isSelected = (CurrentKernelIndex == i);
				if (ImGui::Selectable(kernel.name.c_str(), &isSelected))
				{
					CurrentKernelIndex = i;
				}
				if (isSelected)
				{
					CurrentKernel.Copy(kernels[i]);
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		auto currentPosY = ImGui::GetCursorPosY();

		for (int i = 0; i < 9; i++)
		{
			if (i % 3 == 0)
			{
				ImGui::SetCursorPosX(currentPosX);
			}
			else
			{
				ImGui::SameLine();
			}
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat((idText + "_data_" + std::to_string(i)).c_str(), &CurrentKernel.data[i], 0.001f);
		}

		ImGui::SameLine(0, 20);
		ImGui::SetCursorPosY(currentPosY);
		ImGui::BeginChild(("Kernel right area" + idText).c_str(), ImVec2(0,70), ImGuiChildFlags_Border);
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat(("offset" + idText).c_str(), &offset, 0.0001f, 0, 0, "%.4f");
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat(("scale" + idText).c_str(), &kernelScale, 0.001f);
		ImGui::EndChild();
	}

	KernelEffect* CreateObject() override {
		return new KernelEffect();
	}

	float offset = 1.0f / 1000;
	Kernel CurrentKernel;
	int CurrentKernelIndex = 0;
	float kernelScale = 1.0f;
	const std::vector<Kernel> kernels = {
		Kernel
		{
			"平滑图像(高斯滤波器)",
			{
				1.0f / 16, 1.0f / 8, 1.0f / 16,
				1.0f / 8,  1.0f / 4, 1.0f / 8,
				1.0f / 16, 1.0f / 8, 1.0f / 16
			}
		},
		Kernel
		{
			"平滑图像(均值滤波器)",
			{
				1.0f / 9, 1.0f / 9, 1.0f / 9,
				1.0f / 9, 1.0f / 9, 1.0f / 9,
				1.0f / 9, 1.0f / 9, 1.0f / 9
			}
		},
		Kernel
		{
			"边缘检测(Prewitt算子-右梯度)",
			{
				-1, 0, 1,
				-1, 0, 1,
				-1, 0, 1
			}
		},
		Kernel
		{
			"边缘检测(Prewitt算子-右上梯度)",
			{
				 0, 1, 1,
				-1, 0, 1,
				-1,-1, 0
			}
		},
		Kernel
		{
			"边缘检测(Prewitt算子-上梯度)",
			{
				 1, 1, 1,
				 0, 0, 0,
				-1,-1,-1
			}
		},
		Kernel
		{
			"边缘检测(Sobel算子-右梯度)",
			{
				-1, 0, 1,
				-2, 0, 2,
				-1, 0, 1
			}
		},
		Kernel
		{
			"边缘检测(Sobel算子-右上梯度)",
			{
				 0, 1, 2,
				-1, 0, 1,
				-2,-1, 0
			}
		},
		Kernel
		{
			"边缘检测(Sobel算子-上梯度)",
			{
				 1, 2, 1,
				 0, 0, 0,
				-1,-2,-1
			}
		},
		Kernel
		{
			"边缘检测(Laplacian算子-8邻接)",
			{
				1, 1, 1,
				1,-8, 1,
				1, 1, 1
			}
		},
		Kernel
		{
			"边缘检测(Laplacian算子-4邻接)",
			{
				 0, 1, 0,
				 1,-4, 1,
				 0, 1, 0
			}
		},
		Kernel
		{
			"锐化图像(Laplacian算子-8邻接)",
			{
				-1,-1,-1,
				-1, 9,-1,
				-1,-1,-1
			}
		},
		Kernel
		{
			"锐化图像(Laplacian算子-4邻接)",
			{
				 0,-1, 0,
				-1, 5,-1,
				 0,-1, 0
			}
		},
	};
};

