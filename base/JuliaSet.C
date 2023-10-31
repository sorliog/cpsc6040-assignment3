#include "JuliaSet.h"
#include "Point.h"
#include <complex>

using namespace img;


Point img::julia(const Point& p, int iterations = 100, int cycles = 2){
    std::complex<float> Pc(p.getX(),p.getY());
    std::complex<float> zc(0.8*cos(254.3 * 3.14159265/180.0),0.8*sin(254.3 * 3.14159265/180.0));
    

    for(int i = 0; i < iterations; i++)
    {
        std::complex<float> temp = Pc;
        for(int c=1;c<cycles;c++)
        {
            temp = temp * Pc;
        }
        Pc = temp + zc;
    }
    Point Pout;
    Pout.setX(Pc.real());
    Pout.setY(Pc.imag());
    return Pout;
}
