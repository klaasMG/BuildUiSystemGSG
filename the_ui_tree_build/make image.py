from PIL import Image

image = Image.new("RGBA", (8192,8192),(255,255,255,255))
image.save("assets/image_atlases/atlas.png",format="png")