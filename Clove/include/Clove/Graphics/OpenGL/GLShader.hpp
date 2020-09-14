#pragma once

#include "Clove/Graphics/Shader.hpp"

#include <glad/glad.h>
#include <span>

namespace clv::gfx::ogl {
	class GLShader : public Shader {
		//VARIABLES
	private:
		std::shared_ptr<GraphicsFactory> factory;

		GLuint shaderID = 0;

		ShaderDescriptor descriptor;

		//FUNCTIONS
	public:
		GLShader() = delete;
		GLShader(std::shared_ptr<GraphicsFactory> factory, ShaderDescriptor descriptor, std::string_view pathToShader);
        GLShader(std::shared_ptr<GraphicsFactory> factory, ShaderDescriptor descriptor, std::span<const std::byte> sourceBytes);

		GLShader(const GLShader& other) = delete;
		GLShader(GLShader&& other) noexcept;

		GLShader& operator=(const GLShader& other) = delete;
		GLShader& operator=(GLShader&& other) noexcept;

		~GLShader();

		const std::shared_ptr<GraphicsFactory>& getFactory() const override;

		const ShaderDescriptor& getDescriptor() const override;

		GLuint getShaderID() const;

	private:
		void compileShader(GLenum stage, std::string_view source);
	};
}