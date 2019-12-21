#version 310 es
precision mediump float;
layout(location = 0) out vec4 fragColor;

in vec2 fTexcoord;

uniform sampler2D inputImage;

float normpdf(float x, float sigma)
{
  return 0.39894*exp(-0.5*x*x / (sigma*sigma)) / sigma;
}


void main()
{
    vec4 c = texture(inputImage, fTexcoord);

    const int mSize = 11;
    const int kSize = int((float(mSize) - 1.0) / 2.0);
    float kernel[mSize];
    vec4 final_color = vec4(0.0);

    // Create the kernel
    float sigma = 7.0;
    float Z = 0.0;
    for (int j = 0; j <= kSize; ++j)
    {
        kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), sigma);
    }

    //get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j)
    {
        Z += kernel[j];
    }

    //read out the texels
    for (int i = -kSize; i <= kSize; ++i)
    {
        for (int j = -kSize; j <= kSize; ++j)
        {
            final_color += kernel[kSize + j] * kernel[kSize + i] * texture(inputImage, (fTexcoord + vec2(float(i), float(j))/vec2(textureSize(inputImage, 0).xy)));
        }
    }
    fragColor = texture(inputImage, fTexcoord);
    //fragColor = vec4(final_color / (Z*Z));
}