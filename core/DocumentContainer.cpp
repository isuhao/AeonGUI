#include "DocumentContainer.h"
#include "Log.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>

namespace AeonGUI
{
#include "OpenSans_Bold_ttf.h"
#include "OpenSans_Light_ttf.h"
#include "OpenSans_BoldItalic_ttf.h"
#include "OpenSans_LightItalic_ttf.h"
#include "OpenSans_ExtraBold_ttf.h"
#include "OpenSans_Regular_ttf.h"
#include "OpenSans_ExtraBoldItalic_ttf.h"
#include "OpenSans_Semibold_ttf.h"
#include "OpenSans_Italic_ttf.h"
#include "OpenSans_SemiboldItalic_ttf.h"

    DocumentContainer::DocumentContainer() : mFreeType ( nullptr )
    {
        FT_Error ft_error;
        if ( ( ft_error = FT_Init_FreeType ( &mFreeType ) ) != 0 )
        {
            AEONGUI_LOG_ERROR ( "FT_Init_FreeType returned error code 0x%02x", ft_error );
        }
    }

    DocumentContainer::~DocumentContainer()
    {
        FT_Error ft_error;
        for ( auto i = mFonts.begin(); i != mFonts.end(); ++i )
        {
            if ( ( ft_error = FT_Done_Face ( i->face ) ) != 0 )
            {
                AEONGUI_LOG_WARN ( "FT_Done_Face returned error code 0x%02x", ft_error );
            }
        }
        if ( ( ft_error = FT_Done_FreeType ( mFreeType ) ) != 0 )
        {
            AEONGUI_LOG_WARN ( "FT_Done_FreeType returned error code 0x%02x", ft_error );
        }
    }

    litehtml::uint_ptr DocumentContainer::create_font ( const litehtml::tchar_t * faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics * fm )
    {
        /* Hardcoding any font request to OpenSans for now. */
        Font font = {size, weight, italic, nullptr};
        std::vector<Font>::iterator it = std::lower_bound ( mFonts.begin(), mFonts.end(), font );
        if ( ( it != mFonts.end() ) && ( *it == font ) )
        {
            return reinterpret_cast<litehtml::uint_ptr> ( it->face );
        }
        unsigned char* buffer;
        unsigned int  buffer_size;
        switch ( italic )
        {
        case litehtml::fontStyleNormal:
            if ( size < 400 )
            {
                buffer = OpenSans_Light_ttf;
                buffer_size = OpenSans_Light_ttf_len;
            }
            else if ( size < 600 )
            {
                buffer = OpenSans_Regular_ttf;
                buffer_size = OpenSans_Regular_ttf_len;
            }
            else if ( size < 700 )
            {
                buffer = OpenSans_Semibold_ttf;
                buffer_size = OpenSans_Semibold_ttf_len;
            }
            else if ( size < 800 )
            {
                buffer = OpenSans_Bold_ttf;
                buffer_size = OpenSans_Bold_ttf_len;
            }
            else
            {
                buffer = OpenSans_ExtraBold_ttf;
                buffer_size = OpenSans_ExtraBold_ttf_len;
            }
            break;
        case litehtml::fontStyleItalic:
            if ( size < 400 )
            {
                buffer = OpenSans_LightItalic_ttf;
                buffer_size = OpenSans_LightItalic_ttf_len;
            }
            else if ( size < 600 )
            {
                buffer = OpenSans_Italic_ttf;
                buffer_size = OpenSans_Italic_ttf_len;
            }
            else if ( size < 700 )
            {
                buffer = OpenSans_SemiboldItalic_ttf;
                buffer_size = OpenSans_SemiboldItalic_ttf_len;
            }
            else if ( size < 800 )
            {
                buffer = OpenSans_BoldItalic_ttf;
                buffer_size = OpenSans_BoldItalic_ttf_len;
            }
            else
            {
                buffer = OpenSans_ExtraBoldItalic_ttf;
                buffer_size = OpenSans_ExtraBoldItalic_ttf_len;
            }
            break;
        }
        FT_Error ft_error;
        if ( ( ft_error = FT_New_Memory_Face ( mFreeType, buffer, buffer_size, 0, &font.face ) ) != 0 )
        {
            AEONGUI_LOG_ERROR ( "FT_New_Memory_Face returned error code 0x%02x", ft_error );
            return NULL;
        }
        if ( ft_error = FT_Set_Pixel_Sizes ( font.face, 0, font.size ) )
        {
            AEONGUI_LOG_ERROR ( "FT_Set_Pixel_Sizes returned error code 0x%02x", ft_error );
            FT_Done_Face ( font.face );
            return NULL;
        }
        if ( fm != nullptr )
        {
            fm->height = font.face->height;
            fm->ascent = font.face->ascender;
            fm->descent = font.face->descender;
        }
        return reinterpret_cast<litehtml::uint_ptr> ( mFonts.insert ( it, font )->face );
    }

