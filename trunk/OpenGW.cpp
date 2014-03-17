// OpenGW.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <MMSYSTEM.H>

#include "glew.h"
#include "wglew.h"

#include "defines.h"
#include "OpenGW.h"
#include "scene.h"

#include "blur.h"


#define filterWidth 5 
#define filterHeight 5 
#define imageWidth 320 
#define imageHeight 240

//declare image buffers 
const int blurBufferWidth = 500;
const int blurBufferHeight = 250;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ColorRGBA;

ColorRGBA blurBuffer[blurBufferWidth][blurBufferHeight];

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static void CALLBACK TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);


void OGLCreate();
void OGLDestroy();
void OGLSize(int cx, int cy);
void OGLPaint(HDC hDC);

scene oglScene;

BOOL oglInited = FALSE;


HWND hWnd;
HGLRC hRC;

MMRESULT timerId;

//our OpenGL texture handles
unsigned int texOffscreen;

void createOffscreens();
void drawOffscreens();



#define CONTEXT_PRIMARY 0
#define CONTEXT_GLOW    1

int mWidth, mHeight;


static DWORD lastTime;
static DWORD fpsTime;
static int frameCount;
static int fps;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OPENGW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENGW));

    OGLCreate();

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    OGLDestroy();

	return (int) msg.wParam;
}


void CALLBACK TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    HDC hdc = ::GetDC(hWnd);
    OGLPaint(hdc);
    ::ReleaseDC(hWnd, hdc);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENGW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//#define FULLSCREEN
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   float aspect = 9.0f/16.0f;

   DWORD style = 0;

#ifndef FULLSCREEN
   style |= WS_OVERLAPPEDWINDOW;
#endif

   hWnd = CreateWindow(szWindowClass, szTitle, style,
      20, 20, 720/aspect, 720, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

#ifdef FULLSCREEN
   ::SetWindowLong(hWnd, GWL_STYLE, 0);
   ShowWindow(hWnd, SW_SHOWMAXIMIZED);
#else
   ShowWindow(hWnd, SW_SHOW);
#endif

   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
    case WM_SIZE:
        OGLSize(LOWORD(lParam), HIWORD(lParam));
        break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        OGLPaint(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// ***************************************************************************

void OGLCreate()
{
    // Define pixel format
	static	PIXELFORMATDESCRIPTOR pfd=							// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),							// Size Of This Pixel Format Descriptor
		1,														// Version Number
		PFD_DRAW_TO_WINDOW |									// Format Must Support Window
		PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,										// Must Support Double Buffering
		PFD_TYPE_RGBA,											// Request An RGBA Format
		8,			    										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,										// Color Bits Ignored
		0,														// Alpha Buffer
		0,														// Shift Bit Ignored
		0,														// No Accumulation Buffer
		0, 0, 0, 0,												// Accumulation Bits Ignored
		0,														// No Z-Buffer (Depth Buffer)  
		0,														// No Stencil Buffer
		0,														// No Auxiliary Buffer
		PFD_MAIN_PLANE,											// Main Drawing Layer
		0,														// Reserved
		0, 0, 0													// Layer Masks Ignored
	};


    // Set pixel format
    HDC hDC = GetDC(hWnd);
    int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, nPixelFormat, &pfd);

    // Create RC
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    // Init GLEW
    {
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            TCHAR s[256];
            wsprintf(s, L"glewInit failed: %s\n", glewGetErrorString(err));
            OutputDebugString(s);
        }
        else
        {
            TCHAR s[256];
            wsprintf(s, L"Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
            OutputDebugString(s);
        }

        if (!glewIsSupported("GL_EXT_framebuffer_object"))
        {
            OutputDebugString(L"GL_EXT_framebuffer_object is required\n");
        }
        else
        {
            OutputDebugString(L"YAY! GL_EXT_framebuffer_object OK\n");
        }
    }

    // ***********************************************************************

    // Size viewport
    RECT rc;
    GetClientRect(hWnd, &rc);
    int width = rc.right-rc.left;
    int height = rc.bottom-rc.top;
    OGLSize(width, height);


    // Do stuff with the context here if needed...
    createOffscreens();

	// Set vblank syncing
	wglSwapIntervalEXT(true);


    wglMakeCurrent(0, 0);

    // Clean up
    ReleaseDC(hWnd, hDC);


    // create the timer
    timerId = timeSetEvent(
        1,//16,
        0,//resolution,
        TimerFunction,
        (DWORD)NULL,
        TIME_PERIODIC|TIME_KILL_SYNCHRONOUS);


    oglInited = TRUE;

}

