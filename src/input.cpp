#include "input.h"
#include "puzzles.h"
#include "draw.h"

#include <vector>
#include <iostream>

///////////////
//  General  //
///////////////

void switch_to_play_on_key(RenderTexture2D& game_texture) {
	if (IsKeyPressed(KEY_P) && mode != Mode::Play) {
		switch_to_play(game_texture);
	}
}

void switch_to_editor_on_key() {
	if (IsKeyPressed(KEY_E) && mode != Mode::Edit) {
		switch_to_editor();
	}
}

void switch_to_level_menu_on_key() {
	if (IsKeyPressed(KEY_L) && mode != Mode::Level_Menu) {
		switch_to_level_menu();
	}
}

void switch_to_main_menu_on_key() {
	//need to return to previous mode on second press...
	if (IsKeyPressed(KEY_ESCAPE) && mode != Mode::Main_Menu) {
		game_over = false;
		mode = Mode::Main_Menu;
	}
}

void switch_to_play(RenderTexture2D& game_texture) {
	mode = Mode::Play;
	game_over = false;
	clear_background(game_texture);
	restart_level();
	history.clear();
}

void switch_to_editor() {
	game_over = false;
	mode = Mode::Edit;
		
	int edit_puzzle_width 	= get_current_puzzle_width();
	int edit_puzzle_height 	= get_current_puzzle_height(); 

	for (int i = 0; i < canvas.size(); ++i) {
		canvas[i] = 0xffff;
	}

	restart_level();
	history.clear();

	std::vector<u16> current_puzzle = get_current_puzzle().puzzle;

	//this can actually be done once when we select a puzzle,
	//then we can just display the canvas on mode switch...
	load_puzzle_into_canvas(canvas, current_puzzle, edit_puzzle_width, edit_puzzle_height);
}

void switch_to_level_menu() {
	game_over = false;
	mode = Mode::Level_Menu;		
}

////////////
//  Play  //
////////////

void move(Puzzle& puzzle) {
	if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT)) {
		//this will double-count a position when you push into a wall
		//and no elements move

		u8 pink_index = get_pink_player_index();
		u8 blue_index = get_blue_player_index();

		if (history.empty() || history.back().state != puzzle.puzzle) {
			History event = {
				.state = puzzle.puzzle,
				.blue_player = puzzle.blue_player,
				.pink_player = puzzle.pink_player,
			};
			
			history.push_back(event);

		}

		int input = GetKeyPressed();

		if (puzzle.type == PuzzleType::Default) {
			try_move(input, blue_index);
		
			u8 new_pink_index = get_pink_player_index();

			if (new_pink_index == pink_index) {
				try_move(input, pink_index);
			}
		}

		if (puzzle.type == PuzzleType::Pink_Slow) {
			try_move(input, blue_index);
		
			u8 new_pink_index = get_pink_player_index();

			if (!current_puzzle.pink_disabled && new_pink_index == pink_index) {
				try_move(input, pink_index);
			}

			current_puzzle.pink_disabled = !current_puzzle.pink_disabled;
		}

		if (puzzle.type == PuzzleType::Pink_Sticky) {
			bool are_touching = false;

			if (std::abs(blue_index - pink_index) == 1 || std::abs(blue_index - pink_index) == get_current_puzzle_width()) {
				are_touching = true;
			}

			try_move(input, blue_index);

			u8 new_pink_index = get_pink_player_index();

			if (new_pink_index == pink_index && are_touching) {
				try_move(input, pink_index);
			}
		}

		if (puzzle.type == PuzzleType::Both_Slow) {
			if (!current_puzzle.blue_disabled) {
				try_move(input, blue_index);
			}
		
			u8 new_pink_index = get_pink_player_index();

			if (!current_puzzle.pink_disabled && new_pink_index == pink_index) {
				try_move(input, pink_index);
			}

			current_puzzle.pink_disabled = !current_puzzle.pink_disabled;
			current_puzzle.blue_disabled = !current_puzzle.blue_disabled;
		}

		if (puzzle.type == PuzzleType::Both_Sticky) {
			bool are_touching = false;

			if (std::abs(blue_index - pink_index) == 1 || std::abs(blue_index - pink_index) == get_current_puzzle_width()) {
				are_touching = true;
			}

			if (are_touching) {
				try_move(input, blue_index);
				
				u8 new_pink_index = get_pink_player_index();

				if (new_pink_index == pink_index && are_touching) {
					try_move(input, pink_index);
				}
			}
		}

		if (puzzle.type == PuzzleType::Pink_Slow_Blue_Sticky) {
			bool are_touching = false;

			if (std::abs(blue_index - pink_index) == 1 || std::abs(blue_index - pink_index) == get_current_puzzle_width()) {
				are_touching = true;
			}

			if (are_touching) {				
				try_move(input, blue_index);
			}

			u8 new_pink_index = get_pink_player_index();

			if (!current_puzzle.pink_disabled && new_pink_index == pink_index) {
				try_move(input, pink_index);
			}

			current_puzzle.pink_disabled = !current_puzzle.pink_disabled;
		}

		if (puzzle.type == PuzzleType::Blue_Slow_Pink_Sticky) {
			bool are_touching = false;

			if (std::abs(blue_index - pink_index) == 1 || std::abs(blue_index - pink_index) == get_current_puzzle_width()) {
				are_touching = true;
			}

			if (!current_puzzle.blue_disabled) {
				try_move(input, blue_index);
			}

			u8 new_pink_index = get_pink_player_index();

			if (are_touching && new_pink_index == pink_index) {
				try_move(input, pink_index);
			}

			current_puzzle.blue_disabled = !current_puzzle.blue_disabled;
		}

		try_teleport_entity();
		try_grow_mushrooms();
		is_on_fire();
	}
}

