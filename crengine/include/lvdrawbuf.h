/** \file lvdrawbuf.h
    \brief Drawing buffer, gray bitmap buffer

    CoolReader Engine

    (c) Vadim Lopatin, 2000-2006
    This source code is distributed under the terms of
    GNU General Public License.

    See LICENSE file for details.

*/

#ifndef __LVDRAWBUF_H_INCLUDED__
#define __LVDRAWBUF_H_INCLUDED__

#include "crsetup.h"

#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
extern "C" {
#include <windows.h>
}
#elif __SYMBIAN32__
#include <e32base.h>
#include <w32std.h>
#endif

#include "lvtypes.h"
#include "lvimg.h"

enum cr_rotate_angle_t {
    CR_ROTATE_ANGLE_0 = 0,
    CR_ROTATE_ANGLE_90,
    CR_ROTATE_ANGLE_180,
    CR_ROTATE_ANGLE_270
};

class LVFont;
class GLDrawBuf; // workaround for no-rtti builds

/// Abstract drawing buffer
class LVDrawBuf : public CacheableObject
{
public:
    // GL draw buffer support
    /// GL draw buffer compatibility - requires this call before any drawing
    virtual void beforeDrawing() {}
    /// GL draw buffer compatibility - requires this call after any drawing
    virtual void afterDrawing() {}

    // tiles support
    /// returns true if drawing buffer is tiled
    virtual bool isTiled() { return false; }
    /// returns tile width (or just width if no tiles)
    virtual int tileWidth() { return GetWidth(); }
    /// returns tile height (or just height if no tiles)
    virtual int tileHeight() { return GetHeight(); }
    /// returns tile drawbuf for tiled image, returns this for non tiled draw buffer
    virtual LVDrawBuf * getTile(int x, int y) {
        CR_UNUSED2(x, y);
        return this;
    }
    /// returns number of tiles in row
    virtual int getXtiles() {
        return 1;
    }
    /// returns number of tiles in column
    virtual int getYtiles() {
        return 1;
    }

    /// returns tile rectangle
    virtual void getTileRect(lvRect & rc, int x, int y) {
        CR_UNUSED2(x, y);
        rc.left = rc.top = 0;
        rc.right = GetWidth();
        rc.bottom = GetHeight();
    }

