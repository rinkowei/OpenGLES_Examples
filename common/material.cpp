#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMatCache;
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> Material::mTextureCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles)
	{
		mProgram = std::make_unique<Program>(shaderFiles);

		for (std::size_t i = 0; i < textureFiles.size(); i++)
		{
			std::shared_ptr<Texture2D> tex2d;
			if (mTextureCache.find(textureFiles[i]) == mTextureCache.end())
			{
				tex2d = std::make_shared<Texture2D>(textureFiles[i], 0, false);
				mTextureCache[textureFiles[i]] = tex2d;
			}
			else
			{
				tex2d = mTextureCache[textureFiles[i]];
			}
			mTextures.push_back(tex2d);
		}
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
			mMatCache[name] = std::make_shared<Material>(mat);
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