#ifndef IMGPROC_H
#define IMGPROC_H
#include <vector>
#include <string>

namespace img{

class ImgProc
{
	public:
 		ImgProc();
 		~ImgProc();
 		void clear();
 		void clear(int nX, int nY, int nC);
 		int nx() const { return Nx; }
 		int ny() const { return Ny; }
 		int depth() const { return Nc; }
 		void value( int i, int j, std::vector<float>& pixel ) const;
 		void set_value( int i, int j, const std::vector<float>& pixel );
 		ImgProc( const ImgProc& img ); // copy constructor
 		ImgProc& operator=(const ImgProc& img ); // copy assignment
		void load(int nX, int nY, int Nc, float* data);
		float* get_raw_data() const;
        bool read_image(std::string filename);
        void write_image_jpeg(std::string filename);
        void write_image_png(std::string filename);
        bool is_loaded() const {return loaded;}

 	private:
 		long Nx, Ny, Nc;
 		long Nsize;
 		float* img_data;
        bool loaded;
		long index(long i, long j, long c) const;
	};
}
#endif
