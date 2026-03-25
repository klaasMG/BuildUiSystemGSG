# noinspection PyUnresolvedReferences
from font_holder import Font
# noinspection PyUnresolvedReferences
from GridPacker import TextPacker, PlacedRect
from PIL import Image
from pathlib import Path
from hold_lock import HoldLock

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
        self.font_map_image = Image.new("L",(8192,8912),1)
        self.current_text_id: int = 0
        self.text_packer = TextPacker()
        self.placed_rects: dict[int, list[int]] = {}
        self.fonts: dict[str, Font] = {}
        self.text_lock = HoldLock()
        self.has_changed = False
        self.last_text_box_list = []
        self.bool = True
    
    def render_text(self, text: str, font: str, text_height: int, text_id: int):
        self.has_changed = True
        render_font = self.fonts[font]
        
        # Compute width
        text_length = 0
        for char in text:
            advance = render_font.get_render_info(text_height, ord(char))[0]
            text_length += advance
        
        # Image
        text_image = Image.new("L", (text_length, text_height), 0)
        
        # Use ascent as baseline
        if len(text) > 0:
            _, _, scaledAscent, scaledDescent, _ = render_font.get_render_info(text_height, ord(text[0]))
        else:
            scaledAscent = text_height
        
        baseline_y = scaledAscent
        
        cursor_x = 0
        for char in text:
            rendered_char, rendered_info = self.render_char(char, render_font, text_height)
            char_image = Image.fromarray(rendered_char, "L")
            
            advance, lsb, scaledAscent, scaledDescent, lineGap = rendered_info
            
            # --- FIX ---
            # Place glyph so its bottom roughly aligns with baseline
            y = baseline_y - char_image.height
            
            text_image.paste(char_image, (cursor_x + lsb, int(y)))
            
            cursor_x += advance
        
        self.update_render_image(text_image, text_id)
    
    def update_render_image(self, text_image: Image.Image, text_id: int):
        width, height = text_image.size
        pack_data = self.text_packer.add(text_id, width, height)
        is_packed: bool = pack_data[0]
        if not is_packed:
            raise Exception("")
        placed_rect: PlacedRect = pack_data[1]
        pos_x = placed_rect.pos_x
        pos_y = placed_rect.pos_y
        self.text_lock.lock()
        self.font_map_image.paste(text_image,(pos_x, pos_y, pos_x + width, pos_y + height))
        self.placed_rects[placed_rect.id] = [pos_x, pos_y, pos_x + width, pos_y + height]
        self.text_lock.release()
        
    def add_font(self, font_name: str, font_file: Path):
        font_file = str(font_file)
        font = Font(font_file)
        self.fonts[font_name] = font
    
    def remove_font(self, font_name):
        self.fonts[font_name] = None
        
    def get_render_info(self):
        locked: bool = self.text_lock.lock(0.01)
        if locked:
            text_box_data = dict_to_flat_list(self.placed_rects)
            self.last_text_box_list = text_box_data
        else:
            text_box_data = self.last_text_box_list
        if self.has_changed:
            has_changed: bool = True
            self.has_changed = False
        else:
            has_changed = False
        return text_box_data, has_changed
        
    @staticmethod
    def render_char( char: str, render_font: Font, text_height: int):
        try:
            char_array = render_font.get_raster_from_glyph(text_height, ord(char))
            render_info = render_font.get_render_info(text_height, ord(char))
        except ValueError:
            char_array = render_font.get_raster_from_glyph(text_height, 0)
            render_info = render_font.get_render_info(text_height, 0)
        
        return char_array, render_info