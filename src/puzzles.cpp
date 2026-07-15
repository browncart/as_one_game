#include "puzzles.h"
#include "data.h"

#include <raylib.h>
#include <fstream>
#include <iostream>
#include <vector>

//Data

//must be reset properly on puzzle restart...
//must be undone proplery on undo...
bool pink_disabled = false;

u16 teleport_index = 0;
std::vector<u16> mushrooms;
std::vector<History> history;

int puzzle_index = 0;
Puzzle current_puzzle;
std::vector<Puzzle> puzzles;

///////////////
//  Helpers  //
///////////////

bool is_blue_disabled() {
	return current_puzzle.blue_disabled;
}

bool is_pink_disabled() {
	return current_puzzle.pink_disabled;
}

void toggle_pink_disabled() {
	current_puzzle.pink_disabled = !current_puzzle.pink_disabled;
}

bool are_pink_and_blue_touching() {
	if (std::abs(current_puzzle.blue_player - current_puzzle.pink_player) == 1 || std::abs(current_puzzle.blue_player - current_puzzle.pink_player) == get_current_puzzle_width()) {
		return true;
	}

	return false;
}

bool is_blue_slow_puzzle() {
	return current_puzzle.type == PuzzleType::Blue_Slow_Pink_Sticky || current_puzzle.type == PuzzleType::Both_Slow;
}

bool is_blue_sticky_puzzle() {
	return current_puzzle.type == PuzzleType::Pink_Slow_Blue_Sticky || current_puzzle.type == PuzzleType::Both_Sticky;
}

bool is_pink_slow_puzzle() {
	return current_puzzle.type == PuzzleType::Pink_Slow || current_puzzle.type == PuzzleType::Pink_Slow_Blue_Sticky || current_puzzle.type == PuzzleType::Both_Slow;
}

bool is_pink_sticky_puzzle() {
	return current_puzzle.type == PuzzleType::Pink_Sticky || current_puzzle.type == PuzzleType::Blue_Slow_Pink_Sticky || current_puzzle.type == PuzzleType::Both_Sticky;
}

std::vector<Puzzle> get_puzzles() {
	return puzzles;
}

int get_puzzle_index() {
	return puzzle_index;
}

Puzzle get_const_puzzle() {
	return puzzles[puzzle_index];
}

Puzzle get_current_puzzle() {
	return current_puzzle;
}

u8 get_current_puzzle_width() {
	return current_puzzle.width;
}

u8 get_current_puzzle_height() {
	return current_puzzle.height;
}

u8 get_blue_player_index() {
	return current_puzzle.blue_player;
}

u8 get_pink_player_index() {
	return current_puzzle.pink_player;
}

void set_blue_player_index(u8 index) {
	current_puzzle.blue_player = index;
}

void set_pink_player_index(u8 index) {
	current_puzzle.pink_player = index;
}

void set_current_puzzle_and_index(int index) {
	if (0 <= index && index < puzzles.size()) {
		puzzle_index = index;
		current_puzzle = puzzles[puzzle_index];
		history.clear();
		pink_disabled = false;
	}
}

bool is_edit_puzzle_same_as_saved_puzzle() {
	Puzzle edit_puzzle = puzzles[puzzle_index];

	return current_puzzle.puzzle == edit_puzzle.puzzle && current_puzzle.type == edit_puzzle.type;
}

void create_new_puzzle_and_update_vals() {
	puzzles.push_back(current_puzzle);
	puzzle_index = puzzles.size() - 1;
	save_puzzles_to_file();
}

void overwrite_puzzle_in_puzzles(Puzzle& puzzle, int index) {
	puzzles[index] = puzzle;
}

bool try_increment_puzzle() {
	if (puzzle_index + 1 < puzzles.size()) {
		pink_disabled = false;
		puzzle_index++;
		current_puzzle = puzzles[puzzle_index];
		history.clear();
		return true;
	} else {
		std::cerr << "INCREMENTED INDEX OUT OF RANGE!" << std::endl;
		return false;
	}
}

bool try_decrement_puzzle() {
	if (puzzle_index - 1 >= 0) {
		pink_disabled = false;
		puzzle_index--;
		current_puzzle = puzzles[puzzle_index];
		history.clear();
		return true;
	} else {
		std::cerr << "DECREMENTED INDEX OUT OF RANGE!" << std::endl;
		return false;
	}
}

