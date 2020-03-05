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

void JRenderer::InitRenderer()
{
	mCurrentTextureFilter = TEX_FILTER_NONE;

	mCurrTexBlendSrc = BLEND_SRC_ALPHA;
	mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

	// Load shaders
	JResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH_F),
		static_cast<GLfloat>(SCREEN_HEIGHT_F), 0.0f, -1.0f, 1.0f);
	JShader spriteShader = JResourceManager::GetShader("sprite");
	spriteShader.Use();
	spriteShader.SetInteger("image", 0);
	spriteShader.SetMatrix4("projection", projection);

	JResourceManager::LoadShader("simple.vert", "simple.frag", nullptr, "simple");
	JShader simpleShader = JResourceManager::GetShader("simple");
	simpleShader.Use();
	simpleShader.SetMatrix4("projection", projection);
	colorUniformLoc = glGetUniformLocation(simpleShader.Program, "color");

	// Load sprite renderer
	mSpriteRenderer = new JSpriteRenderer(spriteShader);

	// Load Vertex Array Object
	JRenderer::InitVAO();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void JRenderer::InitVAO()
{
	bufferSize = 20;
	elementBufferSize = 20;

	JShader shader = JResourceManager::GetShader("simple").Use();

	GLuint VBO, EBO;
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

	GLint vertexLocation = glGetAttribLocation(shader.Program, "vertex");
	glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glBindVertexArray(0);
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
	// glFlush ();
}

void JRenderer::EnableTextureFilter(bool flag)
{
	if (flag)
		mCurrentTextureFilter = TEX_FILTER_LINEAR;
	else
		mCurrentTextureFilter = TEX_FILTER_NEAREST;
}

JTexture* JRenderer::CreateTexture(int width, int height )
{
	int size = width * height * sizeof(PIXEL_TYPE);			// RGBA

	u8* buffer = new u8[size];

	JTexture *tex = new JTexture();

	if (buffer && tex)
	{
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
	JColor col;
	col.color = color;
	glClearColor(col.r / 255.f, 
				col.g / 255.f, 
				col.b / 255.f, 
				col.a / 255.f);
	glClear (GL_COLOR_BUFFER_BIT);
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

void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale)
{
	glm::vec2 hotspot = glm::vec2(quad->mHotSpotX, quad->mHotSpotY);
	glm::vec4 spriteRect = glm::vec4(quad->mX, quad->mY, quad->mWidth, quad->mHeight);
	glm::vec2 position = glm::vec2(xo, yo);
	glm::vec2 scale = glm::vec2(xScale, yScale);
	glm::vec4 color = glm::vec4(quad->mColor.r, quad->mColor.g, quad->mColor.b, quad->mColor.a) / 255.f; // normalize to 0-1
	
	mSpriteRenderer->DrawSprite(quad->mTex, spriteRect, position, hotspot, scale, angle, 
								quad->mHFlipped, quad->mVFlipped, color, mCurrentTextureFilter);
}

void JRenderer::DrawPolygon(float* x, float* y, int count, PIXEL_TYPE color, GLenum mode)
{
	JShader shader = JResourceManager::GetShader("simple").Use();

	// vertices array
	int buf_size = 2 * count; // 2 coordinates per vertex
	GLfloat vertices[buf_size]; 
	for(int i=0; i<count; i++)
	{
		vertices[2*i] = x[i];
		vertices[2*i + 1] = y[i];
	}

	if(buf_size > bufferSize)
		printf("Vertex buffer too small!");

	//set color normalized to 0-1
	JColor col;
	col.color = color;
	glUniform4f(colorUniformLoc, 
				col.r / 255.f, 
				col.g / 255.f, 
				col.b / 255.f, 
				col.a / 255.f);
	
	glBindVertexArray(mVAO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawArrays(mode, 0, count);

	glBindVertexArray(0);
}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	float vertices_x[] = {x, x+width, x+width, x};
	float vertices_y[] = {y, y, y+height, y+height};
	FillConvexPolygon(vertices_x, vertices_y, 4, color);
}


void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	float vertices_x[] = {x, x+width, x+width, x};
	float vertices_y[] = {y, y, y+height, y+height};
	DrawPolygon(vertices_x, vertices_y, 4, color);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color)
{
	float vertices_x[] = {x1, x2};
	float vertices_y[] = {y1, y2};
	DrawPolygon(vertices_x, vertices_y, 4, color, GL_LINES);
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

void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color)
{
	JShader shader = JResourceManager::GetShader("simple").Use();
	
	// Poligon triangulation
	using Point = std::array<float, 2>;

	std::vector<Point> polygon;

	for(int i=0; i<count; i++)
		polygon.push_back({x[i], y[i]});

	std::vector<std::vector<Point>> v =  {polygon};
	std::vector<GLint> indices = mapbox::earcut<GLint>(v);

	// vertices array
	int buf_size = 2 * count; // 2 coordinates per vertex
	GLfloat vertices[buf_size]; 
	for(int i=0; i<count; i++)
	{
		vertices[2*i] = x[i];
		vertices[2*i + 1] = y[i];
	}

	if(buf_size > bufferSize)
		printf("Vertex buffer too small!");

	//set color normalized to 0-1
	JColor col;
	col.color = color;
	glUniform4f(colorUniformLoc, 
				col.r / 255.f, 
				col.g / 255.f, 
				col.b / 255.f, 
				col.a / 255.f);
	
	glBindVertexArray(mVAO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size()*sizeof(GLint), indices.data());

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
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

void JRenderer::FillConvexPolygon(float* x, float* y, int count, PIXEL_TYPE color)
{
	DrawPolygon(x, y, count, color, GL_TRIANGLE_FAN);
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