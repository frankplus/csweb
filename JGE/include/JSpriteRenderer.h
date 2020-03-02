#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "JShader.h"
#include "JTypes.h"

class JSpriteRenderer
{
public:
	JSpriteRenderer(JShader &shader);
	~JSpriteRenderer();
	void DrawSprite(JTexture &texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));
private:
	JShader shader;
	GLuint quadVAO;
	void initRenderData();
};