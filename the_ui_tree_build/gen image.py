from PIL import Image

w, h = 100, 100
img = Image.new("RGBA", (w, h))
px = img.load()

for y in range(h):
    for x in range(w):
        if (x // 10 + y // 10) % 2 == 0:
            px[x, y] = (255, 0, 255)
        else:
            px[x, y] = (0, 0, 255)

img.save("pattern.png")