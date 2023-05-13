//
//  shaderProgram.cpp
//  4d projection
//
//  Created by David on 19.09.22.
//

#include "shaderProgram.hpp"


Program::Program(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
	
#if DEBUG_FILE_LOAD
	printf("vertexFile: %s\n", vertexFile);
	printf("fragmentFile: %s\n", fragmentFile);
	
	path p = vertexFile;
	std::cout << p << '\n';
	printf("v: %s\n", p.c_str());
#endif
    // vertex shader
    string vertexString = readFile(vertexFile);
//    if (vertexString.size() == 0) {
//        goto fragment_shader_label;
//    }
    const char* vertexSource = vertexString.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    
//    fragment_shader_label:
    // fragment shader
    string fragmentString = readFile(fragmentFile);
    const char* fragmentSource = fragmentString.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    // geometry shader (if available
    GLuint geometryShader = -1;
    if (geometryFile) {
        string geometryString = readFile(geometryFile);
        const char* geometrySource = geometryString.c_str();
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometrySource, NULL);
        glCompileShader(geometryShader);
    }
    
#define ERROR_LOG_LENGTH 1024
    int vertexSuccess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
    if (!vertexSuccess) {
        GLchar errorLog[ERROR_LOG_LENGTH];
        glGetShaderInfoLog(vertexShader, ERROR_LOG_LENGTH, nullptr, &errorLog[0]);
        std::cout << "vertex shader failed to compile:\n"<< errorLog << std::endl;
    }
    int fragmentSuccess;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
    if (!fragmentSuccess) {
        GLchar errorLog[ERROR_LOG_LENGTH];
        glGetShaderInfoLog(fragmentShader, ERROR_LOG_LENGTH, nullptr, &errorLog[0]);
        std::cout << "fragment shader failed to compile:\n" << fragmentFile << "\n" << errorLog << std::endl;
    }
    if (geometryFile) {
        int geometrySuccess;
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &geometrySuccess);
        if (!geometrySuccess) {
            GLchar errorLog[ERROR_LOG_LENGTH];
            glGetShaderInfoLog(geometryShader, ERROR_LOG_LENGTH, nullptr, &errorLog[0]);
            std::cout << "geometry shader failed to compile:\n" << errorLog << std::endl;
        }
    }
    
    
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (geometryFile) {
        glAttachShader(program, geometryShader);
    }
    glLinkProgram(program);
    
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = ERROR_LOG_LENGTH;

        // The maxLength includes the NULL character
        GLchar infoLog[ERROR_LOG_LENGTH];
        glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
        
        std::cout << "program failed to link:\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryFile) {
        glDeleteShader(geometryShader);
    }
}

void Program::activate()
{
    glUseProgram(program);
}

GLuint Program::getUniformLocation(const char* var) const
{
    return glGetUniformLocation(program, var);
}

void Program::destroy()
{
    glDeleteProgram(program);
}
