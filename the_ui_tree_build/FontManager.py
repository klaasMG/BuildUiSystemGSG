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
        self.font_map_image = Image.new("F",(8192,8912),0.0)
        self.font_map = Texture(self.font_map_image,"uFontMap", TextureType.GREY_SCALE)
        self.fonts: dict[str, Font] = {}
        
        
    def render_text(self, text: str, font: str ,text_height: int):
        render_font: Font = self.fonts[font]
        text_length = 0
        for i in text:
            render_info_1 = render_font.get_render_info(text_height, ord(i))
            advance = render_info_1[0]
            text_length += advance
        text_image = Image.new("F",(text_height, 10), 0.0)
        for i in text:
            rendered_char, rendered_info = self.render_char(i, render_font, text_height)
        
    def add_font(self, font_name: str, font_file: Path):
        font_file = str(font_file)
        font = Font(font_file)
        self.fonts[font_name] = font
    
    def remove_font(self, font_name):
        self.fonts[font_name] = None
        
    @staticmethod
    def render_char( char: str, render_font: Font, text_height: int):
        try:
            char_array = render_font.get_raster_from_glyph(text_height, ord(char))
            render_info = render_font.get_render_info(text_height, ord(char))
        except ValueError:
            char_array = render_font.get_raster_from_glyph(text_height, 0)
            render_info = render_font.get_render_info(text_height, 0)
        
        return char_array, render_info
    
f = FontManager()