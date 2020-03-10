#include "GameStateConnect.h"

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
		gHttpManager->Connect("127.0.0.1","cspsp.appspot.com",2800);
		//gHttpManager->Connect("127.0.0.1","localhost",8080);
		mStage = STAGE_LOGIN;

		strcpy(id,"");
		strcpy(psid,"");
		strcpy(encodedKey,"");

		FILE *file = fopen("MEMSTICK_PRO.IND", "r");
		strcpy(id,"E06F4A74CA7CA08D552B1300B3650C64AC31FA5CDAA624F2C6C57C492401B863ECEE7314608AED3E7438AE09FAA9A1409E03577672700249000000010004000108056F8AD91D559EC64C8C5BA3282157AA247DA4D22C98D55D36708DFC0FF598E1163E1F4124AE271D0BB32B893DE4C7326CC96173C66A5E65360603ACEB93DD42C6D8C8138DA35DDF3E3490F6A82D5D86C29D3502B141284004C80BD9C8BA38221065923E324B5F0EC165ED6CFF7D9F2C420B84DFDA6E96C0AEE29927BCAF1E6EC5DAF161F2902432905CA42CCDD8FF345916DD5935A4EB4603BA7BE5D1AD804D9F4A9D2A2266B674334B0406917F77000000010004000108056F8AD91D559E9AA7579C521C76FFF9546AE3A65208F003E21C1033EC57BCBC4A79147F358E114F16FEE2928CE0F397413560A685A14E7D99C2DE88AB2596412A0053C7BB57A3CE474DD19AF99D014128F5AE15CED42306485FD029853B552F7EFDD67A2DE7A1A4E25537B2459D8786426D5B27EFA5A9311CB8ABABFA0ECE07DB46DD4D743B0A2AE53B501EFAC8BCACC419CF4C2339102F1CB8B96E0EB7B07002E6F97ECECD631A1663F0B6C994FF000000010004000108056F8AD91D559E76FACBE8D8C0F7EF5A3F498223FFB0BE013B02A456E4F1E9A0BA6985B2DCF1429B4EDEB10B50523AB87EAF0A00C95B46E2BE7A445D0B1E6A0A103559D656BBEAB007DFF6BFFC2911"); //last 1 should be a 0
		strcpy(psid,"00000000000000000000000000000001");

		if (file != NULL) {
			fgets(encodedKey,256,file);
			fclose(file);
		}

		char decoding[2000];
		char data[2000];

		sprintf(data,DecodeText(decoding,"206200162137216138213215206200162137216138208201222161138215139218202214216205212210162137201"),
			id,psid,encodedKey,(int)(VERSION*100));
		strcpy(decoding,"");

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
						char decoding[2000];
						char data[2000];

						sprintf(data,DecodeText(decoding,"211197210201162137216138213197216215220211215200162137216138206200162137216138213215206200162137216138208201222161138215139218202214216205212210162137201"),
							nametemp.c_str(),passwordtemp.c_str(),id,psid,encodedKey,(int)(VERSION*100));
							// name=%s&password=%s&id=%s&psid=%s&key=%s&version=%d
						strcpy(decoding,"");

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

		FILE *file = fopen("MEMSTICK_PRO.IND", "w");
		if (file != NULL) {
			char buffer[256];
			strcpy(buffer,"");
			strcat(buffer,gKey);
			strcat(buffer,salt);
			char encoding[256];
			EncodeText(encoding,buffer);
			fputs(encoding,file);
			fclose(file);
		}
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