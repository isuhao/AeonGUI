/******************************************************************************
Copyright 2010-2012 Rodrigo Hernandez Cordoba

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
******************************************************************************/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <crtdbg.h>
#include "wglext.h"
#include "AeonGUI.h"
#include "OpenGLRenderer.h"
#include "MainWindow.h"
#include "glcommon.h"
#include "logo.h"
#include "Vera.h"
#include "Color.h"
#include "Cursor.h"

class Window
{
public:
    Window() :
        hWnd ( NULL ),
        hDC ( NULL ),
        hRC ( NULL ),
        width ( 0 ),
        height ( 0 ),
        mousex ( 0 ),
        mousey ( 0 ),
        aeongames_logo ( NULL ),
        aeongui_logo ( NULL ),
        font ( NULL ),
        window ( NULL )
    {};
    ~Window() {};
    void Initialize ( HINSTANCE hInstance );
    void Finalize ( );
    LRESULT OnSize ( WPARAM type, WORD newwidth, WORD newheight );
    LRESULT OnPaint();
    LRESULT OnMouseMove ( int32_t x, int32_t y );
    LRESULT OnMouseButtonDown ( uint8_t button, int32_t x, int32_t y );
    LRESULT OnMouseButtonUp ( uint8_t button, int32_t x, int32_t y );
    static LRESULT CALLBACK WindowProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    static void Register ( HINSTANCE hInstance );
    void RenderLoop();
private:
    static ATOM atom;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    PIXELFORMATDESCRIPTOR pfd;
    int32_t width;
    int32_t height;
    int32_t mousex;
    int32_t mousey;
    AeonGUI::OpenGLRenderer renderer;
    AeonGUI::Image* aeongames_logo;
    AeonGUI::Image* aeongui_logo;
    AeonGUI::Cursor cursor;
    AeonGUI::Image cursor_image;
    AeonGUI::Font* font;
    AeonGUI::MainWindow* window;
};

ATOM Window::atom = 0;

void Window::Initialize ( HINSTANCE hInstance )
{
    width  = 800;
    height = 600;
    int pf;

    RECT rect = {0, 0, width, height};

    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

    if ( atom == 0 )
    {
        Register ( hInstance );
    }
    AdjustWindowRectEx ( &rect, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE );
    hWnd = CreateWindowEx ( WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
                            "AeonGUI", "AeonGUI",
                            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                            0, 0, // Location
                            rect.right - rect.left, rect.bottom - rect.top, // dimensions
                            NULL,
                            NULL,
                            hInstance,
                            this );
    SetWindowLongPtr ( hWnd, 0, ( LONG_PTR ) this );
    hDC = GetDC ( hWnd );
    pfd.nSize = sizeof ( PIXELFORMATDESCRIPTOR );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cRedBits = 0;
    pfd.cRedShift = 0;
    pfd.cGreenBits = 0;
    pfd.cGreenShift = 0;
    pfd.cBlueBits = 0;
    pfd.cBlueShift = 0;
    pfd.cAlphaBits = 0;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = 0;
    pfd.cAccumRedBits = 0;
    pfd.cAccumGreenBits = 0;
    pfd.cAccumBlueBits = 0;
    pfd.cAccumAlphaBits = 0;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 0;
    pfd.cAuxBuffers = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.bReserved = 0;
    pfd.dwLayerMask = 0;
    pfd.dwVisibleMask = 0;
    pfd.dwDamageMask = 0;
    pf = ChoosePixelFormat ( hDC, &pfd );
    SetPixelFormat ( hDC, pf, &pfd );
    hRC = wglCreateContext ( hDC );
    wglMakeCurrent ( hDC, hRC );

    //---OpenGL 3.2 Context---//
    wglGetExtensionsStringARB = ( PFNWGLGETEXTENSIONSSTRINGARBPROC ) wglGetProcAddress ( "wglGetExtensionsStringARB" );
    if ( wglGetExtensionsStringARB != NULL )
    {
        if ( strstr ( wglGetExtensionsStringARB ( hDC ), "WGL_ARB_create_context" ) != NULL )
        {
            const int ctxAttribs[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 2,
                WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };

            wglCreateContextAttribsARB = ( PFNWGLCREATECONTEXTATTRIBSARBPROC ) wglGetProcAddress ( "wglCreateContextAttribsARB" );
            wglMakeCurrent ( hDC, NULL );
            wglDeleteContext ( hRC );
            hRC = wglCreateContextAttribsARB ( hDC, NULL, ctxAttribs );
            wglMakeCurrent ( hDC, hRC );
        }
    }
    //---OpenGL 3.2 Context---//
    glClearColor ( 0, 0, 0, 0 );
    window = new AeonGUI::MainWindow();
    aeongames_logo = new AeonGUI::Image;
    aeongames_logo->Load ( logo_width, logo_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, logo_data );
    aeongui_logo = new AeonGUI::Image;
#ifdef USE_PNG
    //aeongui_logo->LoadFromFile("AeonGUILogoBlBkg.png");
    aeongui_logo->LoadFromFile ( "WindowFrame.png" );
    cursor_image.LoadFromFile ( "cursor.png" );
#else
    //aeongui_logo->LoadFromFile ( "AeonGUILogoBlBkg.pcx" );
    //aeongui_logo->LoadFromFile ( "Patch9Test.pcx" );
    aeongui_logo->LoadFromFile ( "WindowFrame.pcx" );
    //aeongui_logo->LoadFromFile ( "ScaleTest.pcx" );
    cursor_image.LoadFromFile ( "cursor.pcx" );
#endif
    font = new AeonGUI::Font;
    font->Load ( Vera.data, Vera.size );
    renderer.Initialize ( );
    renderer.ChangeScreenSize ( width, height );
    renderer.SetFont ( font );
    std::wstring hello ( L"Hello World" );
    window->SetFrameImage ( aeongui_logo );
    window->SetCaption ( hello.c_str() );
    renderer.AddWidget ( window );
    cursor.SetCursorImage ( &cursor_image );
    renderer.SetCursor ( &cursor );
    ShowWindow ( hWnd, SW_SHOW );

}

