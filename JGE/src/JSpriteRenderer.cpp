#include "../include/JSpriteRenderer.h"

JSpriteRenderer::JSpriteRenderer(JShader & shader)
{
	this->shader = shader;
	initRenderData();

	shader.Use();
	modelLocation = glGetUniformLocation(shader.Program, "model");
	spriteRectLocation = glGetUniformLocation(shader.Program, "spriteRect");
	textureSizeLocation = glGetUniformLocation(shader.Program, "textureSize");
	colorLocation = glGetUniformLocation(shader.Program, "color");
	flippedLocation = glGetUniformLocation(shader.Program, "flipped");
}

JSpriteRenderer::~JSpriteRenderer()
{
	
}

void JSpriteRenderer::BindTexture(JTexture *tex, int textureFilter)
{
	glBindTexture(GL_TEXTURE_2D, tex->mTexId);

	if (textureFilter == TEX_FILTER_LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	else if (textureFilter == TEX_FILTER_NEAREST)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	}
}

void JSpriteRenderer::DrawSprite(JTexture *texture, glm::vec4 spriteRect, glm::vec2 position, 
								glm::vec2 hotspot, glm::vec2 scale, GLfloat rotate, 
								bool hFlipped, bool vFlipped, glm::vec4 color, int textureFilter)
{
	this->shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));
	model = glm::translate(model, glm::vec3(-hotspot, 0.0f));
	model = glm::scale(model, glm::vec3(spriteRect[2], spriteRect[3], 1.0f));

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4f(spriteRectLocation, spriteRect.x, spriteRect.y, spriteRect.z, spriteRect.w);
	glUniform2f(textureSizeLocation, texture->mTexWidth, texture->mTexHeight);
	glUniform2i(flippedLocation, hFlipped, vFlipped);
	glUniform4f(colorLocation, color.x, color.y, color.z, color.w);

	glActiveTexture(GL_TEXTURE0);
	BindTexture(texture, textureFilter);
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// printf("%s ; texw=%d texh=%d\n", glm::to_string(spriteRect).c_str(), texture->mWidth, texture->mHeight);
}

void JSpriteRenderer::initRenderData()
{
	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(this->quadVAO);
	GLint vertexLocation = glGetAttribLocation(this->shader.Program, "vertex");
	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
