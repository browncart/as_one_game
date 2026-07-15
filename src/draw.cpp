#include "draw.h"
#include "data.h"
#include "puzzles.h"

#include <iostream>

////////// 
// Play //
//////////

std::vector<Texture2D> entities;
std::vector<Texture2D> tiles;

int get_sprite_scale() {
	u8 puzzle_height 	= get_current_puzzle_height();
	u8 puzzle_width		= get_current_puzzle_width(); 

	u8 possible_height 	= (screenHeight / puzzle_height) / spriteSize;
	u8 possible_width	= (screenWidth / puzzle_width) / spriteSize; 

	return std::min(std::min(possible_height, possible_width), (u8)8);
}
int get_tile_size() {
	return get_sprite_scale() * spriteSize;
}

RenderTexture2D create_texture(int screenWidth, int screenHeight) {
	RenderTexture2D texture = LoadRenderTexture(screenWidth, screenHeight);
	BeginTextureMode(texture);
		ClearBackground(BLACK);
	EndTextureMode();
	return texture;
}

Texture2D load_sprite(const char* path) {
	Image image = LoadImage(path);
	Texture2D texture = LoadTextureFromImage(image);
	UnloadImage(image);
	return texture;
};

void load_sprites() {
	entities = {
		load_sprite("data/sprites/boy.png"),
		load_sprite("data/sprites/girl.png"),
		load_sprite("data/sprites/box.png")
	};

	tiles = {
		load_sprite("data/sprites/black_tile.png"),
		load_sprite("data/sprites/gray_tile.png"),
		load_sprite("data/sprites/horizontal_wall.png"),
		load_sprite("data/sprites/horizontal_wall_shadow.png"),
		load_sprite("data/sprites/vertical_wall.png"),
		load_sprite("data/sprites/fire_black.png"),
		load_sprite("data/sprites/fire_gray.png"),
		load_sprite("data/sprites/embers_black.png"),
		load_sprite("data/sprites/embers_gray.png"),
		load_sprite("data/sprites/mushroom_black.png"),
		load_sprite("data/sprites/mushroom_gray.png"),
		load_sprite("data/sprites/sprout_black.png"),
		load_sprite("data/sprites/sprout_gray.png"),
		load_sprite("data/sprites/blue_end_black.png"),
		load_sprite("data/sprites/blue_end_gray.png"),
		load_sprite("data/sprites/pink_end_black.png"),
		load_sprite("data/sprites/pink_end_gray.png"),
		load_sprite("data/sprites/bluepink_end_black.png"),
		load_sprite("data/sprites/bluepink_end_gray.png"),
		load_sprite("data/sprites/purple_end_black.png"),
		load_sprite("data/sprites/purple_end_gray.png"),
		load_sprite("data/sprites/blue_tile_0.png"),
		load_sprite("data/sprites/blue_tile_1.png"),
		load_sprite("data/sprites/blue_tile_2.png"),
		load_sprite("data/sprites/blue_tile_3.png"),
		load_sprite("data/sprites/pink_tile_0.png"),
		load_sprite("data/sprites/pink_tile_1.png"),
		load_sprite("data/sprites/pink_tile_2.png"),
		load_sprite("data/sprites/pink_tile_3.png"),
		load_sprite("data/sprites/teleporter_entrance.png"),
		load_sprite("data/sprites/teleporter_exit.png")
	};
};

int get_puzzle_draw_offset(int tileSize, int axisWidth, int axisScreenWidth) {
	return (axisScreenWidth - (tileSize * axisWidth)) / 2;
}

void clear_background(RenderTexture2D& texture) {
	BeginTextureMode(texture);
		ClearBackground(BLACK);
	EndTextureMode();
}

