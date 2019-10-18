#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMaterialCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::string>& textureFiles)
		:mName(name)
	{
		mProgram = Program::createFromFiles(shaderFiles);
		mProgram->apply();

		int location = 0;
		const std::unordered_map<std::string, GLuint>& uniformMap = mProgram->getUniformLocationMap();
		for (auto iter = textureFiles.begin(); iter != textureFiles.end(); iter++)
		{
			std::shared_ptr<Texture2D> tex2d = Texture2D::createFromFile(iter->second, 0, false);

			for (auto uniform = uniformMap.begin(); uniform != uniformMap.end(); uniform++)
			{
				if (iter->first == uniform->first)
				{
					location = static_cast<int>(uniform->second);
					mProgram->setUniform(iter->first, location);
					mTextureMap[std::make_pair(iter->first, location)] = tex2d;
				}
			}
		}

		mProgram->unapply();
	}

	Material::~Material()
	{
		mProgram.reset(nullptr);

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

	bool Material::setUniform(const std::string& name, int value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, bool value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, float value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::vec2& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::vec3& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::vec4& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::mat2& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::mat3& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, const glm::mat4& value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, int* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, float* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::vec2* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::vec3* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::vec4* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::mat2* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::mat3* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}

	bool Material::setUniform(const std::string& name, int count, glm::mat4* value)
	{
		if (mProgram != nullptr)
		{
			mProgram->setUniform(name, count, value);
		}
		return true;
	}
}