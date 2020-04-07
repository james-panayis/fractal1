#pragma once

#include "ui_event_interface.hpp"

/*	An ui_event implementation using glfw3

	To install glfw:

		wget https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip
		unzip glfw-3.3.2.zip
		cd glfw-3.3.2
		cmake ./
		make
		sudo make install

	see the glfw window guide: https://www.glfw.org/docs/latest/window_guide.html
	and input guide:					 https://www.glfw.org/docs/latest/input_guide.html

*/

class ui_event_impl : public ui_event_interface
{

public:


	ui_event_impl(bool debug_enabled = false) :
		ui_event_interface(debug_enabled)
	{
	};


	void start(GLFWwindow * window)
	{
		window_ = window;

		init();
	};


private:


	GLFWwindow * window_;	// a ui_event is associated with a specific window


	void init()
	{
		glfwSetWindowUserPointer(window_, this);


		// event: window_size

		glfwSetWindowSizeCallback(window_, [] (GLFWwindow* window, int width, int height) -> void
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->window_size(width, height);
		});


		// event: window_focus

		glfwSetWindowFocusCallback(window_, [] (GLFWwindow* window, int focused)
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->window_focus(focused != 0);
		});


		// event: window_iconified

		glfwSetWindowIconifyCallback(window_, [] (GLFWwindow* window, int iconified)
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->window_iconified(iconified != 0);
		});


		// event: key

		glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE); // include modifier keys pressed in key updates

		glfwSetKeyCallback(window_, [] (GLFWwindow* window, int key, int scancode, int action, int mods) -> void
		{
			// The action needs to be converted. The rest are the same format.

			enum KeyEvent event;

			switch (action)
			{
				case GLFW_PRESS:	 event = KeyEventDown;	 break;
				case GLFW_RELEASE: event = KeyEventUp;		 break;
				case GLFW_REPEAT:	event = KeyEventRepeat; break;

				default: return;
			}

			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->key(key, event, (enum KeyMod)mods);
		});


		// event: mouse_position

		glfwSetCursorPosCallback(window_, [] (GLFWwindow* window, double x_pos, double y_pos) -> void
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->mouse_position(x_pos, y_pos);
		});


		// event: mouse_over

		glfwSetCursorEnterCallback(window_, [] (GLFWwindow* window, int entered) -> void
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->mouse_over(entered != 0);
		});


		// event: mouse_button

		glfwSetMouseButtonCallback(window_, [] (GLFWwindow* window, int button, int action, int mods) -> void
		{
			enum MouseButton b;
			switch (button)
			{
				case GLFW_MOUSE_BUTTON_LEFT:	 b = MouseButtonLeft;	 break;
				case GLFW_MOUSE_BUTTON_MIDDLE: b = MouseButtonMiddle; break;
				case GLFW_MOUSE_BUTTON_RIGHT:	b = MouseButtonRight;	break;

				default: return;
			}

			enum MouseButtonEvent event;
			switch (action)
			{
				case GLFW_PRESS:	 event = MouseButtonEventDown; break;
				case GLFW_RELEASE: event = MouseButtonEventUp;	 break;

				default: return;
			}

			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->mouse_button(b, event, (enum KeyMod)mods);
		});


		// event: scroll_wheel

		glfwSetScrollCallback(window_, [] (GLFWwindow* window, double xoffset, double yoffset) -> void
		{
			ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(glfwGetWindowUserPointer(window));

			if (ui) ui->scroll_wheel(-xoffset, -yoffset);
		});
	};


}; // class ui_event_impl