/////////////////
//  Edit Menu  //
/////////////////

bool is_edit_puzzle_valid(std::vector<u16>& canvas, int canvas_tile_width) {
	u8 expected_width	= 0;
	u8 row_width		= 0;
	u8 potential_gap 	= 0;

	bool blue_player	= false;
	bool pink_player	= false;
	u8 blue_end			= 0;
	u8 pink_end			= 0;
	u8 bluepink_end		= 0;

	u8 boxes			= 0;
	u8 box_ends			= 0;

	for (int i = 0; i < canvas_tile_width; ++i) {
		row_width		= 0;
		potential_gap 	= 0;

		for (int j = 0; j < canvas_tile_width; ++j) {
			u8 index 	= (i * canvas_tile_width) + j;
			u16 value	= canvas[index];

			u8 high 	= value >> 8;
			u8 low 		= value;

			if (high == 0x0) blue_player = true;
			if (high == 0x1) pink_player = true;

			if (high == 0x2) boxes++;
			if (low == 0x13 || low == 0x14) box_ends++;

			if (low == 0xd || low == 0xe) blue_end++;
			if (low == 0xf || low == 0x10) pink_end++;
			if (low == 0x11 || low == 0x12) bluepink_end++;

			if (canvas[index] != 0xffff && potential_gap > 0) return false;
			if (canvas[index] != 0xffff) row_width++;
			if (row_width > 0 && low == 0xff) potential_gap++;
		}

		if (expected_width == 0) expected_width = row_width;

		if (row_width > 0 && row_width != expected_width) return false;
	}

	if (!blue_player || !pink_player) return false;
	if (boxes < box_ends) return false;
	if (blue_end + pink_end + bluepink_end < 2) return false;

	return true;
}

u8 get_edit_puzzle_width(std::vector<u16>& canvas) {
	int width = 0;
	
	for (auto cell : canvas) {
		if (width > 0 && cell == 0xffff) return width;
		if (cell != 0xffff) width++;
	}

	return width;
}

void set_current_puzzle_to_edit_puzzle(std::vector<u16>& canvas, int canvas_tile_width) {
	if (!is_edit_puzzle_valid(canvas, canvas_tile_width)) return;

	u8 blue_index = 0;
	u8 pink_index = 0;
	u8 width = get_edit_puzzle_width(canvas);

	std::vector<u16> puzzle_cutout;
	
	for (int i = 0; i < canvas.size(); ++i) {
		if (canvas[i] >> 8 == 0x0) blue_index = puzzle_cutout.size();
		if (canvas[i] >> 8 == 0x1) pink_index = puzzle_cutout.size();

		if (canvas[i] != 0xffff) puzzle_cutout.push_back(canvas[i]);
	}
	
	u8 height = puzzle_cutout.size() / width;	
	u16 dimensions = width << 8 | height;
	u16 player_index = pink_index << 8 | blue_index;

	std::cout << "pink index: " << +(player_index >> 8) << std::endl;
	std::cout << "blue index: " << +((u8)player_index) << std::endl;

	current_puzzle.puzzle 		= puzzle_cutout;
	current_puzzle.blue_player	= blue_index;
	current_puzzle.pink_player 	= pink_index;
	current_puzzle.width 		= width;
	current_puzzle.height 		= height;

	pink_disabled = false;
}

////////////////////
//  File  Methods //
////////////////////

