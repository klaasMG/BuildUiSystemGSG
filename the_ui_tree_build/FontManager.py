# noinspection PyUnresolvedReferences
from font_holder import Font
# noinspection PyUnresolvedReferences
from GridPacker import TextPacker, PlacedRect
from PassSystem import Texture, TextureType
from PIL import Image
from pathlib import Path

ttf_path = Path("assets/fonts/AovelSansRounded-rdDL.ttf")
class FontManager:
    def __init__(self):
        self.font_map_image = Image.new("L",(8192,8912),0)
        #self.font_map = Texture(self.font_map_image,"uFontMap", TextureType.GREY_SCALE)
        self.fonts: dict[str, Font] = {}
    
    def render_text(self, text: str, font: str, text_height: int):
        render_font: Font = self.fonts[font]
        
        # First pass: compute total width using advances
        text_length = 0
        for char in text:
            render_info = render_font.get_render_info(text_height, ord(char))
            advance = render_info[0]
            text_length += advance
        
        # Create image tall enough for the line
        text_image = Image.new("L", (text_length, text_height), 0)
        
        # Compute baseline (top + ascent)
        if len(text) > 0:
            # get ascent from first char (all glyphs share font ascent)
            _, _, scaledAscent, scaledDescent, _ = render_font.get_render_info(text_height, ord(text[0]))
        else:
            scaledAscent = text_height
        baseline_y = scaledAscent
        
        cursor_x = 0
        for char in text:
            rendered_char, rendered_info = self.render_char(char, render_font, text_height)
            char_image = Image.fromarray(rendered_char, "L")
            
            advance, lsb, scaledAscent, scaledDescent, lineGap = rendered_info
            # y0 = char bitmap offset relative to baseline
            y0 = rendered_info[2] - scaledAscent  # or get from glyph bitmap box if available
            
            # Position: x = cursor + lsb, y = baseline + y0
            text_image.paste(char_image, (cursor_x + lsb, baseline_y + y0))
            
            cursor_x += advance  # move pen
        return text_image
    
    def update_texture(self):
        pass
        #self.font_map.resend(self.font_map_image)
        
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
f.add_font("font",ttf_path)
f.render_text("lk","font", 45)