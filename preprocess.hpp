#pragma once
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "data_path.hpp"
#include "load_save_png.hpp"
#include "read_write_chunk.hpp"

const std::vector<std::string> sprite_filenames = {
    "octopus1",
    "octopus2",
    "crown",
    "splash",
    "splash2",
    "potion"
};
const std::string SPRITE_MAGIC = "sprt";
const std::string PALLETE_MAGIC = "palt";
const std::string SPRITE_INPUT_FILEPATH = "../sprites/";
const std::string SPRITE_OUTPUT_FILEPATH = "resources/";

static std::array< std::array< glm::u8vec4, 4 >, 8 > palette_table;
static uint32_t pallette_idx = 1; // next pallette index available
static std::unordered_map<std::string, uint32_t> sprite_to_pallete_idx;

void process_img(std::string filename, uint32_t existing_pallete_idx = 0, glm::uvec2 size = glm::uvec2(16,16), enum OriginLocation origin = LowerLeftOrigin);

void store_pallete();

void preprocess_sprite();

bool check_binary_file_existence();