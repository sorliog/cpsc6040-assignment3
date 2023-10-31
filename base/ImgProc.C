#include <vector>
#include <iostream>
#include <OpenImageIO/imageio.h>
#include "ImgProc.h"




using namespace OIIO;
using namespace std;
using namespace img;

ImgProc::ImgProc() :
	Nx (0),
 	Ny (0),
 	Nc (0),
 	Nsize (0),
 	img_data (0),
    loaded(false)
{
}

ImgProc::~ImgProc()
{
	clear();
}

void ImgProc::clear()
{
 	if( img_data != 0 ){ 
		delete[] img_data; img_data = 0; 
	}
 	Nx = 0;
 	Ny = 0;
 	Nc = 0;
 	Nsize = 0;
    loaded = false;
}

void ImgProc::clear(int nX, int nY, int nC)
{
 	clear();
 	Nx = nX;
 	Ny = nY;
 	Nc = nC;
 	Nsize = (long)Nx * (long)Ny * (long)Nc;
 	img_data = new float[Nsize];
	#pragma omp parallel for
 	for( long i=0;i<Nsize;i++ ){ 
		img_data[i] = 0.0; 
	}
    loaded = false;

}

void ImgProc::load(int nX, int nY, int nC, float* data){
	clear(nX,nY,nC);
	#pragma omp parallel for
 	for( long i=0;i<Nsize;i++ ){ 
		img_data[i] = data[i];
	}
    loaded = true;


}

float* ImgProc::get_raw_data() const{
    return img_data;
}

ImgProc::ImgProc(const ImgProc& v) :
	Nx (v.Nx),
 	Ny (v.Ny),
 	Nc (v.Nc),
 	Nsize (v.Nsize)
{   
	load(v.Nx, v.Ny, v.Nc, v.img_data);
    
    /*img_data = new float[Nsize];
	#pragma omp parallel for
 	for( long i=0;i<Nsize;i++){ 
		img_data[i] = v.img_data[i];	
	 }
     */
}





ImgProc& ImgProc::operator=(const ImgProc& v)
{
 	if( this == &v ){ 
		return *this; 
	}
 	
	if( Nx != v.Nx || Ny != v.Ny || Nc != v.Nc )
	{
 		clear();
 		Nx = v.Nx;
 		Ny = v.Ny;
 		Nc = v.Nc;
 		Nsize = v.Nsize;
	}
 	img_data = new float[Nsize];
	#pragma omp parallel for
 	for( long i=0;i<Nsize;i++){ 
		img_data[i] = v.img_data[i]; 
	}
 	return *this;
}


long ImgProc::index(long i, long j, long c) const
{
	return c + Nc*(i + Nx*j);
}


void ImgProc::value( int i, int j, std::vector<float>& pixel) const
{
	pixel.clear();
 	if( img_data == nullptr ){ return; }
 	if( i<0 || i>=Nx ){ return; }
 	if( j<0 || j>=Ny ){ return; }
 	pixel.resize(Nc);
 	for( int c=0;c<Nc;c++ )
 	{
 		pixel[c] = img_data[index(i,j,c)];
 	}
 	return;
}




void ImgProc::set_value( int i, int j, const std::vector<float>& pixel)
{
	if( img_data == nullptr ){ return; }
 	if( i<0 || i>=Nx ){ return; }
 	if( j<0 || j>=Ny ){ return; }
 	if( Nc > (int)pixel.size() ){ return; }
	// #pragma omp parallel for
 	for( int c=0;c<Nc;c++ )
 	{
 		img_data[index(i,j,c)] = pixel[c];
 	}
 	return;
}


// https://openimageio.readthedocs.io/en/latest/imageinput.html
bool ImgProc::read_image(std::string filename){
        std::cout << "\nLoading " << filename << std::endl;
        auto inp = ImageInput::open(filename);
        if (!inp){
			std::cout << "Error opening image " << filename << std::endl;
            return false;
        }

		const ImageSpec &spec = inp->spec();
        int xres = spec.width;
        int yres = spec.height;
        int nchannels = spec.nchannels;
        std::cout << "Image Dim: {" << xres << ", " << yres << "} Number Channels: {" << nchannels << "}" << std::endl;
        clear(xres, yres, nchannels);

        inp->read_image(0, 0, 0, nchannels, TypeDesc::FLOAT, &img_data[0]);
        inp->close();
        return true;
}


// https://openimageio.readthedocs.io/en/latest/imageoutput.html
void ImgProc::write_image_jpeg(std::string filename){   
	std::cout << "\nWriting JPEG to " << filename << std::endl;

	std::string extension1 = filename.substr((filename.length() <= 5) ? filename.length() : filename.length() - 5, (filename.length() <= 5) ? filename.length() :  5);
	
	std::string extension2 = filename.substr((filename.length() <= 4) ? filename.length() : filename.length() - 4, (filename.length() <= 4) ? filename.length() :  4);

	if(extension1.compare(".jpeg") != 0 && extension2.compare(".jpg") != 0){
		// No extension in filename, so create a .jpeg extension	
		filename = filename.append(".jpeg");
		std::cout << "Modified filename to " << filename << std::endl;
	}
	
    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    if (!out){
		std::cout << "Error in creating ImageOutput " << std::endl;
		return;
	}
    ImageSpec spec(nx(), ny(), depth(), TypeDesc::FLOAT);
    out->open(filename, spec);
	out->write_image(TypeDesc::FLOAT, &img_data[0]);
	out->close();
}


void  ImgProc::write_image_png(std::string filename){   
	std::cout << "\nWriting PNG to " << filename << std::endl;

//	std::string extension1 = filename.substr((filename.length() <= 5) ? filename.length() : filename.length() - 5, (filename.length() <= 5) ? filename.length() :  5);
	
	std::string extension1 = filename.substr((filename.length() <= 4) ? filename.length() : filename.length() - 4, (filename.length() <= 4) ? filename.length() :  4);

	if(extension1.compare(".png") != 0){
		// No extension in filename, so create a .jpeg extension	
		filename = filename.append(".png");
		std::cout << "Modified filename to " << filename << std::endl;
	}
	
    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    if (!out){
		std::cout << "Error in creating ImageOutput " << std::endl;
		return;
	}
    ImageSpec spec(nx(), ny(), depth(), TypeDesc::FLOAT);
    out->open(filename, spec);
	out->write_image(TypeDesc::FLOAT, &img_data[0]);
	out->close();
}











