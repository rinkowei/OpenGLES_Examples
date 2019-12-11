#include "image.h"

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
}
