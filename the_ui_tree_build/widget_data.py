from enum import Enum

class WidgetDataType(Enum):
    POSITION = 0
    SHADER_PASS = 1
    COLOUR = 2
    SHAPE = 3
    ASSETS_ID = 4
    TEXT_ID = 5
    ASSETS = 6
    TEXT = 7
    PARENT = 8
    ASSET_OR_TEXT = 9
    PATH_OR_DATA = 10