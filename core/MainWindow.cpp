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
#include "MainWindow.h"
#include <iostream>

#include "resources/close.h"
#include "resources/maximize.h"
#include "resources/minimize.h"
#include "resources/restore.h"
#include "resources/close_down.h"
#include "resources/maximize_down.h"
#include "resources/minimize_down.h"
#include "resources/restore_down.h"

namespace AeonGUI
{
    MainWindow::MainWindow () :
        frameimage ( NULL ),
        caption ( NULL ),
        xoffset ( 0 ),
        yoffset ( 0 ),
        verticalscroll ( ScrollBar::VERTICAL ),
        horizontalscroll ( ScrollBar::HORIZONTAL )
    {
        padding = 4;
        captionheight = 16 + ( padding * 2 );
        captioncolor.r = 64;
        captioncolor.g = 64;
        captioncolor.b = 255;
        captioncolor.a = 255;
        hasborder = true;
        moving = false;
        drawfilled = true;
        Image* image = NULL;
        close.SetParent ( this );

        image = new Image;
        image->Load ( close_width, close_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, close_data );
        close.SetNormalImage ( image );

        image = new Image;
        image->Load ( close_down_width, close_down_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, close_down_data );
        close.SetPressedImage ( image );
        close.SetDimensions ( close_width, close_height );

        maximize.SetParent ( this );
        image = new Image;
        image->Load ( maximize_width, maximize_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, maximize_data );
        maximize.SetNormalImage ( image );

        image = new Image;
        image->Load ( maximize_down_width, maximize_down_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, maximize_down_data );
        maximize.SetPressedImage ( image );
        maximize.SetDimensions ( maximize_width, maximize_height );

        minimize.SetParent ( this );
        image = new Image;
        image->Load ( minimize_width, minimize_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, minimize_data );
        minimize.SetNormalImage ( image );
        image = new Image;
        image->Load ( minimize_down_width, minimize_down_height, AeonGUI::Image::RGBA, AeonGUI::Image::BYTE, minimize_down_data );
        minimize.SetPressedImage ( image );
        minimize.SetDimensions ( minimize_width, minimize_height );

        close.SetPosition ( ( rect.GetWidth() - ( 16 + padding ) ), padding );
        maximize.SetPosition ( ( rect.GetWidth() - ( 32 + padding * 2 ) ), padding );
        minimize.SetPosition ( ( rect.GetWidth() - ( 48 + padding * 3 ) ), padding );

        verticalscroll.SetParent ( this );
        verticalscroll.SetDimensions ( 16, rect.GetHeight() - ( bordersize * 2 ) - captionheight - 16 );
        verticalscroll.SetPosition ( rect.GetWidth() - ( 16 + bordersize ), bordersize + captionheight );

        horizontalscroll.SetParent ( this );
        horizontalscroll.SetDimensions ( rect.GetWidth() - ( bordersize * 2 ) - 16, 16 );
        horizontalscroll.SetPosition ( bordersize, rect.GetHeight() - ( 16 + bordersize ) );
    }

    MainWindow::~MainWindow()
    {
        if ( caption != NULL )
        {
            delete[] caption;
        }
        delete close.GetNormalImage();
        delete maximize.GetNormalImage();
        delete minimize.GetNormalImage() ;
        delete close.GetPressedImage();
        delete maximize.GetPressedImage();
        delete minimize.GetPressedImage() ;
    }

    void MainWindow::SetCaption ( const wchar_t* newcaption )
    {
        size_t len = wcslen ( newcaption );
        if ( caption != NULL )
        {
            delete[] caption;
        }
        if ( len == 0 )
        {
            return;
        }
        caption = new wchar_t[len + 1];
        wcscpy ( caption, newcaption );
    }

    void MainWindow::SetFrameImage ( const Image* image )
    {
        frameimage = image;
    }

    void MainWindow::OnRender ( Renderer* renderer )
    {
        Widget::OnRender ( renderer );
        Rect screenrect;
        if ( frameimage != NULL )
        {
            GetScreenRect ( &screenrect );
            renderer->DrawImage ( frameimage, screenrect.GetX(), screenrect.GetY(), screenrect.GetWidth(), screenrect.GetHeight() );
            renderer->DrawString (
                textcolor,
                screenrect.GetX() + frameimage->GetPadXStart(),
                screenrect.GetY() + renderer->GetFont()->GetHeight() + renderer->GetFont()->GetDescender() + frameimage->GetPadYStart(),
                caption );
        }
    }

    void MainWindow::OnMouseButtonDown ( uint8_t button, uint32_t X, uint32_t Y )
    {
        int x = X;
        int y = Y;
        ScreenToClientCoords ( x, y );
        if ( ( frameimage != NULL ) && ( y < static_cast<int32_t> ( frameimage->GetStretchYStart() ) ) )
        {
            std::cout << "Caption Down " << std::dec << static_cast<int> ( button ) << std::endl;
            moving = true;
            xoffset = X - rect.GetLeft();
            yoffset = Y - rect.GetTop();
        }
    }

    void MainWindow::OnMouseButtonUp ( uint8_t button, uint32_t X, uint32_t Y )
    {
        int x = X;
        int y = Y;
        ScreenToClientCoords ( x, y );
        if ( ( frameimage != NULL ) && ( y < static_cast<int32_t> ( frameimage->GetStretchYStart() ) ) )
        {
            std::cout << "Caption Up " << std::dec << static_cast<int> ( button ) << std::endl;
            moving = false;
        }
    }

    bool MainWindow::OnMouseClick ( uint8_t button, uint32_t x, uint32_t y, Widget* clicked_widget )
    {
        if ( clicked_widget == &close )
        {
            std::cout << "Close" << std::endl;
        }
        else if ( clicked_widget == &minimize )
        {
            std::cout << "Minimize" << std::endl;
        }
        else if ( clicked_widget == &maximize )
        {
            std::cout << "Maximize" << std::endl;
        }
        return true;
    }

    void MainWindow::OnMouseMove ( uint32_t X, uint32_t Y )
    {
        if ( moving )
        {
            rect.SetPosition ( X - xoffset, Y - yoffset );
        }
    }
}
