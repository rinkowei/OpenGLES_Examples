#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <ogles.h>
#include <program.h>
#include <texture.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

namespace es
{
	class Material
	{
	public:
		static Material* createFromFiles(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles);
	private:
		Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles);
		~Material();

		static std::unordered_map<std::string, std::shared_ptr<Material>> mMatCache;
		static std::unordered_map<std::string, std::shared_ptr<Texture2D>> mTextureCache;

		std::unique_ptr<Program> mProgram;
		std::vector<std::shared_ptr<Texture2D>> mTextures;
	};
}

#endif