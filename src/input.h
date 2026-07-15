#ifndef INPUT_H
#define INPUT_H

#include "puzzles.h"
#include <raylib.h>

enum Movement_Type {
	Normal,
	Half_Step,
	Sticky
};

///////////////
//  General  //
///////////////

void switch_to_play_on_key			(RenderTexture2D& game_texture);
void switch_to_editor_on_key		();
void switch_to_level_menu_on_key	();
void switch_to_main_menu_on_key		();

void switch_to_play					(RenderTexture2D& game_texture);
void switch_to_editor				();
void switch_to_level_menu			();

////////////
//  Play  //
////////////

void create_new_puzzle	();
void try_save			(Puzzle& puzzle, int index, bool valid_save);

void move				(Puzzle& puzzle);
void go_next_puzzle		(RenderTexture2D& game_texture);
void go_prev_puzzle		(RenderTexture2D& game_texture);
void undo				();
void restart			();

////////////
// Editor //
////////////

extern std::vector<std::vector<u16>> editor_history;

void editor_undo				();

void handle_mouse_hover			();
void handle_left_mouse_click	();
void handle_left_mouse_held		();
void handle_left_mouse_release	();

void handle_right_mouse_click	();
void handle_right_mouse_held	();

void in_puzzle_type				(Vector2 mousePos, Puzzle& puzzle);
int in_canvas					(Vector2 mousePos);
int in_palette					(std::vector<Texture2D>& palette, int xOffset, int yOffset, Vector2 mousePos);

//////////////////
//  Level Menu  //
//////////////////

void select_puzzle			(int index);
void select_puzzle_and_move	(int index);
void move_puzzle			(int index);
void delete_puzzle			();

/////////////////
//  Main Menu  //
/////////////////

void exit			();
void hover_button	(Color& play_button, Color& edit_button, Color& level_button, Color& quit_button);
void click_button	(Color& play_button, Color& edit_button, Color& level_button, Color& quit_button);
void release_button	(RenderTexture2D& game_texture, Color& play_button, Color& edit_button, Color& level_button, Color& quit_button);

#endif