/******************************************************************************
Copyright 2010-2012,2015 Rodrigo Hernandez Cordoba

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
#include "Window.h"
#include "agg_pixfmt_rgba.h"
#include "agg_renderer_base.h"
#include "agg_renderer_primitives.h"

namespace AeonGUI
{
    template<typename T> inline int32_t to_24_8 ( T x )
    {
        return x * static_cast<T> ( 256 );
    }

    ATOM Window::atom = 0;

    Window::Window ( HINSTANCE hInstance, LONG aWidth, LONG aHeight ) : hWnd ( nullptr ), hDC ( nullptr ), hRC ( nullptr )
    {
        RECT rect = { 0, 0, aWidth, aHeight };

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
                                nullptr,
                                nullptr,
                                hInstance,
                                this );
        SetWindowLongPtr ( hWnd, 0, ( LONG_PTR ) this );
        hDC = GetDC ( hWnd );
        PIXELFORMATDESCRIPTOR pfd;
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
        int pf = ChoosePixelFormat ( hDC, &pfd );
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
        mRenderer.Initialize();
        glClearColor ( 0, 0, 0, 255 );
        ShowWindow ( hWnd, SW_SHOW );
    }

    Window::~Window()
    {
        mRenderer.Finalize();
        wglMakeCurrent ( hDC, NULL );
        wglDeleteContext ( hRC );
        ReleaseDC ( hWnd, hDC );
        DestroyWindow ( hWnd );
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
        if ( newheight == 0 )
        {
            newheight = 1;
        }
        if ( newwidth == 0 )
        {
            newwidth = 1;
        }
        glViewport ( 0, 0, newwidth, newheight );
        mRenderer.ReSize ( newwidth, newheight );
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
        return 0;
    }

    LRESULT Window::OnMouseButtonDown ( uint8_t button, int32_t x, int32_t y )
    {
        return 0;
    }

    LRESULT Window::OnMouseButtonUp ( uint8_t button, int32_t x, int32_t y )
    {
        return 0;
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

        // Typedefs of the low level renderers to simplify the declarations.
        // Here you can use any other pixel format renderer and
        // agg::renderer_mclip if necessary.
        //--------------------------
        uint8_t* buffer = mRenderer.MapMemory();
        agg::rendering_buffer rbuf ( buffer, mRenderer.SurfaceWidth(), mRenderer.SurfaceHeight(), mRenderer.SurfaceWidth() * 4 );
        agg::pixfmt_bgra32 pixf ( rbuf );
        agg::renderer_base<agg::pixfmt_bgra32> rbase ( pixf );
        rbase.clear ( agg::rgba8 ( 255, 165, 0 ) );
        agg::renderer_primitives<agg::renderer_base<agg::pixfmt_bgra32>> rprim ( rbase );

        rprim.line_color ( agg::rgba8 ( 255, 0, 0 ) );
        rprim.move_to ( to_24_8 ( 380 ), to_24_8 ( 280 ) );
        rprim.line_to ( to_24_8 ( 420 ), to_24_8 ( 320 ) );
        rprim.move_to ( to_24_8 ( 420 ), to_24_8 ( 280 ) );
        rprim.line_to ( to_24_8 ( 380 ), to_24_8 ( 320 ) );

        mRenderer.UnmapMemory();
        mRenderer.Render();
        SwapBuffers ( hDC );
        last_time = this_time;
    }

}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    AeonGUI::Window window ( hInstance, 800, 600 );
    MSG msg;
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
