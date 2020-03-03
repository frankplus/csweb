#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "JShader.h"
#include "JTypes.h"

class JSpriteRenderer
{
public:
	JSpriteRenderer(JShader &shader);
	~JSpriteRenderer();
	void DrawSprite(JTexture *texture, glm::vec4 spriteRect, glm::vec2 position,
		glm::vec2 scale = glm::vec2(10, 10), GLfloat rotate = 0.0f);
private:
	JShader shader;
	GLuint quadVAO;
	void initRenderData();
};