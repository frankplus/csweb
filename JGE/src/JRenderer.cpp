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
	//mBlend = BLEND_DEFAULT;		
	for (int i=0;i<4;i++)
		mColor[i].color = 0xFFFFFFFF;

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

	mTX0 = x/mTex->mTexWidth;
	mTY0 = y/mTex->mTexHeight;
	mTX1 = (x+w)/mTex->mTexWidth;
	mTY1 = (y+h)/mTex->mTexHeight;

}


void JQuad::GetTextureRect(float *x, float *y, float *w, float *h)  
{ 
	*x=mX; *y=mY; *w=mWidth; *h=mHeight; 
}


// void JQuad::SetColor(JColor color)
// {
// 	for (int i=0;i<4;i++)
// 		mColor[i].color = color.color;
// }
// 

void JQuad::SetColor(PIXEL_TYPE color)
{
	for (int i=0;i<4;i++)
		mColor[i].color = color;
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


void JTexture::UpdateBits(int x, int y, int width, int height, PIXEL_TYPE* bits)
{
	JRenderer::GetInstance()->BindTexture(this);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bits);

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

	mCurrentTex = -1;
	mFOV = 75.0f;

	mCurrentRenderMode = MODE_UNKNOWN;

	// Load shaders
	JResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
	// glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
	// 	static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	// JResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	// JResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// Set render-specific controls
	JShader spriteShader = JResourceManager::GetShader("sprite");
	mSpriteRenderer = new JSpriteRenderer(spriteShader);
}

void JRenderer::DestroyRenderer()
{

}

void JRenderer::BeginScene()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glLoadIdentity ();											// Reset The Modelview Matrix

}


void JRenderer::EndScene()
{
	glFlush ();
}

void JRenderer::BindTexture(JTexture *tex)
{

	if (mCurrentTex != tex->mTexId)
	{
		mCurrentTex = tex->mTexId;
		glBindTexture(GL_TEXTURE_2D, tex->mTexId);

		//if (mCurrentTextureFilter != tex->mFilter)
		{
			//mCurrentTextureFilter = tex->mFilter;
			if (mCurrentTextureFilter == TEX_FILTER_LINEAR)
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			}
			else if (mCurrentTextureFilter == TEX_FILTER_NEAREST)
			{
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			}
		}
	}
}


void JRenderer::EnableTextureFilter(bool flag)
{
	if (flag)
		mCurrentTextureFilter = TEX_FILTER_LINEAR;
	else
		mCurrentTextureFilter = TEX_FILTER_NEAREST;

	mCurrentTex = -1;
}

void Swap(float *a, float *b)
{
	float n=*a;
	*a = *b;
	*b = n;
}


