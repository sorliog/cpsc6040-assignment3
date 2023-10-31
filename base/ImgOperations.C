#include "ImgProc.h"
#include "ImgOperations.h"
#include "Stencil.h"
#include "JuliaSet.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace img;


// Flip image vertically, affine transformation
void img::flip_vertically(const img::ImgProc& input, img::ImgProc& output){
    output.clear(input.nx(), input.ny(), input.depth());

    int nx = input.nx();
    int ny = input.ny();

    int Nsize = nx * ny;

    #pragma omp parallel for 
    for(long a = 0; a < Nsize; a += 1){
        int j = a / nx;
        int i = a % nx;
        std::vector<float> pixel;
        input.value(i,j, pixel);
        output.set_value(i, ny - j, pixel);
    }

}

// Flip image horizontally, affine transformation
void img::flip_horizontally(const img::ImgProc& input, img::ImgProc& output){
    output.clear(input.nx(), input.ny(), input.depth());
    int nx = input.nx();
    int ny = input.ny();

    int Nsize = nx * ny;

    #pragma omp parallel for 
    for(long a = 0; a < Nsize; a += 1){
        int j = a / nx;
        int i = a % ny;
        std::vector<float> pixel;

        input.value(i,j, pixel);
        output.set_value(nx - i, j, pixel);
    }

}

// Increase/decrease brightness in input image, save result to output image
void img::brightness(const img::ImgProc& input, img::ImgProc& output, const float scalar){
    output.clear(input.nx(), input.ny(), input.depth());

    int nx = input.nx();
    int ny = input.ny();

    int Nsize = nx * ny;

    
    #pragma omp parallel for 
    for(long a = 0; a < Nsize; a += 1){
        int j = a / nx;
        int i = a % nx;
        std::vector<float> pixel;

        input.value(i,j, pixel);
        for(long c = 0; c < (long)pixel.size(); c++){
            pixel[c] = pixel[c] * scalar;
        }
        output.set_value(i, j, pixel);
    }

}

// Increase/decrease gamma in input image, save result to output image
void img::gamma(const img::ImgProc& input, img::ImgProc& output, const float scalar){
    output.clear(input.nx(), input.ny(), input.depth());

    int nx = input.nx();
    int ny = input.ny();

    int Nsize = nx * ny;

    #pragma omp parallel for 
    for(long a = 0; a < Nsize; a += 1){
        int j = a / nx;
        int i = a % nx;
        std::vector<float> pixel;

        input.value(i,j, pixel);
        for(long c = 0; c < (long)pixel.size(); c++){
            pixel[c] = std::pow(pixel[c], scalar);
        }
        output.set_value(i, j, pixel);
    }

}

// Simple linear wrap-around convolution
void img::linear_conv(const Stencil& stencil, const ImgProc& input, ImgProc& output){
    output.clear(input.nx(), input.ny(), input.depth());
    
    for(long j = 0; j < output.ny(); j++){
        int jmin = j - stencil.halfsize();
        int jmax = j + stencil.halfsize();
        
        #pragma omp parallel for
        for(int i = 0; i < output.nx(); i++){
            int imin = i - stencil.halfsize();
            int imax = i + stencil.halfsize();
            
            std::vector<float> pixel(output.depth(),0.0);
            std::vector<float> sample(input.depth(),0.0);
            
            for(int jj=jmin;jj<=jmax;jj++){
                int stencilj = jj-j;
                int jjj = jj;
                
                if(jjj < 0 ){ jjj += output.ny(); }
                if(jjj >= output.ny() ){ jjj -= output.ny(); }
                
                for(int ii=imin;ii<=imax;ii++){
                    int stencili = ii-i;
                    int iii = ii;
                    
                    if(iii < 0 ){ iii += output.nx(); }
                    if(iii >= output.nx()){ iii -= output.nx(); }
                    
                    const float& stencil_value = stencil.value(stencili, stencilj);
                    input.value(iii,jjj,sample);
                    for(size_t c=0;c<sample.size();c++){ pixel[c] += sample[c] * stencil_value; }
                }
            }
            output.set_value(i,j,pixel);
        }
    }
}



// Simple linear bounded convolution. Positions outside image are treated as
// fully black 
void img::bounded_linear_conv(const Stencil& stencil, const ImgProc& input, ImgProc& output){
    std::vector<float> out_of_bounds_color(input.depth(), 0.0);
    output.clear(input.nx(), input.ny(), input.depth());
    
    for(int j = 0; j < output.ny(); j++){
        int jmin = j - stencil.halfsize();
        int jmax = j + stencil.halfsize();
        
        #pragma omp parallel for
        for(int i = 0; i < output.nx(); i++){
            int imin = i - stencil.halfsize();
            int imax = i + stencil.halfsize();
            
            std::vector<float> pixel(output.depth(),0.0);
            std::vector<float> sample(input.depth(),0.0);
            
            for(int jj = jmin; jj <= jmax; jj++){
                int stencilj = jj - j;
                int jjj = jj;
                
                //if(jjj < 0 ){ jjj += output.ny(); }
                //if(jjj >= output.ny() ){ jjj -= output.ny(); }
                
                for(int ii = imin; ii <= imax; ii++){
                    int stencili = ii - i;
                    int iii = ii;
                    
                    //if(iii < 0 ){ iii += output.nx(); }
                    //if(iii >= output.nx()){ iii -= output.nx(); }
                    
                    const float& stencil_value = stencil.value(stencili, stencilj);
                    if(iii >= 0 && iii < output.nx() && jjj >= 0 && jjj < output.ny()){
                        input.value(iii, jjj, sample);
                    }
                    else{
                        sample = out_of_bounds_color;
                    }
                    
                    for(size_t c = 0; c < sample.size(); c++){ 
                        pixel[c] += sample[c] * stencil_value; 
                    }
                }
            }
            output.set_value(i, j, pixel);
        }
    }
}





void img::apply_fractal_lut(const Point& center, const ColorLUT *lut, img::ImgProc& out, const float range = 1.0, const int iterations = 100, const int cycles = 2)
{
    float R = 2.0;
    
    for(int j=0;j<out.ny();j++){
        #pragma omp parallel for
        for( int i = 0;i < out.nx(); i++){
            Point P;
            P.setX(2.0*(double)i/(double)out.nx() - 1.0);
            P.setY(2.0*(double)j/(double)out.ny() - 1.0);
            P.setX(P.getX() * range);
            P.setY(P.getY() * range);
            P.setX(P.getX() + center.getX());
            P.setY(P.getY() + center.getY());
            
            Point PP = julia(P, iterations, cycles);
            double rate = std::sqrt(PP.getX() * PP.getX() + PP.getY() * PP.getY())/R;
            
            
            std::vector<float> v(3,0.0);
            lut->lookup(rate, v);
            out.set_value(i, j, v);
        }
    }
}
