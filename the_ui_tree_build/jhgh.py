import print_wrapper
from PIL import Image

image = Image.open("gnbuyhggfjh.png")
image = image.convert("L")
x, y = image.size
for i in range(x):
    for j in range(y):
        p = image.getpixel((i, j))
        if p != 1:
            print(f"pixel at ({i},{j}){p}")