void load_puzzles_from_file() {
	std::cout << "trying to load puzzles" << std::endl;

	std::fstream file;
	file.open("data/puzzles/saved_puzzles", std::ios::in | std::ios::binary);
	std::vector<u16> puzzle;

	if (!file) {
		std::cerr << "Failed to open file!" << std::endl;
		return;
	}

	file.seekg(0, file.end);
	int length = file.tellg();
	file.seekg(0, file.beg);

	if (length & 1) {
		std::cerr << "File length is not even!" << std::endl;
		return;
	}

	u8 buffer[length];

	file.read(reinterpret_cast<char*>(buffer), length);
	file.close();

	u8 pink_player_index;
	u8 blue_player_index;

	u8 width;
	u8 height;

	u16 index;

	u8 puzzle_type = 0;
	u8 screen_number = 0;
	u8 teleport_index = 0;

	for (int i = 0; i < length; i+=2) {
		u16 left = buffer[i] << 8;
		u16 right = buffer[i + 1];
		u16 tile = left | right;

		if (tile == 0xffff) {
			std::cout << "trying to push" << std::endl;

			if (!puzzle.empty()) {

				size_t puzzle_size = puzzle.size();

				u8 puzzle_type = puzzle[puzzle_size - 3];
				
				width 	= puzzle[puzzle_size - 2] >> 8;
				height 	= puzzle[puzzle_size - 2];

				pink_player_index = puzzle[puzzle_size - 1] >> 8;
				blue_player_index = puzzle[puzzle_size - 1];

				puzzle.pop_back();
				puzzle.pop_back();
				puzzle.pop_back();

				struct Puzzle loaded = {
					.puzzle = puzzle,
					.type = static_cast<PuzzleType>(puzzle_type),
					.index = (u16)puzzles.size(),

					.blue_player = blue_player_index,
					.pink_player = pink_player_index,

					.width = width,
					.height = height,

					.screen_number = screen_number,
					.teleport_index = teleport_index,

					.pink_disabled = false,
					.blue_disabled = true
				};

				puzzles.push_back(loaded);
				puzzle.clear();
			}
		} else {
			puzzle.push_back(tile);
		}
	}

	current_puzzle = puzzles[puzzle_index];
}

void save_puzzles_to_file() {
	std::fstream file;
    file.open("data/puzzles/saved_puzzles", std::ios::out | std::ios::binary);
 
	if (!file) {
		std::cerr << "ERROR: FAILED TO OPEN FILE" << std::endl;
		return;
	}

	for (int i = 0; i < puzzles.size(); ++i) {
		Puzzle puzzle = puzzles[i];
		
		//eight cuz 2 times 4
		int length = (puzzle.puzzle.size() * 2) + 8;
		u8 buffer[length];
	
		int bufferIndex = 0;
		for (int i = 0; i < puzzle.puzzle.size(); ++i) {
			bufferIndex = i * 2;
			buffer[bufferIndex] 	= (u8)(puzzle.puzzle[i] >> 8);
			buffer[bufferIndex + 1] = (u8)(puzzle.puzzle[i]); 
		}

		//Puzzle Type
		buffer[length - 8] = 0x0;
		buffer[length - 7] = puzzle.type;

		//Puzzle Width & Height
		buffer[length - 6] = puzzle.width;
		buffer[length - 5] = puzzle.height;

		//Player indexes
		buffer[length - 4] = (u8)puzzle.pink_player;
		buffer[length - 3] = (u8)puzzle.blue_player;

		//Delimiter
		buffer[length - 2] = 0xff;
		buffer[length - 1] = 0xff;
	
		file.write(reinterpret_cast<char*>(buffer), length);
	}		
	
	file.close();
}

void load_progress() {
	std::fstream file;
	file.open("data/saves/saved_progress", std::ios::in | std::ios::binary);

	if (!file) {
		std::cerr << "ERROR: FAILED TO OPEN FILE" << std::endl;
		return;
	}

	file.seekg(0, file.end);
	int length = file.tellg();
	file.seekg(0, file.beg);

	u8 buffer[length];

	file.read(reinterpret_cast<char*>(buffer), length);
	file.close();

	puzzle_index = buffer[0];
	current_puzzle = puzzles[puzzle_index];
}

void save_progress() {
	std::fstream file;
	file.open("data/saves/saved_progress", std::ios::out | std::ios::binary);

	if (!file) {
		std::cerr << "ERROR: FAILED TO OPEN FILE" << std::endl;
		return;
	}

	//possible we will want to save more info at some point...
	u8 buffer[1] = {(u8)puzzle_index};

	file.write(reinterpret_cast<char*>(buffer), 1);
	file.close();

	std::cout << "saved progress!" << std::endl;
}

////////////
//  Play  //
////////////

