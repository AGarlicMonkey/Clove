#include "Clove/Platform/Input/Mouse.hpp"
#include "Clove/Platform/Mac/CloveMac.hpp"

namespace clove {
	vec2i Mouse::getPosition() const {
		//There currently doesn't seem to be a way to get the mouse position outside of the events with cocoa
		return dispatcher.lastPos;
	}

	void Mouse::setPosition(vec2i const &position) {
		CGWarpMouseCursorPosition(CGPointMake(position.x, position.y));
		
		//CGWarpMouseCursorPosition doesn't generate a move event so spoof our own
		dispatcher.onMouseMove(position);
	}

	void Mouse::show(bool shouldShow) { //NOLINT Ignore recommendation for static as on some platforms it can't be
		if(shouldShow) {
			[NSCursor hide];
		} else {
			[NSCursor unhide];
		}
	}
}