    /// rotates buffer contents by specified angle
    virtual void Rotate( cr_rotate_angle_t angle ) = 0;
    /// returns white pixel value
    virtual lUInt32 GetWhiteColor() = 0;
    /// returns black pixel value
    virtual lUInt32 GetBlackColor() = 0;
    /// returns current background color
    virtual lUInt32 GetBackgroundColor() = 0;
    /// sets current background color
    virtual void SetBackgroundColor( lUInt32 cl ) = 0;
    /// returns current text color
    virtual lUInt32 GetTextColor() = 0;
    /// sets current text color
    virtual void SetTextColor( lUInt32 cl ) = 0;
    /// gets clip rect
    virtual void GetClipRect( lvRect * clipRect ) = 0;
    /// sets clip rect
    virtual void SetClipRect( const lvRect * clipRect ) = 0;
    /// set to true for drawing in Paged mode, false for Scroll mode
    virtual void setHidePartialGlyphs( bool hide ) = 0;
    /// set to true to invert images only (so they get inverted back to normal by nightmode)
    virtual void setInvertImages( bool invert ) = 0;
    /// set to true to enforce dithering (only relevant for 8bpp Gray drawBuf)
    virtual void setDitherImages( bool dither ) = 0;
    /// set to true to switch to a more costly smooth scaler instead of nearest neighbor
    virtual void setSmoothScalingImages( bool smooth ) = 0;
    /// invert image
    virtual void  Invert() = 0;
    /// get buffer width, pixels
    virtual int  GetWidth() = 0;
    /// get buffer height, pixels
    virtual int  GetHeight() = 0;
    /// get buffer bits per pixel
    virtual int  GetBitsPerPixel() = 0;
    /// fills buffer with specified color
    virtual int  GetRowSize() = 0;
    /// fills buffer with specified color
    virtual void Clear( lUInt32 color ) = 0;
    /// get pixel value
    virtual lUInt32 GetPixel( int x, int y ) = 0;
    /// get average pixel value for area (coordinates are fixed floating points *16)
    virtual lUInt32 GetAvgColor(lvRect & rc16) = 0;
    /// get linearly interpolated pixel value (coordinates are fixed floating points *16)
    virtual lUInt32 GetInterpolatedColor(int x16, int y16) = 0;
    /// draw gradient filled rectangle with colors for top-left, top-right, bottom-right, bottom-left
    virtual void GradientRect(int x0, int y0, int x1, int y1, lUInt32 color1, lUInt32 color2, lUInt32 color3, lUInt32 color4) {
        CR_UNUSED8(x0, x1, y0, y1, color1, color2, color3, color4);
    }
    /// fills rectangle with specified color
    virtual void FillRect( int x0, int y0, int x1, int y1, lUInt32 color ) = 0;
    /// draw frame
    inline void DrawFrame(const lvRect & rc, lUInt32 color, int width = 1)
    {
        FillRect( rc.left, rc.top, rc.right, rc.top + width, color );
        FillRect( rc.left, rc.bottom - width, rc.right, rc.bottom, color );
        FillRect( rc.left, rc.top + width, rc.left + width, rc.bottom - width, color );
        FillRect( rc.right - width, rc.top + width, rc.right, rc.bottom - width, color );
    }
    /// fills rectangle with specified color
    inline void FillRect( const lvRect & rc, lUInt32 color )
    {
        FillRect( rc.left, rc.top, rc.right, rc.bottom, color );
    }
    /// draws rectangle with specified color
    inline void Rect( int x0, int y0, int x1, int y1, lUInt32 color )
    {
        FillRect( x0, y0, x1-1, y0+1, color );
        FillRect( x0, y0, x0+1, y1-1, color );
        FillRect( x1-1, y0, x1, y1, color );
        FillRect( x0, y1-1, x1, y1, color );
    }
    /// draws rectangle with specified width and color
    inline void Rect( int x0, int y0, int x1, int y1, int borderWidth, lUInt32 color )
    {
        FillRect( x0, y0, x1-1, y0+borderWidth, color );
        FillRect( x0, y0, x0+borderWidth, y1-1, color );
        FillRect( x1-borderWidth, y0, x1, y1, color );
        FillRect( x0, y1-borderWidth, x1, y1, color );
    }
    /// draws rounded rectangle with specified line width, rounding radius, and color
    void RoundRect( int x0, int y0, int x1, int y1, int borderWidth, int radius, lUInt32 color, int cornerFlags=0x0F  );
    /// draws rectangle with specified color
    inline void Rect( const lvRect & rc, lUInt32 color )
    {
        Rect( rc.left, rc.top, rc.right, rc.bottom, color );
    }
    /// draws rectangle with specified color
    inline void Rect( const lvRect & rc, int borderWidth, lUInt32 color )
    {
        Rect( rc.left, rc.top, rc.right, rc.bottom, borderWidth, color );
    }
    /// fills rectangle with pattern
    virtual void FillRectPattern( int x0, int y0, int x1, int y1, lUInt32 color0, lUInt32 color1, lUInt8 * pattern ) = 0;
    /// inverts image in specified rectangle
    virtual void InvertRect(int x0, int y0, int x1, int y1) = 0;
    /// sets new size
    virtual void Resize( int dx, int dy ) = 0;
    /// draws bitmap (1 byte per pixel) using specified palette
    virtual void Draw( int x, int y, const lUInt8 * bitmap, int width, int height, lUInt32 * palette ) = 0;
    /// draws image
    virtual void Draw( LVImageSourceRef img, int x, int y, int width, int height, bool dither=true ) = 0;
    /// draws part of source image, possible rescaled
    virtual void Draw( LVImageSourceRef img, int x, int y, int width, int height, int srcx, int srcy, int srcwidth, int srcheight, bool dither=true ) { CR_UNUSED10(img, x, y, width, height, srcx, srcy, srcwidth, srcheight, dither); }
    /// for GL buf only - rotated drawing
    virtual void DrawRotated( LVImageSourceRef img, int x, int y, int width, int height, int rotationAngle) { Draw(img, x, y, width, height); CR_UNUSED(rotationAngle); }
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( LVDrawBuf * buf, int x, int y, int options, lUInt32 * palette ) = 0;
    // draws buffer on top of another buffer to implement background
    virtual void DrawOnTop( LVDrawBuf * buf, int x, int y) = 0;
    /// draws rescaled buffer content to another buffer doing color conversion if necessary
    virtual void DrawRescaled(LVDrawBuf * src, int x, int y, int dx, int dy, int options) = 0;
    /// draws rescaled buffer content to another buffer doing color conversion if necessary
    virtual void DrawFragment(LVDrawBuf * src, int srcx, int srcy, int srcdx, int srcdy, int x, int y, int dx, int dy, int options) {
        CR_UNUSED10(src, srcx, srcy, srcdx, srcdy, x, y, dx, dy, options);
    }
    /// draw lines
    virtual void DrawLine(int x0,int y0,int x1,int y1,lUInt32 color0 ,int length1,int length2,int direction)=0;
#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( HDC dc, int x, int y, int options, lUInt32 * palette ) = 0;
#endif
    /// draws text string
    /*
    virtual void DrawTextString( int x, int y, LVFont * pfont,
                       const lChar16 * text, int len,
                       lChar16 def_char, lUInt32 * palette, bool addHyphen=false ) = 0;
    */

/*
    /// draws formatted text
    virtual void DrawFormattedText( formatted_text_fragment_t * text, int x, int y ) = 0;
*/
    /// returns scanline pointer
    virtual lUInt8 * GetScanLine( int y ) = 0;


