namespace clove {
	id<MTLFunction> MetalShader::getFunction() const {
		return function;
	}
    
    vec3ui const &MetalShader::getWorkgroupSize() const {
        return workgroupSize;
    }
}