//update to account for puzzle width....
void draw_puzzle_to_texture(RenderTexture2D& texture, int screenWidth, int screenHeight) {
	int spriteScale = get_sprite_scale();
	int tileSize	= get_tile_size();

	std::vector<u16> puzzle = get_current_puzzle().puzzle;

	int puzzle_size 	= puzzle.size();
	u8 puzzle_width 	= get_current_puzzle_width();
	u8 puzzle_height 	= get_current_puzzle_height();

	BeginTextureMode(texture);
	for (int i = 0; i < puzzle_size; ++i) {

		int tile_x_offset 	= i % puzzle_width;
		int tile_y_offset 	= i / puzzle_width;

		int window_x_offset = get_puzzle_draw_offset(tileSize, puzzle_width, screenWidth);
		int window_y_offset = get_puzzle_draw_offset(tileSize, puzzle_height, screenHeight);

		u8 entity 	= puzzle[i] >> 8;
		u8 tile 	= puzzle[i]; 
		
		Texture2D tile_texture;
		Texture2D entity_texture;

		if (tile < tiles.size() && tile >= 0) {
			tile_texture = tiles[tile];
			DrawTextureEx(tile_texture, Vector2{(float)(tile_x_offset * tileSize) + window_x_offset, (float)(tile_y_offset * tileSize) + window_y_offset}, 0.0, spriteScale, WHITE);
		}
		if (entity < entities.size() && entity >= 0) {
			entity_texture = entities[entity];

			//Darken player sprites on certain puzzle types...
			if (entity == 1 && ((is_pink_disabled() && is_pink_slow_puzzle()) || (!are_pink_and_blue_touching() && is_pink_sticky_puzzle()))) {
				DrawTextureEx(entity_texture, Vector2{(float)(tile_x_offset * tileSize) + window_x_offset, (float)(tile_y_offset * tileSize) + window_y_offset}, 0.0, spriteScale, DARKGRAY);
			} else if (entity == 0 && ((is_blue_disabled() && is_blue_slow_puzzle()) || (!are_pink_and_blue_touching() && is_blue_sticky_puzzle()))) {
				DrawTextureEx(entity_texture, Vector2{(float)(tile_x_offset * tileSize) + window_x_offset, (float)(tile_y_offset * tileSize) + window_y_offset}, 0.0, spriteScale, DARKGRAY);
			} else {
				DrawTextureEx(entity_texture, Vector2{(float)(tile_x_offset * tileSize) + window_x_offset, (float)(tile_y_offset * tileSize) + window_y_offset}, 0.0, spriteScale, WHITE);
			}
		}
	}

	EndTextureMode();
};

/////////////////
//  Play_Menu  //
/////////////////

RenderTexture2D play_menu_setup(int screenWidth, int screenHeight) {
	RenderTexture2D texture = LoadRenderTexture(screenWidth, screenHeight);

	BeginTextureMode(texture);
		ClearBackground(BLACK);
	EndTextureMode();

	return texture;
}

////////////
// Editor //
////////////

std::vector<Texture2D> editor_entities;
std::vector<Texture2D> editor_tiles;

void draw_save_status(bool save_status) {
	if (save_status) {
		DrawText("Saved!", xTilesOffset, yOffset + canvasHeight - fontSize, fontSize, GREEN);
	} else {
		DrawText("Unsaved", xTilesOffset, yOffset + canvasHeight - fontSize, fontSize, RED);
	}
}

