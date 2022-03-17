#pragma once

#include <windows.h>

#define MEMBRANE_EXPORT extern "C" __declspec(dllexport)

MEMBRANE_EXPORT void initialise();
MEMBRANE_EXPORT bool isRunning();
MEMBRANE_EXPORT void tick();
MEMBRANE_EXPORT void shutDown();

MEMBRANE_EXPORT void startSession();

MEMBRANE_EXPORT HWND createChildWindow(HWND parent, int width, int height);

MEMBRANE_EXPORT void loadGameDll();
