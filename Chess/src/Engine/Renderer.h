#pragma once

#include "glad/glad.h"

#include "Shader.h"

namespace Engine
{


	struct RendererObject
	{
		float position[3];
		unsigned int vao;
		unsigned int vbo;
		unsigned int bufferSize;
		Shader shader;
	};

	class Renderer
	{
	public:
		static RendererObject GenObject(const float pos[3], int size, const float* Vertices, const char* vertShaderPath, const char* fragShaderPath);

		static RendererObject GenQuad(const float pos[3], const float sideLen, const char* vertShaderPath, const char* fragShaderPath);

		static void MoveQuad(RendererObject& obj, const float newPos[3], float sideLen);

		static void DeleteQuad(RendererObject& object);

		static void SubmitObject(RendererObject obj);

	private:
		const static float m_QuadVerts[];
	};


}