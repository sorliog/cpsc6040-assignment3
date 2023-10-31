#include "Stencil.h"

using namespace img;

Stencil::Stencil():
    halfSize(0),
    Nsize(0),
    weights(0)
{
}

Stencil::Stencil(int halfsize){
    halfSize = halfsize;
    Nsize = ny() * nx();
    weights = new float[Nsize]; 
}

Stencil::~Stencil(){
    clear();
}

void Stencil::clear(){
    if( weights != 0 ){ 
		delete[] weights; weights = 0; 
	}
 	halfSize = 0;
 	Nsize = 0;
}

int Stencil::index(int i, int j) const{
    int new_i = i + halfSize;
    int new_j = j + halfSize;

    return new_i + nx() * new_j;
}



float Stencil::value( int i, int j) const{
    return weights[index(i,j)];
}

void Stencil::set_value( int i, int j, float weight){
    weights[index(i, j)] = weight;
}

Stencil::Stencil( const Stencil& sten){
    halfSize = sten.halfSize;
    weights = sten.get_raw_data();
}

 		
float* Stencil::get_raw_data() const{
    return weights;
}





