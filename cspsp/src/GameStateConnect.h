#ifndef _GAME_STATE_CONNECT_H_
#define _GAME_STATE_CONNECT_H_

#include <vector>
#include "JGui.h"
#include "GameState.h"
#include "Wlan.h"
#include "ListBox.h"

#define STAGE_CONNECTING 1
#define STAGE_LOGIN 2
#define STAGE_NEWACCOUNT 3
#define STAGE_NEWACCOUNTSUBMIT 4
#define STAGE_SUSPENDED 5
#define STAGE_MAINTENANCE 6
#define STAGE_ERROR 7
static char instructions[] =
"If you already have an account, please enter your name and password \
to sign in.\n\n\
Otherwise, you can create an account. Make sure to remember your password\
though (also, *DO NOT* use a password that you use for anything else)!";


#define ID_LEN 127

class ConnectionItem : public ListItem 
{
public:
	ConnectionConfig config;
	ConnectionItem(ConnectionConfig config) {
		this->config = config;
	}
	~ConnectionItem() {}

	void Render(float x, float y, bool selected) {
		gFont->DrawShadowedString(config.name,SCREEN_WIDTH_2,y+3,JGETEXT_CENTER);
	}
};


class GameStateConnect:	public GameState
{
private:
	//Socket* socket;
	char id[ID_LEN];
	char name[16];
	char password[16];
	char* tempname;
	char* temppassword;
	int index;

	int mStage;
	int mConnectState;
	int mConnectId;

	int mLoginStatus;
	std::vector<char*> mInstructions;

	int mSuspendHours;
	char mSuspendReason[64];

	char mConfigs[4][256];
	char mConfigInfo[4][256];
	float mInfoX;

	bool mArePluginsModified;
	bool mArePluginsLoaded;
	char mErrorString[64];

	void GenerateToken(char* token, int len);
	int StoreToken(char* token);
	int ReadToken(char* token, int len);



public:
	GameStateConnect(GameApp* parent);
	~GameStateConnect();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();

	int CheckLogin(char* buffer);
	int CheckNewAccount(char* buffer);
};


#endif

