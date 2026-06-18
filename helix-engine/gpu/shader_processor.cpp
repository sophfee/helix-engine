#include "shader_processor.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

#include "png.hpp"
#include "render_server.h"
#include "util.hpp"


namespace detail {
	struct include_info {
		std::string file;
		// For when backing out.
		std::size_t line_included_at;
	};

	void skip_utf8_bom(std::ifstream& file) {
		const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
		char buf[3] = {};
		auto pos = file.tellg();
		if (file.read(buf, 3) &&
			static_cast<unsigned char>(buf[0]) == bom[0] &&
			static_cast<unsigned char>(buf[1]) == bom[1] &&
			static_cast<unsigned char>(buf[2]) == bom[2]) {
			return; // BOM consumed, stream positioned after it
			}
		file.seekg(pos); // No BOM found, rewind
	}

	std::string parse(std::deque<include_info> &files) {
		include_info info = files.front();
		std::string file = info.file;
		std::size_t line_included_at = info.line_included_at;
		
		std::ifstream f(file, std::ios::in | std::ios::binary);
		assert(!f.bad());
		
		if (!f.is_open()) {
			throw std::runtime_error("Failed to open shader file: " + file);
		}

		f.seekg(0, std::ios::beg); //< I keep getting a random ï»¿ at the start of the file, idk what it is

		skip_utf8_bom(f);

		std::string cont;
		bool const is_root_file = files.size() == 1;

		if (!is_root_file) {
			cont += "#line " + std::to_string(0) + " " + std::to_string(files.size()) + "\n";
		}
		bool bindless_checked = false;
		std::string line;
		size_t line_number = 0;
		while (std::getline(f, line)) {

			if (line_number > 2 && !bindless_checked) {
				if (!RenderServer::singleton().extensionSupported("GL_ARB_bindless_texture"))
					cont += "#ifndef NO_BINDLESS\n#define NO_BINDLESS\n#endif\n#line " + std::to_string(line_number) + " " + std::to_string(files.size()) + "\n";
				bindless_checked = true;
			}
			line_number++;

			std::string const &first_word = line.substr(0, line.find_first_of(' '));
			switch (hash(first_word)) {
				case hash("#pragma"): {
					std::string const &command = line.substr(line.find_first_of(' ') + 1, line.find_first_of(' ', line.find_first_of(' ') + 1) - line.find_first_of(' ') - 1);
					if (hash(command) == hash("include")) {
						std::size_t first_quote = line.find_first_of('"');
						std::size_t last_quote = line.find_last_of('"');
						
						std::string include_path = line.substr(first_quote + 1, last_quote - first_quote - 1);
						bool already_included = false;
						
						for (auto &[inc_file, _] : files) {
							std::string &include_file = inc_file;
							//< Check if it's already included.
							if (hash(include_file) == hash(include_path))
								already_included = true;
						}

						if (already_included)
							break; //< Will be the harbinger of errors if we don't do this.

						
						files.push_front({
							.file = include_path,
							.line_included_at = line_number + 1
						});
						
						cont += parse(files);
					}
					break;
				}
				default:
					cont += line + '\n';
					break;
			}
		}

		if (f.eof()) {
			files.pop_front(); // Finished processing this file.
		}

		if (!is_root_file) {
			cont += "#line " + std::to_string(line_included_at) + " " + std::to_string(files.size() - 1) + "\n";
		}

		return cont;
	}
}

std::string ShaderProcessor::load(std::string const &path) {
	std::deque<detail::include_info> files;
	files.push_front({
		.file = path,
		.line_included_at = 0
	});
	std::string result = detail::parse(files);
	return result;
}
