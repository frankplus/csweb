//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// Copyright (c) 2007 Sijiu Duan (a.k.a. Chi80) <sijiu49@gmail.com>
//
//-------------------------------------------------------------------------------------

#include <stdio.h>

#include "../include/JGBKFont.h"
#include "../include/JFileSystem.h"

JRenderer* JGBKFont::mRenderer = NULL;

JGBKFont::JGBKFont()
{
	mRenderer = JRenderer::GetInstance();

	mChnFont = NULL;
	mEngFont = NULL;

	mCurr = 0;

	mColor = ARGB(255,255,255,255);
	mBgColor = ARGB(0,0,0,0);

	mScale = 1.0f;

	mRotation = 0.0f;

	mCount = 0;
	mTexture = NULL;
	mSprites = NULL;
	mGBCode = NULL;

	mCharBuffer = NULL;
}



JGBKFont::~JGBKFont()
{
	SAFE_DELETE(mEngFont);

	SAFE_DELETE(mChnFont);

	SAFE_DELETE(mTexture);

	if (mSprites)
	{
		for (int i=0;i<mCacheSize;i++)
		{
			if (mSprites[i])
				delete mSprites[i];
		}

		delete [] mSprites;
	}

	if (mGBCode)
		delete [] mGBCode;

	if (mCharBuffer)
		delete[] mCharBuffer;


}


bool JGBKFont::Init(const char* engFileName, const char* chnFileName, int fontsize, bool smallEnglishFont)
{
	mSmallEnglishFont = smallEnglishFont;
	mFontSize = fontsize;

	mCacheImageWidth = 256;
	mCacheImageHeight = 256;

	mBytesPerRow = (mFontSize+7)/8;
	mBytesPerChar = mBytesPerRow*mFontSize;

	mCol = mCacheImageWidth/mFontSize;
	mRow = mCacheImageHeight/mFontSize;
	mCacheSize = mCol*mRow;

	mSprites = new JQuad*[mCacheSize];
	mGBCode = new int[mCacheSize];

	mCharBuffer = new DWORD[mFontSize*mFontSize];

	mTexture = mRenderer->CreateTexture(mCacheImageWidth, mCacheImageHeight, true);

	int index = 0;
	for (int y=0;y<mRow;y++)
	{
		for (int x=0;x<mCol;x++)
		{
			mGBCode[index] = -1;

			mSprites[index] = new JQuad(mTexture, x*mFontSize, y*mFontSize, mFontSize, mFontSize);
			mSprites[index]->SetHotSpot(mFontSize/2, mFontSize/2);
		
			index++;
		}
	}

	int size;

	JFileSystem *fileSys = JFileSystem::GetInstance();
	if (!fileSys->OpenFile(engFileName))
		return false;

	size = fileSys->GetFileSize();
	mEngFont = new BYTE[size];

	fileSys->ReadFile(mEngFont, size);
	fileSys->CloseFile();


	if (!fileSys->OpenFile(chnFileName))
		return false;

	size = fileSys->GetFileSize();

	mChnFont = new BYTE[size];

	fileSys->ReadFile(mChnFont, size);
	fileSys->CloseFile();

	return true;
}

