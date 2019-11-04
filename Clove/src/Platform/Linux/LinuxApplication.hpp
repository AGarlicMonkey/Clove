#pragma once

#include "Core/Platform/Application.hpp"

namespace clv::plt{
	class LinuxApplication : public Application{
		//FUNCTIONS
	private:
		virtual std::unique_ptr<Window> createWindow(const WindowProps& props = WindowProps()) override;
		virtual std::unique_ptr<Window> createWindow(const WindowProps& props, gfx::API api) override;
	};
}
