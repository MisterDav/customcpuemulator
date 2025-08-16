from PIL import Image
import sys

image_path = sys.argv[1]
x_size = int(sys.argv[2])
y_size = int(sys.argv[3])
outfile = sys.argv[4]

if x_size % 8 != 0 or y_size % 8 != 0:
    exit(f"X and Y sizes must be in multiples of 8!\nGot {x_size}x{y_size}")

colors = [
    (0x00,0x00,0x00,0xFF),
    (0x55,0x55,0x55,0xFF),
    (0xBB,0xBB,0xBB,0xFF),
    (0xFF,0xFF,0xFF,0xFF)
]

with Image.open(image_path) as a:
    image = a.load()
    for y in range(x_size):
        for x in range(y_size):
            byte_sequence = []
            bytes_a = []
            bytes_b = []
            for tile_y in range(8):
                mask_a = 0x00
                mask_b = 0x00
                for tile_x in range(8):
                    color = image[tile_x+x*8, tile_y+y*8]
                    #print(color)
                    selected = 0
                    if color in colors: selected = colors.index(color)
                    mask_a |= (selected & 0x01) << (7 - tile_x)
                    mask_b |= ((selected & 0x02) >> 1) << (7 - tile_x)
                bytes_a.append(mask_a) 
                bytes_b.append(mask_b)
            byte_sequence = bytes_a + bytes_b
            string_sequence = [hex(_) for _ in byte_sequence]
            print("byte " + ' '.join(string_sequence).replace("0x", "#"))
        
        