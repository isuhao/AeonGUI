#ifndef AEONGUI_MAINWINDOW_H
#define AEONGUI_MAINWINDOW_H
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
#include "Platform.h"
#include "Widget.h"
#include "Button.h"
#include "ScrollBar.h"

namespace AeonGUI
{
    /*! \brief Top window with borders and frame. */
    class DLL MainWindow : public Widget
    {
    public:
        MainWindow();
        virtual ~MainWindow();
        /*! \brief Set the window caption to be displayed on the window frame.
            \param newcaption Text for the caption or NULL to unset.
        */
        void SetCaption ( const wchar_t* newcaption );

        /*! \brief Set the window frame image.
            \param newframe Pointer to new frame image or NULL to unset.
        */
        void SetFrameImage ( const Image* image );

        /*! \copydoc Widget::OnRender */
        virtual void OnRender ( Renderer* renderer );

    protected:

        /*! \copydoc Widget::OnMouseButtonDown */
        virtual void OnMouseButtonDown ( uint8_t button, uint32_t x, uint32_t y );
        /*! \copydoc Widget::OnMouseButtonUp */
        virtual void OnMouseButtonUp ( uint8_t button, uint32_t x, uint32_t y );
        /*! \copydoc Widget::OnMouseClick */
        virtual bool OnMouseClick ( uint8_t button, uint32_t x, uint32_t y, Widget* clicked_widget );

        /*! \copydoc Widget::OnMouseMove */
        virtual void OnMouseMove ( uint32_t x, uint32_t y );

        const Image* frameimage;
        wchar_t* caption;               ///< Caption text.
        uint32_t padding;               ///< Window content padding. (to be removed)
        uint32_t captionheight;         ///< Caption height. (to be removed)
        Color captioncolor;             ///< Caption text color.
        uint16_t xoffset;               ///< Mouse X offset for move operation.
        uint16_t yoffset;               ///< Mouse Y offset for move operation.
        bool moving;                    ///< Wether the window is currently moving.
        Button close;                   ///< Close window button.
        Button maximize;                ///< Maximize window button.
        Button minimize;                ///< Minimize window button.
        ScrollBar verticalscroll;       ///< Window vertical scroll control.
        ScrollBar horizontalscroll;     ///< Window horizontal scroll control
    };
}
#endif
