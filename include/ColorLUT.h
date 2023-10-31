#ifndef STARTER_COLORLUT_H
#define STARTER_COLORLUT_H
#include <vector>
#include "Color.h"
#include <string>

using namespace std;

namespace img{

class ColorLUT{
    public:
        ColorLUT(std::string name, int num_colors, Color* colors);
 
        ~ColorLUT();
        void lookup(float value, std::vector<float>& v) const;
        std::string getName() const;
    private:
        float lookup_color_component(float value, int component_index) const;
        int num_colors;
        Color* T;
        std::string name;
};

static Color* black = new Color(0,0,0,1);
static Color* white = new Color(1,1,1,1);

static ColorLUT* blackWhite = new ColorLUT("blackWhite", 2, new Color[2]{*black, *white});

static Color* c0 = new Color(0.005625, 0.80234375, 0.2828125, 1.0);
static Color* c1 = new Color(0.95703125, 0.71484375, 0.0, 1.0);
static Color* c2 = new Color(0.859375, 0.0, 0.44921875, 1.0);
static Color* c3 = new Color(0.0, 0.54296875, 0.96875, 1.0);
static Color* c4 = new Color(0.53515625, 0.984375, 0.0, 1.0);
static ColorLUT* bright = new ColorLUT("bright",5, new Color[5]{ *c0, *c1, *c2, *c3, *c4 });

static Color* c5 = new Color(0.4078125, 0.019375, 0.005625, 1.0);
static Color* c6 = new Color(0.50390625, 0.44921875, 0.3515625, 1.0);
static Color* c7 = new Color(0.484375, 0.203125, 0.01953125, 1.0);
static Color* c8 = new Color(0.046875, 0.015625, 0.015625, 1.0);
static Color* c9 = new Color(0.3125, 0.26171875, 0.265625, 1.0);
static ColorLUT* brown = new ColorLUT("brown",5, new Color[5]{ *c5, *c6, *c7, *c8, *c9 });

static Color* c10 = new Color(0.1734375, 0.259375, 0.509375, 1.0);
static Color* c11 = new Color(0.359375, 0.328125, 0.6796875, 1.0);
static Color* c12 = new Color(0.265625, 0.23828125, 0.57421875, 1.0);
static Color* c13 = new Color(0.359375, 0.37890625, 0.76953125, 1.0);
static Color* c14 = new Color(0.51171875, 0.62890625, 0.94921875, 1.0);
static ColorLUT* blue = new ColorLUT("blue",5, new Color[5]{ *c10, *c11, *c12, *c13, *c14 });

static Color* c15 = new Color(0.775, 0.8296875, 0.8375, 1.0);
static Color* c16 = new Color(0.9453125, 0.84765625, 0.91796875, 1.0);
static Color* c17 = new Color(0.62890625, 0.51953125, 0.3515625, 1.0);
static Color* c18 = new Color(0.8046875, 0.87109375, 0.91015625, 1.0);
static Color* c19 = new Color(0.9453125, 0.81640625, 0.37109375, 1.0);
static ColorLUT* flower = new ColorLUT("flower",5, new Color[5]{ *c15, *c16, *c17, *c18, *c19 });

static Color* c20 = new Color(0.37265625, 0.38828125, 0.2828125, 1.0);
static Color* c21 = new Color(0.60546875, 0.60546875, 0.4765625, 1.0);
static Color* c22 = new Color(0.7265625, 0.64453125, 0.52734375, 1.0);
static Color* c23 = new Color(0.84765625, 0.6796875, 0.578125, 1.0);
static Color* c24 = new Color(0.6796875, 0.6875, 0.640625, 1.0);
static Color* c25 = new Color(0.85546875, 0.85546875, 0.83203125, 1.0);
static Color* c26 = new Color(0.90625, 0.671875, 0.39453125, 1.0);
static Color* c27 = new Color(0.8125, 0.546875, 0.375, 1.0);
static Color* c28 = new Color(0.70703125, 0.515625, 0.38671875, 1.0);
static Color* c29 = new Color(0.59765625, 0.48046875, 0.3984375, 1.0);
static ColorLUT* vintage = new ColorLUT("vintage",10, new Color[10]{ *c20, *c21, *c22, *c23, *c24, *c25, *c26, *c27, *c28, *c29 });

static Color* c30 = new Color(0.2515625, 0.759375, 0.82578125, 1.0);
static Color* c31 = new Color(0.24609375, 0.64453125, 0.6953125, 1.0);
static Color* c32 = new Color(0.00390625, 0.38671875, 0.4375, 1.0);
static Color* c33 = new Color(0.6875, 0.8671875, 0.3046875, 1.0);
static Color* c34 = new Color(0.99609375, 0.78125, 0.0, 1.0);
static Color* c35 = new Color(0.99609375, 0.34765625, 0.0, 1.0);
static Color* c36 = new Color(0.99609375, 0.56640625, 0.0, 1.0);
static Color* c37 = new Color(0.08203125, 0.00390625, 0.4375, 1.0);
static Color* c38 = new Color(0.17578125, 0.17578125, 0.265625, 1.0);
static Color* c39 = new Color(0.17578125, 0.17578125, 0.265625, 1.0);
static Color* c40 = new Color(0.0, 0.0, 0.0, 1.0);
static ColorLUT* cool = new ColorLUT("cool",11, new Color[11]{ *c30, *c31, *c32, *c33, *c34, *c35, *c36, *c37, *c38, *c39, *c40 });



static int numberOfPalletes = 7;
static ColorLUT* palletes[7] = {blackWhite, bright, brown, blue, flower, vintage, cool };

}
#endif