// ***************************************************************************

void OGLDestroy()
{
    oglInited = FALSE;
    timeKillEvent(timerId);

    HDC hDC = GetDC(hWnd);
    wglMakeCurrent(hDC, hRC);
    wglMakeCurrent(0, 0);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

// ***************************************************************************

void OGLSize(int cx, int cy)
{
	oglScene.size(cx, cy);
    mWidth = cx;
    mHeight = cy;
}


// ***************************************************************************

void createOffscreens()
{
    // new array
    char* colorBits = new char[ blurBufferWidth * blurBufferHeight * 3 ];

    // texture creation..
    glGenTextures(1, &texOffscreen);
    glBindTexture(GL_TEXTURE_2D, texOffscreen);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, blurBufferWidth, blurBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBits);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clean up
    delete[] colorBits;

    glBindTexture(GL_TEXTURE_2D, 0);

}

#define DO_BLUR // PERFORMANCE: Enabling this has a large impact on performance!

void drawOffscreens()
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT,(int*)viewport);

#ifdef DO_BLUR
    // Draw to the blur texture
    {
        glViewport(0, 0, blurBufferWidth, blurBufferHeight);

        oglScene.draw(scene::RENDERPASS_BLUR);

        // Transfer image to the blur texture
        glBindTexture(GL_TEXTURE_2D, texOffscreen);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, blurBufferWidth, blurBufferHeight, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
#endif

    // Draw the scene normally
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
    oglScene.draw(scene::RENDERPASS_PRIMARY);

#ifdef DO_BLUR

    ///////////////////////////////////////////////////////
    // Do blur

    // Bind the blur texture and copy the screen bits to it
    glBindTexture(GL_TEXTURE_2D, texOffscreen);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

    superFastBlur((unsigned char*)&blurBuffer[0][0], blurBufferWidth, blurBufferHeight, 3);

    // Bind the blur result back to our texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

    ////////////////////////////////////////////////
    // Draw the blur texture on top of the existing scene

	// Glowy blending effect
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glColor4f(1, 1, 1, .8); // .8

    // Draw it on the screen
    glBegin( GL_QUADS );
    glTexCoord2d(0.0,0.0); glVertex2d(-1.0, -1.0);
    glTexCoord2d(1.0,0.0); glVertex2d(1.0, -1.0);
    glTexCoord2d(1.0,1.0); glVertex2d(1.0, 1.0);
    glTexCoord2d(0.0,1.0); glVertex2d(-1.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable( GL_TEXTURE_2D );
#endif
}


void OGLPaint(HDC hDC)
{
    if (!oglInited) return;

    static BOOL inpaint = FALSE;
    if (inpaint) return;
    inpaint = TRUE;


    oglScene.run();


    // ****************************************
/*
    ++frameCount;

    DWORD newTime = GetTickCount();
    lastTime = newTime;


    if ((newTime - fpsTime) > 1000)
    {
        fpsTime = newTime;
        fps = (fps + frameCount) / 2;
        frameCount = 0;

        wchar_t  s[256];
        wsprintf(s, L"FPS = %d", fps);
        ::SetWindowText(hWnd, s);

    }
*/
    // ****************************************

    wglMakeCurrent(hDC, hRC);



    drawOffscreens();



    SwapBuffers(hDC);
    wglMakeCurrent(0, 0);

    inpaint = FALSE;
}