void Window::Finalize()
{
    if ( window != NULL )
    {
        renderer.RemoveWidget ( window );
        delete window;
        window = NULL;
    }
    if ( aeongames_logo != NULL )
    {
        delete aeongames_logo;
        aeongames_logo = NULL;
    }
    if ( aeongui_logo != NULL )
    {
        delete aeongui_logo;
        aeongui_logo = NULL;
    }
    if ( font != NULL )
    {
        delete font;
        font = NULL;
    }
    renderer.Finalize();
    wglMakeCurrent ( hDC, NULL );
    wglDeleteContext ( hRC );
    ReleaseDC ( hWnd, hDC );
    DestroyWindow ( hWnd );
}

void Window::RenderLoop()
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER this_time;
    QueryPerformanceCounter ( &this_time );
    QueryPerformanceFrequency ( &frequency );
    static LARGE_INTEGER last_time = this_time;
    float delta = static_cast<float> ( this_time.QuadPart - last_time.QuadPart ) / static_cast<float> ( frequency.QuadPart );
    if ( delta > 1e-1f )
    {
        delta = 1.0f / 30.0f;
    }
    //wglMakeCurrent ( hDC, hRC );
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    const AeonGUI::Color color ( 0xFFFFFFFF );
    renderer.BeginRender();
    renderer.RenderWidgets();
#if 0
    renderer.DrawImage ( aeongames_logo, width - logo_width, height - logo_height );
    //renderer.DrawImage ( aeongui_logo, 0, height - aeongui_logo->GetHeight(), aeongui_logo->GetWidth() * 2 );
    renderer.DrawImage ( aeongui_logo, 0, height - aeongui_logo->GetHeight() * 2, aeongui_logo->GetWidth() * 2 , aeongui_logo->GetHeight() * 2, AeonGUI::NEAREST );
    renderer.DrawImage ( aeongui_logo, aeongui_logo->GetWidth() * 2, height - aeongui_logo->GetHeight() * 2, aeongui_logo->GetWidth() * 2 , aeongui_logo->GetHeight() * 2, AeonGUI::TILE );
    renderer.DrawImage ( aeongui_logo, aeongui_logo->GetWidth() * 4, height - aeongui_logo->GetHeight() * 2, aeongui_logo->GetWidth() * 2 , aeongui_logo->GetHeight() * 2, AeonGUI::LANCZOS );
    renderer.DrawImage ( aeongui_logo, aeongui_logo->GetWidth() * 6, height - aeongui_logo->GetHeight() * 4, aeongui_logo->GetWidth() * 4 , aeongui_logo->GetHeight() * 4, AeonGUI::LINEAR );
    //renderer.DrawSubImage ( aeongui_logo, 0, height - aeongui_logo->GetHeight() * 2, 4, 4, 56, 56, 56, 56 * 2 );
    //renderer.DrawImage ( aeongui_logo, 0, height - 80, 80, 80 );
#endif
    renderer.EndRender();
    SwapBuffers ( hDC );
    last_time = this_time;
}

