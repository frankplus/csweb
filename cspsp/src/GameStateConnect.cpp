#include "GameStateConnect.h"

#undef T // due to conflict emscripten.h with game.h 
#include <emscripten.h>
#include <math.h>

GameStateConnect::GameStateConnect(GameApp* parent): GameState(parent) 
{
	mStage = STAGE_CONNECTING;
}

GameStateConnect::~GameStateConnect() {}

void GameStateConnect::Create()
{
	mArePluginsLoaded = true;
	mLoginStatus = 0;

	FormatText(mInstructions,instructions,SCREEN_WIDTH_F-40,0.75f);
	mStage = STAGE_CONNECTING;
	strcpy(mSuspendReason,"");
	mSuspendHours = 0;

	gKills = 0;
	gDeaths = 0;
	gKills2 = 0;
	gDeaths2 = 0;

	for (int i=0; i<4; i++) {
		strcpy(mConfigs[i],"");
		strcpy(mConfigInfo[i],"");
	}
	strcpy(mConfigs[0],"name");
	strcpy(mConfigInfo[0],"Enter your name (15 characters max)");
	strcpy(mConfigs[1],"password");
	strcpy(mConfigInfo[1],"Enter your password (15 characters max)");
	strcpy(mConfigs[3],"signin/signup");
	strcpy(mConfigInfo[3],"Signin to your account and start playing online!");
}

void GameStateConnect::Destroy() 
{
	for (int i=0; i<mInstructions.size(); i++) {
		//delete mInstructions[i];
	}
	mInstructions.clear();
}

void GameStateConnect::Start()
{
	index = 0;

	strcpy(name,"");
	strcpy(password,"");
	mLoginStatus = 0;

	mStage = STAGE_CONNECTING;

	mInfoX = -400.0f;

	WlanInit();
	mConnectState = 1;
}


void GameStateConnect::End()
{
}


