#include "../include/JSoundSystem.h"

JMusic::JMusic()
{

}

JMusic::~JMusic()
{
	JSoundSystem::GetInstance()->StopMusic(this);

}

JSample::JSample()
{
	mVoice = -1;
	mVolume = 255;
	mPanning = 127;
}


JSample::~JSample()
{
	Mix_FreeChunk( mSample );
	mSample = NULL;
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
	if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 512 ) < 0 )
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
}


void JSoundSystem::DestroySoundSystem()
{
	Mix_CloseAudio();
}


JMusic *JSoundSystem::LoadMusic(const char *fileName)
{
	return new JMusic();
}

JSample *JSoundSystem::LoadSample(const char *fileName)
{
	JSample *sample = new JSample();
	sample->mSample = Mix_LoadWAV( fileName );
	if( sample->mSample == NULL )
		printf( "Failed to load %s! SDL_mixer Error: %s\n", fileName, Mix_GetError() );

	return sample;
}


void JSoundSystem::PlayMusic(JMusic *music, bool looping)
{

}


void JSoundSystem::PlaySample(JSample *sample)
{
	int chan = Mix_PlayChannel( -1, sample->mSample, 0 );
	// if(chan == -1)
	// 	printf("failed to play sound sample \n");
	
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

