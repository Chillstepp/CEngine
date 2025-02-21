//
// Created by Chillstep on 25-2-14.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CEFILEUTIL_H_
#define CENGINE_PLATFORM_PUBLIC_CEFILEUTIL_H_

#include "CEngine.h"

#ifdef CE_DEFINE_RES_ROOT_DIR
#define CE_RES_ROOT_DIR CE_DEFINE_RES_ROOT_DIR
#else
#define CE_RES_ROOT_DIR "Resource/"
#endif

#define CE_RES_CONFIG_DIR           CE_RES_ROOT_DIR"Config/"
#define CE_RES_SHADER_DIR           CE_RES_ROOT_DIR"Shader/"
#define CE_RES_FONT_DIR             CE_RES_ROOT_DIR"Font/"
#define CE_RES_MODEL_DIR            CE_RES_ROOT_DIR"Model/"
#define CE_RES_MATERIAL_DIR         CE_RES_ROOT_DIR"Material/"
#define CE_RES_TEXTURE_DIR          CE_RES_ROOT_DIR"Texture/"
#define CE_RES_SCENE_DIR            CE_RES_ROOT_DIR"Scene/"

namespace CE{
	static std::string GetFileName(const std::string &filePath){
		if(filePath.empty()){
			return filePath;
		}
		std::filesystem::path path(filePath);
		return path.filename().string();
	}

	static void FormatFileSize(std::uintmax_t fileSize, float *outSize, std::string &outUnit) {
		float size = static_cast<float>(fileSize);
		if (size < 1024) {
			outUnit = "B";
		} else if (size < 1024 * 1024) {
			size /= 1024;
			outUnit = "KB";
		} else if (size < 1024 * 1024 * 1024) {
			size /= (1024 * 1024);
			outUnit = "MB";
		} else {
			size /= (1024 * 1024 * 1024);
			outUnit = "GB";
		}
		*outSize = size;
	}

	static std::string FormatSystemTime(std::filesystem::file_time_type fileTime) {
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm* tm = std::localtime(&time);

		std::stringstream ss;
		ss << std::put_time(tm, "%Y/%m/%d %H:%M");

		return ss.str();
	}

	static std::vector<char> ReadCharArrayFromFile(const std::string& filePath) {
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if(!file.is_open()){
			throw std::runtime_error("Could not open the file: " + filePath);
		}

		auto fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}

#endif //CENGINE_PLATFORM_PUBLIC_CEFILEUTIL_H_