    virtual int getAlpha() { return 0; }
    virtual void setAlpha(int alpha) { CR_UNUSED(alpha); }
    virtual lUInt32 applyAlpha(lUInt32 cl) { return cl; }

    /// virtual destructor
    virtual ~LVDrawBuf() { }
    virtual GLDrawBuf * asGLDrawBuf() { return NULL; }
};

/// LVDrawBufferBase
class LVBaseDrawBuf : public LVDrawBuf
{
protected:
    int _dx;
    int _dy;
    int _rowsize;
    lvRect _clip;
    unsigned char * _data;
    lUInt32 _backgroundColor;
    lUInt32 _textColor;
    bool _hidePartialGlyphs;
    bool _invertImages;
    bool _ditherImages;
    bool _smoothImages;
    int _drawnImagesCount;
    int _drawnImagesSurface;
public:
    /// set to true for drawing in Paged mode, false for Scroll mode
    virtual void setHidePartialGlyphs( bool hide ) { _hidePartialGlyphs = hide; }
    /// set to true to invert images only (so they get inverted back to normal by nightmode)
    virtual void setInvertImages( bool invert ) { _invertImages = invert; }
    /// set to true to enforce dithering (only relevant for 8bpp Gray drawBuf)
    virtual void setDitherImages( bool dither ) { _ditherImages = dither; }
    /// set to true to switch to a more costly smooth scaler instead of nearest neighbor
    virtual void setSmoothScalingImages( bool smooth ) { _smoothImages = smooth; }
    /// returns current background color
    virtual lUInt32 GetBackgroundColor() { return _backgroundColor; }
    /// sets current background color
    virtual void SetBackgroundColor( lUInt32 cl ) { _backgroundColor=cl; }
    /// returns current text color
    virtual lUInt32 GetTextColor() { return _textColor; }
    /// sets current text color
    virtual void SetTextColor( lUInt32 cl ) { _textColor = cl; }
    /// gets clip rect
    virtual void GetClipRect( lvRect * clipRect ) { *clipRect = _clip; }
    /// sets clip rect
    virtual void SetClipRect( const lvRect * clipRect );
    /// get average pixel value for area (coordinates are fixed floating points *16)
    virtual lUInt32 GetAvgColor(lvRect & rc16);
    /// get linearly interpolated pixel value (coordinates are fixed floating points *16)
    virtual lUInt32 GetInterpolatedColor(int x16, int y16);
    /// get buffer width, pixels
    virtual int  GetWidth();
    /// get buffer height, pixels
    virtual int  GetHeight();
    /// get row size (bytes)
    virtual int  GetRowSize() { return _rowsize; }
    virtual void DrawLine(int x0, int y0, int x1, int y1, lUInt32 color0,int length1,int length2,int direction)=0;
    /// draws text string
    /*
    virtual void DrawTextString( int x, int y, LVFont * pfont,
                       const lChar16 * text, int len,
                       lChar16 def_char,
                       lUInt32 * palette, bool addHyphen=false );
    */
    /// draws formatted text
    //virtual void DrawFormattedText( formatted_text_fragment_t * text, int x, int y );

    /// Get nb of images drawn on buffer
    int getDrawnImagesCount() { return _drawnImagesCount; }
    /// Get surface of images drawn on buffer
    int getDrawnImagesSurface() { return _drawnImagesSurface; }

