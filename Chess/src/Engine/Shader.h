#pragma once

#include <string>
#include <vector>

namespace Engine
{

	class Texture;

	class Shader
	{
	public:
		explicit Shader(
			const char *vertShaderSource = nullptr,
			const char *fragShaderSource = nullptr
		);

		static Shader Compile(const char *vertPath, const char *fragPath);

		void SetUniform(int loc, int value) const;

		void SetUniform(int loc, bool value) const;

		void SetUniform(int loc, float value) const;

		void SetUniformVec(int loc, int size, const float value[]) const;

		void SetUniformMat(int loc, int size, float **value) const;

		int GetUniformLocation(const char *name) const;

		int GetAttribLocation(const char *name) const;

		void Bind() const;

		void UnBind() const;

		void AttachTexture(Texture tex);

		void Destroy();

	private:
		unsigned int m_ShaderProgramID;

		std::vector<Texture> m_Textures;

	private:
		static std::string ReadFile(const char *path);

		static bool CheckErrors(unsigned int shader);
	};

	class Texture
	{
	public:
		explicit Texture(const char *path);

		void Bind() const;

		void UnBind() const;

		void Destroy();

	private:
		unsigned int m_TextureID {};
	};

} // namespace Engine