void undo_last_move() {
	if (!history.empty()) {
		History prev = history.back();
		history.pop_back();
	
		current_puzzle.puzzle 		= prev.state;
		current_puzzle.blue_player 	= prev.blue_player;
		current_puzzle.pink_player 	= prev.pink_player;

		//If Pink_Slow, toggle pink_disabled...
		if (current_puzzle.type == PuzzleType::Pink_Slow || current_puzzle.type == PuzzleType::Both_Slow || current_puzzle.type == PuzzleType::Pink_Slow_Blue_Sticky) {
			current_puzzle.pink_disabled = !current_puzzle.pink_disabled;
		}

		if (current_puzzle.type == PuzzleType::Both_Slow || current_puzzle.type == PuzzleType::Blue_Slow_Pink_Sticky) {
			current_puzzle.blue_disabled = !current_puzzle.blue_disabled;
		}
	}
}

void restart_level() {
	if (!history.empty()) {
		std::cout << "restarting without edit puzzle" << std::endl;

		History history_event = history[0];

		current_puzzle.puzzle = history_event.state;
		current_puzzle.blue_player = history_event.blue_player;
		current_puzzle.pink_player = history_event.pink_player;
		current_puzzle.pink_disabled = false;
		current_puzzle.blue_disabled = true;
	}

	history.clear();
}

bool is_possible_move(u8 entity, int newPos) {
	Puzzle puzz = current_puzzle;

	//Although this will allow wrapping movement when borders aren't sealed by walls...
	if (newPos >= 0 && newPos < puzz.puzzle.size()) {
		u16 cell 		= puzz.puzzle[newPos];
		u8 new_entity 	= cell >> 8;
		u8 tile 		= cell;
		
		//Blue trying pink tiles
		if (entity == Boy && tile > 24 && tile < 29) {
			return false;
		}

		//Pink trying blue tiles
		if (entity == Girl && tile > 20 && tile < 25) {
			return false;
		}

		//If pink is moving and disabled or pink in next square is disabled
		if (((new_entity == Girl && puzz.pink_disabled) || (entity == Girl && puzz.pink_disabled)) && (puzz.type == PuzzleType::Pink_Slow || puzz.type == PuzzleType::Both_Slow || puzz.type == PuzzleType::Pink_Slow_Blue_Sticky)) {
			return false;
		}

		if (((new_entity == Boy && puzz.blue_disabled) || (entity == Boy && puzz.blue_disabled)) && (puzz.type == PuzzleType::Both_Slow || puzz.type == PuzzleType::Blue_Slow_Pink_Sticky)) {
			return false;
		}

		//When pink is sticky...
		if (new_entity == Girl && entity == Box && puzz.type == PuzzleType::Pink_Sticky) {
			return false;
		}

		//When two boxes collide...
		if (new_entity == Box && entity == Box) {
			return false;
		}

		//When boxes contact fire...
		if (entity == Box && (tile == Fire_Black || tile == Fire_Gray)) {
			return false;
		}

		//If new tile is not a fixed obstacle...
		if (tile != Horizontal_Wall && tile != Horizontal_Wall_Shadow && tile != Vertical_Wall && tile != Mushroom_Black && tile != Mushroom_Gray) {
			return true;
		}
	}
	return false;
}

void is_on_fire() {
	u8 blue_index = get_blue_player_index();
	u8 pink_index = get_pink_player_index();

	u8 blue_tile = current_puzzle.puzzle[blue_index];
	u8 pink_tile = current_puzzle.puzzle[pink_index];

	if (blue_tile == Fire_Black || blue_tile == Fire_Gray || pink_tile == Fire_Black || pink_tile == Fire_Gray) {
		game_over = true;
	}
}

void try_teleport_entity() {
	if (teleport_index == 0) return;
	
	bool teleported = false;
	u16 entity_and_tile = current_puzzle.puzzle[teleport_index];
	u8 entity 			= entity_and_tile >> 8;
	u8 tile				= entity_and_tile;

	for (int i = 0; i < current_puzzle.puzzle.size(); ++i) {
		u16 cell = current_puzzle.puzzle[i];

		if (cell == (0xff00 | Teleporter_Exit)) {
			current_puzzle.puzzle[teleport_index] = 0xff00 | Teleporter_Entrance;
			current_puzzle.puzzle[i] = entity << 8 | Teleporter_Exit;

			if (entity == Boy) set_blue_player_index(i);
			if (entity == Girl) set_pink_player_index(i);

			teleported = true;
		}
	}

	if (!teleported) return;
	else teleport_index = 0;
}

