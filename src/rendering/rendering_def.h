#pragma once

class Shader;

namespace v3d {
namespace rendering {
enum class GraphicsBackendType { NONE,
                                 OPENGL_API,
                                 VULKAN_API };
enum class WindowBackendHint { NONE,
                               OPENGL_API,
                               VULKAN_API };

class IRenderable {
   public:
    virtual ~IRenderable() = default;
    virtual void renderElement() = 0;
    virtual void renderElementInstanced() = 0;

    // Set uniforms values before rendering
    virtual void setUniforms(Shader *shader) = 0;
};

}  // namespace rendering

}  // namespace v3d
