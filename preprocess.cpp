#include "preprocess.hpp"

void process_img(std::string filename, uint32_t existing_pallete_idx, glm::uvec2 size, enum OriginLocation origin) {
	std::vector< glm::u8vec4 > data;
	load_png(data_path(SPRITE_FILEPATH+filename+".png"), &size, &data, origin);
	assert(data.size() == size.x * size.y);

	// find all colors in the sprite
    std::unordered_map<glm::u8vec4, uint8_t> colors;
	std::vector<uint8_t> data_as_index; // convert rgba to index in pallete
	data_as_index.reserve(data.size()+1);
	uint32_t curr_pallete_idx;
	if(existing_pallete_idx == 0) {
		curr_pallete_idx = pallette_idx++;
	} else {
		curr_pallete_idx = existing_pallete_idx;
        for(uint32_t i=0; i<4; i++) {
            colors[palette_table[existing_pallete_idx][i]] = uint8_t(i);
        }
	}

    sprite_to_pallete_idx[filename] = curr_pallete_idx;
	auto& pallete = palette_table[curr_pallete_idx];
    data_as_index.push_back(uint8_t(curr_pallete_idx)); // first number is the pallete index

	for(auto& pixel : data) {
		if(pixel[3]==0){
			// transparent pixel
			data_as_index.push_back(uint8_t(0));
		} else {
			if(colors.count(pixel) == 0) {
                colors[pixel] = uint8_t(colors.size()+1);
			}
			data_as_index.push_back(colors[pixel]);
		}
	}

    if(existing_pallete_idx == 0) {
        assert(colors.size() <= 3);
        for(auto itr=colors.begin(); itr!=colors.end(); itr++) {
            pallete[itr->second] = itr->first;
        }
    }

    // store data as binary
    std::ofstream to(data_path(SPRITE_FILEPATH+filename+".bin").c_str(), std::ios::binary);
    write_chunk(SPRITE_MAGIC, data_as_index, &to);
	
	std::cout<<"Successfully loaded and processed "<<filename<<std::endl;
}

void store_pallete(){
    std::ofstream to(data_path(SPRITE_FILEPATH+"pallete.bin").c_str(), std::ios::binary);
    std::vector<uint8_t> palletes;
    for(uint32_t i=0; i<palette_table.size(); i++) {
        for(uint32_t j=0; j<4; j++) {
            for(uint32_t k=0; k<4; k++) {
                
                palletes.push_back(palette_table[i][j][k]);
            }
            
        }
        
    }
    write_chunk(PALLETE_MAGIC, palletes, &to);
}

void preprocess_sprite() {
    // Initialize the pallete table
    for(uint32_t i=0; i<8; i++) {
        for(uint32_t j=0; j<4; j++) {
            palette_table[i][j] = {0,0,0,0};
        }
    }
    // Load all the tiles
    for(auto& filename : sprite_filenames) {
        process_img(filename);
    }
    process_img("octopus1_move", sprite_to_pallete_idx["octopus1"]);
    process_img("octopus2_move", sprite_to_pallete_idx["octopus2"]);

    store_pallete();
}

bool check_binary_file_existence(){
    for(auto& filename : sprite_filenames) {
        std::ifstream from(data_path(SPRITE_FILEPATH+filename.substr(0,filename.find(".png"))+".bin").c_str(), std::ios::binary);
        if(!from.good()) {
            return false;
        }
    }
    return true;
}