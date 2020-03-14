#include <JGameLauncher.h>

#include "GameApp.h"


//-------------------------------------------------------------------------------------
JApp* JGameLauncher::GetGameApp()
{
	return new GameApp();
};


//-------------------------------------------------------------------------------------
char *JGameLauncher::GetName()
{
	return "template_desc";
}


//-------------------------------------------------------------------------------------
u32 JGameLauncher::GetInitFlags()
{
	return JINIT_FLAG_ENABLE3D;
}