void go_next_puzzle(RenderTexture2D& game_texture) {
	if (IsKeyPressed(KEY_EQUAL)) {
		if (try_increment_puzzle()) {
			clear_background(game_texture);
		}
	}
}

void go_prev_puzzle(RenderTexture2D& game_texture) {
	if (IsKeyPressed(KEY_MINUS)) {
		if (try_decrement_puzzle()) {
			history.clear();
			clear_background(game_texture);
		}
	}
}

void undo() {
	if (IsKeyPressed(KEY_Z)) {
		game_over = false;
		undo_last_move();
	}	
}

void restart() {
	if (IsKeyPressed(KEY_R)) {
		game_over = false;
		restart_level();
		history.clear();
	}
}

////////////
// Editor //
////////////

std::vector<std::vector<u16>> editor_history;

void try_save(Puzzle& puzzle, int index, bool valid_save) {
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S) && valid_save) {
		std::cout << "trying to save!" << std::endl;
		overwrite_puzzle_in_puzzles(puzzle, index);

		reload_puzzle_preview(get_puzzles(), get_puzzle_index());
		save_puzzles_to_file();
	}
}

void editor_undo() {
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z)) {
		if (!editor_history.empty()) {
			canvas = editor_history.back();
			editor_history.pop_back();
		}
	}
}

void create_new_puzzle() {
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_N)) {
		RenderTexture2D new_preview = load_puzzle_preview(current_puzzle);
		puzzle_previews.push_back(new_preview);

		std::cout << "creating new puzzle" << std::endl;
		create_new_puzzle_and_update_vals();
	}
}

void handle_mouse_hover() {
	Vector2 mousePos = GetMousePosition();
	int index = in_canvas(mousePos);

	if (index < 0) return; 

	float y = ((index / canvasTileWidth) * tileSize) + yOffset;
	float x = ((index % canvasTileWidth) * tileSize) + xOffset;

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		DrawRectangleLinesEx(Rectangle{x, y, tileSize, tileSize}, 6, RED);
	} else {
		if (storedTile.storedIndex >= 0) {
			if (storedTile.isEntity) {
				DrawTextureEx(entities[storedTile.storedIndex], Vector2{x,y}, 0, 4.0, RAYWHITE);
			} else {
				DrawTextureEx(tiles[storedTile.storedIndex], Vector2{x,y}, 0, 4.0, RAYWHITE);
			}
		}
	}
}

