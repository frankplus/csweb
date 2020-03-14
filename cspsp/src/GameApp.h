#ifndef _GAMEAPP_H_
#define _GAMEAPP_H_

#include <JApp.h>
#include "JGE.h"
#include "JRenderer.h"
#include "JLBFont.h"
#include "GameState.h"
#include "Globals.h"

#include <ctime>

class GameState;

#define VERSION 1.92f
#define NETVERSION 9

#define MAX_STATE			10

class GameApp:	public JApp
{
private:
	static JRenderer* mRenderer;

	GameState* mCurrentState;
	GameState* mNextState;
	GameState* mGameStates[MAX_STATE];

public:
	GameApp();
	virtual ~GameApp();
	virtual void Create();
	virtual void Destroy();
	virtual void Update();
	virtual void Render();
	virtual void Pause();
	virtual void Resume();

	void LoadGameStates();
	void SetNextState(int state);
};

#endif
