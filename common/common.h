#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "camera.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#include "object.h"
#include "mesh.h"
#include "model.h"
#include "benchmark.h"
#include "UIOverlay.h"
#include "examplebase.h"