void GameStateConnect::Update(float dt)
{
	if (mStage == STAGE_ERROR) {
		if (mEngine->GetButtonClick(CTRL_CIRCLE)) {
			mParent->SetNextState(GAME_STATE_MENU);
		}
		return;
	}

	if (mStage != STAGE_NEWACCOUNTSUBMIT) {
		if (mEngine->GetButtonClick(CTRL_CIRCLE) && !gDanzeff->mIsActive) {

			WlanTerm();

			mConnectState = 0;
			mLoginStatus = 0;
			gHttpManager->ClearRequests();

			mParent->SetNextState(GAME_STATE_MENU);
			return;
		}
	}
	if (mStage == STAGE_CONNECTING) {
		mConnectState = 4;

		//SocketConnect(gSocket,"74.125.19.118",80);
		//74.125.53.141
		gHttpManager->Connect("frankinfotech.it","cspsp.appspot.com",2800);
		//gHttpManager->Connect("127.0.0.1","localhost",8080);
		mStage = STAGE_LOGIN;

		int ret = ReadToken(id, ID_LEN);
		if(ret < 0) {
			GenerateToken(id, ID_LEN);
			StoreToken(id);
		}
		char data[2000];
		sprintf(data,"id=%s&version=%d", id, (int)(VERSION*100));

		gHttpManager->SendRequest("/accounts/login.html",data,REQUEST_POST);
		mLoginStatus = 0;
	}
	else if (mStage == STAGE_LOGIN) {
	}
	else if (mStage == STAGE_NEWACCOUNT) {
		mInfoX *= 0.75f;

		if (!gDanzeff->mIsActive) {
			int temp = index;
			if (mEngine->GetButtonState(CTRL_UP) || mEngine->GetAnalogY()<64) {
				if (KeyRepeated(CTRL_UP,dt)) {
					index--;
					if (index < 0) {
						index = 4-1;
					}
				}
			}
			else if (mEngine->GetButtonState(CTRL_DOWN) || mEngine->GetAnalogY()>192) {
				if (KeyRepeated(CTRL_DOWN,dt)) {
					index++;
					if (index > 4-1) {
						index = 0;
					}
				}
			}
			else {
				gLastKey = 0;
			}
			if (temp != index) {
				mInfoX = -400.0f;
			}

			if (mEngine->GetButtonClick(CTRL_CROSS)) {
				if (index == 0) {
					gDanzeff->Enable();
					gDanzeff->mString = name;
					tempname = name;
				}
				else if (index == 1) {
					gDanzeff->Enable();
					gDanzeff->mString = password;
					temppassword = password;
				}
				else if (index == 4-1) {
					if (strlen(name) > 0 && strlen(password) > 0) {
						mStage = STAGE_NEWACCOUNTSUBMIT;
						mLoginStatus = 0;

						string nametemp = "";
						for (int i=0; i<strlen(name); i++) {
							if (name[i] == '&') {
								nametemp += "%26";
							}
							else {
								nametemp += name[i];
							}
						}

						string passwordtemp = "";
						for (int i=0; i<strlen(password); i++) {
							if (password[i] == '&') {
								passwordtemp += "%26";
							}
							else {
								passwordtemp += password[i];
							}
						}

						char data[2000];
						sprintf(data,"name=%s&password=%s&id=%s&version=%d",
							nametemp.c_str(),passwordtemp.c_str(),id,(int)(VERSION*100));

						gHttpManager->SendRequest("/accounts/login.html",data,REQUEST_POST);
					}
					else {
					}
				}
			}
		}
		else if (gDanzeff->mIsActive) {
			gDanzeff->Update(dt);
			if (gDanzeff->mString.length() > 15) {
				gDanzeff->mString = gDanzeff->mString.substr(0,15);
			}
			if (gDanzeff->mString.length() > 0) {
				char character = gDanzeff->mString[gDanzeff->mString.length()-1];
				if (character == ' ' || character == ':' || character == '[' || character == ']') {
					gDanzeff->mString = gDanzeff->mString.substr(0,gDanzeff->mString.length()-1);
				}
			}

			if (index == 0) {
				tempname = (char*)gDanzeff->mString.c_str();
			}
			if (index == 1) {
				temppassword = (char*)gDanzeff->mString.c_str();
			}

			if (mEngine->GetButtonClick(CTRL_START)) {
				if (index == 0) {
					strcpy(name,tempname);
				}
				if (index == 1) {
					strcpy(password,temppassword);
				}	
				gDanzeff->Disable();
			}
			else if (mEngine->GetButtonClick(CTRL_SELECT)) {
				gDanzeff->Disable();
			}
		}
	}
	else if (mStage == STAGE_NEWACCOUNTSUBMIT) {
		if (mEngine->GetButtonClick(CTRL_CIRCLE)) {
			if (mLoginStatus == 0) {
				mStage = STAGE_NEWACCOUNT;
			}
			else if (mLoginStatus == 5) {
				mStage = STAGE_NEWACCOUNT;
			}
			else {
				mStage = STAGE_NEWACCOUNT;
			}
			return;
		}
	}
	else if (mStage == STAGE_SUSPENDED) {
	}

	gHttpManager->Update(dt);
	char buffer[8192];
	int size = gHttpManager->GetResponse(buffer);
	if (size > 0) {
		if (strstr(buffer,"LOGIN")) {
			int code = CheckLogin(buffer);
			if (code == 0) {
				mParent->SetNextState(GAME_STATE_LOBBY);
				return;
			}
			else if (code == 1) {
				mStage = STAGE_SUSPENDED;
			}
			else if (code == 2) {
				mStage = STAGE_NEWACCOUNT;
			}
			else if (code == 3) {
				mParent->SetNextState(GAME_STATE_UPDATE);
				return;
			}
			else if (code == 4) {
				mStage = STAGE_MAINTENANCE;
			}
			else if (code == 5) {
				mLoginStatus = 5;
			}
			else if (code == -1) {
				mLoginStatus = -1;
			}
		}
	}
}


