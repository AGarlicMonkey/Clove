#include "clvpch.hpp"
#include "GL4VertexBuffer.hpp"

#include <glad/glad.h>

namespace clv::gfx{
	GLenum getGLElementType(VertexElementType type){
		switch(type){
			case VertexElementType::position2D:
			case VertexElementType::position3D:
			case VertexElementType::texture2D:
			case VertexElementType::normal:
				return GL_FLOAT;
			default:
				CLV_ASSERT(false, "Invalid element type");
				return GL_FLOAT;
		}
	}

	GLboolean isTypeNormalised(VertexElementType type){
		switch(type){
			case VertexElementType::position2D:
			case VertexElementType::position3D:
			case VertexElementType::texture2D:
			case VertexElementType::normal:
				return GL_FALSE;
			default:
				CLV_ASSERT(false, "Invalid element type");
				return GL_FALSE;
		}
	}

	GL4VertexBuffer::GL4VertexBuffer(GL4VertexBuffer&& other) noexcept = default;

	GL4VertexBuffer& GL4VertexBuffer::operator=(GL4VertexBuffer&& other) noexcept = default;

	GL4VertexBuffer::~GL4VertexBuffer(){
		glDeleteVertexArrays(1, &arrayID);
		glDeleteBuffers(1, &bufferID);
	}

	GL4VertexBuffer::GL4VertexBuffer(const VertexBufferData& bufferData, Shader& shader)
		: VertexBuffer(bufferData){
		const uint32 size = static_cast<uint32>(this->bufferData.sizeBytes());
		const void* data = bufferData.data();

		glGenVertexArrays(1, &arrayID);
		glBindVertexArray(arrayID);
		
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		uint64 offset = 0; //void* expects 64 bits
		for(uint32 i = 0; i < bufferData.getLayout().count(); ++i){
			const auto& element = bufferData.getLayout().resolve(i);
			const VertexElementType elementType = element.getType();

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.getCount(), getGLElementType(elementType), isTypeNormalised(elementType), static_cast<GLsizei>(bufferData.getLayout().size()), reinterpret_cast<const void*>(offset));
			offset += VertexElement::sizeOf(element.getType());
		}
	}

	void GL4VertexBuffer::bind(){
		glBindVertexArray(arrayID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	}
}