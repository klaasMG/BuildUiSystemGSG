# noinspection PyUnresolvedReferences
from font_holder import Font
# noinspection PyUnresolvedReferences
from GridPacker import TextPacker, PlacedRect
from PassSystem import Texture, TextureType
from PIL import Image
from pathlib import Path

ttf_path = "assets/fonts/AovelSansRounded-rdDL.ttf"
class FontManager:
    def __init__(self):
        self.font_map_image = Image.new("F",(8192,8912),(0,0,0,0))
        self.font_map = Texture(self.font_map_image,"uFontMap", TextureType.GREY_SCALE)
        self.fonts: dict[str, Font] = {}
        
        
    def render_text(self, text: str, font: str):
        render_font: Font = self.fonts[font]
        for i in text:
            pass
        
    def add_font(self, font_name: str, font_file: Path):
        font_file = str(font_file)
        font = Font(font_file)
        self.fonts[font_name] = font
    
    def remove_font(self, font_name):
        self.fonts[font_name] = None