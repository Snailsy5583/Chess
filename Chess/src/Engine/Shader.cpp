#include "Shader.h"

#include "stb/stb_image.h"
#include "glad/glad.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

namespace Engine {
    Shader::Shader(const char *vertShaderSource/*=NULL*/,
                   const char *fragShaderSource/*=NULL*/) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (vertShaderSource == nullptr || fragShaderSource == nullptr)
            return;

        m_ShaderProgramID = glCreateProgram();

        unsigned int vertShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShaderID, 1, &vertShaderSource, nullptr);
        glCompileShader(vertShaderID);
        if (!CheckErrors(vertShaderID))
            glAttachShader(m_ShaderProgramID, vertShaderID);

        unsigned int fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShaderID, 1, &fragShaderSource, nullptr);
        glCompileShader(fragShaderID);
        if (!CheckErrors(fragShaderID))
            glAttachShader(m_ShaderProgramID, fragShaderID);

        glLinkProgram(m_ShaderProgramID);

        int success = false;
        // check for linking errors
        glGetProgramiv(m_ShaderProgramID,
                       GL_LINK_STATUS, &success);
        if (!success) {
            std::vector<char> errorLog(512);
            glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, &errorLog[0]);

            std::stringstream logStream;
            for (char character: errorLog) {
                logStream << character;
            }

            std::cout << logStream.str();
        }

        glDetachShader(m_ShaderProgramID, vertShaderID);
        glDetachShader(m_ShaderProgramID, fragShaderID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);
    }

    Shader Shader::Compile(const char *vertPath, const char *fragPath) {
        std::string vertShaderSource = ReadFile(vertPath);
        std::string fragShaderSource = ReadFile(fragPath);

        return Shader(vertShaderSource.c_str(), fragShaderSource.c_str());
    }

#pragma region SetUniformFuncs

    void Shader::SetUniform(int loc, int value) const {
        Bind();
        glUniform1i(loc, value);
        UnBind();
    }

    void Shader::SetUniform(int loc, bool value) const {
        Bind();
        glUniform1i(loc, value);
        UnBind();
    }

    void Shader::SetUniform(int loc, float value) const {
        Bind();
        glUniform1f(loc, value);
        UnBind();
    }

    void Shader::SetUniformVec(int loc, int size, float value[]) const {
        Bind();

        switch (size) {
            case 2:
                glUniform2f(loc, *(value), *(value + 1));
                break;
            case 3:
                glUniform3f(loc, *(value), *(value + 1), *(value + 2));
                break;
            case 4:
                glUniform4f(loc, *(value), *(value + 1), *(value + 2),
                            *(value + 3));
                break;
        }

        UnBind();
    }

    void Shader::SetUniformMat(int loc, int size, float **value) const {
        Bind();

        // TODO: switch case for SetUniformMat

        UnBind();
    }

#pragma endregion

    int Shader::GetUniformLocation(const char *name) const {
        return glGetUniformLocation(m_ShaderProgramID, name);
    }

    int Shader::GetAttribLocation(const char *name) const {
        return glGetAttribLocation(m_ShaderProgramID, name);
    }

    void Shader::Bind() const {
        for (const Texture &tex: m_Textures) {
            tex.Bind();
        }
        glUseProgram(m_ShaderProgramID);
    }

    void Shader::UnBind() const {
        for (const Texture &tex: m_Textures) {
            tex.UnBind();
        }
        glUseProgram(0);
    }

    void Shader::AttachTexture(Texture tex) {
        m_Textures.push_back(tex);
    }

    void Shader::Destroy() {
        glDeleteProgram(m_ShaderProgramID);
        for (Texture &texture: m_Textures) {
            texture.Destroy();
        }
    }

    std::string Shader::ReadFile(const char *path) {
        std::ifstream file;
        file.open(path);


        std::string contents;

        if (!file.is_open()) {
            std::cout << "ERROR OPENING FILE: " << path << std::endl;
            return "";
        }

        std::string line;
        while (std::getline(file, line)) {
            contents.append(line + "\n");
        }

        file.close();

        return contents;
    }

    bool Shader::CheckErrors(unsigned int shader) {
        int isCompiled = false;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (!isCompiled) {
            int maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<char> errorLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

            std::stringstream logStream;

            for (char character: errorLog) {
                logStream << character;
            }

            std::cout << logStream.str();

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(shader); // Don't leak the shader.
        }

// 		std::cout << isCompiled << std::endl;

        return !isCompiled;
    }

    // Texture ////////////////////////////////////////////////////////////////////

    Texture::Texture(const char *path) {
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels = 4;
        unsigned char *image = stbi_load(path, &width, &height, &channels,
                                         STBI_rgb_alpha);

        if (!image) {
            std::cout << "ERROR: Image not opened" << std::endl;
        }

        glGenTextures(1, &m_TextureID);
        Bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image);
        UnBind();

        stbi_image_free(image);
    }

    void Texture::Bind() const {
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }

    void Texture::UnBind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Destroy() {
        glDeleteTextures(1, &m_TextureID);
    }

}