void handle_right_mouse_click() {
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		Vector2 mousePos = GetMousePosition();
		
		int index = in_canvas(mousePos);
		if (index >= 0) {
			editor_history.push_back(canvas);
		}
	}
}

void handle_right_mouse_held() {
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		Vector2 mousePos = GetMousePosition();

		int index = in_canvas(mousePos);
		if (index >= 0) {
			canvas[index] = 0xffff;
			return;
		}	
	}
}

void handle_left_mouse_click() {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();

		int index = in_canvas(mousePos);
		if (index >= 0) {
			editor_history.push_back(canvas);
			return;
		}

		//Entities
		index = in_palette(entities, xEntitiesOffset, yEntitiesOffset, mousePos);
		if (index >= 0) {
			storedTile.storedIndex 	= index;
			storedTile.isEntity 	= true;
			return;
		}

		int yTilesOffset = get_tiles_palette_y_offset(entities.size() / paletteWidth);

		//Tiles
		index = in_palette(tiles, xTilesOffset, yTilesOffset, mousePos);
		if (index >= 0) {
			storedTile.storedIndex 	= index;
			storedTile.isEntity		= false;
			return;
		}

		//Puzzle Types
		in_puzzle_type(mousePos, current_puzzle);

	}
}

void handle_left_mouse_held() {
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		Vector2 mousePos = GetMousePosition();

		//Canvas
		int index = in_canvas(mousePos);
		if (index >= 0) {
			u16 replacement;
			if (storedTile.isEntity) {
				u16 cell	= canvas[index];
				u16 mask 	= 0x00ff; 
				u16 masked 	= cell & mask;
				replacement	= storedTile.storedIndex << 8 | masked;
			} else {
				u16 cell	= canvas[index];
				u16 mask 	= 0xff00;
				u16 masked	= cell & mask;
				replacement = masked | storedTile.storedIndex;
			}

			canvas[index] = replacement;
		}
	}
}

void handle_left_mouse_release() {
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		std::cout << "released left mouse button..." << std::endl;
	}
}

void in_puzzle_type(Vector2 mousePos, Puzzle& puzzle) {
	int y = mousePos.y - (yOffset + 1024 + 7);
	int x = mousePos.x - xOffset;
	PuzzleType new_type = PuzzleType::Default;

	if (y >= 0 || y < 34 && x >= 0 && x < 1680) {
		if (x < 240) {
			new_type = PuzzleType::Default;
		} else if (x < 480) {
			new_type = PuzzleType::Pink_Slow;
		} else if (x < 720) {
			new_type = PuzzleType::Pink_Sticky;
		} else if (x < 960) {
			new_type = PuzzleType::Both_Slow;
		} else if (x < 1200) {
			new_type = PuzzleType::Both_Sticky;
		} else if (x < 1440) {
			new_type = PuzzleType::Pink_Slow_Blue_Sticky;
		} else if (x < 1680) {
			new_type = PuzzleType::Blue_Slow_Pink_Sticky;
		}

		puzzle.type = new_type;
	}
}

int in_canvas(Vector2 mousePos) {
	int x = mousePos.x - xOffset;
	int y = mousePos.y - yOffset;

	if (x >= 0 && x < canvasWidth && y >= 0 && y < canvasHeight) {
		int xTile = x / tileSize;
		int yTile = y / tileSize;

		return (yTile * canvasTileWidth) + xTile;
	}
	return -1;
}

int in_palette(std::vector<Texture2D>& palette, int xOffset, int yOffset, Vector2 mousePos) {
	int x 				= mousePos.x - xOffset;
	int y 				= mousePos.y - yOffset;
	int fullRows 		= palette.size() / 3;
	int fullRowHeight 	= fullRows * tileSize;

	if (fullRowHeight > 0 && x >= 0 && x < paletteWidthP && y >= 0 && y < fullRowHeight) {
		int xTile = x / tileSize;
		int yTile = y / tileSize;

		return (yTile * paletteWidth) + xTile;
	}

	int partialRow 			= palette.size() % 3;
	int partialRowLength 	= partialRow * tileSize;

	y = y - fullRowHeight;

	if (x >= 0 && x < partialRowLength && y >= 0 && y < tileSize) {
		return (fullRows * paletteWidth) + (x / tileSize);
	}
	return -1;
}

