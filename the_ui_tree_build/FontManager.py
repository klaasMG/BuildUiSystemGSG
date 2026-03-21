# noinspection PyUnresolvedReferences
from font_holder import Font
# noinspection PyUnresolvedReferences
from GridPacker import TextPacker, PlacedRect
from PIL import Image
from pathlib import Path

ttf_path = Path("assets/fonts/AovelSansRounded-rdDL.ttf")


def dict_to_flat_list(d: dict[int, list[int]]) -> list[int]:
    if not d:
        return []
    max_id = max(d.keys())
    result = [-1] * ((max_id + 1) * 4)
    for i, values in d.items():
        base = i * 4
        result[base:base + 4] = values
    return result

class FontManager:
    def __init__(self):
        self.font_map_image = Image.new("L",(8192,8912),0)
        self.current_text_id: int = 0
        self.text_packer = TextPacker()
        self.placed_rects: dict[int, list[int]] = {}
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
        self.update_render_image(text_image)
    
    def update_render_image(self, text_image: Image.Image):
        width, height = text_image.size
        text_id = self.current_text_id
        self.current_text_id += 1
        pack_data = self.text_packer.add(text_id, width, height)
        is_packed: bool = pack_data[0]
        if not is_packed:
            raise Exception("")
        placed_rect: PlacedRect = pack_data[1]
        pos_x = placed_rect.pos_x
        pos_y = placed_rect.pos_y
        self.font_map_image.paste(text_image,(pos_x, pos_y, pos_x + width, pos_y + height))
        self.placed_rects[placed_rect.id] = [pos_x, pos_y, pos_x + width, pos_y + height]
        
    def add_font(self, font_name: str, font_file: Path):
        font_file = str(font_file)
        font = Font(font_file)
        self.fonts[font_name] = font
    
    def remove_font(self, font_name):
        self.fonts[font_name] = None
        
    def get_render_info(self):
        return dict_to_flat_list(self.placed_rects)
        
    @staticmethod
    def render_char( char: str, render_font: Font, text_height: int):
        try:
            char_array = render_font.get_raster_from_glyph(text_height, ord(char))
            render_info = render_font.get_render_info(text_height, ord(char))
        except ValueError:
            char_array = render_font.get_raster_from_glyph(text_height, 0)
            render_info = render_font.get_render_info(text_height, 0)
        
        return char_array, render_info