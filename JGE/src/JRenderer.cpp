#include "../include/JGE.h"
#include "../include/JRenderer.h"
#include "../include/JResourceManager.h"
#include "../include/JFileSystem.h"
#include "../include/JAssert.h"
#include <png.h>

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
bool JRenderer::m3DEnabled = false;

void JRenderer::Set3DFlag(bool flag) { m3DEnabled = flag; }

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
	mImageFilter = NULL;

	mCurrTexBlendSrc = BLEND_SRC_ALPHA;
	mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

//	mLineWidth = 1.0f;
	mCurrentTex = -1;
	mFOV = 75.0f;

#ifdef USING_MATH_TABLE
	for (int i=0;i<360;i++)
	{
		mSinTable[i] = sinf(i*DEG2RAD);
		mCosTable[i] = cosf(i*DEG2RAD);
	}
#endif

	mCurrentRenderMode = MODE_UNKNOWN;
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

	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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



void JRenderer::ScreenShot(const char* filename)
{

}


static int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

static void PNGCustomWarningFn(png_structp png_ptr, png_const_charp warning_msg)
{
        // ignore PNG warnings
}


static void PNGCustomReadDataFn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_voidp io_ptr = png_get_io_ptr(png_ptr);

	JFileSystem &fileSystem = *(JFileSystem *)io_ptr;
	const size_t bytesRead = fileSystem.ReadFile(
		(unsigned char *)data,
		(size_t)length);

	if ((png_size_t)bytesRead != length)
		png_error(png_ptr, "Read Error!");
}


JTexture* JRenderer::LoadTexture(const char* filename, int mode)
{
	TextureInfo textureInfo;
	
	textureInfo.mBits = NULL;
	
	LoadPNG(textureInfo, filename);

	if (textureInfo.mBits == NULL)
		return NULL;

	bool ret = false;

	JTexture *tex = new JTexture();

 	if (tex)
	{
		if (mImageFilter != NULL)
			mImageFilter->ProcessImage((PIXEL_TYPE*)textureInfo.mBits, textureInfo.mWidth, textureInfo.mHeight);

		tex->mFilter = TEX_FILTER_LINEAR;
		tex->mWidth = textureInfo.mWidth;
		tex->mHeight = textureInfo.mHeight;
		tex->mTexWidth = textureInfo.mTexWidth;
		tex->mTexHeight = textureInfo.mTexHeight;

		GLuint texid; 
		glGenTextures(1, &texid);
		tex->mTexId = texid;

		if (texid != 0)
		{

			// OpenGL texture has (0,0) at lower-left
			// Pay attention when doing texture mapping!!!

			glBindTexture(GL_TEXTURE_2D, texid);								// Bind To The Texture ID


			if (mode == TEX_TYPE_MIPMAP)			// generate mipmaps
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);	
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, textureInfo.mTexWidth, textureInfo.mTexHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureInfo.mBits);
			}
			else if (mode == TEX_TYPE_SKYBOX)		// for skybox
			{
#define GL_CLAMP_TO_EDGE	0x812F	

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, textureInfo.mTexWidth, textureInfo.mTexHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureInfo.mBits);
			}
			else									// single texture
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureInfo.mTexWidth, textureInfo.mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureInfo.mBits);
			}

			ret = true;

		}


	}

	delete [] textureInfo.mBits;
	//delete textureInfo;

	if (!ret)
	{
		if (tex)
			delete tex;
		tex = NULL;
	}

	return tex;
}