    LVBaseDrawBuf() : _dx(0), _dy(0), _rowsize(0), _data(NULL), _hidePartialGlyphs(true),
                        _invertImages(false), _ditherImages(false), _smoothImages(false),
                        _drawnImagesCount(0), _drawnImagesSurface(0) { }
    virtual ~LVBaseDrawBuf() { }
};

/// use to simplify saving draw buffer state
class LVDrawStateSaver
{
    LVDrawBuf & _buf;
    lUInt32 _textColor;
    lUInt32 _backgroundColor;
    int _alpha;
    lvRect _clipRect;
	LVDrawStateSaver & operator = (LVDrawStateSaver &) {
		// no assignment
        return *this;
	}
public:
    /// save settings
    LVDrawStateSaver( LVDrawBuf & buf )
    : _buf( buf )
    , _textColor( buf.GetTextColor() )
    , _backgroundColor( buf.GetBackgroundColor() )
    , _alpha(buf.getAlpha())
    {
        _buf.GetClipRect( &_clipRect );
    }
    void restore()
    {
        _buf.SetTextColor( _textColor );
        _buf.SetBackgroundColor( _backgroundColor );
        _buf.setAlpha(_alpha);
        _buf.SetClipRect( &_clipRect );
    }
    /// restore settings on destroy
    ~LVDrawStateSaver()
    {
        restore();
    }
};

#define SAVE_DRAW_STATE( buf ) LVDrawStateSaver drawBufSaver( buf )

enum DrawBufPixelFormat
{
    DRAW_BUF_1_BPP = 1, /// 1 bpp, 8 pixels per byte packed
    DRAW_BUF_2_BPP = 2, /// 2 bpp, 4 pixels per byte packed
    DRAW_BUF_3_BPP = 3, /// 3 bpp, 1 pixel per byte, higher 3 bits are significant
    DRAW_BUF_4_BPP = 4, /// 4 bpp, 1 pixel per byte, higher 4 bits are significant
    DRAW_BUF_8_BPP = 8, /// 8 bpp, 1 pixel per byte, all 8 bits are significant
    DRAW_BUF_16_BPP = 16, /// color 16bit RGB 565
    DRAW_BUF_32_BPP = 32  /// color 32bit RGB 888
};

/**
 * 2-bit gray bitmap buffer, partial support for 1-bit buffer
 * Supported pixel formats for LVGrayDrawBuf :
 *    1 bpp, 8 pixels per byte packed
 *    2 bpp, 4 pixels per byte packed
 *    3 bpp, 1 pixel per byte, higher 3 bits are significant
 *    4 bpp, 1 pixel per byte, higher 4 bits are significant
 *    8 bpp, 1 pixel per byte, all 8 bits are significant
 *
 */
class LVGrayDrawBuf : public LVBaseDrawBuf
{
private:
    int _bpp;
    bool _ownData;
public:
    /// rotates buffer contents by specified angle
    virtual void Rotate( cr_rotate_angle_t angle );
    /// returns white pixel value
    virtual lUInt32 GetWhiteColor();
    /// returns black pixel value
    virtual lUInt32 GetBlackColor();
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( LVDrawBuf * buf, int x, int y, int options, lUInt32 * palette );
    // draws buffer on top of another buffer to implement background
    virtual void DrawOnTop( LVDrawBuf * buf, int x, int y);
    /// draws rescaled buffer content to another buffer doing color conversion if necessary
    virtual void DrawRescaled(LVDrawBuf * src, int x, int y, int dx, int dy, int options);
#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( HDC dc, int x, int y, int options, lUInt32 * palette );
#endif
    /// invert image
    virtual void  Invert();
    /// get buffer bits per pixel
    virtual int  GetBitsPerPixel();
    /// returns scanline pointer
    virtual lUInt8 * GetScanLine( int y );
    /// fills buffer with specified color
    virtual void Clear( lUInt32 color );
    /// get pixel value
    virtual lUInt32 GetPixel( int x, int y );
    /// fills rectangle with specified color
    virtual void FillRect( int x0, int y0, int x1, int y1, lUInt32 color );
    /// inverts image in specified rectangle
    virtual void InvertRect( int x0, int y0, int x1, int y1 );
    /// fills rectangle with pattern
    virtual void FillRectPattern( int x0, int y0, int x1, int y1, lUInt32 color0, lUInt32 color1, lUInt8 * pattern );
    /// sets new size
    virtual void Resize( int dx, int dy );
    /// draws image
    virtual void Draw( LVImageSourceRef img, int x, int y, int width, int height, bool dither );
    /// draws bitmap (1 byte per pixel) using specified palette
    virtual void Draw( int x, int y, const lUInt8 * bitmap, int width, int height, lUInt32 * palette );
    /// constructor
    LVGrayDrawBuf(int dx, int dy, int bpp=2, void * auxdata = NULL );
    /// destructor
    virtual ~LVGrayDrawBuf();
    /// convert to 1-bit bitmap
    void ConvertToBitmap(bool flgDither);
    virtual void DrawLine(int x0, int y0, int x1, int y1, lUInt32 color0,int length1,int length2,int direction=0);
};

