//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include "../include/JGE.h"
#include "../include/JGui.h"

JGE* JGuiObject::mEngine = NULL;

JGE* JGuiController::mEngine = NULL;


JGuiObject::JGuiObject(int id, bool hasFocus): mId(id), mHasFocus(hasFocus)
{ 
	mEngine = JGE::GetInstance(); 
}


JGuiObject::~JGuiObject() 
{ 
};


bool JGuiObject::Leaving(u32 key) 
{
	return true; 
}


void JGuiObject::Entering()
{
	
}


int JGuiObject::GetId()
{
	return mId;
}

bool JGuiObject::HasFocus()
{
	return mHasFocus;
}

void JGuiObject::Update(float dt) 
{
}

JGuiController::JGuiController(int id, JGuiListener* listener, int direction) : mId(id), mListener(listener)
{
	mEngine = JGE::GetInstance();
	
	mBg = NULL;
	mShadingBg = NULL;

	mCount = 0;
	mCurr = 0;
	mDefault = 0;

	mActionButton = CTRL_CROSS;
	mLastKey = 0;

	mWrapping = true;
	mDirection = direction;

	mActive = true;
}


JGuiController::~JGuiController()
{
	for (int i=0;i<mCount;i++)
		if (mObjects[i]!=NULL)
			delete mObjects[i];
}


void JGuiController::Render(float x, float y)
{
	for (int i=0;i<mCount;i++)
		if (mObjects[i]!=NULL)
			mObjects[i]->Render(x, y);
}


bool JGuiController::KeyRepeated(u32 key, float dt)
{

	bool doKey = false;
	if (mLastKey != CTRL_UP)
	{
		mLastKey = CTRL_UP;
		doKey = true;
		mKeyRepeatDelay = JGUI_INITIAL_DELAY;
	}
	else
	{
		mKeyRepeatDelay -= dt;
		if (mKeyRepeatDelay <= 0.0f)
		{
			mKeyRepeatDelay = JGUI_REPEAT_DELAY;
			doKey = true;
		}
	}

	return doKey;
}


void JGuiController::Update(float dt)
{
	for (int i=0;i<mCount;i++)
		if (mObjects[i]!=NULL)
			mObjects[i]->Update(dt);

	if (mEngine->GetButtonClick(mActionButton))
	{
		if (mObjects[mCurr] != NULL)
		{
			if (mListener != NULL)
			{
				mListener->ButtonPressed(mId, mObjects[mCurr]->GetId());
				return;
			}
		}
	}
	
	if (mDirection == JGUI_STYLE_LEFTRIGHT) {
		if (mEngine->GetButtonState(CTRL_LEFT) || mEngine->GetAnalogX()<64)
		{
			if (KeyRepeated(CTRL_UP, dt))
			{
				int n = mCurr;
				n--;
				if (n<0)
				{
					if (mWrapping)
						n = mCount-1;
					else
						n = 0;
				}

				if (n != mCurr && mObjects[mCurr] != NULL && mObjects[mCurr]->Leaving(CTRL_UP))
				{
					mCurr = n;
					mObjects[mCurr]->Entering();
				}
			}
		}
		else if (mEngine->GetButtonState(CTRL_RIGHT) || mEngine->GetAnalogX()>192)
		{
			if (KeyRepeated(CTRL_DOWN, dt))
			{
				int n = mCurr;
				n++;
				if (n>mCount-1)
				{
					if (mWrapping)
						n = 0;
					else
						n = mCount-1;
				}

				if (n != mCurr && mObjects[mCurr] != NULL && mObjects[mCurr]->Leaving(CTRL_DOWN))
				{
					mCurr = n;
					mObjects[mCurr]->Entering();
				}
			}
		}
		else
			mLastKey = 0;
	}
	else if (mDirection == JGUI_STYLE_UPDOWN) {
		if (mEngine->GetButtonState(CTRL_UP) || mEngine->GetAnalogY()<64)
		{
			if (KeyRepeated(CTRL_UP, dt))
			{
				int n = mCurr;
				n--;
				if (n<0)
				{
					if (mWrapping)
						n = mCount-1;
					else
						n = 0;
				}

				if (n != mCurr && mObjects[mCurr] != NULL && mObjects[mCurr]->Leaving(CTRL_UP))
				{
					mCurr = n;
					mObjects[mCurr]->Entering();
				}
			}
		}
		else if (mEngine->GetButtonState(CTRL_DOWN) || mEngine->GetAnalogY()>192)
		{
			if (KeyRepeated(CTRL_DOWN, dt))
			{
				int n = mCurr;
				n++;
				if (n>mCount-1)
				{
					if (mWrapping)
						n = 0;
					else
						n = mCount-1;
				}

				if (n != mCurr && mObjects[mCurr] != NULL && mObjects[mCurr]->Leaving(CTRL_DOWN))
				{
					mCurr = n;
					mObjects[mCurr]->Entering();
				}
			}
		}
		else
			mLastKey = 0;
	}
}


void JGuiController::Add(JGuiObject* ctrl)
{
	if (mCount<MAX_GUIOBJECT)
	{
		mObjects[mCount++] = ctrl;
		if (ctrl->HasFocus())
			mDefault = mCount;
	}
}


void JGuiController::Remove(int id)
{
	for (int i=0;i<mCount;i++)
	{
		if (mObjects[i] != NULL && mObjects[i]->GetId()==id)
		{
			delete mObjects[i];
			
			for (int j=i;j<mCount-1;j++)
			{
				mObjects[j] = mObjects[j+1];
			}
			mCount--;
			return;
		}
	}
}

void JGuiController::SetActionButton(u32 button) { mActionButton = button; }
void JGuiController::SetWrapping(bool flag) { mWrapping = flag; }
bool JGuiController::IsActive() { return mActive; }
void JGuiController::SetActive(bool flag) { mActive = flag; }


void JGuiController::Reset() { 
	SetCurr(mDefault);
}

void JGuiController::SetCurr(int curr) { 
	for (int i=0;i<mCount;i++)
	{
		if (mObjects[i] != NULL) {
			mObjects[i]->Leaving(CTRL_UP);
		}
	}
	mObjects[curr]->Entering();
	mCurr = curr;
}

int JGuiController::GetCurr() { 
	return mCurr;
}

int JGuiController::GetCount() {
	return mCount;
}

JGuiObject* JGuiController::GetGuiObject(int index) {
	if (index < mCount) {
		return mObjects[index];
	}
	else {
		return NULL;
	}
}