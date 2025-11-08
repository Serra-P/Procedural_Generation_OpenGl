#include "Shader.hh"

#include <fstream>
#include <iostream>
#include <sstream>

#include "Logger.hh"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath,
               const std::string& geometryPath)
{
    GLuint vert = LoadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint frag = LoadShader(fragmentPath, GL_FRAGMENT_SHADER);
    GLuint geom = 0;

    if (!geometryPath.empty())
        geom = LoadShader(geometryPath, GL_GEOMETRY_SHADER);

    program_ = glCreateProgram();
    glAttachShader(program_, vert);
    glAttachShader(program_, frag);
    if (geom)
        glAttachShader(program_, geom);

    glLinkProgram(program_);

    GLint success;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetProgramInfoLog(program_, 1024, nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    if (geom)
        glDeleteShader(geom);
}

Shader::~Shader()
{
    glDeleteProgram(program_);
}

GLuint Shader::LoadShader(const std::string& path, GLenum type)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    GLuint shader = glCreateShader(type);
    const char* src = code.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
    }

    return shader;
}

void Shader::Use() const
{
    glUseProgram(program_);
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(program_, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1,
                       GL_FALSE, &mat[0][0]);
}

GLuint Shader::LoadComputeShader(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = code.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        return 0;
    }

    glDeleteShader(shader);
    return program;
}
