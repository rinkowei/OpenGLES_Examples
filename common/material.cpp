#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMatCache;
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> Material::mTextureCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles)
	{

	}

	Material::~Material()
	{
		mProgram.release();

		for (std::size_t i = 0; i < mTextures.size(); i++)
		{
			mTextures[i].reset();
			mTextures[i] = nullptr;
		}
	}

	Material* Material::createFromFiles(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles)
	{
		Material* mat = nullptr;
		if (mMatCache.find(name) == mMatCache.end())
		{
			mat = new (std::nothrow) Material(name, shaderFiles, textureFiles);
		}
		else
		{
			mat = mMatCache[name].get();
		}
		
		if (mat)
		{
			return mat;
		}
		delete(mat);
		return nullptr;
	}
}