void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale)
{
	float width = quad->mWidth;
	float height = quad->mHeight;
	float x = -quad->mHotSpotX;
	float y = quad->mHotSpotY;

	Vector2D pt[4];
	pt[3] = Vector2D(x, y);
	pt[2] = Vector2D(x+width, y);
	pt[1] = Vector2D(x+width, y-height);
	pt[0] = Vector2D(x, y-height);

	Vector2D uv[4];
	uv[0] = Vector2D(quad->mTX0, quad->mTY1);
	uv[1] = Vector2D(quad->mTX1, quad->mTY1);
	uv[2] = Vector2D(quad->mTX1, quad->mTY0);
	uv[3] = Vector2D(quad->mTX0, quad->mTY0);

	if (quad->mHFlipped)
	{
		Swap(&uv[0].x, &uv[1].x);
		Swap(&uv[2].x, &uv[3].x);
	}
	
	if (quad->mVFlipped)
	{
		Swap(&uv[0].y, &uv[2].y);
		Swap(&uv[1].y, &uv[3].y);
	}

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindTexture(quad->mTex);


	////glRasterPos2f(x, y);
	

	yo = SCREEN_HEIGHT_F - yo;

	glPushMatrix();
	glTranslatef(xo, yo, 0.0f);
	glRotatef(-angle*RAD2DEG, 0.0f, 0.0f, 1.0f);
	glScalef(xScale, yScale, 1.0f);

	glBegin(GL_QUADS);
		// bottom left corner
		glColor4ub(quad->mColor[0].r, quad->mColor[0].g, quad->mColor[0].b, quad->mColor[0].a);
		glTexCoord2f(uv[0].x, uv[0].y); glVertex2f(pt[0].x, pt[0].y);		

		// bottom right corner
		glColor4ub(quad->mColor[1].r, quad->mColor[1].g, quad->mColor[1].b, quad->mColor[1].a);
		glTexCoord2f(uv[1].x, uv[1].y); glVertex2f(pt[1].x, pt[1].y);	
		
		// top right corner
		glColor4ub(quad->mColor[2].r, quad->mColor[2].g, quad->mColor[2].b, quad->mColor[2].a);
		glTexCoord2f(uv[2].x, uv[2].y); glVertex2f(pt[2].x, pt[2].y);	

		// top left corner
		glColor4ub(quad->mColor[3].r, quad->mColor[3].g, quad->mColor[3].b, quad->mColor[3].a);
		glTexCoord2f(uv[3].x, uv[3].y); glVertex2f(pt[3].x, pt[3].y);		
	glEnd();
	
	glPopMatrix();

	//glDisable(GL_BLEND);
	
	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::RenderQuad(JQuad* quad, VertexColor* pt)
{

	for (int i=0;i<4;i++)
	{
		pt[i].y = SCREEN_HEIGHT_F - pt[i].y;
		quad->mColor[i].color = pt[i].color;
	}

	Vector2D uv[4];
	uv[0] = Vector2D(quad->mTX0, quad->mTY1);
	uv[1] = Vector2D(quad->mTX1, quad->mTY1);
	uv[2] = Vector2D(quad->mTX1, quad->mTY0);
	uv[3] = Vector2D(quad->mTX0, quad->mTY0);

	BindTexture(quad->mTex);

	glRasterPos2f(pt[0].x, pt[0].y);
	
	//float w = quad->mWidth;
	//float h = quad->mHeight;

	glBegin(GL_QUADS);
		// bottom left corner
		glColor4ub(quad->mColor[0].r, quad->mColor[0].g, quad->mColor[0].b, quad->mColor[0].a);
		glTexCoord2f(uv[0].x, uv[0].y); glVertex2f(pt[0].x, pt[0].y);		

		// bottom right corner
		glColor4ub(quad->mColor[1].r, quad->mColor[1].g, quad->mColor[1].b, quad->mColor[1].a);
		glTexCoord2f(uv[1].x, uv[1].y); glVertex2f(pt[1].x, pt[1].y);	
		
		// top right corner
		glColor4ub(quad->mColor[2].r, quad->mColor[2].g, quad->mColor[2].b, quad->mColor[2].a);
		glTexCoord2f(uv[2].x, uv[2].y); glVertex2f(pt[2].x, pt[2].y);	

		// top left corner
		glColor4ub(quad->mColor[3].r, quad->mColor[3].g, quad->mColor[3].b, quad->mColor[3].a);
		glTexCoord2f(uv[3].x, uv[3].y); glVertex2f(pt[3].x, pt[3].y);		
	glEnd();
	
	//glDisable(GL_BLEND);
	
	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	y = SCREEN_HEIGHT_F - y - height;
	
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_QUADS);
		// top left corner
		glVertex2f(x, y+height);		

		// bottom left corner
		glVertex2f(x, y);		

		// bottom right corner
		glVertex2f(x+width, y);	
		
		// top right corner
		glVertex2f(x+width, y+height);	

	glEnd();
	
	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color)
{
	y = SCREEN_HEIGHT_F - y - height;

	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINES);
		
		glVertex2f(x, y);		
		glVertex2f(x, y+height);	

		glVertex2f(x, y+height);
		glVertex2f(x+width, y+height);

		glVertex2f(x+width, y+height);
		glVertex2f(x+width, y);

		glVertex2f(x+width, y);
		glVertex2f(x, y);

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE* colors)
{
	JColor col[4];
	for (int i=0;i<4;i++)
		col[i].color = colors[i];

	FillRect(x, y, width, height, col);
}