RenderTexture2D editor_texture_setup(int screenWidth, int screenHeight) {
	RenderTexture2D texture = LoadRenderTexture(screenWidth, screenHeight);
	
	BeginTextureMode(texture);
		ClearBackground(GRAY);
		draw_canvas_border();

		//Draw Entity Palette
		DrawText("Entities", xEntitiesFont, yEntitiesFont, fontSize, BLACK);
		draw_palette(entities, paletteWidth, xEntitiesOffset, yEntitiesOffset, tileSize);

		int yTilesFont 		= get_tiles_font_y_offset(entities.size() / paletteWidth);
		int yTilesOffset 	= get_tiles_palette_y_offset(entities.size() / paletteWidth);

		//Draw Tile Palette
		DrawText("Tiles", xTilesFont, yTilesFont, fontSize, BLACK);
		draw_palette(tiles, paletteWidth, xTilesOffset, yTilesOffset, tileSize);

		int button_width = 240;
		int button_height = 33;
		int button_x_offset = xOffset;
		int button_y_offset = yOffset + 1024 + 7;

		for (int i = 0; i < 7; ++i) {
			DrawRectangle(button_x_offset, button_y_offset, button_width, button_height, YELLOW);

			if (i == 0) {
				DrawText("Default", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 1) {
				DrawText("Pink Slow", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 2) {
				DrawText("Pink Sticky", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 3) {
				DrawText("Both_Slow", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 4) {
				DrawText("Both_Sticky", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 5) {
				DrawText("Pink_Slow_Blue_Sticky", button_x_offset, button_y_offset, 20, BLACK);
			} else if (i == 6) {
				DrawText("Blue_Slow_Pink_Sticky", button_x_offset, button_y_offset, 20, BLACK);
			}

			button_x_offset += button_width + 5;
		}

	EndTextureMode();
	
	return texture;
}

void draw_selected_puzzle_type_square() {
	Puzzle puzzle = get_current_puzzle();

	float y = yOffset + 1024 + 7;
	float x = xOffset + ((240 + 5) * puzzle.type);

	DrawRectangleLinesEx(Rectangle{x,y,240,33}, 3.0, RED);
}

void draw_selected_palette_square() {
	int yTilesOffset = get_tiles_palette_y_offset(entities.size() / paletteWidth);

	if (storedTile.isEntity) {
		int index = storedTile.storedIndex;
		float y = ((index / paletteWidth) * tileSize) + yEntitiesOffset;
		float x = ((index % paletteWidth) * tileSize) + xEntitiesOffset;

		DrawRectangleLinesEx(Rectangle{x, y, 64, 64}, 4, RED);
	} else {
		int index = storedTile.storedIndex;
		float y = ((index / paletteWidth) * tileSize) + yTilesOffset;
		float x = ((index % paletteWidth) * tileSize) + xTilesOffset;

		DrawRectangleLinesEx(Rectangle{x, y, 64, 64}, 4, RED);
	}
}

void draw_canvas_border() {
	float thickness = 3.0;
	float width		= canvasWidth + (thickness * 2);
	DrawRectangleLinesEx(Rectangle{xOffset - thickness, yOffset - thickness, width, width}, 3.0, BLACK);
}

void load_puzzle_into_canvas(std::vector<u16>& canvas, std::vector<u16>& puzzle, int puzzleWidth, int puzzleHeight) {
	if (puzzle.size() <= canvas.size()) {
		for (int i = 0; i < puzzle.size(); ++i) {
			int row = (i / puzzleWidth) + ((canvasTileWidth - puzzleHeight) / 2);
			int col = (i % puzzleWidth) + ((canvasTileWidth - puzzleWidth) / 2);

			int index = (row * canvasTileWidth) + col;

			canvas[index] = puzzle[i];
		}
	}	
}

void draw_canvas(RenderTexture2D& texture, std::vector<u16>& canvas, int width, int x, int y, int tileSize) {
	BeginTextureMode(texture);
	for (int i = 0; i < canvas.size(); ++i) {
		int row = i / width;
		int col = i - (row * width);

		float xTileOffset = x + (col * tileSize);
		float yTileOffset = y + (row * tileSize);
		
		Rectangle tile = Rectangle{xTileOffset, yTileOffset, (float)tileSize, (float)tileSize};

		if (row + col & 1) DrawRectangleRec(tile, RAYWHITE);
		else DrawRectangleRec(tile, LIGHTGRAY);

		u16 canvasTile 	= canvas[i];
		u8 tileByte 	= canvasTile;
		u8 entityByte 	= canvasTile >> 8;

		if (tileByte != 0xff) DrawTextureEx(tiles[tileByte], Vector2{xTileOffset, yTileOffset}, (float)0, (float)4, RAYWHITE);
		if (entityByte != 0xff) DrawTextureEx(entities[entityByte], Vector2{xTileOffset, yTileOffset}, (float)0, (float)4, RAYWHITE);
	}
	EndTextureMode();
}

void draw_palette(std::vector<Texture2D>& palette, int width, int x, int y, int tileSize) {
	for (int i = 0; i < palette.size(); ++i) {
		int row = i / width;
		int col = i - (row * width);

		float xTileOffset = x + (col * tileSize);
		float yTileOffset = y + (row * tileSize);

		Rectangle tile = Rectangle{xTileOffset, yTileOffset, (float)tileSize, (float)tileSize};
		DrawRectangleRec(tile, BLACK);
		DrawTextureEx(palette[i], Vector2{xTileOffset, yTileOffset}, (float)0, (float)4, RAYWHITE);
	}	
}

//////////////////
//  Level Menu  //
//////////////////

std::vector<RenderTexture2D> puzzle_previews;

std::vector<RenderTexture2D> get_puzzle_previews() {
	return puzzle_previews;
}

void draw_move_puzzle_overlay(int hover_index, bool left) {
	float y = hover_index / 16;
	float x = hover_index % 16;

	if (left) {
		DrawRectangleRec((Rectangle){x * 120, y * 120, (float)60, (float)120}, GREEN);
	} else {
		DrawRectangleRec((Rectangle){(x * 120) + 60, y * 120, (float)60, (float)120}, GREEN);
	}
}

void draw_hovered_puzzle_outline(int selected_index, int hover_index) {
	if (selected_index != hover_index) {
		float y = hover_index / 16;
		float x = hover_index % 16;

		DrawRectangleLinesEx((Rectangle){x * 120, y * 120, (float)120, (float)120}, 6.0, GRAY);
	}
}

void draw_selected_puzzle_outline(int selected_index) {
	float y = selected_index / 16;
	float x = selected_index % 16;

	DrawRectangleLinesEx((Rectangle){x * 120, y * 120, (float)120, (float)120}, 6.0, GREEN);
}

RenderTexture2D load_puzzle_preview(Puzzle puzzle) {
	u8 texture_width 	= 120;
	u8 texture_height 	= 120;

	RenderTexture2D texture = LoadRenderTexture(texture_width, texture_height);
	
	BeginTextureMode(texture);
		ClearBackground(BLACK);
		draw_puzzle_to_preview_texture(texture, texture_width, texture_height, puzzle);
	EndTextureMode();
	
	return texture;
}

void load_puzzle_previews(std::vector<Puzzle> puzzles) {
	for (auto puzzle : puzzles) {
		puzzle_previews.push_back(load_puzzle_preview(puzzle));
	}
}

void reload_puzzle_preview(std::vector<Puzzle> puzzles, int puzzle_index) {
	RenderTexture2D old_preview = puzzle_previews[puzzle_index];
	RenderTexture2D new_preview = load_puzzle_preview(puzzles[puzzle_index]);

	puzzle_previews[puzzle_index] = new_preview;
	UnloadRenderTexture(old_preview);
}

std::vector<RenderTexture2D> reload_puzzle_previews(std::vector<Puzzle> puzzles) {
	for (auto preview : puzzle_previews) {
		UnloadRenderTexture(preview);
	}
	puzzle_previews.clear();

	for (auto puzzle : puzzles) {
		puzzle_previews.push_back(load_puzzle_preview(puzzle));
	}

	return puzzle_previews;
}

void draw_puzzle_to_preview_texture(RenderTexture2D& texture, int texture_width, int texture_height, Puzzle puzzle) {
	int puzzle_size 	= puzzle.puzzle.size();
	u8 puzzle_width 	= puzzle.width;
	u8 puzzle_height 	= puzzle.height;
	u8 cell_size 		= 8;

	u8 smaller_scale = std::min(puzzle_width, puzzle_height);

	int spriteScale = (texture_height / smaller_scale) / cell_size;

	BeginTextureMode(texture);
	for (int i = 0; i < puzzle_size; ++i) {

		int tile_x_offset 	= i % puzzle_width;
		int tile_y_offset 	= i / puzzle_width;

		int window_x_offset = get_puzzle_draw_offset(cell_size, puzzle_width, texture_width);
		int window_y_offset = get_puzzle_draw_offset(cell_size, puzzle_height, texture_height);

		u8 entity 	= puzzle.puzzle[i] >> 8;
		u8 tile 	= puzzle.puzzle[i]; 
		
		Texture2D tile_texture;
		Texture2D entity_texture;

		if (tile < tiles.size() && tile >= 0) {
			Color tile_color = WHITE;

			if (tile == Horizontal_Wall || tile == Horizontal_Wall_Shadow || tile == Vertical_Wall) {
				tile_color = WHITE;
			}
			if (tile == Fire_Black || tile == Fire_Gray || tile == Embers_Black || tile == Embers_Gray) {
				tile_color = RED;
			}
			if (tile == Black_Tile || tile == Gray_Tile) {
				tile_color = BLACK;
			}
			if (tile == Mushroom_Black || tile == Mushroom_Gray || tile == Sprout_Black || tile == Sprout_Gray) {
				tile_color = GREEN;
			}
			if (tile == Pink_End_Black || tile == Pink_End_Gray) {
				tile_color = PINK;
			}
			if (tile == Blue_End_Black || tile == Blue_End_Gray) {
				tile_color = SKYBLUE;
			}
			if (tile == Purple_End_Black || tile == Purple_End_Gray) {
				tile_color = PURPLE;
			}
			if (tile == Pink_Tile_0 || tile == Pink_Tile_1 || tile == Pink_Tile_2 || tile == Pink_Tile_3) {
				tile_color = PINK;
			}
			if (tile == Blue_Tile_0 || tile == Blue_Tile_1 || tile == Blue_Tile_2 || tile == Blue_Tile_3) {
				tile_color = SKYBLUE;
			}
			if (tile == Teleporter_Entrance || tile == Teleporter_Exit) {
				tile_color = YELLOW;
			}

			if (tile == BluePink_End_Black || tile == BluePink_End_Gray) {
				Vector2 v1 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset, (float)(tile_y_offset * cell_size) + window_y_offset};
				Vector2 v2 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset, (float)(tile_y_offset * cell_size) + window_y_offset + 8};
				Vector2 v3 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset + 8, (float)(tile_y_offset * cell_size) + window_y_offset};

				DrawTriangle(v1, v2, v3, PINK);

				v1 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset + 8, (float)(tile_y_offset * cell_size) + window_y_offset};
				v2 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset, (float)(tile_y_offset * cell_size) + window_y_offset + 8};
				v3 = Vector2{(float)(tile_x_offset * cell_size) + window_x_offset + 8, (float)(tile_y_offset * cell_size) + window_y_offset + 8};

				DrawTriangle(v1, v2, v3, SKYBLUE);
			} else {
				DrawRectangleV(Vector2{(float)(tile_x_offset * cell_size) + window_x_offset, (float)(tile_y_offset * cell_size) + window_y_offset}, Vector2{8,8}, tile_color); 
			}
		}

		if (entity < entities.size() && entity >= 0) {
			Color tile_color = BLUE;

			if (entity == Boy) {
				tile_color = BLUE;
			}
			if (entity == Girl) {
				tile_color = MAGENTA;
			}
			if (entity == Box) {
				tile_color = VIOLET;
			}

			DrawCircleV(Vector2{(float)(tile_x_offset * cell_size) + window_x_offset + 4, (float)(tile_y_offset * cell_size) + window_y_offset + 4}, 4.0, tile_color);
		}
	}

	EndTextureMode();
};

