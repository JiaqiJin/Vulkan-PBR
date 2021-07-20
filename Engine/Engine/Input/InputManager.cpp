#include "InputManager.h"
#include <GLFW/glfw3.h>
#include <cstring>

// Static declarations
bool InputManager::s_Keys[MAX_KEYS];
float InputManager::s_KeyPressure[MAX_KEYS];
bool InputManager::s_Buttons[MAX_BUTTONS];
double InputManager::s_MouseX, InputManager::s_MouseY, InputManager::s_MouseXDelta, InputManager::s_MouseYDelta;
double InputManager::s_ScrollXDelta, InputManager::s_ScrollYDelta;

InputManager::InputManager() {
	s_ScrollXDelta = s_ScrollYDelta = 0;
	s_MouseXDelta = s_MouseYDelta = 0;

	memset(s_Keys, 0, sizeof(bool) * MAX_KEYS);
	memset(s_KeyPressure, 0, sizeof(float) * MAX_KEYS);
	memset(s_Buttons, 0, sizeof(bool) * MAX_BUTTONS);
}

InputManager::~InputManager() {}

void InputManager::Update() {
	s_MouseXDelta = s_MouseYDelta = 0;
	s_ScrollXDelta = 0; s_ScrollYDelta = 0;
}

bool InputManager::IsKeyPressed(unsigned int keycode) {

	return s_Keys[keycode];
}

float InputManager::GetKeyPressure(unsigned int keycode) {
	return s_KeyPressure[keycode];
}

bool InputManager::IsMouseButtonPressed(unsigned int code) {
	return s_Buttons[code];
}

void InputManager::KeyCallback(int key, int scancode, int action, int mods) {
	s_Keys[key] = action != GLFW_RELEASE;
	s_KeyPressure[key] = s_Keys[key];
}

void InputManager::MouseButtonCallback(int button, int action, int mods) {
	s_Buttons[button] = action != GLFW_RELEASE;
}

void InputManager::CursorPositionCallback(double xpos, double ypos) {
	s_MouseXDelta = xpos - s_MouseX;
	s_MouseYDelta = ypos - s_MouseY;
	s_MouseX = xpos;
	s_MouseY = ypos;
}

void InputManager::ScrollCallback(double xoffset, double yoffset) {
	s_ScrollXDelta = xoffset;
	s_ScrollYDelta = yoffset;
}

bool InputManager::GetButton(int keyCode) {
	return s_Keys[keyCode] != GLFW_RELEASE;
}

bool InputManager::GetButtonDown(int keyCode) {
	return s_Keys[keyCode] == GLFW_PRESS;
}