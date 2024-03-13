#include <iostream>
#include <memory>

#include "Renderer.h"

namespace Engine {


	const float Renderer::m_QuadVerts[] = {
			//   x	,	y  ,  z  ,	 u ,  v ,
			-0.5f, -0.5f, 0.0f, 0.f, 0.f,
			0.5f, -0.5f, 0.0f, 1.f, 0.f,
			-0.5f, 0.5f, 0.0f, 0.f, 1.f,
			0.5f, 0.5f, 0.0f, 1.f, 1.f,
			0.5f, -0.5f, 0.0f, 1.f, 0.f,
			-0.5f, 0.5f, 0.0f, 0.f, 1.f
	};

	RendererObject
	Renderer::GenObject(const float pos[3], int size, const float *vertices,
	                    const char *vertShaderPath,
	                    const char *fragShaderPath) {
		RendererObject obj = {{pos[0], pos[1], pos[2]}, 0, 0, 0,
		                      Shader::Compile(vertShaderPath, fragShaderPath)};

		glGenVertexArrays(1, &obj.vao);
		glBindVertexArray(obj.vao);

		obj.bufferSize = size;

		glGenBuffers(1, &obj.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
		glBufferData(GL_ARRAY_BUFFER, obj.bufferSize, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);

		glVertexAttribPointer(
				0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
				(void *) 0
		);
		glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
				(void *) (3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		return obj;
	}

	RendererObject Renderer::GenQuad(const float pos[3], const float sideLen,
	                                 const char *vertShaderPath,
	                                 const char *fragShaderPath) {
		float verts[sizeof(m_QuadVerts) / sizeof(float)];

		for (
				int row = 0; row < 6; row++
				) {
			for (
					int col = 0; col < 3; col++
					)
				verts[(row * 5) + col] =
						(m_QuadVerts[(row * 5) + col] * sideLen) + pos[col];
			for (
					int col = 3; col < 5; col++
					)
				verts[(row * 5) + col] = m_QuadVerts[(row * 5) + col];
		}

		return GenObject(
				pos, sizeof(verts), verts, vertShaderPath,
				fragShaderPath
		);
	}

	void Renderer::MoveQuad(RendererObject &obj, const float newPos[3],
	                        float sideLen) {
		for (
				int i = 0; i < 3; i++
				)
			obj.position[i] = newPos[i];

		float verts[sizeof(m_QuadVerts) / sizeof(float)];

		for (
				int row = 0; row < 6; row++
				) {
			for (
					int col = 0; col < 3; col++
					)
				verts[(row * 5) + col] =
						(m_QuadVerts[(row * 5) + col] * sideLen) + newPos[col];
			for (
					int col = 3; col < 5; col++
					)
				verts[(row * 5) + col] = m_QuadVerts[(row * 5) + col];
		}

		// update vbo
		glBindVertexArray(obj.vao);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
		glBufferData(GL_ARRAY_BUFFER, obj.bufferSize, verts, GL_STATIC_DRAW);

		glVertexAttribPointer(
				0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
				(void *) 0
		);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
	}

	void Renderer::DeleteQuad(RendererObject &obj) {
		glDeleteBuffers(1, &obj.vbo);
		glDeleteVertexArrays(1, &obj.vao);

		obj.shader.Destroy();
	}

	void Renderer::SubmitObject(RendererObject obj) {
		glBindVertexArray(obj.vao);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
		obj.shader.Bind();

		glDrawArrays(GL_TRIANGLES, 0, obj.bufferSize);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		obj.shader.UnBind();
	}


}