    void DocumentContainer::delete_font ( litehtml::uint_ptr hFont )
    {
        std::vector<Font>::iterator it = std::remove_if ( mFonts.begin(), mFonts.end(),
                                         [ = ] ( const Font & aFont )
        {
            return ( aFont.face == reinterpret_cast<FT_Face> ( hFont ) );
        } );
        if ( it != mFonts.end() )
        {
            mFonts.erase ( it, mFonts.end() );
            FT_Error ft_error;
            if ( ( ft_error = FT_Done_Face ( reinterpret_cast<FT_Face> ( hFont ) ) ) != 0 )
            {
                AEONGUI_LOG_ERROR ( "FT_Done_Face returned error code 0x%02x", ft_error );
            }
        }
    }

    int DocumentContainer::text_width ( const litehtml::tchar_t * text, litehtml::uint_ptr hFont )
    {
        return 0;
    }

    void DocumentContainer::draw_text ( litehtml::uint_ptr hdc, const litehtml::tchar_t * text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position & pos )
    {
    }

    int DocumentContainer::pt_to_px ( int pt )
    {
        /*  This code is temporary.
            Instead use DPI aware code:
            https://msdn.microsoft.com/en-us/library/ms701681%28v=vs.85%29.aspx
            https://msdn.microsoft.com/en-us/library/windows/desktop/dn469266%28v=vs.85%29.aspx
            and eventually find what the solution for Linux is.*/
        HDC dc = GetDC ( NULL );
        int ret = MulDiv ( pt, GetDeviceCaps ( dc, LOGPIXELSY ), 72 );
        ReleaseDC ( NULL, dc );
        return ret;
    }

    int DocumentContainer::get_default_font_size() const
    {
        return 400;
    }

    const litehtml::tchar_t * DocumentContainer::get_default_font_name() const
    {
        return TEXT ( "opensans" );
    }

    void DocumentContainer::draw_list_marker ( litehtml::uint_ptr hdc, const litehtml::list_marker & marker )
    {
    }

    void DocumentContainer::load_image ( const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl, bool redraw_on_ready )
    {
    }

    void DocumentContainer::get_image_size ( const litehtml::tchar_t * src, const litehtml::tchar_t * baseurl, litehtml::size & sz )
    {
    }

    void DocumentContainer::draw_background ( litehtml::uint_ptr hdc, const litehtml::background_paint & bg )
    {
    }

    void DocumentContainer::draw_borders ( litehtml::uint_ptr hdc, const litehtml::borders & borders, const litehtml::position & draw_pos, bool root )
    {
    }

    void DocumentContainer::set_caption ( const litehtml::tchar_t * caption )
    {
    }

    void DocumentContainer::set_base_url ( const litehtml::tchar_t * base_url )
    {
    }

    void DocumentContainer::link ( const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr & el )
    {
    }

    void DocumentContainer::on_anchor_click ( const litehtml::tchar_t * url, const litehtml::element::ptr & el )
    {
    }

    void DocumentContainer::set_cursor ( const litehtml::tchar_t * cursor )
    {
    }

    void DocumentContainer::transform_text ( litehtml::tstring & text, litehtml::text_transform tt )
    {
    }

    void DocumentContainer::import_css ( litehtml::tstring & text, const litehtml::tstring & url, litehtml::tstring & baseurl )
    {
    }

    void DocumentContainer::set_clip ( const litehtml::position & pos, const litehtml::border_radiuses & bdr_radius, bool valid_x, bool valid_y )
    {
    }

    void DocumentContainer::del_clip()
    {
    }

    void DocumentContainer::get_client_rect ( litehtml::position & client ) const
    {
    }

    std::shared_ptr<litehtml::element> DocumentContainer::create_element ( const litehtml::tchar_t * tag_name, const litehtml::string_map & attributes, const std::shared_ptr<litehtml::document>& doc )
    {
        return std::shared_ptr<litehtml::element>();
    }

    void DocumentContainer::get_media_features ( litehtml::media_features & media ) const
    {
    }

    void DocumentContainer::get_language ( litehtml::tstring & language, litehtml::tstring & culture ) const
    {
    }

    /*--------------------------------------------------------------------------------------------------------------------------------*/

    bool DocumentContainer::Font::operator< ( const Font & aRhs )
    {
        if ( size < aRhs.size )
        {
            return true;
        }
        else if ( ( size == aRhs.size ) && ( weight < aRhs.weight ) )
        {
            return true;
        }
        else if ( ( size == aRhs.size ) && ( weight == aRhs.weight ) && ( italic < aRhs.italic ) )
        {
            return true;
        }
        return false;
    }

    bool DocumentContainer::Font::operator== ( const Font & aRhs )
    {
        return ( ( size == aRhs.size ) && ( size == aRhs.size ) && ( italic == aRhs.italic ) );
    }
}