//////////////////
//  Level Menu  //
//////////////////

void select_puzzle(int index) {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mouse_position = GetMousePosition();

		set_current_puzzle_and_index(index);
	}
}

void select_puzzle_and_move(int index) {
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		int x = GetMouseX();
		int y = GetMouseY();

		if (x > screenWidth || x < 0 || y > screenHeight || y < 0) return;

		int puzzles_size = puzzles.size();

		if (index != get_puzzle_index() && index >= 0 && index < puzzles_size) {
			int x_local = x % 120;

			if (x_local <= 60) {
				//Left
				draw_move_puzzle_overlay(index, true);
			} else {
				//Right
				draw_move_puzzle_overlay(index, false);
			}
		}
	}
}

void move_puzzle(int mouse_hover_puzzle_index) {
	int current_index = get_puzzle_index();
	if (mouse_hover_puzzle_index < 0 || mouse_hover_puzzle_index >= puzzles.size() || mouse_hover_puzzle_index == current_index) return;

	int x 								= GetMouseX();
	bool left 							= x % 120 <= 60 ? true : false;

	Puzzle moving_puzzle 				= get_current_puzzle();
	RenderTexture2D moving_preview 	= puzzle_previews[puzzle_index];

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		if (current_index > mouse_hover_puzzle_index) {
			mouse_hover_puzzle_index = left ? mouse_hover_puzzle_index : mouse_hover_puzzle_index + 1;
			
			for (int i = mouse_hover_puzzle_index; i <= current_index; ++i) {
				Puzzle temp_puzzle 				= puzzles[i];
				RenderTexture2D temp_preview 	= puzzle_previews[i];
				puzzle_previews[i] 				= moving_preview; 
				puzzles[i] 						= moving_puzzle;
				moving_preview 				= temp_preview; 
				moving_puzzle 					= temp_puzzle;
			}

			set_current_puzzle_and_index(mouse_hover_puzzle_index);
		} else {
			mouse_hover_puzzle_index = left ? mouse_hover_puzzle_index - 1 : mouse_hover_puzzle_index;

			for (int i = mouse_hover_puzzle_index; i >= current_index; --i) {
				Puzzle temp_puzzle 				= puzzles[i];
				RenderTexture2D temp_preview 	= puzzle_previews[i];
				puzzle_previews[i] 				= moving_preview; 
				puzzles[i] 						= moving_puzzle;
				moving_preview 				= temp_preview; 
				moving_puzzle 					= temp_puzzle;
			}

			set_current_puzzle_and_index(mouse_hover_puzzle_index);
		}

		save_puzzles_to_file();
	}
}

void delete_puzzle() {
	int current_index = get_puzzle_index();

	if (IsKeyPressed(KEY_D)) {

		for (int i = current_index; i < puzzles.size() - 1; ++i) {
			puzzles[i] = puzzles[i + 1];
		}
		puzzles.pop_back();

		for (int i = current_index; i < puzzle_previews.size() - 1; ++i) {
			puzzle_previews[i] = puzzle_previews[i + 1];
		}
		puzzle_previews.pop_back();

		if (current_index >= puzzles.size()) {
			current_index = puzzles.size() - 1;
		}
		set_current_puzzle_and_index(current_index);
		save_puzzles_to_file();
	}
}

/////////////////
//  Main Menu  //
/////////////////