void JRenderer::FillRect(float x, float y, float width, float height, JColor* colors)
{
	y = SCREEN_HEIGHT_F - y - height;
	
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		// top left corner
		glColor4ub(colors[0].r, colors[0].g, colors[0].b, colors[0].a);
		glVertex2f(x, y+height);		

		// bottom left corner
		glColor4ub(colors[2].r, colors[2].g, colors[2].b, colors[2].a);
		glVertex2f(x, y);		

		// bottom right corner
		glColor4ub(colors[3].r, colors[3].g, colors[3].b, colors[3].a);
		glVertex2f(x+width, y);	
		
		// top right corner
		glColor4ub(colors[1].r, colors[1].g, colors[1].b, colors[1].a);
		glVertex2f(x+width, y+height);	

	glEnd();
	
	glEnable(GL_TEXTURE_2D);

	//glDisable(GL_BLEND);
	
	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color)
{
//	glLineWidth (mLineWidth);
	glDisable(GL_TEXTURE_2D);
	JColor col;
	col.color = color;
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINES);
		glVertex2f(x1, SCREEN_HEIGHT_F-y1);
		glVertex2f(x2, SCREEN_HEIGHT_F-y2);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::Plot(float x, float y, PIXEL_TYPE color)
{
	glDisable(GL_TEXTURE_2D);
	JColor col;
	col.color = color;
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_POINTS);
		glVertex2f(x, SCREEN_HEIGHT_F-y);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::PlotArray(float *x, float *y, int count, PIXEL_TYPE color)
{
	glDisable(GL_TEXTURE_2D);
	JColor col;
	col.color = color;
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_POINTS);
		for (int i=0;i<count;i++)
			glVertex2f(x[i], SCREEN_HEIGHT_F-y[i]);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);
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
	if (mCurrentRenderMode == MODE_2D)
		return;

	mCurrentRenderMode = MODE_2D;

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	
	gluOrtho2D(0.0f, SCREEN_WIDTH_F-1.0f, 0.0f, SCREEN_HEIGHT_F-1.0f);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix

	glDisable (GL_DEPTH_TEST);
}


void JRenderer::SetClip(int x, int y, int width, int height)
{
	glScissor(x, y, width, height);
}


void JRenderer::LoadIdentity()
{
	glLoadIdentity();
}


void JRenderer::RenderTriangles(JTexture* texture, Vertex3D *vertices, int start, int count)
{
	if (texture)
		BindTexture(texture);

	glBegin(GL_TRIANGLES);
		int index = start*3;
		for (int i = 0; i < count; i++)
		{
			glTexCoord2f(vertices[index].u, vertices[index].v);
			//glNormal3f(vertices[index].nx, vertices[index].ny, vertices[index].nz);
			glVertex3f(vertices[index].x, vertices[index].y, vertices[index].z);

			index++;

			glTexCoord2f(vertices[index].u, vertices[index].v);
			//glNormal3f(vertices[index].nx, vertices[index].ny, vertices[index].nz);
			glVertex3f(vertices[index].x, vertices[index].y, vertices[index].z);

			index++;
			
			glTexCoord2f(vertices[index].u, vertices[index].v);
			//glNormal3f(vertices[index].nx, vertices[index].ny, vertices[index].nz);
			glVertex3f(vertices[index].x, vertices[index].y, vertices[index].z);

			index++;
	
		}
	glEnd();

}


void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color)
{
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_TRIANGLE_FAN);

	for(int i=0; i<count;i++)
	{
		glVertex2f(x[i],SCREEN_HEIGHT_F-y[i]);
	}

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::DrawPolygon(float* x, float* y, int count, PIXEL_TYPE color)
{
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINE_STRIP);

	for(int i=0; i<count;i++)
	{
		glVertex2f(x[i],SCREEN_HEIGHT_F-y[i]);
	}

	glVertex2f(x[0],SCREEN_HEIGHT_F-y[0]);

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
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
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINE_STRIP);

		for(int i=0; i<360;i+=2)
		{
			glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i));
		}

		glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0));

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}

void JRenderer::FillCircle(float x, float y, float radius, PIXEL_TYPE color)
{
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_TRIANGLE_FAN);

		glVertex2f(x, SCREEN_HEIGHT_F-y);

		for(int i=0; i<360;i+=2)
		{
			glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i));
		}

		glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0));

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::DrawPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color)
{
	JColor col;
	col.color = color;

	float angle = -startingAngle*RAD2DEG;
	float steps = 360.0f/count;
	size /= 2;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINE_LOOP);

		for(int i=0; i<count;i++)
		{
			glVertex2f(x+size*COSF((int)angle), SCREEN_HEIGHT_F-(y+size*SINF((int)angle)));

			angle += steps;
			if (angle >= 360.0f)
				angle -= 360.0f;
		}

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}


