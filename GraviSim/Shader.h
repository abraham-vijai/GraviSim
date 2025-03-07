#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID; // ID of the shader program

    /**
     * @brief Constructor that generates the shader program on the fly.
     *
     * @param vertexPath Path to the vertex shader source file.
     * @param fragmentPath Path to the fragment shader source file.
     */
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        unsigned int vertex, fragment;
        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // Fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // Shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    /**
     * @brief Activates the shader program.
     */
    void use()
    {
        glUseProgram(ID);
    }

    /**
     * @brief Sets a boolean uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value Boolean value to set.
     */
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    /**
     * @brief Sets an integer uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value Integer value to set.
     */
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    /**
     * @brief Sets a float uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value Float value to set.
     */
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    /**
     * @brief Sets a 2-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value glm::vec2 value to set.
     */
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    /**
     * @brief Sets a 2-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param x First component of the vector.
     * @param y Second component of the vector.
     */
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    /**
     * @brief Sets a 3-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value glm::vec3 value to set.
     */
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    /**
     * @brief Sets a 3-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param x First component of the vector.
     * @param y Second component of the vector.
     * @param z Third component of the vector.
     */
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    /**
     * @brief Sets a 4-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param value glm::vec4 value to set.
     */
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    /**
     * @brief Sets a 4-component vector uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param x First component of the vector.
     * @param y Second component of the vector.
     * @param z Third component of the vector.
     * @param w Fourth component of the vector.
     */
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    /**
     * @brief Sets a 2x2 matrix uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param mat glm::mat2 value to set.
     */
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    /**
     * @brief Sets a 3x3 matrix uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param mat glm::mat3 value to set.
     */
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    /**
     * @brief Sets a 4x4 matrix uniform in the shader.
     *
     * @param name Name of the uniform variable.
     * @param mat glm::mat4 value to set.
     */
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    /**
     * @brief Checks for shader compilation or linking errors.
     *
     * @param shader Shader or program ID to check.
     * @param type Type of shader or program ("VERTEX", "FRAGMENT", or "PROGRAM").
     */
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#endif