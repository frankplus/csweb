#include "../include/JGE.h"
#include "../include/JRenderer.h"
#include "../include/JAssert.h"
#include "../include/JResourceManager.h"

JQuad::JQuad(JTexture *tex, float x, float y, float width, float height)
		:mTex(tex), mX(x), mY(y), mWidth(width), mHeight(height)
{

	JASSERT(tex != NULL);

	mHotSpotX = 0.0f;
	mHotSpotY = 0.0f;
		
	mColor.color = 0xFFFFFFFF;

	mHFlipped = false;
	mVFlipped = false;

	SetTextureRect(x, y, width, height);

}

void JQuad::SetTextureRect(float x, float y, float w, float h) 
{ 
	mX = x;
	mY = y;
	mWidth = w;
	mHeight = h;
}

void JQuad::SetColor(PIXEL_TYPE color)
{
	mColor.color = color;
}



void JQuad::SetHotSpot(float x, float y)
{
	mHotSpotX = x;
	mHotSpotY = y;
}


//////////////////////////////////////////////////////////////////////////

JTexture::JTexture()
{
	mTexId = -1;
}

JTexture::~JTexture()
{
	if (mTexId != -1)
		glDeleteTextures(1, &mTexId);
}

//////////////////////////////////////////////////////////////////////////

JRenderer* JRenderer::mInstance = NULL;

JRenderer* JRenderer::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JRenderer();

		JASSERT(mInstance != NULL);

		mInstance->InitRenderer();
	}
	
	return mInstance;
}


void JRenderer::Destroy()
{
	if (mInstance)
	{
		mInstance->DestroyRenderer();
		delete mInstance;
		mInstance = NULL;
	}
}

JRenderer::JRenderer()
{
}


JRenderer::~JRenderer()
{

}

void JRenderer::InitRenderer()
{
	mCurrentTextureFilter = TEX_FILTER_NONE;

	mCurrTexBlendSrc = BLEND_SRC_ALPHA;
	mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

	// Load shaders
	JResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH_F),
		static_cast<GLfloat>(SCREEN_HEIGHT_F), 0.0f, -1.0f, 1.0f);
	JResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	JResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	JResourceManager::LoadShader("simple.vert", "simple.frag", nullptr, "simple");
	JResourceManager::GetShader("simple").Use().SetMatrix4("projection", projection);

	JShader spriteShader = JResourceManager::GetShader("sprite");
	mSpriteRenderer = new JSpriteRenderer(spriteShader);
}

void JRenderer::DestroyRenderer()
{
	JResourceManager::Clear();
}

void JRenderer::BeginScene()
{
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
}


void JRenderer::EndScene()
{
	glFlush ();
}

void JRenderer::EnableTextureFilter(bool flag)
{
	if (flag)
		mCurrentTextureFilter = TEX_FILTER_LINEAR;
	else
		mCurrentTextureFilter = TEX_FILTER_NEAREST;
}

void Swap(float *a, float *b)
{
	float n=*a;
	*a = *b;
	*b = n;
}


void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale)
{
	glm::vec2 hotspot = glm::vec2(quad->mHotSpotX, quad->mHotSpotY);
	glm::vec4 spriteRect = glm::vec4(quad->mX, quad->mY, quad->mWidth, quad->mHeight);
	glm::vec2 position = glm::vec2(xo, yo);
	glm::vec2 scale = glm::vec2(xScale, yScale);
	glm::vec4 color = glm::vec4(quad->mColor.r, quad->mColor.g, quad->mColor.b, quad->mColor.a) / 255.f; // normalize to 0-1

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	mSpriteRenderer->DrawSprite(quad->mTex, spriteRect, position, hotspot, scale, angle, 
								quad->mHFlipped, quad->mVFlipped, color, mCurrentTextureFilter);
}


void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	JShader shader = JResourceManager::GetShader("simple").Use();

	GLfloat vertices[] = {
		x+width,  y,  // Top Right
		x+width, y+height,  // Bottom Right
		x, y+height,  // Bottom Left
		x,  y   // Top Left 
	};

	GLuint indices[] = {  
		0, 1, 3,   // First Triangle
		1, 2, 3    // Second Triangle
	};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	JColor col;
	col.color = color;
	glm::vec4 my_color = glm::vec4(col.r, col.g, col.b, col.a) / 255.f; // normalize to 0-1
	shader.SetVector4f("color", my_color);

	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
	glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}


void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	JShader shader = JResourceManager::GetShader("simple").Use();

	GLfloat vertices[] = {
		x+width,  y,  // Top Right
		x+width, y+height,  // Bottom Right
		x, y+height,  // Bottom Left
		x,  y   // Top Left 
	};

	GLuint indices[] = {  
		0, 1,
		1, 2,
		2, 3, 
		3, 0  
	};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	JColor col;
	col.color = color;
	glm::vec4 my_color = glm::vec4(col.r, col.g, col.b, col.a) / 255.f; // normalize to 0-1
	shader.SetVector4f("color", my_color);

	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
	glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color)
{
	JShader shader = JResourceManager::GetShader("simple").Use();

	GLfloat vertices[] = {
		x1, y1,  
		x2,  y2   
	};

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	JColor col;
	col.color = color;
	glm::vec4 my_color = glm::vec4(col.r, col.g, col.b, col.a) / 255.f; // normalize to 0-1
	shader.SetVector4f("color", my_color);

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
	glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glDrawArrays(GL_LINES, 0, 2);
	
	glBindVertexArray(0);
}

