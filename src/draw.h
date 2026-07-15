#ifndef DRAW_H
#define DRAW_H

#include "puzzles.h"

#include <raylib.h>
#include <vector>

////////////
//  Play  //
////////////

extern std::vector<Texture2D> entities;
extern std::vector<Texture2D> tiles;
extern std::vector<Texture2D> sprites;

int get_sprite_scale			();
int get_tile_size				();

RenderTexture2D create_texture	(int screenWidth, int screenHeight);

Texture2D load_sprite			(const char* path);
void load_sprites				();

void draw_puzzle_to_texture		(RenderTexture2D& texture, int screenWidth, int screenHeight);

int get_puzzle_draw_offset		(int tileSize, int axisWidth, int axisScreenWidth);
void clear_background			(RenderTexture2D& texture);

////////////
// Editor //
////////////

extern std::vector<Texture2D> editor_entities;
extern std::vector<Texture2D> editor_tiles;

void draw_save_status					(bool save_status);

RenderTexture2D editor_texture_setup	(int screenWidth, int screenHeight);

void draw_selected_puzzle_type_square	();
void draw_selected_palette_square		();
void draw_canvas_border					();
void load_puzzle_into_canvas			(std::vector<u16>& canvas, std::vector<u16>& puzzle, int puzzleWidth, int puzzleHeight);
void draw_canvas						(RenderTexture2D& texture, std::vector<u16>& canvas, int width, int x, int y, int tileSize);
void draw_palette						(std::vector<Texture2D>& palette, int width, int x, int y, int tileSize);

//////////////////
//  Level Menu  //
//////////////////

extern std::vector<RenderTexture2D> puzzle_previews;

void 							draw_move_puzzle_overlay		(int hover_index, bool left);
void 							draw_selected_puzzle_outline	(int selected_index);
void 							draw_hovered_puzzle_outline		(int selected_index, int hover_index);
std::vector<RenderTexture2D> 	get_puzzle_previews	();
RenderTexture2D 				load_puzzle_preview				(Puzzle puzzle);
void 							load_puzzle_previews			(std::vector<Puzzle> puzzles);
void 							reload_puzzle_preview			(std::vector<Puzzle> puzzles, int puzzle_index);
std::vector<RenderTexture2D> 	reload_puzzle_previews			(std::vector<Puzzle> puzzles);
void 							draw_puzzle_to_preview_texture	(RenderTexture2D& texture, int texture_width, int texture_height, Puzzle puzzle);

/////////////////
//  Main Menu  //
/////////////////

void draw_game_title		(RenderTexture2D& texture);
void draw_main_menu_buttons	(RenderTexture2D& texture, Color play_button, Color edit_button, Color level_button, Color quit_button);

#endif