int JGBKFont::PreCacheChar(const BYTE *ch)
{
	int code;
	BOOL isChinese = true;
	if (*ch > 0x80)
	{
		// get offset to the proper character bits (GBK encoding)
		code = (((DWORD)(*ch - 0x81)) * 0xBF + ((DWORD)(*(ch + 1) - 0x40)));
	}
	else
	{
		code = ((DWORD)*ch)|0x10000;
		isChinese = false;
	}
	
	if (mGBCode[mCurr] != -1)
	{
		
		for (int i=0;i<mCacheSize;i++)
		{
			if (mGBCode[i] == code)
				return i;
		}
	}
	
	mCount++;

	int index = mCurr++;
	if (mCurr >= mCacheSize)
		mCurr = 0;

	int x = 0;
	int y = 0;

	memset(mCharBuffer, 0, sizeof(DWORD)*mFontSize*mFontSize);


	BYTE* src;
	BYTE bits;
	int bitCount;

	if (isChinese)
	{
	
		src = mChnFont +  code * mBytesPerChar;
		for (int i=0;i<mBytesPerChar;)
		{

			bitCount = mFontSize;

			x = 0;

			for (int j=0;j<mBytesPerRow;j++)
			{
				bits = src[i++];
				BYTE bitMask = 0x80;
				for (int z=0;z<8&&bitCount;z++)
				{
					if ((bits & bitMask) != 0)
						mCharBuffer[y*mFontSize+x] = ARGB(255,255,255,255);
					else
						mCharBuffer[y*mFontSize+x] = ARGB(0,0,0,0);
					bitMask >>= 1;
					x++;

					bitCount--;
					
				}
				
			}
			y++;
		}
	}
	else
	{
		int size = (mFontSize <= 16)?mFontSize:mFontSize*2;
		src = mEngFont +  (code-0x10000) * size;

		int n;

		for (int i=0;i<size;)
		{
			x = 0;
			// width of the English fonts is only half of the Chinese ones
			// put char in the middle
			if (mFontSize <= 16)
			{
		
				for (n=0;n<(mFontSize-8)/2;n++)
				{
					mCharBuffer[y*mFontSize+x] = ARGB(0,0,0,0);
					x++;
				}
			}

			int count = (mFontSize <= 16)?1:2;
			bitCount = mFontSize;
			for (int k=0;k<count;k++)
			{
				bits = src[i++];
				BYTE bitMask = 0x80;
				for (int z=0;z<8&&bitCount;z++)
				{
					if ((bits & bitMask) != 0)
						mCharBuffer[y*mFontSize+x] = ARGB(255,255,255,255);
					else
						mCharBuffer[y*mFontSize+x] = ARGB(0,0,0,0);		
					x++;
					bitMask >>= 1;
					bitCount--;
				}
			}

			if (mFontSize <= 16)
			{
				
				for (n=0;n<(mFontSize-8)/2;n++)
				{
					mCharBuffer[y*mFontSize+x] = ARGB(0,0,0,0);
					x++;
				}
			}

			y++;

		}
	}
	
	mGBCode[index] = code;

	x = (int)mSprites[index]->mX;
	y = (int)mSprites[index]->mY;

	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, mFontSize, mFontSize, GL_RGBA, GL_UNSIGNED_BYTE, mCharBuffer);
	
	return index;
}


int JGBKFont::PrepareString(BYTE* str, int* dest)
{
	mRenderer->BindTexture(mTexture);

	BYTE* src = str;
	int count = 0;
	while (*src != 0)
	{
		if (*src > 0x80)
		{
			dest[count++] = PreCacheChar(src);
			src += 2;
		}
		else if (*src >= ' ')
		{
			dest[count++] = PreCacheChar(src);
			src += 1;
		}
		else	// control characters
		{
			dest[count++] = 0x80000 | *src;
			src += 1;
		}
	}

	return count;
}


void JGBKFont::RenderEncodedString(const int* text, int count, float x, float y)
{
	int n = 0;
	float xx = x;
	float yy = y;
	bool isChinese;

	for (int i=0;i<count;i++)
	{
		if (text[n] == 0x8000a)
		{
			xx = x;
			yy += (mFontSize*mScale);
		}
		else if (text[n] == 0x8000d)
		{
			
		}
		else
		{
			isChinese = ((text[n] & 0x10000) == 0x10000);
			mSprites[text[n]]->SetColor(mColor);
			mRenderer->RenderQuad(mSprites[text[n]], xx, yy, mRotation, mScale, mScale);
			if (mSmallEnglishFont && !isChinese)
				xx += (mFontSize*mScale)/2;
			else
				xx += (mFontSize*mScale);
			if (xx >= 480)
			{
				xx = x;
				yy += (mFontSize*mScale);
			}
		}
		n++;
	}
}
int JGBKFont::GetStringWidth( BYTE* str )
{
	int w=0;
	int h=0;
	GetStringArea(str, &w, &h);
	return w;
}

