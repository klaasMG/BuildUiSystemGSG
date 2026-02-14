# noinspection PyUnresolvedReferences
from font_holder import Font
from PassSystem import Texture, TextureType
from PIL import Image

ttf_path = "assets/fonts/AovelSansRounded-rdDL.ttf"
class FontManager:
    def __init__(self):
        self.font_map_image = Image.new("F",(8192,8912),(0,0,0,0))
        self.font_map = Texture(self.font_map_image,"uFontMap", TextureType.GREY_SCALE)