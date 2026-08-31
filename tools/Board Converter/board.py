from PIL import Image

INPUT_IMAGE = "board.png"

SHAPE_OUTPUT = "board.txt"

THRESHOLD = 128
INVERT = False

img = Image.open(INPUT_IMAGE).convert("RGB")

width, height = img.size

def is_wall_pixel(x, y):
    r, g, b = img.getpixel((x, y))
    return g > 0


def is_playable_pixel(x, y):
    r, g, b = img.getpixel((x, y))

    if INVERT:
        return r > THRESHOLD and g > THRESHOLD and b > THRESHOLD

    return r < THRESHOLD and g < THRESHOLD and b < THRESHOLD

shape = []

for y in range(height):
    row = ""

    for x in range(width):
        if is_playable_pixel(x, y):
            row += "0"
        else:
            row += "X"

    shape.append(row)

with open(SHAPE_OUTPUT, "w") as f:

    f.write("const char *shape[] =\n")
    f.write("{\n")

    for y, row in enumerate(shape):
        comma = "," if y < height - 1 else ""
        f.write(f'    "{row}"{comma}\n')

    f.write("};\n\n")

    f.write(f"// Width : {width}\n")
    f.write(f"// Height: {height}\n")


print(f"Saved {SHAPE_OUTPUT}")
print(f"Board size: {width} x {height}")