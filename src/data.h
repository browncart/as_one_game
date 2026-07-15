#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <raylib.h>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;

enum Mode {
	Play,
	Edit,
	Level_Menu,
	Main_Menu
};

extern Mode mode;

struct StoredTile {
	bool isEntity;
	u8 storedIndex;

	StoredTile(bool isEntity, u8 storedIndex) {
		this->isEntity 		= isEntity;
		this->storedIndex 	= storedIndex;
	}
};

extern bool game_over;

//Window
extern const int spriteSize; 
extern const int screenWidth;
extern const int screenHeight;

//Puzzle
extern const int yPadding;
extern const int xPadding;

//Draw Selection Squar
extern StoredTile storedTile;

//Canvas
extern float tileSize;
extern float canvasWidth;
extern float canvasHeight;
extern float xOffset;
extern float yOffset;
extern int canvasTileWidth;

//Palettes
extern int paletteWidthP;
extern int paletteWidth;

//Font
extern int fontSize;

//Entities
extern int xEntitiesFont;
extern int yEntitiesFont;
extern int xEntitiesOffset;
extern int yEntitiesOffset;

//Tiles
extern int xTilesFont;
extern int xTilesOffset;
int get_tiles_font_y_offset(int entities_palette_height);
int get_tiles_palette_y_offset(int entities_palette_height);

//Arrays
extern std::vector<u16> canvas;

#endif