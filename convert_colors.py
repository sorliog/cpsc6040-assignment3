

palletes = {
        'bright': [0x04e762,0xf5b700,0xdc0073,0x008bf8,0x89fc00],
        'brown': [0x821c04, 0x81735a, 0x7c3405, 0x0c0404, 0x504344],
        'blue': [0x465c9c, 0x5c54ae, 0x443d93, 0x5c61c5, 0x83a1f3],
        'flower': [0xe0eef0, 0xf2d9eb, 0xa1855a, 0xcedfe9, 0xf2d15f],
        'vintage': [0x797D62, 0x9B9B7A, 0xBAA587, 0xD9AE94, 0xAEB0A4, 0xDBDBD5,
            0xE8AC65, 0xD08C60, 0xB58463, 0x997B66],
        'cool':[0x5ADCED, 0x3FA5B2, 0x016370, 0xB0DE4E, 0xFFC800, 0xFF5900,
            0xFF9100, 0x150170, 0x2D2D44, 0x2D2D44, 0x000000]
        }


color_count = 0
for name, pallete in palletes.items():
    variables = []
    for i, hex_code in enumerate(pallete):
        red = (hex_code & 0xff0000) >> 16;
        green = (hex_code & 0x00ff00) >> 8;
        blue = (hex_code & 0x0000ff);
    
        red_norm = red / 256
        green_norm = green / 256
        blue_norm = blue / 256
        variables.append(f"*c{color_count}")

        print(f"static Color* c{color_count} = new Color({red_norm}, {green_norm}, {blue_norm}, 1.0);")
        color_count += 1 
    print(f"static ColorLUT* {name} = new ColorLUT(\"{name}\",{len(variables)}, new Color[{len(variables)}]{{ {', '.join(variables)} }});")
    print()







