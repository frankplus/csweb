#include <chrono>
#include <stdio.h>
#include <time.h>
#include <map>
#include <functional>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL.h>
#include <GLES3/gl3.h>

#include <JTypes.h>
#include <JGE.h>
#include <JApp.h>
#include <JRenderer.h>
#include <JGameLauncher.h>

#include <JSoundSystem.h>

using namespace std;
using namespace chrono;

SDL_Window *window;
SDL_Renderer *renderer;

JGE* g_engine = NULL;
JApp* g_app = NULL;
JGameLauncher* g_launcher = NULL;

unsigned int lastTickCount;

u32 gButtons = 0;
u32 gOldButtons = 0;
double analogX = 0; 
double analogY = 0; 

map<int, int> gKeyboardMap = {
    {SDL_SCANCODE_B, CTRL_TRIANGLE},
    {SDL_SCANCODE_N, CTRL_CIRCLE},
    {SDL_SCANCODE_SPACE, CTRL_CROSS},
    {SDL_SCANCODE_R, CTRL_SQUARE},
    {SDL_SCANCODE_UP, CTRL_UP},
    {SDL_SCANCODE_DOWN, CTRL_DOWN},
    {SDL_SCANCODE_LEFT, CTRL_LEFT},
    {SDL_SCANCODE_RIGHT, CTRL_RIGHT},
    {SDL_SCANCODE_Q, CTRL_LTRIGGER},
    {SDL_SCANCODE_E, CTRL_RTRIGGER},
    {SDL_SCANCODE_ESCAPE, CTRL_START},
    {SDL_SCANCODE_TAB, CTRL_SELECT},
    {SDL_SCANCODE_LCTRL, CTRL_NOTE}
};

map<int, int> gGamepadMap = {
    {0, CTRL_CROSS},
    {1, CTRL_CIRCLE},
    {2, CTRL_SQUARE},
    {3, CTRL_TRIANGLE},
    {4, CTRL_LTRIGGER},
    {5, CTRL_RTRIGGER},
    {8, CTRL_SELECT},
    {9, CTRL_START},
    {12, CTRL_UP},
    {13, CTRL_DOWN},
    {14, CTRL_LEFT},
    {15, CTRL_RIGHT}
};

bool JGEGetButtonState(u32 button)
{
	return (gButtons&button)==button;
}


bool JGEGetButtonClick(u32 button)
{
	return (gButtons&button)==button && (gOldButtons&button)!=button;
}

u8 JGEGetAnalogX()
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
    if(currentKeyStates[SDL_SCANCODE_D]) return 0xff;
    if(currentKeyStates[SDL_SCANCODE_A]) return 0;
    if(analogX > -0.01 && analogX < 0.01) return 0x80;
    return (analogX+1) * 127.0;
}

u8 JGEGetAnalogY()
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
    if(currentKeyStates[SDL_SCANCODE_W]) return 0;
    if(currentKeyStates[SDL_SCANCODE_S]) return 0xff;
    if(analogY > -0.01 && analogY < 0.01) return 0x80;
    return (analogY+1) * 127.0;
}


int InitGame(GLvoid)
{
    g_launcher = new JGameLauncher();
	u32 flags = g_launcher->GetInitFlags();

	g_engine = JGE::GetInstance();
	
	g_app = g_launcher->GetGameApp();
	g_app->Create();
	g_engine->SetApp(g_app);
	
	JRenderer::GetInstance()->Enable2D();
	
	lastTickCount = SDL_GetTicks();

    srand (time(NULL));

	return true;
}

void process_input() {
    gOldButtons = gButtons;
    gButtons = 0;

    // keyboard
    SDL_PumpEvents();
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
    map<int,int>::iterator it = gKeyboardMap.begin();
    while(it != gKeyboardMap.end()) 
    {
        if(currentKeyStates[it->first])
            gButtons |= it->second;

        it++;
    }

    // gamepad
    if(emscripten_sample_gamepad_data() == EMSCRIPTEN_RESULT_SUCCESS)
    {
        int numGamepads = emscripten_get_num_gamepads();
        for(int id=0; id<numGamepads; id++)
        {
            EmscriptenGamepadEvent ge;
            int ret = emscripten_get_gamepad_status(id, &ge);
            if (ret == EMSCRIPTEN_RESULT_SUCCESS)
            {
                // digital buttons
                map<int,int>::iterator it = gGamepadMap.begin();
                while(it != gGamepadMap.end()) 
                {
                    if(ge.digitalButton[it->first])
                        gButtons |= it->second;

                    it++;
                }

                // analogs
                analogX = ge.axis[0];
                analogY = ge.axis[1];
            }
        }
    }
}

void main_loop() 
{ 
    process_input();

    unsigned int tickCount = SDL_GetTicks();   // Get The Tick Count
    unsigned int delta = tickCount - lastTickCount;
	g_engine->SetDelta(delta);
	g_engine->Update();
	g_engine->mClicked = false;
    lastTickCount = tickCount;
    
	g_engine->Render();
    SDL_GL_SwapWindow(window);
}

EM_BOOL windowSizeChanged(int eventType, const EmscriptenUiEvent *e, void *userData)
{
    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    SDL_SetWindowSize(window, width, height);
    glViewport (0, 0, (GLsizei)width, (GLsizei)height);
    // printf("window resized %f %f\n", width, height);
    return true;
}

int on_button_click(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData)
{
    EmscriptenFullscreenChangeEvent fsce;
    EMSCRIPTEN_RESULT ret = emscripten_get_fullscreen_status(&fsce);
    if (!fsce.isFullscreen)
    {
        printf("Requesting fullscreen..\n");
        ret = emscripten_request_fullscreen("#canvas", 1);
    }
    else
    {
        printf("Exiting fullscreen..\n");
        ret = emscripten_exit_fullscreen();
        ret = emscripten_get_fullscreen_status(&fsce);
        if (fsce.isFullscreen)
        {
            fprintf(stderr, "Fullscreen exit did not work!\n");
        }
    }
    return 1;
}

int main()
{   
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
    attr.enableExtensionsByDefault = 1;
    attr.premultipliedAlpha = 0;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );

    // initialize window
    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    glViewport (0, 0, (GLsizei)width, (GLsizei)height);

    // load IDBFS persistent storage
    EM_ASM(
        FS.mkdir('/persistent_data');
        FS.mount(IDBFS, {}, '/persistent_data');
        Module.syncdone = 0;

        // sync from persisted state into memory
        FS.syncfs(true, function(err) {
            if(!err)
                Module.syncdone = 1;
        });
    );

    InitGame();
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, 1, windowSizeChanged);
    emscripten_set_click_callback("#fullscreen", (void*)0, 1, on_button_click);
    emscripten_set_main_loop(main_loop, -1, true);

    return EXIT_SUCCESS;
}