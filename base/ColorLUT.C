#include "ColorLUT.h"
#include <iostream>
#include <math.h>


using namespace img;
using namespace std;




ColorLUT::ColorLUT(std::string name, int num_colors, Color* colors)
{

    this->num_colors = num_colors;
    this->name = name;
    T = colors;
}


std::string ColorLUT::getName() const{
    return name;
}


void ColorLUT::lookup(float value, std::vector<float>& v) const
{
    
    for(int i = 0; i < v.size(); i++){
        v[i] = lookup_color_component(value, i);
    }

    /*
    v[0] = lookup_color_component(value, 0);
    v[1] = 0.5;
    v[2] = 0.5;
*/
}



float ColorLUT::lookup_color_component(float value, int component_index) const{
    int M = num_colors;
   
    
    if(value < 0){
        return T[0][component_index];
    }
    if(value > 1){
        return T[M - 1][component_index];
    }

    // Check for NaN values. If it is a NaN, return first color.
    if(value != value){
        return T[0][component_index];
    }

    float x = value * M;
    int m = (int)x;
    int mp = m + 1;
    float w = x - m;

    
    m = m % M;
    mp = mp % M;

    float C = T[m][component_index] * (1-w) + T[mp][component_index] * w;
//    std::cout << "Value: " << value << " C=" << C << std::endl;
    return C;
 
}


