#include "Clove/Platform/Mac/MacWindow.hpp"

@implementation MacWindowProxy
- (void)windowWillClose:(NSNotification *)notification{
	_cloveWindow->close();
}

- (void)sendEvent:(NSEvent *)event{
	//Forward any events into our window
	_cloveWindow->handleNsEvent(event);
	[super sendEvent:event];
}
@end

namespace garlic::clove{
    MacWindow::MacWindow(const WindowDescriptor& descriptor)
        : Window(keyboardDispatcher, mouseDispatcher) {
		NSString* nameString = [NSString stringWithCString:descriptor.title.c_str() encoding:[NSString defaultCStringEncoding]];
		NSRect const rect{ NSMakeRect(0, 0, descriptor.width, descriptor.height) };
		NSWindowStyleMask const styleMask{ NSWindowStyleMaskHUDWindow | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable };
			
		windowProxy = [[MacWindowProxy alloc] initWithContentRect:rect
												  styleMask:styleMask
													backing:NSBackingStoreBuffered
													  defer:NO];

		[windowProxy setTitle:nameString];
		[windowProxy setDelegate:windowProxy];
		[windowProxy setBackgroundColor:NSColor.redColor];
		[windowProxy makeKeyAndOrderFront:nil];

		//[windowProxy setContentView:view];
			
		windowProxy.cloveWindow = this;

		open = true;
			
		[nameString release];
    }

	MacWindow::~MacWindow(){
		[windowProxy release];
	}
	
	void MacWindow::processInput(){
		@autoreleasepool {
			/*
			 Currently in Clove each window also pumps the application's event queue.
			 It's no different on macOS but it does mean we have to send each event
			 to the 'application' for it to get routed back into this window.
			 */
			
			while(NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES]){
				[NSApp sendEvent:event];
			}
		}
	}
	
	std::any MacWindow::getNativeWindow() const{
		//Make sure the any holds an NSWindow and not our custom window.
		return static_cast<NSWindow*>(windowProxy);
	}
	
	vec2i MacWindow::getPosition() const{
		NSRect frame = [windowProxy frame];
		return { frame.origin.x, frame.origin.y };
	}
	
	vec2i MacWindow::getSize() const{
		NSRect frame = [windowProxy frame];
		return { frame.size.width, frame.size.height };
	}

	void MacWindow::moveWindow(const vec2i& position){
		const vec2i size = getSize();
		[windowProxy setFrame:NSMakeRect(position.x, position.x, size.x, size.y) display:YES];
	}
	
	void MacWindow::resizeWindow(const vec2i& size){
		const vec2i position = getPosition();
		[windowProxy setFrame:NSMakeRect(position.x, position.x, size.x, size.y) display:YES];
	}
	
	bool MacWindow::isOpen() const {
		return open;
	}

	void MacWindow::close() {
		if(onWindowCloseDelegate.isBound()) {
			onWindowCloseDelegate.broadcast();
		}
		open = false;
	}
	
	void MacWindow::handleNsEvent(NSEvent* event){
		vec2i mouseLoc{ static_cast<int32_t>([NSEvent mouseLocation].x), static_cast<int32_t>([NSEvent mouseLocation].y) };
		switch ([event type]){
			case NSEventTypeKeyDown:
				keyboardDispatcher.onKeyPressed(static_cast<Key>([event keyCode]));
				break;
			
			case NSEventTypeKeyUp:
				keyboardDispatcher.onKeyReleased(static_cast<Key>([event keyCode]));
				break;
			
			//TODO: Is there a char or key typed event?
		
			case NSEventTypeMouseEntered:
				mouseDispatcher.onMouseEnter();
				break;
			
			case NSEventTypeMouseExited:
				mouseDispatcher.onMouseLeave();
				break;
			
			case NSEventTypeMouseMoved:
				mouseDispatcher.onMouseMove(mouseLoc.x, mouseLoc.y);
				break;
			
			case NSEventTypeLeftMouseDown:
				mouseDispatcher.onButtonPressed(MouseButton::_1, mouseLoc.x, mouseLoc.y);
				break;
			
			case NSEventTypeLeftMouseUp:
				mouseDispatcher.onButtonReleased(MouseButton::_1, mouseLoc.x, mouseLoc.y);
				break;
			
			case NSEventTypeRightMouseDown:
				mouseDispatcher.onButtonPressed(MouseButton::_2, mouseLoc.x, mouseLoc.y);
				break;
			
			case NSEventTypeRightMouseUp:
				mouseDispatcher.onButtonReleased(MouseButton::_2, mouseLoc.x, mouseLoc.y);
				break;
			
			case NSEventTypeOtherMouseDown:
				if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_3)) != 0){
					mouseDispatcher.onButtonPressed(MouseButton::_3, mouseLoc.x, mouseLoc.y);
				}else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_4)) != 0){
					mouseDispatcher.onButtonPressed(MouseButton::_4, mouseLoc.x, mouseLoc.y);
				}else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_5)) != 0){
					mouseDispatcher.onButtonPressed(MouseButton::_5, mouseLoc.x, mouseLoc.y);
				}
				break;
			
			case NSEventTypeOtherMouseUp:
				if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_3)) != 0){
					mouseDispatcher.onButtonReleased(MouseButton::_3, mouseLoc.x, mouseLoc.y);
				}else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_4)) != 0){
					mouseDispatcher.onButtonReleased(MouseButton::_4, mouseLoc.x, mouseLoc.y);
				}else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_5)) != 0){
					mouseDispatcher.onButtonReleased(MouseButton::_5, mouseLoc.x, mouseLoc.y);
				}
				break;
			
			
			case NSEventTypeScrollWheel:
				mouseDispatcher.onWheelDelta(static_cast<int32_t>([event scrollingDeltaY]), mouseLoc.x, mouseLoc.y);
				break;
				
			default:
				break;
		}
	}
}
