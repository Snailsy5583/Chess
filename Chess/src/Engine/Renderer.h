#pragma once

#include "glad/glad.h"

#include "Shader.h"

namespace Engine {


	struct RendererObject {
		float position[3]{0,0,0};
		unsigned int vao = -1;
		unsigned int vbo = -1;
		unsigned int bufferSize = -1;
		Shader shader;
	};

	class Renderer {
	public:
		static RendererObject
		GenObject(const float pos[3], int size, const float *Vertices,
		          const char *vertShaderPath, const char *fragShaderPath);

		static RendererObject GenQuad(const float pos[3], float sideLen,
		                              const char *vertShaderPath,
		                              const char *fragShaderPath);

		static void
		MoveQuad(RendererObject &obj, const float newPos[3], float sideLen);

		static void DeleteQuad(RendererObject &object);

		static void SubmitObject(const RendererObject& obj);

	private:
		const static float m_QuadVerts[];
	};


}