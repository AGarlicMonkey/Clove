#include "Clove/Graphics/OpenGL/GLRenderTarget.hpp"

#include "Clove/Graphics/OpenGL/GLTexture.hpp"

namespace clv::gfx::ogl{
	GLRenderTarget::GLRenderTarget(std::shared_ptr<GraphicsFactory> factory)
		: factory(std::move(factory)){
	}

	GLRenderTarget::GLRenderTarget(std::shared_ptr<GraphicsFactory> factory, Texture* colourTexture, Texture* depthStencilTexture) 
		: factory(std::move(factory)) {
		glGenFramebuffers(1, &frameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

		CLV_ASSERT(colourTexture != nullptr || depthStencilTexture != nullptr, "{0}: Render target needs at least one valid texture", CLV_FUNCTION_NAME);

		//Colour buffer
		if(colourTexture != nullptr){
			GLTexture* glColourTexture = static_cast<GLTexture*>(colourTexture);
			const TextureUsage usage = glColourTexture->getDescriptor().usage;
			const GLuint textureRenderID = glColourTexture->getTextureID();

			CLV_ASSERT(usage == TextureUsage::RenderTarget_Colour, "Incorrect texture type for colour texture");

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureRenderID, 0);
		} else{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		//Depth / Stencil buffer
		if(depthStencilTexture != nullptr){
			GLTexture* gldepthStencilTexture = static_cast<GLTexture*>(depthStencilTexture);
			const TextureUsage usage = gldepthStencilTexture->getDescriptor().usage;
			const TextureStyle style = gldepthStencilTexture->getDescriptor().style;
			const uint8_t arraySize = gldepthStencilTexture->getDescriptor().arraySize; 
			const GLuint textureRenderID = gldepthStencilTexture->getTextureID();

			//NOTE: Asserting here but this will need to change when adding stencil buffers
			CLV_ASSERT(usage == TextureUsage::RenderTarget_Depth, "Incorrect texture type for depth stencil texture");

			if(style == TextureStyle::Cubemap || arraySize > 1) {
				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureRenderID, 0);
			} else {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureRenderID, 0);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		setClearColour({ 0.0f, 0.0f, 0.0f, 0.0f });
	}

	GLRenderTarget::GLRenderTarget(GLRenderTarget&& other) noexcept = default;

	GLRenderTarget& GLRenderTarget::operator=(GLRenderTarget&& other) noexcept = default;

	GLRenderTarget::~GLRenderTarget(){
		glDeleteFramebuffers(1, &frameBufferID);
		glDeleteRenderbuffers(1, &renderBufferID);
	}

	const std::shared_ptr<GraphicsFactory>& GLRenderTarget::getFactory() const {
		return factory;
	}

	void GLRenderTarget::clear(){
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLRenderTarget::setClearColour(const mth::vec4f& colour){
		clearColour = colour;
	}

	const mth::vec4f& GLRenderTarget::getClearColour() const{
		return clearColour;
	}

	uint32_t GLRenderTarget::getGLFrameBufferID() const{
		return frameBufferID;
	}
}