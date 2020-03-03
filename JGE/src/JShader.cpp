#include "../include/JShader.h"

JShader &JShader::Use()
{
	glUseProgram(this->Program);
	return *this;
}

void JShader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource)
{
	GLuint sVertex, sFragment, gShader;
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	// If geometry shader source code is given, also compile geometry shader
	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, sVertex);
	glAttachShader(this->Program, sFragment);
	glLinkProgram(this->Program);
	checkCompileErrors(this->Program, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
}

void JShader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
	if (useShader)
        this->Use();
	glUniform1f(glGetUniformLocation(this->Program, name), value);
}
void JShader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->Program, name), value);
}
void JShader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->Program, name), x, y);
}
void JShader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->Program, name), value.x, value.y);
}
void JShader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
	if (useShader)
        this->Use();
	glUniform3f(glGetUniformLocation(this->Program, name), x, y, z);
}
void JShader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
	if (useShader)
        this->Use();
	glUniform3f(glGetUniformLocation(this->Program, name), value.x, value.y, value.z);
}
void JShader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->Program, name), x, y, z, w);
}
void JShader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->Program, name), value.x, value.y, value.z, value.w);
}
void JShader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
	if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->Program, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void JShader::checkCompileErrors(GLuint object, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}