JTexture* JRenderer::CreateTexture(int width, int height )
{
	int size = width * height * sizeof(PIXEL_TYPE);			// RGBA

	u8* buffer = new u8[size];

	JTexture *tex = new JTexture();

	if (buffer && tex)
	{
		tex->mFilter = TEX_FILTER_LINEAR;
		tex->mWidth = width;
		tex->mHeight = height;
		tex->mTexWidth = width;
		tex->mTexHeight = height;

		GLuint texid; 
		glGenTextures(1, &texid);
		tex->mTexId = texid;

		memset(buffer, 0, size);

		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		delete buffer;

		return tex;
	}
	else
		return NULL;
}


void JRenderer::ClearScreen(PIXEL_TYPE color)
{
	FillRect(0.0f, 0.0f, SCREEN_WIDTH_F, SCREEN_HEIGHT_F, color);
}


void JRenderer::SetTexBlend(int src, int dest)
{
	if (src != mCurrTexBlendSrc || dest != mCurrTexBlendDest)
	{
		mCurrTexBlendSrc = src;
		mCurrTexBlendDest = dest;
		
		glBlendFunc(src, dest);
	}
}


void JRenderer::SetTexBlendSrc(int src)
{
	if (src != mCurrTexBlendSrc)
	{
		mCurrTexBlendSrc = src;
		glBlendFunc(mCurrTexBlendSrc, mCurrTexBlendDest);
	}
}


void JRenderer::SetTexBlendDest(int dest)
{
	if (dest != mCurrTexBlendDest)
	{
		mCurrTexBlendDest = dest;
		glBlendFunc(mCurrTexBlendSrc, mCurrTexBlendDest);
	}
}


void JRenderer::Enable2D()
{
	glDisable (GL_DEPTH_TEST);
}


void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color)
{
	// JColor col;
	// col.color = color;

	// glDisable(GL_TEXTURE_2D);
	// glColor4ub(col.r, col.g, col.b, col.a);
	// glBegin(GL_TRIANGLE_FAN);

	// for(int i=0; i<count;i++)
	// {
	// 	glVertex2f(x[i],SCREEN_HEIGHT_F-y[i]);
	// }

	// glEnd();

	// glEnable(GL_TEXTURE_2D);

	// // default color
	// glColor4ub(255, 255, 255, 255);
}


void JRenderer::DrawLine(float x1, float y1, float x2, float y2, float lineWidth, PIXEL_TYPE color)
{
	float dy=y2-y1;
	float dx=x2-x1;
	if(dy==0 && dx==0)
		return;

	float l=(float)hypot(dx,dy);

	float x[4];
	float y[4];

	x[0]=x1+lineWidth*(y2-y1)/l;
	y[0]=y1-lineWidth*(x2-x1)/l;

	x[1]=x1-lineWidth*(y2-y1)/l;
	y[1]=y1+lineWidth*(x2-x1)/l;

	x[2]=x2-lineWidth*(y2-y1)/l;
	y[2]=y2+lineWidth*(x2-x1)/l;

	x[3]=x2+lineWidth*(y2-y1)/l;
	y[3]=y2-lineWidth*(x2-x1)/l;

	FillPolygon(x, y, 4, color);
}


void JRenderer::DrawCircle(float x, float y, float radius, PIXEL_TYPE color)
{
	// JColor col;
	// col.color = color;

	// glDisable(GL_TEXTURE_2D);
	// glColor4ub(col.r, col.g, col.b, col.a);
	// glBegin(GL_LINE_STRIP);

	// 	for(int i=0; i<360;i+=2)
	// 	{
	// 		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i));
	// 	}

	// 	glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0));

	// glEnd();

	// glEnable(GL_TEXTURE_2D);

	// // default color
	// glColor4ub(255, 255, 255, 255);
}

void JRenderer::FillCircle(float x, float y, float radius, PIXEL_TYPE color)
{
	// JColor col;
	// col.color = color;

	// glDisable(GL_TEXTURE_2D);
	// glColor4ub(col.r, col.g, col.b, col.a);
	// glBegin(GL_TRIANGLE_FAN);

	// 	glVertex2f(x, SCREEN_HEIGHT_F-y);

	// 	for(int i=0; i<360;i+=2)
	// 	{
	// 		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i));
	// 	}

	// 	glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0));

	// glEnd();

	// glEnable(GL_TEXTURE_2D);

	// // default color
	// glColor4ub(255, 255, 255, 255);
}


void JRenderer::FillPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color)
{
	// JColor col;
	// col.color = color;

	// float angle = -startingAngle*RAD2DEG;
	// float firstAngle = angle;
	// float steps = 360.0f/count;
	// size /= 2;

	// glDisable(GL_TEXTURE_2D);
	// glColor4ub(col.r, col.g, col.b, col.a);
	// glBegin(GL_TRIANGLE_FAN);

	// 	glVertex2f(x, SCREEN_HEIGHT_F-y);

	// 	for(int i=0; i<count;i++)
	// 	{
	// 		glVertex2f(x+size*COSF((int)angle), SCREEN_HEIGHT_F-y+size*SINF((int)angle));
	// 		angle += steps;
	// 		if (angle >= 360.0f)
	// 			angle -= 360.0f;
	// 	}

	// 	glVertex2f(x+size*COSF((int)firstAngle), SCREEN_HEIGHT_F-y+size*SINF((int)firstAngle));


	// glEnd();

	// glEnable(GL_TEXTURE_2D);

	// // default color
	// glColor4ub(255, 255, 255, 255);
}