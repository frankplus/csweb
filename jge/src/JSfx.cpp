#include "../include/JSoundSystem.h"

JMusic::JMusic()
{
	mTrack = NULL;
}

JMusic::~JMusic()
{
	JSoundSystem::GetInstance()->StopMusic(this);

	if (mTrack)
		delete mTrack;
}

JSample::JSample()
{
	mSample = NULL;
	mVoice = -1;
	mVolume = 255;
	mPanning = 127;
}


JSample::~JSample()
{

}

JSoundSystem* JSoundSystem::mInstance = NULL;

JSoundSystem* JSoundSystem::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JSoundSystem();
		mInstance->InitSoundSystem();
	}
	
	return mInstance;
}


void JSoundSystem::Destroy()
{
	if (mInstance)
	{
		mInstance->DestroySoundSystem();
		delete mInstance;
		mInstance = NULL;
	}
}


JSoundSystem::JSoundSystem()
{

}


JSoundSystem::~JSoundSystem()
{

}


void JSoundSystem::InitSoundSystem()
{

}


void JSoundSystem::DestroySoundSystem()
{

}


JMusic *JSoundSystem::LoadMusic(const char *fileName)
{

}

JSample *JSoundSystem::LoadSample(const char *fileName)
{
	
}


void JSoundSystem::PlayMusic(JMusic *music, bool looping)
{

}


void JSoundSystem::PlaySample(JSample *sample)
{

}

void JSoundSystem::StopSample(int voice)
{

}

void JSoundSystem::SetVolume(int volume)
{

}


void JSoundSystem::StopMusic(JMusic *music)
{

}


void JSoundSystem::ResumeMusic(JMusic *music)
{

}