void try_grow_mushrooms() {
	if (mushrooms.empty()) return;

	for (auto index : mushrooms) {
		u16 entity_and_tile = current_puzzle.puzzle[index];
		u8 entity 			= entity_and_tile >> 8;
		u8 tile 			= entity_and_tile;

		if (entity == 0xff) {
			if (tile == Sprout_Black) {
				current_puzzle.puzzle[index] = 0xff00 | Mushroom_Black;
			}
			if (tile == Sprout_Gray) {
				current_puzzle.puzzle[index] = 0xff00 | Mushroom_Gray;
			}
			if (tile == Embers_Black) {
				current_puzzle.puzzle[index] = 0xff00 | Fire_Black;
			}
			if (tile == Embers_Gray) {
				current_puzzle.puzzle[index] = 0xff00 | Fire_Gray;
			}
		}
	}

	mushrooms.clear();
}

int try_move(int input, int currentCellIndex) {
	u8 width = get_current_puzzle_width();
	u8 height = get_current_puzzle_height();
	u8 index = get_puzzle_index();

	int newPos = currentCellIndex;
	if (input == KEY_W || input == KEY_UP) 		newPos += -width;
	if (input == KEY_A || input == KEY_LEFT) 	newPos += -1;
	if (input == KEY_S || input == KEY_DOWN) 	newPos += width;
	if (input == KEY_D || input == KEY_RIGHT) 	newPos += 1;

	u8 entityAtCurrPos 	= current_puzzle.puzzle[currentCellIndex] >> 8;

	if (is_possible_move(entityAtCurrPos, newPos)) {
		u8 tileAtCurrPos	= current_puzzle.puzzle[currentCellIndex];

		u8 entityAtNewPos 	= current_puzzle.puzzle[newPos] >> 8;
		u8 tileAtNewPos 	= current_puzzle.puzzle[newPos]; 

		if (entityAtNewPos == Boy || entityAtNewPos == Girl || entityAtNewPos == Box) {
			try_move(input, newPos);
		}

		//check that all movable entities have moved
		entityAtNewPos = current_puzzle.puzzle[newPos] >> 8;

		if (entityAtNewPos == 0xff) {

			current_puzzle.puzzle[newPos]				= entityAtCurrPos << 8 | tileAtNewPos;
			current_puzzle.puzzle[currentCellIndex] 	= 0xff00 | tileAtCurrPos;

			if (tileAtCurrPos == Sprout_Black || tileAtCurrPos == Sprout_Gray || tileAtCurrPos == Embers_Black || tileAtCurrPos == Embers_Gray) mushrooms.push_back(currentCellIndex);

			if (entityAtCurrPos == 0x0) {
				set_blue_player_index(newPos);
			} else if (entityAtCurrPos == 0x1) {
				set_pink_player_index(newPos);
			}

			//Where teleport is currently handled...
			if (tileAtNewPos == Teleporter_Entrance) teleport_index = newPos;
			return newPos;
		}
	}

	return currentCellIndex;
}

bool complete_puzzle() {
	int unmet_blue 		= 0;
	int unmet_pink 		= 0;
	int unmet_bluepink 	= 0;
	int unmet_purple 	= 0;

	int locked_doors = 0;

	for (int i = 0; i < current_puzzle.puzzle.size() - 3; ++i) {
		u16 cell = current_puzzle.puzzle[i];
		u8 high = cell >> 8;
		u8 low	= cell;

		if ((low == BluePink_End_Black || low == BluePink_End_Gray) && (high != Boy || high != Girl)) {
			return false;
		}

		if ((low == Pink_End_Black || low == Pink_End_Gray) && high != Girl) {
			return false;
		}
		
		if ((low == Blue_End_Black || low == Blue_End_Gray) && high != Boy) {
			return false;
		}

		if ((low == Purple_End_Black || low == Purple_End_Gray) && high != Box) {
			return false;
		}
	}

	return true;
}