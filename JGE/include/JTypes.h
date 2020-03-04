//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#ifndef _JTYPES_H
#define _JTYPES_H

#define MAX_CHANNEL		128


#ifndef M_PI
#define M_PI	3.14159265358979323846f
#define M_PI_2	1.57079632679489661923f
#define M_PI_4	0.785398163397448309616f
#define M_1_PI	0.318309886183790671538f
#define M_2_PI	0.636619772367581343076f
#endif

#define RAD2DEG		57.29577951f
#define DEG2RAD		0.017453293f

#define SAFE_DELETE(x)			if (x) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x)	if (x) { delete [] x; x = NULL; }

// Screen dimensions
#define SCREEN_WIDTH 			960 
#define SCREEN_HEIGHT 			544 
// Screen resolution 
#define SCREEN_WIDTH_F 			480.0f
#define SCREEN_HEIGHT_F			272.0f
// Half of screen resolution 
#define SCREEN_WIDTH_2			240.0f
#define SCREEN_HEIGHT_2			136.0f


#include <GL/glew.h>

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned int DWORD;


#define BLEND_ZERO					GL_ZERO
#define BLEND_ONE					GL_ONE
#define BLEND_SRC_COLOR				GL_SRC_COLOR
#define BLEND_ONE_MINUS_SRC_COLOR	GL_ONE_MINUS_SRC_COLOR
#define BLEND_SRC_ALPHA				GL_SRC_ALPHA
#define BLEND_ONE_MINUS_SRC_ALPHA	GL_ONE_MINUS_SRC_ALPHA
#define BLEND_DST_ALPHA				GL_DST_ALPHA
#define BLEND_ONE_MINUS_DST_ALPHA	GL_ONE_MINUS_DST_ALPHA
#define BLEND_DST_COLOR				GL_DST_COLOR
#define BLEND_ONE_MINUS_DST_COLOR	GL_ONE_MINUS_DST_COLOR
#define BLEND_SRC_ALPHA_SATURATE	GL_SRC_ALPHA_SATURATE

#define ARGB(a, r, g, b)		((a << 24) | (r << 16) | (g << 8) | b)
#define RGBA(r, g, b, a)		((a << 24) | (b << 16) | (g << 8) | r)

enum CtrlButtons
{
	CTRL_SELECT     = 0x000001,
	CTRL_START      = 0x000002,
	CTRL_UP         = 0x000004,
	CTRL_RIGHT      = 0x000008,
	CTRL_DOWN      	= 0x000010,
	CTRL_LEFT      	= 0x000020,
	CTRL_LTRIGGER   = 0x000040,
	CTRL_RTRIGGER   = 0x000080,
	CTRL_TRIANGLE   = 0x000100,
	CTRL_CIRCLE     = 0x000200,
	CTRL_CROSS      = 0x000400,
	CTRL_SQUARE     = 0x000800,
	CTRL_HOME       = 0x001000,
	CTRL_HOLD       = 0x002000,
	CTRL_NOTE       = 0x004000,
	CTRL_W			= 0x008000,
	CTRL_A			= 0x010000,
	CTRL_S			= 0x020000,
	CTRL_D			= 0x040000
};


#define PIXEL_TYPE				DWORD


//------------------------------------------------------------------------------------------------
struct Vertex
{
	float u, v;
	PIXEL_TYPE color;
	float x, y, z;
};

//------------------------------------------------------------------------------------------------
struct Vertex3D
{
	float u, v;
	//float nx, ny, nz;
	float x, y, z;
};


//------------------------------------------------------------------------------------------------
struct VertexColor
{
	PIXEL_TYPE color;
	float x, y, z;
};


struct JColor
{
	union
	{
		struct
		{
			u8 b;
			u8 g;
			u8 r;
			u8 a;
		};
		DWORD color;
	};
};



enum
{
	TEX_TYPE_NONE,
	TEX_TYPE_USE_VRAM,
	TEX_TYPE_MIPMAP,
	TEX_TYPE_NORMAL,
	TEX_TYPE_SKYBOX
};


