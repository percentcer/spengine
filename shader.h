#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

const char *vertexShaderSource;
const char *fragmentShaderSource;

void loadShader(const char *fn, const char **store)
{
    FILE *f = fopen(fn, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *result = (char *)malloc(fsize + 1);
    fread(result, 1, fsize, f);
    fclose(f);
    result[fsize] = 0;

    *store = result;
};

class Shader
{
public:
    unsigned int PROGRAM_ID;
    Shader(const char *vertpath, const char *fragpath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertpath);
            fShaderFile.open(fragpath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        unsigned int vertid;
        unsigned int fragid;

        int success;
        char infoLog[512];

        vertid = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertid, 1, &vShaderCode, NULL);
        glCompileShader(vertid);
        glGetShaderiv(vertid, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            int length;
            glGetShaderInfoLog(vertid, 512, &length, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        fragid = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragid, 1, &fShaderCode, NULL);
        glCompileShader(fragid);
        glGetShaderiv(fragid, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            int length;
            glGetShaderInfoLog(fragid, 512, &length, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        PROGRAM_ID = glCreateProgram();
        glAttachShader(PROGRAM_ID, vertid);
        glAttachShader(PROGRAM_ID, fragid);
        glLinkProgram(PROGRAM_ID);
        glGetProgramiv(PROGRAM_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            int length;
            glGetProgramInfoLog(PROGRAM_ID, 512, &length, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }
        glDeleteShader(vertid);
        glDeleteShader(fragid);
    }

    void activate() {
        glUseProgram(PROGRAM_ID);
    }

    // uniforms
    void setBool(const char *name, bool value) const {
        const unsigned int loc = glGetUniformLocation(PROGRAM_ID, name);
        glUniform1i(loc, value);
    }
    void setInt(const char *name, int value) const {
        const unsigned int loc = glGetUniformLocation(PROGRAM_ID, name);
        glUniform1i(loc, value);
    }
    void setFloat(const char *name, float value) const {
        const unsigned int loc = glGetUniformLocation(PROGRAM_ID, name);
        glUniform1f(loc, value);
    }
    void setMat4(const char *name, const float* value) {
        const unsigned int loc = glGetUniformLocation(PROGRAM_ID, name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, value);
    }
};
