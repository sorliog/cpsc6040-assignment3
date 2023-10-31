#ifndef IMGOOPERATIONS_H
#define IMGOOPERATIONS_H
#include "Stencil.h"
#include "ImgProc.h"
#include "Point.h"
#include "ColorLUT.h"

namespace img{

    // Pixel Operations
    void flip_vertically(const img::ImgProc& input, img::ImgProc& output);
    void flip_horizontally(const img::ImgProc& input, img::ImgProc& output);
    void brightness(const img::ImgProc& input, img::ImgProc& output, const float scalar);
    void gamma(const img::ImgProc& input, img::ImgProc& output, const float scalar);

    // Linear filtering Operations
    void linear_conv(const img::Stencil& stencil, const img::ImgProc& input, img::ImgProc& output);
    void bounded_linear_conv(const Stencil& stencil, const ImgProc& input, ImgProc& output);

    // Fractal Operations
    void apply_fractal_lut(const img::Point& center, const img::ColorLUT *lut, img::ImgProc& out, const float range, const int iterations, const int cycles);




}
#endif
