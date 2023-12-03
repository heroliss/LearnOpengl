#pragma once

#include "test.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "Texture.h"

namespace test {
	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> va;
		std::unique_ptr<VertexArray> va2;
		std::unique_ptr<IndexBuffer> ib;
		std::unique_ptr<Material> material;
	};
}

