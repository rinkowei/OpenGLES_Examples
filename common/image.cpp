#include "image.h"
#include <stb_image.h>
#include <stb_image_write.h>

namespace es
{
	Image::Pixel<unsigned char> Image::pixelf2uc(const Pixel<float>& pixel)
	{
		Image::Pixel<unsigned char> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			float mappedVal = 255.99f * pixel[i];
			rst[i] = static_cast<unsigned char>(mappedVal >= 256.0f ? 255 : static_cast<unsigned char>(mappedVal));
		}
		return rst;
	}

	Image::Pixel<unsigned char> Image::pixeld2uc(const Pixel<double>& pixel)
	{
		Image::Pixel<unsigned char> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			float mappedVal = 255.99 * pixel[i];
			rst[i] = static_cast<unsigned char>(mappedVal >= 256.0 ? 255 : static_cast<unsigned char>(mappedVal));
		}
		return rst;
	}

	Image::Pixel<float> Image::pixeluc2f(const Pixel<unsigned char>& pixel)
	{
		Image::Pixel<float> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			rst[i] = 1.0f / 255.0f * pixel[i];
		}
		return rst;
	}

	Image::Pixel<double> Image::pixeluc2d(const Pixel<unsigned char>& pixel)
	{
		Image::Pixel<double> rst(pixel.channel);
		for (uint32_t i = 0; i < pixel.channel; i++)
		{
			rst[i] = 1.0 / 255.0 * pixel[i];
		}
		return rst;
	}

	Image::Image()
		:data(nullptr),
		width(0),
		height(0),
		channel(0),
		type(SrcType::Invalid)
	{

	}

	Image::Image(uint32_t width, uint32_t height, uint32_t channel)
	{
		genBuffer(width, height, channel);
	}

	Image::Image(const std::string& filename, bool flipY)
	{
		data = nullptr;
		type = SrcType::Invalid;
		load(filename, flipY);
	}

	Image::~Image()
	{
		free();
	}

	bool Image::isValid() const
	{
		return data != nullptr && type != SrcType::Invalid;
	}

	unsigned char* Image::getData()
	{
		if (!isValid())
			return nullptr;
		return data;
	}

	const unsigned char* Image::getDataConst() const
	{
		if (!isValid())
			return nullptr;
		return data;
	}

	uint32_t Image::getWidth() const {
		return width;
	}

	uint32_t Image::getHeight() const {
		return height;
	}

	uint32_t Image::getChannel() const {
		return channel;
	}

	unsigned char& Image::at(uint32_t x, uint32_t y, uint32_t channel)
	{
		return data[(y * width + x) * this->channel + channel];
	}

	const unsigned char& Image::at(uint32_t x, uint32_t y, uint32_t channel) const
	{
		return data[(y * width + x) * this->channel + channel];
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<unsigned char>& pixel)
	{
		if (pixel.channel != this->channel)
			return false;

		for (uint32_t i = 0; i < channel; i++)
		{
			at(x, y, i) = pixel[i];
		}

		return true;
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<float>& pixel)
	{
		return setPixel(x, y, pixelf2uc(pixel));
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const Pixel<double>& pixel)
	{
		return setPixel(x, y, pixeld2uc(pixel));
	}

	bool Image::setPixel(uint32_t x, uint32_t y, const glm::vec3& pixel)
	{
		return setPixel(x, y, pixelf2uc(Image::Pixel<float>(pixel.r, pixel.g, pixel.b)));
	}

	Image::Pixel<unsigned char> Image::getPixel(uint32_t x, uint32_t y) const
	{
		Pixel<unsigned char> rst(channel);
		for (uint32_t i = 0; i < channel; i++)
		{
			rst[i] = at(x, y, i);
		}
		return rst;
	}

	Image::Pixel<float> Image::getPixelf(uint32_t x, uint32_t y) const
	{
		return pixeluc2f(getPixel(x, y));
	}

	Image::Pixel<double> Image::getPixeld(uint32_t x, uint32_t y) const
	{
		return pixeluc2d(getPixel(x, y));
	}

	bool Image::load(const std::string& filename, bool flipY)
	{
		free();

		stbi_set_flip_vertically_on_load(flipY);

		int tempW, tempH, tempC;
		data = stbi_load(filename.c_str(), &tempW, &tempH, &tempC, 0);
		width = tempW;
		height = tempH;
		channel = tempC;

		if (!data)
		{
			type = SrcType::Invalid;
			return false;
		}

		type = SrcType::Stb;
		return true;
	}

	void Image::genBuffer(uint32_t width, uint32_t height, uint32_t channel)
	{
		free();

		this->width = width;
		this->height = height;
		this->channel = channel;

		data = new unsigned char[width * height * channel]();
		type = SrcType::New;
	}

	void Image::free()
	{
		if (!data)
		{
			switch (type)
			{
				case SrcType::New:
				{
					delete[] data;
					break;
				}
				case SrcType::Stb:
				{
					stbi_image_free(data);
					break;
				}
				case SrcType::Invalid:
				{
					break;
				}
				default:
					break;
			}

			width = height = channel = 0;
			data = nullptr;
			type = SrcType::Invalid;
		}
	}

	bool Image::saveAsPNG(const std::string& filename, bool flipY) const
	{
		stbi_flip_vertically_on_write(flipY);
		return stbi_write_png(filename.c_str(), width, height, channel, data, width * 3);
	}
}
