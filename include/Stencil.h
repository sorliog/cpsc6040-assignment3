#ifndef STENCIL_H
#define STENCIL_H
#include <vector>

namespace img{

class Stencil
{
	public:
 		Stencil();
        Stencil(int halfsize);
 		~Stencil();
 		void clear();
 		int nx() const { return halfSize * 2 + 1; }
 		int ny() const { return halfSize * 2 + 1; }
        int halfsize() const { return halfSize; }
        float value( int i, int j) const;
 		void set_value( int i, int j, float weight);
 		Stencil( const Stencil& sten); // copy constructor
		float* get_raw_data() const;
 	private:
 		int halfSize;
 		long Nsize;
 		float* weights;
		int index(int i, int j) const;
	};
}
#endif
