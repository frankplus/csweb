#include <stdio.h>

#include "GameApp.h"

#include "GameStateLoading.h"
#include "GameStateMenu.h"
#include "GameStatePlay.h"
#include "GameStateNewGame.h"
#include "GameStateOptions.h"
// #include "GameStateUpdate.h"
#include "GameStateConnect.h"
#include "GameStateLobby.h"
#include "GameStateOnline.h"

JRenderer* GameApp::mRenderer = NULL;
//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameApp::GameApp()
{
	mRenderer = JRenderer::GetInstance();
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameApp::~GameApp()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Create()
{
	// The following will be called at 'loading' phase
	mGameStates[GAME_STATE_LOADING] = new GameStateLoading(this);
	mGameStates[GAME_STATE_MENU] = new GameStateMenu(this);
	mGameStates[GAME_STATE_PLAY] = new GameStatePlay(this);
	mGameStates[GAME_STATE_NEW_GAME] = new GameStateNewGame(this);
	mGameStates[GAME_STATE_OPTIONS] = new GameStateOptions(this);
	// mGameStates[GAME_STATE_UPDATE] = new GameStateUpdate(this);
	mGameStates[GAME_STATE_CONNECT] = new GameStateConnect(this);
	mGameStates[GAME_STATE_LOBBY] = new GameStateLobby(this);
	mGameStates[GAME_STATE_ONLINE] = new GameStateOnline(this);

	mCurrentState = NULL;
	mNextState = mGameStates[GAME_STATE_LOADING];
}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Destroy()
{
}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameApp::Update()
{
	JGE* engine = JGE::GetInstance();	
	
	float dt = engine->GetDelta()*1000;

	if (dt > 35.0f)		// min 30 FPS ;)
		dt = 35.0f;
	else if (dt < 1.0f)
		dt = 16.0f;		// avoid division by 0

	clock_t start_time = clock();

	if (mCurrentState != NULL)
		mCurrentState->Update(dt);

	if (mNextState != NULL)
	{
		if (mCurrentState != NULL)
			mCurrentState->End();

		mCurrentState = mNextState;
		mCurrentState->Start();

		mNextState = NULL;
	}

	// while (12.66666f-(clock()-start_time) > 0.0f) {
	// 	Sleep(12.66666f-(clock()-start_time));
	// }
}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameApp::Render()
{
	if (mCurrentState != NULL)
	{
		mCurrentState->Render();
	}
}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameApp::Pause()
{
	
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameApp::Resume()
{
	
}


void GameApp::LoadGameStates()
{
	mGameStates[GAME_STATE_MENU]->Create();
	mGameStates[GAME_STATE_NEW_GAME]->Create();
	mGameStates[GAME_STATE_PLAY]->Create();
	mGameStates[GAME_STATE_OPTIONS]->Create();
	// mGameStates[GAME_STATE_UPDATE]->Create();
	mGameStates[GAME_STATE_CONNECT]->Create();
	mGameStates[GAME_STATE_LOBBY]->Create();
	mGameStates[GAME_STATE_ONLINE]->Create();
}


void GameApp::SetNextState(int state)
{
	mNextState = mGameStates[state];
}