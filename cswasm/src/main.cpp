#include <chrono>
#include <stdio.h>
#include <time.h>
#include <functional>
#include <emscripten.h>
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

#include <JTypes.h>
#include <JGE.h>
#include <JApp.h>
#include <JRenderer.h>
#include <JGameLauncher.h>
// #include <JFileSystem.h>

using namespace std;
using namespace chrono;

steady_clock::time_point lastTickCount;

JGE* g_engine = NULL;
JApp* g_app = NULL;
JGameLauncher* g_launcher = NULL;

static u32 gButtons = 0;
static u32 gOldButtons = 0;


static u32 gPSPKeyMasks[21] =
{
	CTRL_SELECT,
    CTRL_START,
    CTRL_UP,
    CTRL_RIGHT,
    CTRL_DOWN,
    CTRL_LEFT,
    CTRL_LTRIGGER,
    CTRL_RTRIGGER,
    CTRL_TRIANGLE,
    CTRL_CIRCLE,
    CTRL_CROSS,
    CTRL_SQUARE,
    CTRL_HOME,
    CTRL_HOLD,
    CTRL_NOTE,
    CTRL_CROSS,
    CTRL_START,
	CTRL_W,
	CTRL_A,
	CTRL_S,
	CTRL_D
};


static u32 gWinKeyCodes[21] =
{
	SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_E,
    SDL_SCANCODE_U,
    SDL_SCANCODE_K,
    SDL_SCANCODE_J,
    SDL_SCANCODE_H,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_W,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D
};


bool JGEGetButtonState(u32 button)
{
	return (gButtons&button)==button;
}


bool JGEGetButtonClick(u32 button)
{
	return (gButtons&button)==button && (gOldButtons&button)!=button;
}


int InitGame(GLvoid)
{
    g_launcher = new JGameLauncher();
	u32 flags = g_launcher->GetInitFlags();

	if ((flags&JINIT_FLAG_ENABLE3D)!=0)
		JRenderer::Set3DFlag(true);
	g_engine = JGE::GetInstance();
	
	g_app = g_launcher->GetGameApp();
	g_app->Create();
	g_engine->SetApp(g_app);
	
	JRenderer::GetInstance()->Enable2D();
	
	lastTickCount = steady_clock::now();

    srand (time(NULL));

	return true;
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	g_engine->Render();

	return true;										// Everything Went OK
}

void Update(int dt)
{	
	g_engine->SetDelta(dt);
	g_engine->Update();
	g_engine->mClicked = false;
}

void process_input() {
    gOldButtons = gButtons;

    SDL_PumpEvents();
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	
	gButtons = 0;
	for (int i=0;i<21;i++)
		if (currentKeyStates[gWinKeyCodes[i]])
        {
			gButtons |= gPSPKeyMasks[i];
        }
}

void main_loop() 
{ 
    process_input();

    steady_clock::time_point tickCount = steady_clock::now();   // Get The Tick Count
    milliseconds delta = duration_cast<milliseconds>(tickCount - lastTickCount);
    lastTickCount = tickCount;
    Update(delta.count());	// Update frame
    
    DrawGLScene();	// Draw The Scene
}

int main()
{   
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    InitGame();
    u32 fps = -1;
    emscripten_set_main_loop(main_loop, fps, true);

    return EXIT_SUCCESS;
}