// NOTE: By default, CRe assumes RGB (array order) actually means BGR
//       We don't, so, instead of fixing this at the root (i.e., in a *lot* of places),
//       we simply swap R<->B when rendering to 32bpp, limiting the tweaks to lvdrawbuf
//       c.f., https://github.com/koreader/koreader-base/pull/878#issuecomment-476723747
#ifdef CR_RENDER_32BPP_RGB_PXFMT
inline lUInt32 RevRGB( lUInt32 cl ) {
    return ((cl<<16)&0xFF0000) | ((cl>>16)&0x0000FF) | (cl&0x00FF00);
}

inline lUInt32 RevRGBA( lUInt32 cl ) {
    // Swap B <-> R, keep G & A
    return ((cl<<16)&0x00FF0000) | ((cl>>16)&0x000000FF) | (cl&0xFF00FF00);
}
#else
inline lUInt32 RevRGB( lUInt32 cl ) {
    return cl;
}

inline lUInt32 RevRGBA( lUInt32 cl ) {
    return cl;
}
#endif

inline lUInt32 rgb565to888( lUInt32 cl ) {
    return ((cl & 0xF800)<<8) | ((cl & 0x07E0)<<5) | ((cl & 0x001F)<<3);
}

inline lUInt16 rgb888to565( lUInt32 cl ) {
    return (lUInt16)(((cl>>8)& 0xF800) | ((cl>>5 )& 0x07E0) | ((cl>>3 )& 0x001F));
}

#define DIV255(V)                                                                                        \
({                                                                                                       \
	auto _v = (V) + 128;                                                                             \
	(((_v >> 8U) + _v) >> 8U);                                                                       \
})

// Because of course we're not using <stdint.h> -_-".
#ifndef UINT8_MAX
	#define UINT8_MAX (255)
#endif

// Quantize an 8-bit color value down to a palette of 16 evenly spaced colors, using an ordered 8x8 dithering pattern.
// With a grayscale input, this happens to match the eInk palette perfectly ;).
// If the input is not grayscale, and the output fb is not grayscale either,
// this usually still happens to match the eInk palette after the EPDC's own quantization pass.
// c.f., https://en.wikipedia.org/wiki/Ordered_dithering
// & https://github.com/ImageMagick/ImageMagick/blob/ecfeac404e75f304004f0566557848c53030bad6/MagickCore/threshold.c#L1627
// NOTE: As the references imply, this is straight from ImageMagick,
//       with only minor simplifications to enforce Q8 & avoid fp maths.
static inline lUInt8 dither_o8x8(int x, int y, lUInt8 v)
{
	// c.f., https://github.com/ImageMagick/ImageMagick/blob/ecfeac404e75f304004f0566557848c53030bad6/config/thresholds.xml#L107
	static const lUInt8 threshold_map_o8x8[] = { 1,  49, 13, 61, 4,  52, 16, 64, 33, 17, 45, 29, 36, 20, 48, 32,
						      9,  57, 5,  53, 12, 60, 8,  56, 41, 25, 37, 21, 44, 28, 40, 24,
						      3,  51, 15, 63, 2,  50, 14, 62, 35, 19, 47, 31, 34, 18, 46, 30,
						      11, 59, 7,  55, 10, 58, 6,  54, 43, 27, 39, 23, 42, 26, 38, 22 };

	// Constants:
	// Quantum = 8; Levels = 16; map Divisor = 65
	// QuantumRange = 0xFF
	// QuantumScale = 1.0 / QuantumRange
	//
	// threshold = QuantumScale * v * ((L-1) * (D-1) + 1)
	// NOTE: The initial computation of t (specifically, what we pass to DIV255) would overflow an uint8_t.
	//       With a Q8 input value, we're at no risk of ever underflowing, so, keep to unsigned maths.
	//       Technically, an uint16_t would be wide enough, but it gains us nothing,
	//       and requires a few explicit casts to make GCC happy ;).
	lUInt32 t = DIV255(v * ((15U << 6) + 1U));
	// level = t / (D-1);
	lUInt32 l = (t >> 6);
	// t -= l * (D-1);
	t = (t - (l << 6));

	// map width & height = 8
	// c = ClampToQuantum((l+(t >= map[(x % mw) + mw * (y % mh)])) * QuantumRange / (L-1));
	lUInt32 q = ((l + (t >= threshold_map_o8x8[(x & 7U) + 8U * (y & 7U)])) * 17);
	// NOTE: We're doing unsigned maths, so, clamping is basically MIN(q, UINT8_MAX) ;).
	//       The only overflow we should ever catch should be for a few white (v = 0xFF) input pixels
	//       that get shifted to the next step (i.e., q = 272 (0xFF + 17)).
	return (q > UINT8_MAX ? UINT8_MAX : static_cast<lUInt8>(q));
}