void GameStateConnect::Render()
{
	mRenderer->ClearScreen(ARGB(255,255,255,255));
	mRenderer->RenderQuad(gBgQuad, 0.0f, 0.0f);

	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(0.75f);

	if (mStage == STAGE_CONNECTING || mStage == STAGE_LOGIN || mStage == STAGE_SUSPENDED || mStage == STAGE_MAINTENANCE || mStage == STAGE_ERROR) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Network Connection",20,10);
		gFont->SetScale(0.75f);

		mRenderer->FillRect(0,35,SCREEN_WIDTH_F,200,ARGB(100,0,0,0));

		gFont->DrawShadowedString("[X] Select Connection     [O] Return to Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT_F-20, JGETEXT_CENTER);

		gFont->SetScale(1.0f);
		gFont->SetScale(0.75f);

		mRenderer->FillRect(0,0,SCREEN_WIDTH_F,SCREEN_HEIGHT_F,ARGB(200,0,0,0));
		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.75f);

		if (mStage == STAGE_CONNECTING) {
			gFont->DrawShadowedString("[O] Cancel",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
			
			gFont->SetScale(1.0f);
			if (mConnectState >= 1 && mConnectState <= 4) {
				char buffer[128];
				sprintf(buffer,"Connection state %i of 4",mConnectState);
				gFont->DrawShadowedString(buffer,SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
			else if (mConnectState == 0) {
				gFont->DrawShadowedString("Connection failed",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
			else if (mConnectState == -1) {
				gFont->DrawShadowedString("Connection failed (check wlan switch)",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
		}
		else if (mStage == STAGE_LOGIN) {
			gFont->SetScale(0.75f);
			gFont->DrawShadowedString("[O] Cancel",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
			
			gFont->SetScale(1.0f);
			if (mLoginStatus == -1) {
				gFont->DrawShadowedString("Unspecified error. Please retry.",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
			else {
				gFont->DrawShadowedString("Signing in...",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
		}
		else if (mStage == STAGE_SUSPENDED) {
			gFont->DrawShadowedString("[O] Return",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
			
			gFont->SetScale(1.0f);
			char buffer[128];
			sprintf(buffer,"You are suspended for %i hour(s)",mSuspendHours);
			gFont->DrawShadowedString(buffer,SCREEN_WIDTH_2,SCREEN_HEIGHT_2-10,JGETEXT_CENTER);

			strcpy(buffer,"");
			sprintf(buffer,"Reason: %s",mSuspendReason);
			gFont->DrawShadowedString(buffer,SCREEN_WIDTH_2,SCREEN_HEIGHT_2+10,JGETEXT_CENTER);
		}
		else if (mStage == STAGE_MAINTENANCE) {
			gFont->DrawShadowedString("[O] Return",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

			gFont->SetScale(1.0f);
			gFont->DrawShadowedString("The online system is currently under maintenance.",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
		}
		else if (mStage == STAGE_ERROR) {
			gFont->DrawShadowedString("[O] Return",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

			gFont->SetScale(1.0f);
			gFont->DrawShadowedString(mErrorString,SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
		}
	}

	else if (mStage == STAGE_NEWACCOUNT || mStage == STAGE_NEWACCOUNTSUBMIT) {
		int starty = 115;
		mRenderer->FillRect(0,starty,SCREEN_WIDTH_F,100,ARGB(100,0,0,0));
		mRenderer->FillRect(0,starty+100,SCREEN_WIDTH_F,30,ARGB(175,0,0,0));

		gFont->SetScale(0.75f);
		for (int i=0; i<mInstructions.size(); i++) {
			gFont->DrawShadowedString(mInstructions[i], 20, 20+i*15);
		}

		if (!gDanzeff->mIsActive) {
			mRenderer->FillRect(0,starty+index*25,SCREEN_WIDTH_F,25,ARGB(255,0,0,0));
			gFont->DrawShadowedString("[X] Select     [O] Return",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
		}
		else {
			mRenderer->FillRect(0,starty+index*25,SCREEN_WIDTH_F,25,ARGB(100,0,128,255));
			gFont->DrawShadowedString("[START] Enter    [SELECT] Cancel",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
		}

		mRenderer->DrawLine(160,starty,160,starty+100,ARGB(255,255,255,255));

		gFont->SetScale(1.0f);
		gFont->SetColor(ARGB(255,255,255,255));
		float x = 160-10;

		for (int i=0; i<4; i++) {
			if (i == index) {
				gFont->SetColor(ARGB(255,255,255,255));
				gFont->SetScale(0.75f);
				gFont->DrawShadowedString(mConfigInfo[i],10.0f+mInfoX,starty+100+10);
				gFont->SetScale(1.0f);
				gFont->SetColor(ARGB(255,0,128,255));
			}
			else {
				gFont->SetColor(ARGB(255,255,255,255));
			}
			gFont->DrawShadowedString(mConfigs[i],x,starty+3+i*25,JGETEXT_RIGHT);
		}

		gFont->SetScale(0.75f);
		gFont->SetColor(ARGB(255,255,255,255));

		int startx = 160+10;
		if (!gDanzeff->mIsActive) {
			gFont->DrawShadowedString(name,startx,starty+5);
			gFont->DrawShadowedString(password,startx,starty+5+25);
		}
		else if (gDanzeff->mIsActive) {
			if (index == 0) {
				gFont->DrawShadowedString(password,startx,starty+5+25);

				gFont->DrawShadowedString(tempname,startx,starty+5);
				gFont->DrawShadowedString("|",startx+gFont->GetStringWidth(tempname),starty+5);
			}
			else if (index == 1) {
				gFont->DrawShadowedString(name,startx,starty+5);

				gFont->DrawShadowedString(temppassword,startx,starty+5+25);
				gFont->DrawShadowedString("|",startx+gFont->GetStringWidth(temppassword),starty+5+25);
			}
			gDanzeff->Render(SCREEN_WIDTH_F-175,SCREEN_HEIGHT_F-175);
		}

		if (mStage == STAGE_NEWACCOUNTSUBMIT) {
			mRenderer->FillRect(0,0,SCREEN_WIDTH_F,SCREEN_HEIGHT_F,ARGB(200,0,0,0));

			gFont->SetScale(0.75f);
			gFont->SetColor(ARGB(255,255,255,255));
			if (mLoginStatus == 0) {
				gFont->DrawShadowedString("[O] Cancel", SCREEN_WIDTH_2, SCREEN_HEIGHT_F-20, JGETEXT_CENTER);

				gFont->SetScale(1.0f);
				gFont->DrawShadowedString("Signing in...",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
			/*else if (mNewAccountStatus == 0) {
				gFont->DrawShadowedString("[X/O] Continue to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT_F-20, JGETEXT_CENTER);

				gFont->SetScale(1.0f);
				char buffer[128];
				gFont->DrawShadowedString("Successfully signed in as",SCREEN_WIDTH_2,SCREEN_HEIGHT_2-10.0f,JGETEXT_CENTER);
				gFont->DrawShadowedString(gName,SCREEN_WIDTH_2,SCREEN_HEIGHT_2+10.0f,JGETEXT_CENTER);
			}*/
			else if (mLoginStatus == 5) {
				gFont->DrawShadowedString("[O] Return", SCREEN_WIDTH_2, SCREEN_HEIGHT_F-20, JGETEXT_CENTER);

				gFont->SetScale(1.0f);
				gFont->DrawShadowedString("Name already exists / Wrong password",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
			else {
				gFont->DrawShadowedString("[O] Return", SCREEN_WIDTH_2, SCREEN_HEIGHT_F-20, JGETEXT_CENTER);

				gFont->SetScale(1.0f);
				gFont->DrawShadowedString("Unspecified error. Please retry.",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
		}
	}
}


int GameStateConnect::CheckLogin(char* buffer)
{
	char* s;

	s = strstr(buffer,"LOGIN");
	if (s == NULL) {
		return -1;		
	}

	s += strlen("LOGIN") + 2;

	if (*s == '0') {
		// successful login
		s += 3;
		char salt[32];
		char decoding[2000];
		sscanf(s,DecodeText(decoding,"138215133137216132138215133137216132138215133137216132138215133137201132138200"),
				gSessionKey,gKey,salt,gName,gEncodedName,gDisplayName,gEncodedDisplayName,&gKills,&gDeaths);
			//%s %s %s %s %s %s %s %d %d
		strcpy(decoding,"");

		for (int i=0; i<9; i++) {
			s = strstr(s,"\r\n") + 2;
		}
		memcpy(gIcon,s,300);
		// gIconTexture not used

		gKills2 = gKills*7;
		gDeaths2 = gDeaths*7;

		return 0;
	}
	else if (*s == '1') {
		// suspended
		s += 3;
		char decoding[2000];
		sscanf(s,DecodeText(decoding,"138191195158194158138205"),mSuspendReason,&mSuspendHours);
			//%[^:]:%i
		strcpy(decoding,"");
		return 1;
	}
	else if (*s == '2') {
		// new account
		return 2;
	}
	else if (*s == '3') {
		return 3;
	}
	else if (*s == '4') {
		return 4;
	}
	else if (*s == '5') {
		return 5;
	}
	return -1;
}

void GameStateConnect::GenerateToken(char* token, int len)
{
    static const char alphanum[] = "0123456789ABCDEF";

    for (int i = 0; i < len - 1 ; i++) {
		int index = floor(emscripten_random() * 16);
        token[i] = alphanum[index];
		printf("%d ",alphanum[index]);
    }
	printf("\n");

    token[len - 1] = 0;
}

int GameStateConnect::StoreToken(char* token)
{
	std::FILE* file = std::fopen("/persistent_data/token.dat","w");

	if(file == NULL)
		return -1;

	std::fputs(token, file);
	fclose(file);

	EM_ASM(
        FS.syncfs(false,function (err) {
                          assert(!err);
        });
	);

	return 0;
}

int GameStateConnect::ReadToken(char* token, int len)
{
	if(emscripten_run_script_int("Module.syncdone") != 1)
		return -1;
	
	std::FILE* file = std::fopen("/persistent_data/token.dat","r");

	if(file == NULL)
		return -1;

	std::fgets(token, len, file);
	
	return 0;
}