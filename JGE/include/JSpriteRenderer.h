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
	void DrawSprite(JTexture *texture, glm::vec4 spriteRect, glm::vec2 position, glm::vec2 hotspot,
		glm::vec2 scale = glm::vec2(1.0f), GLfloat rotate = 0.0f, bool hFlipped = 0, bool vFlipped = 0, 
		glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f), int textureFilter = TEX_FILTER_NONE);
private:
	JShader shader;
	GLuint quadVAO;
	void initRenderData();

	//////////////////////////////////////////////////////////////////////////
	/// Bind texture to be used for the rendering followed.
	/// 
	/// @param tex - Texture to use.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void BindTexture(JTexture *tex, int textureFilter);

	GLint modelLocation;
	GLint spriteRectLocation;
	GLint textureSizeLocation;
	GLint colorLocation;
	GLint flippedLocation;
};