void JRenderer::FillPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color)
{
	JColor col;
	col.color = color;

	float angle = -startingAngle*RAD2DEG;
	float firstAngle = angle;
	float steps = 360.0f/count;
	size /= 2;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_TRIANGLE_FAN);

		glVertex2f(x, SCREEN_HEIGHT_F-y);

		for(int i=0; i<count;i++)
		{
			glVertex2f(x+size*COSF((int)angle), SCREEN_HEIGHT_F-y+size*SINF((int)angle));
			angle += steps;
			if (angle >= 360.0f)
				angle -= 360.0f;
		}

		glVertex2f(x+size*COSF((int)firstAngle), SCREEN_HEIGHT_F-y+size*SINF((int)firstAngle));


	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}



void JRenderer::DrawRoundRect(float x, float y, float w, float h, float radius, PIXEL_TYPE color)
{
	x+=w+radius;
	y+=h+radius;
	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_LINE_LOOP);
	int i;
	for(i=0; i<90;i++)
	{
		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-(y+radius*SINF(i)));
	}
	for(i=0; i<w; i++)
	{
		glVertex2f(x+radius*COSF(90)-i, SCREEN_HEIGHT_F-(y+radius*SINF(90)));
	}
	for(i=90; i<180;i++)
	{
		glVertex2f(x+radius*COSF(i)-w, SCREEN_HEIGHT_F-(y+radius*SINF(i)));
	}
	for(i=0; i<h; i++)
	{
		glVertex2f(x+radius*COSF(180)-w, SCREEN_HEIGHT_F-(y+radius*SINF(180)-i));
	}
	for(i=180; i<270;i++)
	{
		glVertex2f(x+radius*COSF(i)-w, SCREEN_HEIGHT_F-(y+radius*SINF(i)-h));
	}
	for(i=0; i<w; i++)
	{
		glVertex2f(x+radius*COSF(270)-w+i, SCREEN_HEIGHT_F-(y+radius*SINF(270)-h));
	}
	for(i=270; i<360;i++)
	{
		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-(y+radius*SINF(i)-h));
	}
	for(i=0; i<h; i++)
	{
		glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-(y+radius*SINF(0)-h+i));
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}



void JRenderer::FillRoundRect(float x, float y, float w, float h, float radius, PIXEL_TYPE color)
{
	x+=w+radius;
	y+=radius;

	JColor col;
	col.color = color;

	glDisable(GL_TEXTURE_2D);
	glColor4ub(col.r, col.g, col.b, col.a);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(x-5, SCREEN_HEIGHT_F-y);

	int i;
	for(i=0; i<90;i++)
	{
		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i));
	}
	for(i=0; i<w; i++)
	{
		glVertex2f(x+radius*COSF(90)-i, SCREEN_HEIGHT_F-y+radius*SINF(90));
	}
	for(i=90; i<180;i++)
	{
		glVertex2f(x+radius*COSF(i)-w, SCREEN_HEIGHT_F-y+radius*SINF(i));
	}
	for(i=0; i<h; i++)
	{
		glVertex2f(x+radius*COSF(180)-w, SCREEN_HEIGHT_F-y+radius*SINF(180)-i);
	}

	for(i=180; i<270;i++)
	{
		glVertex2f(x+radius*COSF(i)-w, SCREEN_HEIGHT_F-y+radius*SINF(i)-h);
	}
	for(i=0; i<w; i++)
	{
		glVertex2f(x+radius*COSF(270)-w+i, SCREEN_HEIGHT_F-y+radius*SINF(270)-h);
	}
	for(i=270; i<360;i++)
	{
		glVertex2f(x+radius*COSF(i), SCREEN_HEIGHT_F-y+radius*SINF(i)-h);
	}
	for(i=0; i<h; i++)
	{
		glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0)-h+i);
	}

	glVertex2f(x+radius*COSF(0), SCREEN_HEIGHT_F-y+radius*SINF(0));

	glEnd();

	glEnable(GL_TEXTURE_2D);

	// default color
	glColor4ub(255, 255, 255, 255);
}