void JRenderer::LoadPNG(TextureInfo &textureInfo, const char *filename, int mode)
{
	//TextureInfo* textureInfo = new TextureInfo;

	//if (textureInfo == NULL) return NULL;

	textureInfo.mBits = NULL;

	bool ret = false;

	DWORD* p32;

    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    png_uint_32 width, height, tw, th;
    int bit_depth, color_type, interlace_type, x, y;
    DWORD* line;
    //FILE *fp;


    //if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	JFileSystem* fileSystem = JFileSystem::GetInstance();
	if (!fileSystem->OpenFile(filename)) return;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) 
	{
        //fclose(fp);
		fileSystem->CloseFile();

        return;
    }

    png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, PNGCustomWarningFn);
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) 
	{
        //fclose(fp);
		fileSystem->CloseFile();

        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);

        return;
    }
    png_init_io(png_ptr, NULL);
	png_set_read_fn(png_ptr, (png_voidp)fileSystem, PNGCustomReadDataFn);

    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, (int*)NULL, (int*)NULL);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    
	line = (DWORD*) malloc(width * 4);
    if (!line) 
	{
        //fclose(fp);
		fileSystem->CloseFile();
		
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return;
    }
   

	tw = getNextPower2(width);
	th = getNextPower2(height);
		
	int size = tw * th * 4;			// RGBA

	u8* buffer = new u8[size];

	//JTexture *tex = new JTexture();

	if (buffer)
	{
// 		tex->mFilter = TEX_FILTER_LINEAR;
// 		tex->mWidth = width;
// 		tex->mHeight = height;
// 		tex->mTexWidth = tw;
// 		tex->mTexHeight = th;

// 		GLuint texid; 
// 		glGenTextures(1, &texid);
// 		tex->mTexId = texid;

//		if (texid != 0)
		{
		
			// OpenGL texture has (0,0) at lower-left
			// Pay attention when doing texture mapping!!!

			p32 = (DWORD*) buffer;// + (height-1)*width;	

			for (y = 0; y < (int)height; y++) 
			{
				png_read_row(png_ptr, (u8*) line, (png_bytep)NULL);
				for (x = 0; x < (int)width; x++) 
				{
					DWORD color32 = line[x];
					//u16 color16;
					int a = (color32 >> 24) & 0xff;
					int r = color32 & 0xff;
					int g = (color32 >> 8) & 0xff;
					int b = (color32 >> 16) & 0xff;

					color32 = r | (g << 8) | (b << 16) | (a << 24);
					*(p32+x) = color32;
                
				}
				p32 += tw;
				
			}			
		}
	}

    free (line);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	
    //fclose(fp);
	fileSystem->CloseFile();

	textureInfo.mBits = buffer;
	textureInfo.mWidth = width;
	textureInfo.mHeight = height;
	textureInfo.mTexWidth = tw;
	textureInfo.mTexHeight = th;

	//return textureInfo;

}


JTexture* JRenderer::CreateTexture(int width, int height, int mode )
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


void JRenderer::EnableVSync(bool flag)
{
//	if (flag)
//		hge->System_SetState(HGE_FPS, 60);	// HGEFPS_VSYNC
//	else
//		hge->System_SetState(HGE_FPS, HGEFPS_UNLIMITED);
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


void JRenderer::ResetPrivateVRAM()
{

}


void JRenderer::Enable2D()
{
	if (mCurrentRenderMode == MODE_2D)
		return;

	mCurrentRenderMode = MODE_2D;

	glViewport (0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);	// Reset The Current Viewport
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	
	gluOrtho2D(0.0f, SCREEN_WIDTH_F-1.0f, 0.0f, SCREEN_HEIGHT_F-1.0f);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix

	glDisable (GL_DEPTH_TEST);
}


void JRenderer::Enable3D()
{
	if (!m3DEnabled)
		return;

	if (mCurrentRenderMode == MODE_3D)
		return;

	mCurrentRenderMode = MODE_3D;

	glViewport (0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);		// Reset The Current Viewport
	glMatrixMode (GL_PROJECTION);												// Select The Projection Matrix
	glLoadIdentity ();															// Reset The Projection Matrix
	gluPerspective (mFOV, (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,	// Calculate The Aspect Ratio Of The Window
					0.5f, 1000.0f);		
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix

	glEnable (GL_DEPTH_TEST);
}


void JRenderer::SetClip(int x, int y, int width, int height)
{
	glScissor(x, y, width, height);
}


void JRenderer::LoadIdentity()
{
	glLoadIdentity();
}


void JRenderer::Translate(float x, float y, float z)
{
	glTranslatef(x, y, z);
}


void JRenderer::RotateX(float angle)
{
	glRotatef(angle*RAD2DEG, 1.0f, 0.0f, 0.0f);
}


void JRenderer::RotateY(float angle)
{
	glRotatef(angle*RAD2DEG, 0.0f, 1.0f, 0.0f);
}


void JRenderer::RotateZ(float angle)
{
	glRotatef(angle*RAD2DEG, 0.0f, 0.0f, 1.0f);
}


void JRenderer::PushMatrix()
{
	glPushMatrix();
}


void JRenderer::PopMatrix()
{
	glPopMatrix();
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


void JRenderer::SetFOV(float fov)
{
	mFOV = fov;
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


void JRenderer::SetImageFilter(JImageFilter* imageFilter)
{
	mImageFilter = imageFilter;
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

