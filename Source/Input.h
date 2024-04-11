#pragma once

#include <GLError.h>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <Window.h>

enum Key {
	a = GLFW_KEY_A,
	b = GLFW_KEY_B,
	c = GLFW_KEY_C,
	d = GLFW_KEY_D,
	e = GLFW_KEY_E,
	f = GLFW_KEY_F,
	g = GLFW_KEY_G,
	h = GLFW_KEY_H,
	i = GLFW_KEY_I,
	j = GLFW_KEY_J,
	k = GLFW_KEY_K,
	l = GLFW_KEY_L,
	m = GLFW_KEY_M,
	n = GLFW_KEY_N,
	o = GLFW_KEY_O,
	p = GLFW_KEY_P,
	q = GLFW_KEY_Q,
	r = GLFW_KEY_R,
	s = GLFW_KEY_S,
	t = GLFW_KEY_T,
	u = GLFW_KEY_U,
	v = GLFW_KEY_V,
	w = GLFW_KEY_W,
	x = GLFW_KEY_X,
	y = GLFW_KEY_Y,
	z = GLFW_KEY_Z,
	apostraphe = GLFW_KEY_APOSTROPHE,
	comma = GLFW_KEY_COMMA,
	dash = GLFW_KEY_MINUS,
	period = GLFW_KEY_KP_DECIMAL,
	slash = GLFW_KEY_SLASH,
	backslash = GLFW_KEY_BACKSLASH,
	semicolon = GLFW_KEY_SEMICOLON,
	equal = GLFW_KEY_EQUAL,
	open_bracket = GLFW_KEY_LEFT_BRACKET,
	close_bracket = GLFW_KEY_RIGHT_BRACKET,
	grave_accent = GLFW_KEY_GRAVE_ACCENT,
	zero = GLFW_KEY_0,
	one = GLFW_KEY_1,
	two = GLFW_KEY_2,
	three = GLFW_KEY_3,
	four = GLFW_KEY_4,
	five = GLFW_KEY_5,
	six = GLFW_KEY_6,
	seven = GLFW_KEY_7,
	eight = GLFW_KEY_8,
	nine = GLFW_KEY_9,
	f1 = GLFW_KEY_F1,
	f2 = GLFW_KEY_F2,
	f3 = GLFW_KEY_F3,
	f4 = GLFW_KEY_F4,
	f5 = GLFW_KEY_F5,
	f6 = GLFW_KEY_F6,
	f7 = GLFW_KEY_F7,
	f8 = GLFW_KEY_F8,
	f9 = GLFW_KEY_F9,
	f10 = GLFW_KEY_F10,
	f11 = GLFW_KEY_F11,
	f12 = GLFW_KEY_F12,
};

class InputHandler {
	private:
		Window* window;
			
	public:
		InputHandler(Window* window);
		~InputHandler();

		bool GetKeyPressed(Key key);
};