#include "PlayMode.hpp"
#include "read_write_chunk.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <bitset>


#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

struct u8vec4_hash
{
    std::size_t operator()(const glm::u8vec4& v) const {
		// std::array<uint32_t,4> arr = {uint32_t(v[0]), uint32_t(v[1]), uint32_t(v[2]), uint32_t(v[3])};
        return std::hash<std::string>()(std::to_string(uint32_t(v[0]))+","+std::to_string(uint32_t(v[1]))+","+std::to_string(uint32_t(v[2]))+","+std::to_string(uint32_t(v[3])));
    }
};

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:
	
	if(!check_binary_file_existence()) {
		preprocess_sprite();
	}

	// load pallete
	load_pallete();

	// load player sprite
	player1.player_at = glm::vec2(256.0f-16.0f, 0.0f);
	load_sprite2x2(player1.player_sprites[0], "octopus1", uint8_t(player1.player_at.x), uint8_t(player1.player_at.y));
	load_sprite2x2(player1.player_sprites[1], "octopus1_move", uint8_t(player1.player_at.x), uint8_t(player1.player_at.y));
	
	player2.player_at = glm::vec2(0.0f);
	load_sprite2x2(player2.player_sprites[0], "octopus2", uint8_t(player2.player_at.x), uint8_t(player2.player_at.y));
	load_sprite2x2(player2.player_sprites[1], "octopus2_move", uint8_t(player2.player_at.x), uint8_t(player2.player_at.y));
	
	// load crown sprite
	load_sprite2x2(crown, "crown", 0, 240);

	// load splash tiles
	load_tile2x2(player1.splash_tile, "splash");
	load_tile2x2(player2.splash_tile, "splash2");

	// load potion sprite
	load_sprite2x2(potion.potion_sprite, "potion", 0, 240);

	// set initial background
	ppu.background_color = glm::u8vec4(230, 196, 166, 0xff);

	for (uint32_t y = 0; y < 8; ++y) {
		for (uint32_t x = 0; x < 8; ++x) {
			ppu.tile_table[background_tile_idx].bit0 = {1,1,1,1,1,1,1,1};
			ppu.tile_table[background_tile_idx].bit1 = {0,0,0,0,0,0,0,0};
		}
	}

	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};
	
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x+PPU466::BackgroundWidth*y] = 0;
		}
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_TAB) {
			tab.downs += 1;
			tab.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			keya.downs += 1;
			keya.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			keyw.downs += 1;
			keyw.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			keys.downs += 1;
			keys.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			keyd.downs += 1;
			keyd.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_TAB) {
			tab.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			keya.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			keyw.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			keys.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			keyd.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	// generate potion with a probability
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	if(dis(gen) < 0.001 && !potion.is_active) {
		gen_potion();
	}

	// Player 1
	if (left.pressed) player1.player_at.x -= player1.player_speed * elapsed;
	if (right.pressed) player1.player_at.x += player1.player_speed * elapsed;
	if (down.pressed) player1.player_at.y -= player1.player_speed * elapsed;
	if (up.pressed) player1.player_at.y += player1.player_speed * elapsed;
	if(left.pressed || right.pressed || down.pressed || up.pressed) {
		player1.time_elapsed += elapsed;
		if(player1.time_elapsed > animation_speed) {
			player1.time_elapsed -= animation_speed;
			std::swap(player1.player_sprite_idx_active, player1.player_sprite_idx_inactive);
		}
	}
	player1.player_at.x = std::clamp(player1.player_at.x, 0.0f, float(PPU466::ScreenWidth-16));
	player1.player_at.y = std::clamp(player1.player_at.y, 0.0f, float(PPU466::ScreenHeight-16));
	if(space.pressed) {
		player1.is_splash = true;
	}

	// Player 2
	if(keya.pressed) player2.player_at.x -= player2.player_speed * elapsed;
	if(keyd.pressed) player2.player_at.x += player2.player_speed * elapsed;
	if(keys.pressed) player2.player_at.y -= player2.player_speed * elapsed;
	if(keyw.pressed) player2.player_at.y += player2.player_speed * elapsed;
	if(keya.pressed || keyd.pressed || keys.pressed || keyw.pressed) {
		player2.time_elapsed += elapsed;
		if(player2.time_elapsed > animation_speed) {
			player2.time_elapsed -= animation_speed;
			std::swap(player2.player_sprite_idx_active, player2.player_sprite_idx_inactive);
		}
	}
	player2.player_at.x = std::clamp(player2.player_at.x, 0.0f, float(PPU466::ScreenWidth-16));
	player2.player_at.y = std::clamp(player2.player_at.y, 0.0f, float(PPU466::ScreenHeight-16));
	if(tab.pressed) {
		player2.is_splash = true;
	}

	// check potion collision
	if(potion.is_active) {
		check_potion_collision();
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;
	keya.downs = 0;
	keyw.downs = 0;
	keys.downs = 0;
	keyd.downs = 0;
	tab.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	
	if(player1.is_splash) {
		paint_splash(player1);
	}
	if(player2.is_splash) {
		paint_splash(player2);
	}

	//player sprite:
	paint_sprite(player1.player_sprites[player1.player_sprite_idx_active], player1.player_at.x, player1.player_at.y);
	// hide inactive sprite
	paint_sprite(player1.player_sprites[player1.player_sprite_idx_inactive], 0, 240);

	paint_sprite(player2.player_sprites[player2.player_sprite_idx_active], player2.player_at.x, player2.player_at.y);
	// hide inactive sprite
	paint_sprite(player2.player_sprites[player2.player_sprite_idx_inactive], 0, 240);

	//--- actually draw ---
	ppu.draw(drawable_size);
}

void PlayMode::load_sprite2x2(Sprite2x2 &sprite, std::string filename, uint8_t pos_x, uint8_t pos_y) {
	std::vector<uint8_t> data_as_index; 
	std::ifstream from(data_path(SPRITE_OUTPUT_FILEPATH+filename+".bin").c_str(), std::ios::binary);
	read_chunk(from, SPRITE_MAGIC, &data_as_index);
	uint32_t pallette_idx = uint32_t(data_as_index[0]);
	data_as_index.erase(data_as_index.begin());

	// create sprite2x2 and record the sprite indices
	for(uint32_t i=0; i<4; i++) {
		sprite.indices[i] = sprite_idx+i;
	}

	// load to tile table
	for (int tile_y = 0; tile_y < 2; ++tile_y) {  // bottom and top tiles
        for (int tile_x = 0; tile_x < 2; ++tile_x) {  // left and right tiles
            // loop through each pixel in the 8x8 tile
            for (int y = 0; y < 8; ++y) {
				ppu.tile_table[tile_idx].bit0[y] = uint8_t(0);
				ppu.tile_table[tile_idx].bit1[y] = uint8_t(0);
                for (int x = 0; x < 8; ++x) {
                    // calculate the source pixel position in the 16x16 image
                    int src_x = tile_x * 8 + x;
                    int src_y = tile_y * 8 + y;
                    int src_index = src_y * 16 + src_x;
					uint8_t color_index = data_as_index[src_index];
					uint8_t bit0 = color_index & 1;
					uint8_t bit1 = (color_index >> 1) & 1;

                    // Copy the pixel to the corresponding tile
                    ppu.tile_table[tile_idx].bit0[y] |= ((bit0) << x);
					ppu.tile_table[tile_idx].bit1[y] |= ((bit1) << x);
                }
            }
			
			ppu.sprites[sprite_idx].index = tile_idx;
			ppu.sprites[sprite_idx].attributes = pallette_idx;
			ppu.sprites[sprite_idx].x = uint8_t(pos_x + tile_x * 8);
			ppu.sprites[sprite_idx].y = uint8_t(pos_y + tile_y * 8);
			
			sprite_idx++;
			tile_idx++;
        }
    }

	std::cout<<"Successfully loaded sprite2x2: "<<filename<<std::endl;
}

void PlayMode::load_tile2x2(Tile2x2 &tile, std::string filename) {
	std::vector<uint8_t> data_as_index; 
	std::ifstream from(data_path(SPRITE_OUTPUT_FILEPATH+filename+".bin").c_str(), std::ios::binary);
	read_chunk(from, SPRITE_MAGIC, &data_as_index);
	uint32_t pallette_idx = uint32_t(data_as_index[0]);
	data_as_index.erase(data_as_index.begin());

	// create tile2x2 and record the tile indices
	for(uint32_t i=0; i<4; i++) {
		tile.indices[i] = tile_idx+i;
	}
	tile.pallete_idx = pallette_idx;

	// load to tile table
	for (int tile_y = 0; tile_y < 2; ++tile_y) {  // bottom and top tiles
        for (int tile_x = 0; tile_x < 2; ++tile_x) {  // left and right tiles
            // loop through each pixel in the 8x8 tile
            for (int y = 0; y < 8; ++y) {
				ppu.tile_table[tile_idx].bit0[y] = uint8_t(0);
				ppu.tile_table[tile_idx].bit1[y] = uint8_t(0);
                for (int x = 0; x < 8; ++x) {
                    // calculate the source pixel position in the 16x16 image
                    int src_x = tile_x * 8 + x;
                    int src_y = tile_y * 8 + y;
                    int src_index = src_y * 16 + src_x;
					uint8_t color_index = data_as_index[src_index];
					uint8_t bit0 = color_index & 1;
					uint8_t bit1 = (color_index >> 1) & 1;

                    // Copy the pixel to the corresponding tile
                    ppu.tile_table[tile_idx].bit0[y] |= ((bit0) << x);
					ppu.tile_table[tile_idx].bit1[y] |= ((bit1) << x);
                }
            }
			
			tile_idx++;
        }
    }

	std::cout<<"Successfully loaded tile2x2: "<<filename<<std::endl;
}

void PlayMode::paint_sprite(Sprite2x2& sprite, uint8_t x, uint8_t y) {
	for (int tile_y = 0; tile_y < 2; ++tile_y) {  // bottom and top tiles
		for (int tile_x = 0; tile_x < 2; ++tile_x) {  // left and right tiles
			ppu.sprites[sprite.indices[tile_x+2*tile_y]].x = uint8_t(x + tile_x * 8);
			ppu.sprites[sprite.indices[tile_x+2*tile_y]].y = uint8_t(y + tile_y * 8);
		}
	}
}

void PlayMode::paint_splash(PlayerState& player) {
	// get a random tile
	Tile2x2& splash_tile = player.splash_tile;
	// get tile index from position
	uint32_t x = (uint32_t(player.player_at.x) % PPU466::ScreenWidth) / 8;
	uint32_t y = (uint32_t(player.player_at.y) % PPU466::ScreenHeight) / 8;
	if(x % 2 == 1) x--;
	if(y % 2 == 1) y--;
	for (int tile_y = 0; tile_y < 2; ++tile_y) {  // bottom and top tiles
		for (int tile_x = 0; tile_x < 2; ++tile_x) {  // left and right tiles
			ppu.background[(x+tile_x)+PPU466::BackgroundWidth*(y+tile_y)] = ((splash_tile.pallete_idx << 8) | splash_tile.indices[tile_x+2*tile_y]); 
			
		}
	}

	player.is_splash = false;
}



std::string PlayMode::end(){
	// count the number of tiles painted by each player
	auto count_painted = [&](Tile2x2& splash_tile) {
		uint32_t count = 0;
		for(uint16_t& background_tile: ppu.background) {
			uint32_t tile_idx = background_tile & 255;
			if(tile_idx == splash_tile.indices[0] || tile_idx == splash_tile.indices[1] || tile_idx == splash_tile.indices[2] || tile_idx == splash_tile.indices[3]) {
					count++;
				}
		}
		return count;
	};

	uint32_t player1_count = count_painted(player1.splash_tile);
	uint32_t player2_count = count_painted(player2.splash_tile);
	if(player1_count > player2_count) {
		std::cout<<"Player 1 wins!"<<std::endl;
		paint_sprite(crown, player1.player_at.x, player1.player_at.y+16);
		return "Player 1 wins!";
	} else if(player1_count < player2_count) {
		std::cout<<"Player 2 wins!"<<std::endl;
		paint_sprite(crown, player2.player_at.x, player2.player_at.y+16);
		return "Player 2 wins!";
	} else {
		std::cout<<"It's a tie!"<<std::endl;
		return "It's a tie!";
	}
}

void PlayMode::gen_potion() {
	// Spawn a potion at a random location
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis_x(0, PPU466::ScreenWidth-16);
	std::uniform_int_distribution<> dis_y(0, PPU466::ScreenHeight-16);
	potion.position = glm::vec2(dis_x(gen), dis_y(gen));
	potion.is_active = true;
	paint_sprite(potion.potion_sprite, uint8_t(potion.position.x), uint8_t(potion.position.y));
}

void PlayMode::check_potion_collision() {
	// Check if player 1 collides with the potion
	if(player1.player_at.x < potion.position.x + 16 && player1.player_at.x + 16 > potion.position.x && player1.player_at.y < potion.position.y + 16 && player1.player_at.y + 16 > potion.position.y) {
		player1.get_potion();
		std::cout<<"Player 1 got a potion!"<<std::endl;
		potion.is_active = false;
	}

	// Check if player 2 collides with the potion
	if(player2.player_at.x < potion.position.x + 16 && player2.player_at.x + 16 > potion.position.x && player2.player_at.y < potion.position.y + 16 && player2.player_at.y + 16 > potion.position.y) {
		player2.get_potion();
		potion.is_active = false;
	}

	if(!potion.is_active) {
		paint_sprite(potion.potion_sprite, 0, 240);
	}
}

void PlayMode::load_pallete(){
	std::ifstream from(data_path(SPRITE_OUTPUT_FILEPATH+"pallete.bin").c_str(), std::ios::binary);
	std::vector<uint8_t> palletes;
	read_chunk(from, PALLETE_MAGIC, &palletes);
	for(uint32_t i=0; i<palette_table.size(); i++) {
		for(uint32_t j=0; j<4; j++) {
			for(uint32_t k=0; k<4; k++) {
				ppu.palette_table[i][j][k] = palletes[i*16+j*4+k];
			}
		}
	}
}