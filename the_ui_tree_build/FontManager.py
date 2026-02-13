from pathlib import Path
# noinspection PyUnresolvedReferences
from font_holder import Font
from PassSystem import Texture
from PIL import Image

ttf_path = "assets/fonts/AovelSansRounded-rdDL.ttf"
class FontManager:
    def __init__(self):
        self.font_image = Image.new("RGBA", (8192,8192), (0,0,0,255))
        self.texture = Texture(self.font_image)