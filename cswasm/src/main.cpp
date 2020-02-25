#include <functional>
#include <emscripten.h>
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

#include <chrono>

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

void JGEControl()
{
}


bool JGEGetKeyState(int key)
{
    return false;
}


bool JGEGetButtonState(u32 button)
{
    return false;
}


bool JGEGetButtonClick(u32 button)
{
    return false;
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

	return true;
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	g_engine->Render();

	return true;										// Everything Went OK
}

void Update(int dt)
{
	JGEControl();
	
	g_engine->SetDelta(dt);
	g_engine->Update();
	g_engine->mClicked = false;
}

void main_loop() 
{ 
    steady_clock::time_point tickCount = steady_clock::now();   // Get The Tick Count
    milliseconds delta = duration_cast<milliseconds>(tickCount - lastTickCount);
    lastTickCount = tickCount;
    Update(delta.count());	// Update frame
    
    DrawGLScene();	// Draw The Scene
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    InitGame();

    emscripten_set_main_loop(main_loop, 0, true);

    return EXIT_SUCCESS;
}