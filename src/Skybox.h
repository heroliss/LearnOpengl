#include <iostream>
#include <format>
#include <vector>
#include "GLCALL.h"
#include "Cubemap.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "../res/Materials/SkyboxMaterial.h"
#include "Application.h"

class Skybox
{
private:
	const std::vector<float> skyboxVertices = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	std::shared_ptr<VertexArray> va;
	std::shared_ptr<SkyboxMaterial> material;
	std::shared_ptr<Cubemap> cubemap;
	std::string cubemapDirectory = "res/textures/skybox/";
	std::vector<std::string> faces
	{
		cubemapDirectory + "right.jpg",
		cubemapDirectory + "left.jpg",
		cubemapDirectory + "top.jpg",
		cubemapDirectory + "bottom.jpg",
		cubemapDirectory + "front.jpg",
		cubemapDirectory + "back.jpg"
	};

public:
	Skybox() {
		va = std::make_shared<VertexArray>();
		auto vb = std::make_shared<VertexBuffer>(VertexBufferParams_vector(skyboxVertices));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		va->AddBuffer(vb, layout);

		cubemap = std::make_shared<Cubemap>();
		cubemap->Load(faces);

		material = std::make_shared<SkyboxMaterial>();
		material->cubemap = cubemap;
	}

	~Skybox() {

	}

	void Draw() {
		Application::GetInstance()->renderer->Draw(*va, *material);
	}

	std::shared_ptr<Cubemap> GetCubemap() { return cubemap; }
};
