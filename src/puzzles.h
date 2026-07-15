#ifndef PUZZLES_H
#define PUZZLES_H

#include "data.h"

#include <vector>
#include <raylib.h>

//Data

enum PuzzleType {
	Default,
	Pink_Slow,
	Pink_Sticky,
	Both_Slow,
	Both_Sticky,
	Pink_Slow_Blue_Sticky,
	Blue_Slow_Pink_Sticky
};

typedef struct History History;
struct History {
	std::vector<u16> state;
	u16 blue_player;
	u16 pink_player;
};

typedef struct Puzzle Puzzle;
struct Puzzle {
	std::vector<u16> puzzle;
	PuzzleType type;
	u16 index;

	u16 blue_player;
	u16 pink_player;

	u8 width;
	u8 height;

	u8 screen_number;
	u8 teleport_index;

	bool pink_disabled;
	bool blue_disabled;
};

enum Entities {
	Boy 	= 0,
	Girl	= 1,
	Box		= 2
};

enum Tiles {
	Black_Tile 				= 0,
	Gray_Tile 				= 1,
	Horizontal_Wall			= 2,
	Horizontal_Wall_Shadow	= 3,
	Vertical_Wall			= 4,
	Fire_Black				= 5,
	Fire_Gray				= 6,
	Embers_Black			= 7,
	Embers_Gray				= 8,
	Mushroom_Black			= 9,
	Mushroom_Gray			= 10,
	Sprout_Black			= 11,
	Sprout_Gray				= 12,
	Blue_End_Black			= 13,
	Blue_End_Gray			= 14,
	Pink_End_Black			= 15,
	Pink_End_Gray			= 16,
	BluePink_End_Black		= 17,
	BluePink_End_Gray		= 18,
	Purple_End_Black		= 19,
	Purple_End_Gray			= 20,
	Blue_Tile_0				= 21,
	Blue_Tile_1				= 22,
	Blue_Tile_2				= 23,
	Blue_Tile_3				= 24,
	Pink_Tile_0				= 25,
	Pink_Tile_1				= 26,
	Pink_Tile_2				= 27,
	Pink_Tile_3				= 28,
	Teleporter_Entrance		= 29,
	Teleporter_Exit			= 30
};

extern bool pink_disabled;

extern std::vector<u16> mushrooms;
extern std::vector<History> history;

extern int puzzle_index;
extern Puzzle current_puzzle;
extern std::vector<Puzzle> puzzles;

//Methods
bool is_blue_disabled						();
											
bool is_pink_disabled						();
void toggle_pink_disabled					();
											
bool are_pink_and_blue_touching				();
											
bool is_blue_slow_puzzle					();
bool is_blue_sticky_puzzle					();
											
bool is_pink_slow_puzzle					();
bool is_pink_sticky_puzzle					();
											
int get_puzzle_index						();
u8 get_edit_puzzle_width					(std::vector<u16>& canvas);
void set_current_puzzle_to_edit_puzzle		(std::vector<u16>& canvas, int canvas_tile_width);
											
std::vector<Puzzle> get_puzzles				();
Puzzle get_const_puzzle						();
Puzzle get_current_puzzle					();
											
u8 get_current_puzzle_width					();
u8 get_current_puzzle_height				();	
											
u8 get_blue_player_index					();
u8 get_pink_player_index					();
void set_blue_player_index					(u8 index);
void set_pink_player_index					(u8 index);
											
void set_current_puzzle_and_index			(int index);
											
void create_new_puzzle_and_update_vals		();
											
bool is_edit_puzzle_same_as_saved_puzzle	();
bool is_edit_puzzle_valid					(std::vector<u16>& canvas, int canvas_tile_width);

void overwrite_puzzle_in_puzzles			(Puzzle& puzzle, int index);
void load_puzzles_from_file					();
void save_puzzles_to_file					();
void load_progress							();
void save_progress							();

bool try_increment_puzzle					();
bool try_decrement_puzzle					();

void undo_last_move							();
void restart_level							();

void is_on_fire								();
void try_teleport_entity					();
void try_grow_mushrooms						();

bool is_possible_move						(u8 entity, int move);
int try_move								(int input, int currentCellIndex);

bool complete_puzzle						();

#endif