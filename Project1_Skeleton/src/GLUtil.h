#pragma once
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

inline GLenum report_gl_error(const std::string id = "")
{
    // http://stackoverflow.com/q/28485180/148668

    // gluErrorString was deprecated
    const auto gluErrorString = [](GLenum errorCode) -> const char * {
        switch (errorCode)
        {
        default:
            return "unknown error code";
        case GL_NO_ERROR:
            return "no error";
        case GL_INVALID_ENUM:
            return "invalid enumerant";
        case GL_INVALID_VALUE:
            return "invalid value";
        case GL_INVALID_OPERATION:
            return "invalid operation";
#ifndef GL_VERSION_3_0
        case GL_STACK_OVERFLOW:
            return "stack overflow";
        case GL_STACK_UNDERFLOW:
            return "stack underflow";
        case GL_TABLE_TOO_LARGE:
            return "table too large";
#endif
        case GL_OUT_OF_MEMORY:
            return "out of memory";
#ifdef GL_EXT_framebuffer_object
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
            return "invalid framebuffer operation";
#endif
        }
    };

    GLenum err = glGetError();
    if (GL_NO_ERROR != err)
    {
        fprintf(stderr, "GL_ERROR: %s%s\n", id.c_str(), gluErrorString(err));
    }
    return err;
}

inline std::string load_glsl(std::string path)
{
    using namespace std;
    ifstream shader_file;
    shader_file.exceptions(ifstream::failbit | ifstream::badbit);

    try
    {
        shader_file.open(path);
        stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        return shader_stream.str();
    }
    catch (ifstream::failure e)
    {
        cout << "Error: Shader file not successfully load" << endl;
    }
    return "";
}

inline void print_shader_info_log(const GLuint obj)
{
    GLint infologLength = 0;
    GLint charsWritten = 0;
    char *infoLog;

    // Get shader info log from opengl
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    // Only print if there is something in the log
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

inline GLuint load_shader(const std::string &src, const GLenum type)
{
    GLuint s = glCreateShader(type);
    if (s == 0)
    {
        fprintf(stderr, "Error: load_shader() failed to create shader.\n");
        return 0;
    }
    // Pass shader source string
    const char *c = src.c_str();
    glShaderSource(s, 1, &c, NULL);
    glCompileShader(s);
    print_shader_info_log(s);
    return s;
}

inline bool create_shader_program(const std::string &vert_source,
                                  const std::string &frag_source,
                                  GLuint &id)
{
    using namespace std;
    id = glCreateProgram();
    GLuint f = 0, v = 0;

    if (vert_source != "")
    {
        // load vertex shader
        v = load_shader(vert_source.c_str(), GL_VERTEX_SHADER);
        if (v == 0)
        {
            cerr << "vertex shader failed to compile." << endl;
            return false;
        }
        glAttachShader(id, v);
    }

    if (frag_source != "")
    {
        // load fragment shader
        f = load_shader(frag_source.c_str(), GL_FRAGMENT_SHADER);
        if (f == 0)
        {
            cerr << "fragment shader failed to compile." << endl;
            return false;
        }
        glAttachShader(id, f);
    }
    return true;
}

inline GLint bind_vertex_attrib_array(
    const GLuint program_shader,
    const std::string &name,
    GLuint bufferID,
    const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &M,
    bool refresh)
{
    GLint id = glGetAttribLocation(program_shader, name.c_str());
    if (id < 0)
        return id;
    if (M.size() == 0)
    {
        glDisableVertexAttribArray(id);
        return id;
    }
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    if (refresh)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * M.size(), M.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(id, M.cols(), GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(id);
    return id;
}

inline bool destroy_shader_program(const GLuint id)
{
    // Don't try to destroy id == 0 (no shader program)
    if (id == 0)
    {
        fprintf(stderr, "Error: destroy_shader_program() id = %d"
                        " but must should be positive\n",
                id);
        return false;
    }
    // Get each attached shader one by one and detach and delete it
    GLsizei count;
    // shader id
    GLuint s;
    do
    {
        // Try to get at most *1* attached shader
        glGetAttachedShaders(id, 1, &count, &s);
        GLenum err = report_gl_error();
        if (GL_NO_ERROR != err)
        {
            return false;
        }
        // Check that we actually got *1*
        if (count == 1)
        {
            // Detach and delete this shader
            glDetachShader(id, s);
            glDeleteShader(s);
        }
    } while (count > 0);
    // Now that all of the shaders are gone we can just delete the program
    glDeleteProgram(id);
    return true;
}