
#include <vector>
#include <string>
#include <OpenImageIO/imageio.h>
#include <iostream>
#include "StarterViewer.h"
#include "ImgProc.h"

using namespace starter;
using namespace OIIO;
using namespace img;



bool does_cmd_arg_exist(std::vector<std::string> args, const std::string& arg){
	for(std::string a : args){
		if(a == arg){
			return true;
		}
	}
	return false;
}

bool does_cmd_arg_exist(std::vector<std::string> args, const std::string& arg1, const std::string& arg2){
	for(std::string a : args){
		if(a == arg1 || a == arg2){
			return true;
		}
	}
	return false;
}

bool does_cmd_arg_exist(std::vector<std::string> args, const std::string& arg1, const std::string& arg2, const std::string& arg3){
	for(std::string a : args){
		if(a == arg1 || a == arg2 || a == arg3){
			return true;
		}
	}
	return false;
}



std::string get_cmd_arg_after_flag(std::vector<std::string> args, const std::string& flag){
	bool found = false;
	for(std::string a : args){
		if(found){
			return a;
		}
		if(a == flag){
			found =  true;
		}
	}
	return "";
}



std::string get_cmd_arg_after_flag(std::vector<std::string> args, const std::string& flag1, const std::string& flag2){
	bool found = false;
	for(std::string a : args){
		if(found){
			return a;
		}
		if(a == flag1 || a == flag2){
			found =  true;
		}
	}
	return "";
}



std::string get_cmd_arg_after_flag(std::vector<std::string> args, const std::string& flag1, const std::string& flag2, const std::string& flag3){
	bool found = false;
	for(std::string a : args){
		if(found){
			return a;
		}
		if(a == flag1 || a == flag2 || a == flag3){
			found =  true;
		}
	}
	return "";
}




void show_help_message(){
	std::string title = " _______                  ___ ___ __    \n"                        
		"|_     _|.--------.-----.|   |   |__|.-----.--.--.--.-----.----.\n"
		" _|   |_ |        |  _  ||   |   |  ||  -__|  |  |  |  -__|   _|\n"
		"|_______||__|__|__|___  | \\_____/|__||_____|________|_____|__|  \n"
		"                  |_____|                                       \n";
	std::cout << title << std::endl;
	std::cout << "\t\tStudent: Samuil Orlioglu\n" << std::endl;
	
	std::cout << "Usage: ./imgviewer -image filename [options] "<< std::endl;
	std::cout << "***Options***\n" << std::endl;
	std::cout << "-image, -i\t\t\tOpen up image by filename. Filename needs to follow this flag." << std::endl;
    std::cout << "-dim, -d\t\t\tSpecify dimensions for new empty images. Dimensions in form x,y need to follow this flag. Only positive integer are excepted" << std::endl;
    std::cout << "-chan, -c\t\t\tSpecify number of channels for new empty images. Positive integer needs to follow this flag." << std::endl;
    std::cout << "-pallete \t\t\tSpecify the color pallete for generating fractals. A string that is name of the pallete needs to follow this flag.\n\t\t\t\t\tOptions are (blackWhite, bright, brown, blue, flower, vintage, cool)" << std::endl;
    std::cout << "-help, -h, -?\t\t\tShow this message." << std::endl;

}

int main( int argc, char** argv )
{
	
	std::vector<std::string> args;

	for(int i=0;i<argc;i++)
	{
		std::string s(argv[i]);
		args.push_back(s);
	}
	
	bool show_help = does_cmd_arg_exist(args, "-help", "-h", "-?"); 	
    if(show_help){
		show_help_message();
		return 0;
	}
	
	bool image_filename_exists = does_cmd_arg_exist(args, "-image") || does_cmd_arg_exist(args, "-i") ;
    bool image_dimensions_exists = does_cmd_arg_exist(args, "-dim") || does_cmd_arg_exist(args, "-d");;
    bool image_num_channels_exists = does_cmd_arg_exist(args, "-chan") || does_cmd_arg_exist(args, "-c");;
    bool fractal_pallete = does_cmd_arg_exist(args, "-pallete");


	if(image_filename_exists){
		std::string filename = get_cmd_arg_after_flag(args, "-image", "-i");
	    
        if(filename.empty()){
            std::cout << "No filename specified. Will do nothing!" << std::endl;
            return 1;
        }
        StarterViewer* viewer = CreateViewer();
        
        // Setup image dimensions if dimension flag found (This only affects
        // images that are created because the given filename is not valid)
        if(image_dimensions_exists){
            std::string dimension_string = get_cmd_arg_after_flag(args, "-dim", "-d");
            int comma_index = dimension_string.find(',');
            if (comma_index != string::npos){
                int x_dim = std::stoi(dimension_string.substr(0,comma_index));
                int y_dim = std::stoi(dimension_string.substr(comma_index+1,dimension_string.length()));
                viewer->SetDefaultXDimension(x_dim);
                viewer->SetDefaultYDimension(y_dim);
            }
        }
        // Setup image number of channels if num channels flag found (This only affects
        // images that are created because the given filename is not valid)
        if(image_num_channels_exists){
            std::string channel_string = get_cmd_arg_after_flag(args, "-chan", "-c");
            int num_chan = std::stoi(channel_string);
            viewer->SetDefaultNumChannels(num_chan);
        }

        if(fractal_pallete){
            std::string pallete_string = get_cmd_arg_after_flag(args, "-pallete");
            viewer->SetLUT(pallete_string);
        }

        

        // Load image before initializing window so that window size is set
        // properly

        viewer->Load(filename);
		viewer->Init(args);
	   	viewer->MainLoop();
	}
	else{
		std::cout << "No -image flag specified. Will do nothing!" << std::endl;
		return 1;
	}
}