void hover_button(Color& play_button, Color& edit_button, Color& level_button, Color& quit_button) {
	Color click = Color{33, 33, 33, 255};
	if (ColorIsEqual(play_button, click) || ColorIsEqual(edit_button, click) || ColorIsEqual(level_button, click) || ColorIsEqual(quit_button, click)) return;
	
	int mouse_x = GetMouseX();
	int mouse_y = GetMouseY();
	
	int x_offset = (screenWidth / 2) - 300; 

	play_button 	= ColorIsEqual(play_button, DARKGRAY) ? GRAY : play_button;
	edit_button 	= ColorIsEqual(edit_button, DARKGRAY) ? GRAY : edit_button;
	level_button 	= ColorIsEqual(level_button, DARKGRAY) ? GRAY : level_button;
	quit_button 	= ColorIsEqual(quit_button, DARKGRAY) ? GRAY : quit_button;
	
	if (mouse_x >= x_offset && mouse_x <= x_offset + 600) {
	
		//Play	
		int play_y = (screenHeight / 20) * 7;
		if (mouse_y >= play_y && mouse_y <= play_y + 100) {
			if (ColorIsEqual(play_button, GRAY)) play_button = DARKGRAY;
		}
	
		//Editor
		int edit_y = (screenHeight / 20) * 10;
		if (mouse_y >= edit_y && mouse_y <= edit_y + 100) {
			if (ColorIsEqual(edit_button, GRAY)) edit_button = DARKGRAY;
		}
	
		//Level Menu
		int menu_y = (screenHeight / 20) * 13;
		if (mouse_y >= menu_y && mouse_y <= menu_y + 100) {	
			if (ColorIsEqual(level_button, GRAY)) level_button = DARKGRAY;
		}
	
		//Quit
		int quit_y = (screenHeight / 20) * 16;
		if (mouse_y >= quit_y && mouse_y <= quit_y + 100) {	
			if (ColorIsEqual(quit_button, GRAY)) quit_button = DARKGRAY;
		}
	}
}

void click_button(Color& play_button, Color& edit_button, Color& level_button, Color& quit_button) {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		int mouse_x = GetMouseX();
		int mouse_y = GetMouseY();

		int x_offset = (screenWidth / 2) - 300;

		Color click_color = Color{33, 33, 33, 255};

		if (mouse_x >= x_offset && mouse_x <= x_offset + 600) {

			//Play	
			int play_y = (screenHeight / 20) * 7;
			if (mouse_y >= play_y && mouse_y <= play_y + 100) {
				play_button = click_color;
			}

			//Editor
			int edit_y = (screenHeight / 20) * 10;
			if (mouse_y >= edit_y && mouse_y <= edit_y + 100) {
				edit_button = click_color;
			}

			//Level Menu
			int menu_y = (screenHeight / 20) * 13;
			if (mouse_y >= menu_y && mouse_y <= menu_y + 100) {	
				level_button = click_color;
			}

			//Quit
			int quit_y = (screenHeight / 20) * 16;
			if (mouse_y >= quit_y && mouse_y <= quit_y + 100) {	
				quit_button = click_color;
			}
		}
	}
}

void release_button(RenderTexture2D& game_texture, Color& play_button, Color& edit_button, Color& level_button, Color& quit_button) {
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		int mouse_x = GetMouseX();
		int mouse_y = GetMouseY();

		int x_offset = (screenWidth / 2) - 300;

		play_button 	= GRAY;
		edit_button 	= GRAY;
		level_button 	= GRAY;
		quit_button 	= GRAY;

		if (mouse_x >= x_offset && mouse_x <= x_offset + 600) {

			//Play	
			int play_y = (screenHeight / 20) * 7;
			if (mouse_y >= play_y && mouse_y <= play_y + 100) {
				switch_to_play(game_texture);
			}

			//Editor
			int edit_y = (screenHeight / 20) * 10;
			if (mouse_y >= edit_y && mouse_y <= edit_y + 100) {
				switch_to_editor();
			}

			//Level Menu
			int menu_y = (screenHeight / 20) * 13;
			if (mouse_y >= menu_y && mouse_y <= menu_y + 100) {	
				switch_to_level_menu();
			}

			//Quit
			int quit_y = (screenHeight / 20) * 16;
			if (mouse_y >= quit_y && mouse_y <= quit_y + 100) {	
				CloseWindow();
			}
		}
	}
}