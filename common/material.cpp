#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMaterialCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::string>& textureFiles)
	{
		mName = name;
		mProgram = Program::createFromFiles(name, shaderFiles);
		mProgram->apply();

		int location = 0;
		const std::unordered_map<std::string, GLuint>& uniformMap = mProgram->getUniformLocationMap();
		for (auto iter = textureFiles.begin(); iter != textureFiles.end(); iter++)
		{
			for (auto uniform = uniformMap.begin(); uniform != uniformMap.end(); uniform++)
			{
				if (iter->first == uniform->first)
				{
					std::shared_ptr<Texture2D> tex2d = Texture2D::createFromFile(iter->second, 1, false);

					mProgram->setUniform(iter->first, location);
					mTextureMap[std::make_pair(iter->first, location)] = tex2d;
					location++;
				}
			}
		}

		mProgram->unapply();
	}

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::shared_ptr<Texture2D>>& textures)
	{
		mName = name;
		mProgram = Program::createFromFiles(name, shaderFiles);
		mProgram->apply();

		int location = 0;
		const std::unordered_map<std::string, GLuint>& uniformMap = mProgram->getUniformLocationMap();
		for (auto iter = textures.begin(); iter != textures.end(); iter++)
		{
			for (auto uniform = uniformMap.begin(); uniform != uniformMap.end(); uniform++)
			{
				if (iter->first == uniform->first)
				{
					mProgram->setUniform(iter->first, location);
					mTextureMap[std::make_pair(iter->first, location)] = iter->second;
					location++;
				}
			}
		}

		mProgram->unapply();
	}

	Material::Material(const std::string& name, std::shared_ptr<Program> program, const std::unordered_map<std::string, std::string>& textureFiles)
	{
		mName = name;
		mProgram = program;
		mProgram->apply();

		int location = 0;
		const std::unordered_map<std::string, GLuint>& uniformMap = mProgram->getUniformLocationMap();
		for (auto iter = textureFiles.begin(); iter != textureFiles.end(); iter++)
		{
			for (auto uniform = uniformMap.begin(); uniform != uniformMap.end(); uniform++)
			{
				if (iter->first == uniform->first)
				{
					std::shared_ptr<Texture2D> tex2d = Texture2D::createFromFile(iter->second, 1, false);

					mProgram->setUniform(iter->first, location);
					mTextureMap[std::make_pair(iter->first, location)] = tex2d;
					location++;
				}
			}
		}

		mProgram->unapply();
	}

	Material::~Material()
	{
		if (mProgram != nullptr)
		{
			mProgram.reset();
			mProgram = nullptr;
		}

		for (auto iter = mTextureMap.begin(); iter != mTextureMap.end(); iter++)
		{
			iter->second.reset();
			iter->second = nullptr;
		}
		mTextureMap.swap(std::unordered_map<std::pair<std::string, GLuint>, std::shared_ptr<Texture2D>, PairHash>());
	}

	std::shared_ptr<Material> Material::createFromFiles(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::string>& textureFiles)
	{
		if (mMaterialCache.find(name) == mMaterialCache.end())
		{
			std::shared_ptr<Material> mat = std::make_shared<Material>(name, shaderFiles, textureFiles);
			mMaterialCache[name] = mat;
			return mat;
		}
		else
		{
			return mMaterialCache[name];
		}
	}

	std::shared_ptr<Material> Material::createFromData(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::shared_ptr<Texture2D>>& textures)
	{
		if (mMaterialCache.find(name) == mMaterialCache.end())
		{
			std::shared_ptr<Material> mat = std::make_shared<Material>(name, shaderFiles, textures);
			mMaterialCache[name] = mat;
			return mat;
		}
		else
		{
			return mMaterialCache[name];
		}
	}

	std::shared_ptr<Material> Material::createFromProgram(const std::string& name, std::shared_ptr<Program> program, const std::unordered_map<std::string, std::string>& textureFiles)
	{
		if (mMaterialCache.find(name) == mMaterialCache.end())
		{
			std::shared_ptr<Material> mat = std::make_shared<Material>(name, program, textureFiles);
			mMaterialCache[name] = mat;
			return mat;
		}
		else
		{
			return mMaterialCache[name];
		}
	}

	void Material::apply()
	{
		if (mProgram != nullptr)
		{
			mProgram->apply();

			for (auto iter = mTextureMap.begin(); iter != mTextureMap.end(); iter++)
			{
				iter->second->bind(iter->first.second);
			}
		}
	}

	void Material::unapply()
	{
		if (mProgram != nullptr)
		{
			mProgram->unapply();

			for (auto iter = mTextureMap.begin(); iter != mTextureMap.end(); iter++)
			{
				iter->second->unbind(iter->first.second);
			}
		}
	}

	std::shared_ptr<Program> Material::getProgram() const
	{
		return mProgram;
	}

	void Material::setTexture(const std::string& name, std::shared_ptr<Texture2D> texture)
	{
		bool isExists = false;
		for (auto& iter = mTextureMap.begin(); iter != mTextureMap.end(); iter++)
		{
			if (iter->first.first == name)
			{
				iter->second = texture;
				isExists = true;
			}
		}

		if (!isExists)
		{
			mTextureMap[std::make_pair(name, static_cast<int>(mTextureMap.size()))] = texture;
		}
	}
}