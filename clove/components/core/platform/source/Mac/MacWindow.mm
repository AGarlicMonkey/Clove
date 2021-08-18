#include "Clove/Platform/Mac/MacWindow.hpp"

#include <Clove/Log/Log.hpp>

@implementation MacWindowProxy
- (void)windowWillClose:(NSNotification *)notification {
    _cloveWindow->close();
}

- (void)sendEvent:(NSEvent *)event{
    //Forward any events into our window
    _cloveWindow->handleNsEvent(event);
    [super sendEvent:event];
}
@end

namespace clove{
    MacWindow::MacWindow(Descriptor const &descriptor)
        : Window(keyboardDispatcher, mouseDispatcher) {
        //Application specific init
        [NSApplication sharedApplication];
        [NSApp finishLaunching];
        
        //This makes it get treated like an app
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        //Window specific init
        NSString *nameString = [NSString stringWithCString:descriptor.title.c_str() encoding:[NSString defaultCStringEncoding]];
        if(nameString == nullptr) {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Critical, "nameString returned nullptr. Aborting");
            abort();
        }
        
        NSRect const rect{ NSMakeRect(0, 0, descriptor.width, descriptor.height) };
        NSWindowStyleMask const styleMask{ NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable };
        
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
    }
    
    MacWindow::~MacWindow() = default;
    
    std::unique_ptr<Window> Window::create(Descriptor const &descriptor) {
        return std::make_unique<MacWindow>(descriptor);
    }
    
    void MacWindow::processInput() {
        /*
         Currently in Clove each window also pumps the application's event queue.
         It's no different on macOS but it does mean we have to send each event
         to the 'application' for it to get routed back into this window.
         */
        
        while(NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES]){
            [NSApp sendEvent:event];
        }
    }
    
    std::any MacWindow::getNativeWindow() const {
        //Make sure the any holds an NSWindow and not our custom window.
        return static_cast<NSWindow*>(windowProxy);
    }
    
    vec2i MacWindow::getPosition(bool clientArea) const {
        /*
         The window coordinates work from the bottom left. Clove expects them to
         be in the top left so we need to transform the Y coordinate.
         */
        NSRect const screenFrame{ [[NSScreen mainScreen] frame] };
        if(clientArea) {
            NSRect const contentFrame{ [windowProxy contentRectForFrameRect:[windowProxy frame]] };
            return { contentFrame.origin.x, screenFrame.size.height - (contentFrame.origin.y + contentFrame.size.height) };
        } else {
            NSRect const frame{ [windowProxy frame] };
            return { frame.origin.x, screenFrame.size.height - (frame.origin.y + frame.size.height) };
        }
    }
    
    vec2i MacWindow::getSize(bool clientArea) const {
        if(clientArea) {
            NSRect const contentFrame{ [windowProxy contentRectForFrameRect:[windowProxy frame]] };
            return { contentFrame.size.width, contentFrame.size.height};
        } else {
            NSRect frame = [windowProxy frame];
            return { frame.size.width, frame.size.height };
        }
    }
    
    void MacWindow::moveWindow(vec2i const &position) {
        vec2i const size{ getSize(false) };
        [windowProxy setFrame:NSMakeRect(position.x, position.x, size.x, size.y) display:YES];
    }
    
    void MacWindow::resizeWindow(vec2i const &size) {
        vec2i const position{ getPosition(false) };
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
                mouseDispatcher.onMouseMove(mouseLoc);
                break;
                
            case NSEventTypeLeftMouseDown:
                mouseDispatcher.onButtonPressed(MouseButton::_1, mouseLoc);
                break;
                
            case NSEventTypeLeftMouseUp:
                mouseDispatcher.onButtonReleased(MouseButton::_1, mouseLoc);
                break;
                
            case NSEventTypeRightMouseDown:
                mouseDispatcher.onButtonPressed(MouseButton::_2, mouseLoc);
                break;
                
            case NSEventTypeRightMouseUp:
                mouseDispatcher.onButtonReleased(MouseButton::_2, mouseLoc);
                break;
                
            case NSEventTypeOtherMouseDown:
                if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_3)) != 0){
                    mouseDispatcher.onButtonPressed(MouseButton::_3, mouseLoc);
                }else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_4)) != 0){
                    mouseDispatcher.onButtonPressed(MouseButton::_4, mouseLoc);
                }else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_5)) != 0){
                    mouseDispatcher.onButtonPressed(MouseButton::_5, mouseLoc);
                }
                break;
                
            case NSEventTypeOtherMouseUp:
                if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_3)) != 0){
                    mouseDispatcher.onButtonReleased(MouseButton::_3, mouseLoc);
                }else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_4)) != 0){
                    mouseDispatcher.onButtonReleased(MouseButton::_4, mouseLoc);
                }else if(([NSEvent pressedMouseButtons] & static_cast<NSUInteger>(MouseButton::_5)) != 0){
                    mouseDispatcher.onButtonReleased(MouseButton::_5, mouseLoc);
                }
                break;
                
                
            case NSEventTypeScrollWheel:
                mouseDispatcher.onWheelDelta(static_cast<int32_t>([event scrollingDeltaY]), mouseLoc);
                break;
                
            default:
                break;
        }
    }
}
