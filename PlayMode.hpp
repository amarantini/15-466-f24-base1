#include "PPU466.hpp"
#include "Mode.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include "preprocess.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

// A large sprite that consists of 2x2 standard sprites
struct Sprite2x2 {
    std::array<uint32_t, 4> indices; // index into sprite table
	uint32_t pallete_idx;
};

// A large tile that consists of 2x2 standard tiles
struct Tile2x2 {
    std::array<uint32_t, 4> indices; // index into tile table
	uint32_t pallete_idx;
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	virtual std::string end() override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space, keya, keyw, keys, keyd, tab;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	//----- drawing handled by PPU466 -----

	PPU466 ppu;

	//----- loading assets -----
	float animation_speed = 0.1f;
	struct PlayerState {
		glm::vec2 player_at = glm::vec2(0.0f);
		std::array<Sprite2x2,2> player_sprites;
		Tile2x2 splash_tile;
		bool is_splash = false;
		float player_speed = 50.0f;
		uint32_t player_sprite_idx_active = 0;
		uint32_t player_sprite_idx_inactive = 1;
		float time_elapsed = 0.0f;

		void get_potion() {
			player_speed += 10.0f;
		}
	} player1, player2;

	Sprite2x2 crown;

	struct Potion {
		Sprite2x2 potion_sprite;
		glm::vec2 position;
		bool is_active = false;
	} potion;
	
	uint32_t tile_idx = 16; // next tile index available
	uint32_t sprite_idx = 0;
	uint32_t background_tile_idx = 0;
	uint32_t background_pallete_idx = 0;
	void load_pallete();
	void load_sprite2x2(Sprite2x2 &sprite, std::string filename, uint8_t pos_x, uint8_t pos_y);
	void load_tile2x2(Tile2x2 &tile, std::string filename);

	//----- helper function -----
	void paint_sprite(Sprite2x2& sprite, uint8_t x, uint8_t y);
	void paint_splash(PlayerState& player);
	void gen_potion();
	void check_potion_collision();
};