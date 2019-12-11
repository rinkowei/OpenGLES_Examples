#ifndef IMAGE_H_
#define IMAGE_H_

#include <glm/glm.hpp>
#include <string>

namespace es
{
	class Image
	{
	public:
		template<typename T>
		struct Pixel
		{
			Pixel(uint32_t channel) : channel(channel)
			{

			};
			Pixel(T r, T g, T b) : r(r), g(g), b(b), channel(3)
			{

			};
			Pixel(T r, T g, T b, T a) : r(r), g(g), b(b), a(a), channel(4)
			{

			};

			T& operator[](uint32_t idx)
			{
				switch (idx)
				{
					case 0:
						return r;
					case 1:
						return g;
					case 2:
						return b;
					case 3:
						return a;
				}
			}

			const T& operator[](uint32_t idx) const
			{
				switch (idx)
				{
				case 0:
					return r;
				case 1:
					return g;
				case 2:
					return b;
				case 3:
					return a;
				}
			}

			T r;
			T g;
			T b;
			T a;
			const uint32_t channel;
		};

		Image();
		Image(uint32_t width, uint32_t height, uint32_t channel);
		Image(const std::string& filename, bool flipY = false);
		~Image();

		bool isValid() const;
		unsigned char* getData();
		const unsigned char* getDataConst() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getChannel() const;

		bool setPixel(uint32_t x, uint32_t y, const Pixel<unsigned char>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const Pixel<float>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const Pixel<double>& pixel);
		bool setPixel(uint32_t x, uint32_t y, const glm::vec3& pixel);

		Pixel<unsigned char> getPixel(uint32_t x, uint32_t y) const;
		Pixel<float> getPixelf(uint32_t x, uint32_t y) const;
		Pixel<double> getPixeld(uint32_t x, uint32_t y) const;

		unsigned char& at(uint32_t x, uint32_t y, uint32_t channel);
		const unsigned char& at(uint32_t x, uint32_t y, uint32_t channel) const;

		bool load(const std::string& filename, bool flipY = false);
		void genBuffer(uint32_t width, uint32_t height, uint32_t channel);
		void free();
		bool saveAsPNG(const std::string& filename, bool flipY = false) const;

		static Pixel<unsigned char> pixelf2uc(const Pixel<float>& pixel);
		static Pixel<unsigned char> pixeld2uc(const Pixel<double>& pixel);
		static Pixel<float> pixeluc2f(const Pixel<unsigned char>& pixel);
		static Pixel<double> pixeluc2d(const Pixel<unsigned char>& pixel);

		Image& operator=(const Image& img) = delete;
	private:
		enum class SrcType
		{
			Invalid,
			New,
			Stb
		};

		unsigned char* data;
		uint32_t width;
		uint32_t height;
		uint32_t channel;
		SrcType type;
	};
}

#endif