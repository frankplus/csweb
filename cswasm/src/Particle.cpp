#include "Particle.h"

JRenderer* Particle::mRenderer = NULL;

Particle::Particle()//JQuad* texture, float x, float y)
{
	mRenderer = JRenderer::GetInstance();
	
	//mTexture = texture;
	mPos = Vector2D(0.0f, 0.0f);
	mActive = false;
	mQuad = NULL;
}
	

Particle::~Particle()
{
}


bool Particle::Update(float dt)
{
	mPos.x += mSpeed * cosf(mAngle) * dt;
	mPos.y += mSpeed * sinf(mAngle) * dt;

	mScale += mScaleDelta*dt;
	if (mScaleDelta<0.0f)
	{
		if (mScale < mScaleEnd)
			mScale = mScaleEnd;
	}
	else
	{
		if (mScale > mScaleEnd)
			mScale = mScaleEnd;
	}

	mAlpha += mAlphaDelta*dt;

	if (mAlphaDelta<0.0f)
	{
		if (mAlpha < mAlphaEnd)
			mActive = false;
	}
	else
	{
		if (mAlpha > mAlphaEnd)
			mActive = false;
	}
	
	
	for (int i=0;i<3;i++)
	{
		mColor[i] += mColorDelta[i]*dt;

		if ((int)mColor[i] == (int)mColorEnd[i])
			mColorDelta[i] = 0.0f;
	}

	mLifetime -= dt;
	if (mLifetime<0.0f)
	{
		mActive = false;
		return false;
	}
		
	return true;
}


void Particle::Render(float x, float y)
{
//	t2d.LoadIdentity();
//	t2d.Scale(scale, scale);
//	t2d.RotateRad(rotation);
//	
//	g->SetColor(Color((int)color.x, (int)color.y, (int)color.z, (int)alpha));
//	g->DrawImageMatrix(texture, t2d, pos.x, pos.y);
	//int a = 
	//if (mQuad)
	{
		PIXEL_TYPE color = ARGB((int)mAlpha, (int)mColor[0], (int)mColor[1], (int)mColor[2]);
		mQuad->SetColor(color);
		mRenderer->RenderQuad(mQuad, mPos.x-(x-SCREEN_WIDTH_2), mPos.y-(y-SCREEN_HEIGHT_2), 0.0f, mScale, mScale);
	}
}


void Particle::SetPosition(float x, float y) { mPos = Vector2D(x, y); }
void Particle::SetQuad(JQuad *quad) { mQuad = quad; }


//void Particle::ResetVelocity()
//{
//	float xx =  mSpeed * cosf(mAngle);
//	float yy =  mSpeed * sinf(mAngle);
//	mVelocity = Vector2D(xx, yy);
//}