void Window::Register ( HINSTANCE hInstance )
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof ( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = ( WNDPROC ) Window::WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof ( Window* );
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon ( NULL, IDI_WINLOGO );
    wcex.hCursor = LoadCursor ( NULL, IDC_ARROW );
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "AeonGUI";
    wcex.hIconSm = NULL;
    Window::atom = RegisterClassEx ( &wcex );
}

LRESULT CALLBACK Window::WindowProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT lresult = 0;
    Window* window_ptr = ( ( Window* ) GetWindowLongPtr ( hwnd, 0 ) );
    switch ( uMsg )
    {
    case WM_PAINT:
        lresult = window_ptr->OnPaint();
        break;
    case WM_CLOSE:
        PostQuitMessage ( 0 );
        break;
    case WM_SIZE:
        lresult = window_ptr->OnSize ( wParam, LOWORD ( lParam ), HIWORD ( lParam ) );
        break;
    case WM_KEYDOWN:
        lresult = DefWindowProc ( hwnd, uMsg, wParam, lParam );
        break;
    case WM_KEYUP:
        lresult = DefWindowProc ( hwnd, uMsg, wParam, lParam );
        break;
    case WM_MOUSEMOVE:
        lresult = window_ptr->OnMouseMove ( GET_X_LPARAM ( lParam ), GET_Y_LPARAM ( lParam ) );
        break;
    case WM_LBUTTONDOWN:
        lresult = window_ptr->OnMouseButtonDown ( 1, GET_X_LPARAM ( lParam ), GET_Y_LPARAM ( lParam ) );
        break;
    case WM_LBUTTONUP:
        lresult = window_ptr->OnMouseButtonUp ( 1, GET_X_LPARAM ( lParam ), GET_Y_LPARAM ( lParam ) );
        break;
    case WM_SETCURSOR:
        if ( LOWORD ( lParam ) == HTCLIENT )
        {
            SetCursor ( NULL );
            return 0;
        }
        else
        {
            return DefWindowProc ( hwnd, uMsg, wParam, lParam );
        }
        break;
    default:
        lresult = DefWindowProc ( hwnd, uMsg, wParam, lParam );
    }
    return lresult;
}

LRESULT Window::OnSize ( WPARAM type, WORD newwidth, WORD newheight )
{
    width = static_cast<int32_t> ( newwidth );
    height = static_cast<int32_t> ( newheight );
    if ( height == 0 )
    {
        height = 1;
    }
    if ( width == 0 )
    {
        width = 1;
    }
    glViewport ( 0, 0, width, height );
    renderer.ChangeScreenSize ( width, height );
    return 0;
}

LRESULT Window::OnPaint()
{
    RECT rect;
    PAINTSTRUCT paint;
    if ( GetUpdateRect ( hWnd, &rect, FALSE ) )
    {
        BeginPaint ( hWnd, &paint );
        EndPaint ( hWnd, &paint );
    }
    return 0;
}

LRESULT Window::OnMouseMove ( int32_t x, int32_t y )
{
    cursor.SetPosition ( x, y );
    window->MouseMove ( x, y );
    return 0;
}

LRESULT Window::OnMouseButtonDown ( uint8_t button, int32_t x, int32_t y )
{
    window->MouseButtonDown ( button, x, y );
    return 0;
}

LRESULT Window::OnMouseButtonUp ( uint8_t button, int32_t x, int32_t y )
{
    window->MouseButtonUp ( button, x, y );
    return 0;
}


int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    Window window;
    window.Initialize ( hInstance );
    MSG msg;
    if ( !AeonGUI::Initialize() )
    {
        return -1;
    }
    memset ( &msg, 0, sizeof ( MSG ) );
    while ( msg.message != WM_QUIT )
    {
        if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( msg.message != WM_QUIT )
            {
                TranslateMessage ( &msg );
                DispatchMessage ( &msg );
            }
        }
        else
        {
            window.RenderLoop();
        }
    }
    assert ( msg.message == WM_QUIT );
    AeonGUI::Finalize();
    window.Finalize();
    return static_cast<int> ( msg.wParam );
}

int main ( int argc, char *argv[] )
{
#ifdef _MSC_VER
#if 0
    _CrtSetBreakAlloc ( 346 );
#endif
#if 1
    // Send all reports to STDOUT
    _CrtSetReportMode ( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile ( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode ( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile ( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode ( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile ( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    // Use _CrtSetBreakAlloc( ) to set breakpoints on allocations.
#endif
#endif
    int ret = WinMain ( GetModuleHandle ( NULL ), NULL, NULL, 0 );
#ifdef _MSC_VER
    _CrtDumpMemoryLeaks();
#endif
    return ret;
}