int JGBKFont::GetStringHeight( BYTE* str )
{
	int w=0;
	int h=0;
	GetStringArea(str, &w, &h);
	return h;
}
void JGBKFont::GetStringArea( BYTE* str,  int *w, int *h)
{
	BYTE* src = str;
	float len= 0;
	float xx = 0;
	float yy = mFontSize*mScale;
	bool isChinese=true;

	while (*src != 0)
	{
		if (yy + mFontSize < 0.0f)		// don't render when outside viewport
		{
			if (*src < ' ')				// control characters
			{
				if (*src == 0x0a)		// NEWLINE
				{
					if(xx>len)
						len=xx;
					xx = 0;
					yy += (mFontSize*mScale);
				}
				src += 1;
			}
			else 
			{
				if (*src > 0x80)
				{
					src += 2;
				}
				else if (*src >= ' ')
				{
					src += 1;
				}

				xx += (mFontSize*mScale);

				if (xx >= 480)
				{
					if(xx>len)
						len=xx;
					xx = 0;
					yy += (mFontSize*mScale);
				}
			}
		}
		else if (yy > SCREEN_HEIGHT_F)
		{
			break;
		}
		else 
		{

			if (*src < ' ')			// control characters
			{
				if (*src == 0x0a)	// NEWLINE
				{
					if(xx>len)
						len=xx;
					xx = 0;
					yy += (mFontSize*mScale);
				}
				src += 1;
			}
			else 
			{
				if (*src > 0x80)
				{
					//index = PreCacheChar(src);
					src += 2;
					if (isChinese==false)
						xx+=5*(mFontSize*mScale)/16;
					isChinese = true;
				}
				else if (*src >= ' ')
				{
					src += 1;
					if(isChinese==true)
						xx-=3*(mFontSize*mScale)/16;
					isChinese = false;
				}

				if (mSmallEnglishFont && !isChinese)
					xx += (mFontSize*mScale)/2;
				else
					xx += (mFontSize*mScale);

				if (xx >= 480)
				{
					if(xx>len)
						len=xx;
					xx = 0;
					yy += (mFontSize*mScale);
				}
			}
		}	
		
	}
	if (xx>len)
	{
		len=xx;
	}
	*w=(int)len;
	*h=(int)yy;

}

void JGBKFont::RenderString(BYTE* str, float x, float y, int alignment)
{
	int w=0;
	int h=0;
	
	switch(alignment)
	{
	case JGETEXT_RIGHT:
		GetStringArea(str,&w,&h);
		x-=w;
		break;
	case JGETEXT_CENTER:
		GetStringArea(str,&w,&h);
		x-=w/2;
		break;
	case JGETEXT_LEFT:
	default:
		break;
	}


	mRenderer->BindTexture(mTexture);

	BYTE* src = str;
	float xx = x;
	float yy = y;
	int index;
	
	bool isChinese=true;
	
	while (*src != 0)
	{
		if (yy + mFontSize < 0.0f)		// don't render when outside viewport
		{
			if (*src < ' ')				// control characters
			{
				if (*src == 0x0a)		// NEWLINE
				{
					xx = x;
					yy += (mFontSize*mScale);
				}
				src += 1;
			}
			else 
			{
				if (*src > 0x80)
				{
					src += 2;
				}
				else if (*src >= ' ')
				{
					src += 1;
				}

				xx += (mFontSize*mScale);

				if (xx >= 480)
				{
					xx = x;
					yy += (mFontSize*mScale);
				}
			}
		}
		else if (yy > SCREEN_HEIGHT_F)
		{
			return;
		}
		else 
		{

			if (*src < ' ')			// control characters
			{
				if (*src == 0x0a)	// NEWLINE
				{
					xx = x;
					yy += (mFontSize*mScale);
				}
				src += 1;
			}
			else 
			{
				if (*src > 0x80)
				{
					index = PreCacheChar(src);
					src += 2;
					if (isChinese==false)
						xx+=5*(mFontSize*mScale)/16;
					isChinese = true;
				}
				else if (*src >= ' ')
				{
					index = PreCacheChar(src);
					src += 1;
					if(isChinese==true)
						xx-=3*(mFontSize*mScale)/16;
					isChinese = false;
				}
				
				mSprites[index]->SetColor(mColor);
				mRenderer->RenderQuad(mSprites[index], xx, yy, mRotation, mScale, mScale);
				if (mSmallEnglishFont && !isChinese)
					xx += (mFontSize*mScale)/2;
				else
					xx += (mFontSize*mScale);

				if (xx >= 480)
				{
					xx = x;
					yy += (mFontSize*mScale);
				}
			}
		}
	}

}

void JGBKFont::SetScale(float scale) { mScale = scale; }
void JGBKFont::SetRotation(float rot) { mRotation = rot; }
void JGBKFont::SetColor(PIXEL_TYPE color) { mColor = color; }
void JGBKFont::SetBgColor(PIXEL_TYPE color) { mBgColor = color; }