/////////////////
//  Main Menu  //
/////////////////

void draw_game_title(RenderTexture2D& texture) {
	int x = texture.texture.width;
	int y = texture.texture.height;

	int font_size = 130;
	int textWidth = MeasureText("As One", font_size);

	
	BeginTextureMode(texture);
		DrawText("As One", (x / 2) - (textWidth / 2), y / 10, font_size, WHITE);
	EndTextureMode();
}

struct Button {
	char* name;
	int x;
	int y;
	int width;
	int height;
	Color plain_color;
	Color hover_color;
	Color click_color;
};

void draw_main_menu_buttons(RenderTexture2D& texture, Color play_button, Color edit_button, Color level_button, Color quit_button) {
	int x = texture.texture.width;
	int y = texture.texture.height;

	int y_offset = y / 20;
	int x_offset = (x / 2);

	int font_size = 80;

	int play_width = MeasureText("Play", font_size);
	int edit_width = MeasureText("Editor", font_size);
	int menu_width = MeasureText("Level Menu", font_size);
	int quit_width = MeasureText("Quit", font_size);

	BeginTextureMode(texture);
		DrawRectangle(x_offset - 300, y_offset * 7, 600, 100, play_button);
		DrawText("Play", x_offset - (play_width / 2), y_offset * 7 + (100 - font_size) / 2, font_size, BLACK);
		DrawRectangleLinesEx((Rectangle){(float)x_offset - 300, (float)y_offset * 7, 600, 100}, 5.0, DARKGRAY);

		DrawRectangle(x_offset - 300, y_offset * 10, 600, 100, edit_button);
		DrawText("Editor", x_offset - (edit_width / 2), y_offset * 10 + (100 - font_size) / 2, font_size, BLACK);
		DrawRectangleLinesEx((Rectangle){(float)x_offset - 300, (float)y_offset * 10, 600, 100}, 5.0, DARKGRAY);

		DrawRectangle(x_offset - 300,  y_offset * 13, 600, 100, level_button);
		DrawText("Level Menu", x_offset - (menu_width / 2), y_offset * 13 + (100 - font_size) / 2, font_size, BLACK);
		DrawRectangleLinesEx((Rectangle){(float)x_offset - 300, (float)y_offset * 13, 600, 100}, 5.0, DARKGRAY);

		DrawRectangle(x_offset - 300,  y_offset * 16, 600, 100, quit_button);
		DrawText("Quit", x_offset - (quit_width / 2), y_offset * 16 + (100 - font_size) / 2, font_size, BLACK);
		DrawRectangleLinesEx((Rectangle){(float)x_offset - 300, (float)y_offset * 16, 600, 100}, 5.0, DARKGRAY);
	EndTextureMode();
}