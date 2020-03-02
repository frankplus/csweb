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
#include "../include/JApp.h"
#include "../include/JRenderer.h"
#include "../include/JSoundSystem.h"
#include "../include/Vector2D.h"
#include "../include/JFileSystem.h"


JGE::JGE()
{
	mApp = NULL;
	
	strcpy(mDebuggingMsg, "");
	mCurrentMusic = NULL;
	Init();
}


JGE::~JGE()
{
	JRenderer::Destroy();
	JFileSystem::Destroy();
	JSoundSystem::Destroy();
}


	
void JGE::Init()
{
	mDone = false;
	mPaused = false;
	mCriticalAssert = false;
	mClicked = false;
	
	JRenderer::GetInstance();
	JFileSystem::GetInstance();
	JSoundSystem::GetInstance();
}

void JGE::Run()
{
	
}

void JGE::SetDelta(int delta)
{
	mDeltaTime = (float)delta / 1000.0f;		// change to second
}


float JGE::GetDelta()
{
	return mDeltaTime;
	//return hge->Timer_GetDelta()*1000;
}


float JGE::GetFPS()
{
	//return (float)hge->Timer_GetFPS();
	return 1.0f/mDeltaTime;
}


bool JGE::GetButtonState(u32 button)
{
	//return (gButtons&button)==button;
	return JGEGetButtonState(button);
}


bool JGE::GetButtonClick(u32 button)
{
	if (mClicked) return false;
	bool click = JGEGetButtonClick(button);
	if (click) {
		mClicked = true;
	}
	return click;
}


u8 JGE::GetAnalogX()
{
	if (JGEGetButtonState(CTRL_A)) return 0;
	if (JGEGetButtonState(CTRL_D)) return 0xff;

	return 0x80;
}


u8 JGE::GetAnalogY()
{
	if (JGEGetButtonState(CTRL_W)) return 0;
	if (JGEGetButtonState(CTRL_S)) return 0xff;

	return 0x80;
}

JGE* JGE::mInstance = NULL;
//static int gCount = 0;

JGE* JGE::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JGE();
	}
	
	//gCount++;
	return mInstance;
}


void JGE::Destroy()
{
	//gCount--;
	if (mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}


void JGE::SetApp(JApp *app)
{
	mApp = app;
}


void JGE::Update()
{
	if (mApp != NULL)
		mApp->Update();
}

void JGE::Render()
{
	JRenderer* renderer = JRenderer::GetInstance();

	renderer->BeginScene();

	if (mApp != NULL)
		mApp->Render();

	renderer->EndScene();
}


void JGE::End()
{
	mDone = true;
}



void JGE::printf(const char *format, ...)
{
	va_list list;
	
	va_start(list, format);
	vsprintf(mDebuggingMsg, format, list);
	va_end(list);
}


void JGE::Pause()
{
	if (mPaused) return;

	mPaused = true;
	if (mApp != NULL)
		mApp->Pause();
}


void JGE::Resume()
{
	if (mPaused)
	{
		mPaused = false;
		if (mApp != NULL)
			mApp->Resume();
	}
}


void JGE::Assert(const char *filename, long lineNumber)
{
	mAssertFile = filename;
	mAssertLine = lineNumber;
	mCriticalAssert = true;


}