// Declare our bit of scaler ripped from Qt5...
namespace CRe {
lUInt8* qSmoothScaleImage(const lUInt8* src, int sw, int sh, bool ignore_alpha, int dw, int dh);
}

/// 32-bit RGB buffer
class LVColorDrawBuf : public LVBaseDrawBuf
{
private:
#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
    HDC _drawdc;
    HBITMAP _drawbmp;
#endif
    int _bpp;
    bool _ownData;
public:
    /// rotates buffer contents by specified angle
    virtual void Rotate( cr_rotate_angle_t angle );
    /// returns white pixel value
    virtual lUInt32 GetWhiteColor();
    /// returns black pixel value
    virtual lUInt32 GetBlackColor();
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( LVDrawBuf * buf, int x, int y, int options, lUInt32 * palette );
    // draws buffer on top of another buffer to implement background
    virtual void DrawOnTop( LVDrawBuf * buf, int x, int y);
    /// draws rescaled buffer content to another buffer doing color conversion if necessary
    virtual void DrawRescaled(LVDrawBuf * src, int x, int y, int dx, int dy, int options);
#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
    /// draws buffer content to another buffer doing color conversion if necessary
    virtual void DrawTo( HDC dc, int x, int y, int options, lUInt32 * palette );
#endif
    /// invert image
    virtual void  Invert();
    /// get buffer bits per pixel
    virtual int  GetBitsPerPixel();
    /// fills buffer with specified color
    virtual void Clear( lUInt32 color );
    /// get pixel value
    virtual lUInt32 GetPixel( int x, int y );
    /// fills rectangle with specified color
    virtual void FillRect( int x0, int y0, int x1, int y1, lUInt32 color );
    /// fills rectangle with pattern
    virtual void FillRectPattern( int x0, int y0, int x1, int y1, lUInt32 color0, lUInt32 color1, lUInt8 * pattern );
    /// inverts specified rectangle
	virtual void InvertRect( int x0, int y0, int x1, int y1 );
    /// sets new size
    virtual void Resize( int dx, int dy );
    /// draws image
    virtual void Draw( LVImageSourceRef img, int x, int y, int width, int height, bool dither );
    /// draws bitmap (1 byte per pixel) using specified palette
    virtual void Draw( int x, int y, const lUInt8 * bitmap, int width, int height, lUInt32 * palette );
    /// returns scanline pointer
    virtual lUInt8 * GetScanLine( int y );

    /// create own draw buffer
    LVColorDrawBuf(int dx, int dy, int bpp=32);
    /// creates wrapper around external RGBA buffer
    LVColorDrawBuf(int dx, int dy, lUInt8 * externalBuffer, int bpp=32 );
    /// destructor
    virtual ~LVColorDrawBuf();
    /// convert to 1-bit bitmap
    void ConvertToBitmap(bool flgDither);
    /// draw line
    virtual void DrawLine(int x0,int y0,int x1,int y1,lUInt32 color0 ,int length1=1,int length2=0,int direction=0);
#if !defined(__SYMBIAN32__) && defined(_WIN32) && !defined(QT_GL)
    /// returns device context for bitmap buffer
    HDC GetDC() { return _drawdc; }
#endif
};



#endif