enum
{
	MODE_UNKNOWN,
	MODE_2D,
	MODE_3D
};


enum
{
	TEX_FILTER_NONE,
	TEX_FILTER_LINEAR,
	TEX_FILTER_NEAREST
};

//------------------------------------------------------------------------------------------------
class JTexture
{
public:
	JTexture();
	~JTexture();

	int mWidth;
	int mHeight;
	int mTexWidth;
	int mTexHeight;
	int mFilter;
	GLuint mTexId;
};


//////////////////////////////////////////////////////////////////////////
/// Image quad.
/// 
//////////////////////////////////////////////////////////////////////////
class JQuad
{
public:

	//////////////////////////////////////////////////////////////////////////
	/// Constructor.
	/// 
	/// @param tex - Texture of the quad.
	/// @param x - X position of the quad in texture.
	/// @param y - Y position of the quad in texture.
	/// @param width - Width of the quad.
	/// @param height - Height of the quad.
	/// 
	//////////////////////////////////////////////////////////////////////////
	JQuad(JTexture *tex, float x, float y, float width, float height);

	//////////////////////////////////////////////////////////////////////////
	/// Set blending color of the quad.
	/// 
	/// @param color - Color.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetColor(PIXEL_TYPE color);

	//////////////////////////////////////////////////////////////////////////
	/// Set anchor point of the quad.
	/// 
	/// @param x - X position of the anchor point.
	/// @param y - Y position of the anchor point.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetHotSpot(float x, float y);
	
	//////////////////////////////////////////////////////////////////////////
	/// Set UV positions of the quad.
	/// 
	/// @param x - X position of the quad in texture.
	/// @param y - Y position of the quad in texture.
	/// @param w - Width of the quad.
	/// @param h - Height of the quad.
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetTextureRect(float x, float y, float w, float h);

	//////////////////////////////////////////////////////////////////////////
	/// Set horizontal flipping.
	/// 
	/// @param hflip - flipping flag;
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetHFlip(bool hflip) { mHFlipped = hflip; }

	//////////////////////////////////////////////////////////////////////////
	/// Set vetical flipping.
	/// 
	/// @param hflip - flipping flag;
	/// 
	//////////////////////////////////////////////////////////////////////////
	void SetVFlip(bool vflip) { mVFlipped = vflip; }

	JTexture* mTex;
	
	JColor mColor;		// up to 4 vertices

	float mX; // X position of the quad in texture.
	float mY; // Y position of the quad in texture.
	float mWidth; // Width of the quad.
	float mHeight; // height of the quad.
	float mHotSpotX; // X pos of anchor point of the quad.
	float mHotSpotY; // Y pos of anchor point of the quad.

	bool mHFlipped; // horizontally flipped
	bool mVFlipped; // vertically flipped
};


//////////////////////////////////////////////////////////////////////////
/// \enum JFONT_TEXT_ALIGNMENT
/// 
/// Font alignment.
/// 
//////////////////////////////////////////////////////////////////////////
enum JFONT_TEXT_ALIGNMENT
{
	JGETEXT_LEFT,		///< Text alignment to left.
	JGETEXT_CENTER,		///< Text alignment to center.
	JGETEXT_RIGHT		///< Text alignment to right.
};


enum JINIT_FLAG
{
	JINIT_FLAG_NORMAL,
	JINIT_FLAG_ENABLE3D
};


//------------------------------------------------------------------------------------------------
class JFont
{
public:
	JQuad* mQuad;
	int mWidth;
	int mHeight;
	int mSpacing;
	int mAlign;
	float mScale;
};


//------------------------------------------------------------------------------------------------
class Rect
{
public:
	int x;
	int y;
	int width;
	int height;

public:
	Rect(int _x, int _y, int _width, int _height): x(_x), y(_y), width(_width), height(_height) {}

};

#endif