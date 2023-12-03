#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <VertexArray.h>
#include <IndexBuffer.h>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	////bone indexes which will influence this vertex
	//int m_BoneIDs[MAX_BONE_INFLUENCE];
	////weights from each bone
	//float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh
{
public:
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	aiMaterial* mat;
	std::unique_ptr<VertexArray> va;
	std::unique_ptr<IndexBuffer> ib;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, aiMaterial* mat) :vertices(vertices), indices(indices), mat(mat)
	{
		SetupMesh();
	}

	void SetupMesh()
	{
		va = std::make_unique<VertexArray>();
		auto vb = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(Vertex), vertices.size());
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		layout.Push<float>(3);
		//layout.Push<float>(3);
		va->AddBuffer(vb, layout);
		ib = std::make_unique<IndexBuffer>(&indices[0], indices.size() * sizeof(unsigned int));
	}
};