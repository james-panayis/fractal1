#pragma once

#include "ui_event_interface.hpp"

/* An ui_event implementation using emscripten

	To install emscripten see: https://emscripten.org/docs/getting_started/downloads.html

	see the emscripten guide: https://emscripten.org/docs/api_reference/html5.h.html#focus

*/


class ui_event_impl : public ui_event_interface
{

public:


	ui_event_impl(bool debug_enabled = false) :
		ui_event_interface(debug_enabled)
	{
	};


	~ui_event_impl()
	{
		// event: window_size

		emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);


		// event: window_focus

		emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);

		// these aren't working...?
		//emscripten_set_focusin_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		//emscripten_set_focusout_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);


		// event: orientation

		emscripten_set_orientationchange_callback(this, 1, nullptr);

		// event: key

		emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_keydown_callback (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_keyup_callback	 (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);


		// event: mouse_position

		emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);

		// event: mouse_over

		emscripten_set_mouseover_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_mouseout_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);

		// event: mouse_button

		emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_dblclick_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);


		// event: scroll_wheel

		emscripten_set_wheel_callback(canvas_.c_str(), this, 1, nullptr);

		// event: touch

		emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
		emscripten_set_touchcancel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, nullptr);
	};


	void start(std::string c)
	{
		canvas_ = c;

		init();
	};


private:


	std::string canvas_; // a ui_event is associated with a specific canvas


	static EM_BOOL f_window_size(int event_type, const EmscriptenUiEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		if (event_type != EMSCRIPTEN_EVENT_RESIZE) return EM_FALSE;

		ui->window_size(e->windowInnerWidth, e->windowInnerHeight);

		return EM_TRUE;
	};


	static EM_BOOL f_window_focus(int event_type, const EmscriptenFocusEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		if (event_type == EMSCRIPTEN_EVENT_FOCUS)
		{
			ui->window_focus(true);

			return EM_TRUE;
		}

		return EM_FALSE;
	};


	static EM_BOOL f_orientation(int event_type, const EmscriptenOrientationChangeEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		ui->orientation((enum Orientation)e->orientationIndex);

		return EM_TRUE;
	};


	static EM_BOOL f_key(int event_type, const EmscriptenKeyboardEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		enum KeyEvent event;

		switch (event_type)
		{
			case EMSCRIPTEN_EVENT_KEYPRESS: event = KeyEventPress; break;
			case EMSCRIPTEN_EVENT_KEYDOWN:	event = KeyEventDown;	break;
			case EMSCRIPTEN_EVENT_KEYUP:		event = KeyEventUp;		break;

			default: return EM_FALSE;
		}

		int mods = (e->shiftKey ? KeyModShift	 : 0) |
							 (e->ctrlKey	? KeyModControl : 0) |
							 (e->altKey	 ? KeyModAlt		 : 0) |
							 (e->metaKey	? KeyModSuper	 : 0);

		int k = *(e->key);
		if (k >= 97 && k <= 122) k -= 'a' - 'A';
		if (strlen(e->key) != 1) k = 0;

		ui->key(k, event, (enum KeyMod)mods);

		return EM_TRUE;
	};


	static EM_BOOL f_mouse_position(int event_type, const EmscriptenMouseEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		int button = 0;
		switch (e->button)
		{
			case 0: button = MouseButtonLeft;		break;
			case 1: button = MouseButtonMiddle;	break;
			case 2: button = MouseButtonRight;	 break;
			case 3: button = MouseButtonBack;		break;
			case 4: button = MouseButtonForward; break;
		}

		int mods = (e->shiftKey ? KeyModShift	 : 0) |
							 (e->ctrlKey	? KeyModControl : 0) |
							 (e->altKey	 ? KeyModAlt		 : 0) |
							 (e->metaKey	? KeyModSuper	 : 0);

		switch (event_type)
		{
			case EMSCRIPTEN_EVENT_CLICK:			ui->mouse_button((enum MouseButton)button, MouseButtonEventClick, (enum KeyMod)mods); break;
			case EMSCRIPTEN_EVENT_MOUSEDOWN:	ui->mouse_button((enum MouseButton)button, MouseButtonEventDown, (enum KeyMod)mods); break;
			case EMSCRIPTEN_EVENT_MOUSEUP:		ui->mouse_button((enum MouseButton)button, MouseButtonEventUp, (enum KeyMod)mods); break;
			case EMSCRIPTEN_EVENT_DBLCLICK:	 ui->mouse_button((enum MouseButton)button, MouseButtonEventDoubleClick, (enum KeyMod)mods); break;
			case EMSCRIPTEN_EVENT_MOUSEMOVE:	ui->mouse_position((double)e->clientX, (double)e->clientY); break;
			case EMSCRIPTEN_EVENT_MOUSEENTER: break;
			case EMSCRIPTEN_EVENT_MOUSELEAVE: break;
			case EMSCRIPTEN_EVENT_MOUSEOVER:	ui->mouse_over(true); break;
			case EMSCRIPTEN_EVENT_MOUSEOUT:	 ui->mouse_over(false); break;

			default: return EM_FALSE;
		}

		return EM_TRUE;
	};


	static EM_BOOL f_scroll_wheel(int event_type, const EmscriptenWheelEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		ui->scroll_wheel(e->deltaX, e->deltaY);

		return EM_TRUE;
	};


	static EM_BOOL f_touch(int event_type, const EmscriptenTouchEvent *e, void *user_data)
	{
		ui_event_impl * ui = reinterpret_cast<ui_event_impl *>(user_data);

		for (const auto& t : e->touches)
		{
			if (t.isChanged)
			{
				switch (event_type)
				{
					case EMSCRIPTEN_EVENT_TOUCHSTART:	ui->touch(t.identifier, TouchEventDown, t.clientX, t.clientY); break;
					case EMSCRIPTEN_EVENT_TOUCHEND:		ui->touch(t.identifier, TouchEventUp, t.clientX, t.clientY); break;
					case EMSCRIPTEN_EVENT_TOUCHMOVE:	 ui->touch(t.identifier, TouchEventMove, t.clientX, t.clientY); break;
					case EMSCRIPTEN_EVENT_TOUCHCANCEL: ui->touch(t.identifier, TouchEventCancel, t.clientX, t.clientY); break;
				}
			}
		}

		return EM_TRUE;
	};



	void init()
	{
		// event: window_resize

		emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_window_size);


		// event: window_focus

		emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_window_focus);

		// these aren't working...?
		//emscripten_set_focusin_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_window_focus);
		//emscripten_set_focusout_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_window_focus);


		// event: orientation

		emscripten_set_orientationchange_callback(this, 1, f_orientation);


		// event: key

		emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_key);
		emscripten_set_keydown_callback (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_key);
		emscripten_set_keyup_callback	 (EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_key);


		// event: mouse_position

		emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);

		// event: mouse_over

		emscripten_set_mouseover_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);
		emscripten_set_mouseout_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);

		// event: mouse_button

		emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);
		emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);
		emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);
		emscripten_set_dblclick_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_mouse_position);


		// event: scroll_wheel

		emscripten_set_wheel_callback(canvas_.c_str(), this, 1, f_scroll_wheel);


		// event: touch

		emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_touch);
		emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_touch);
		emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_touch);
		emscripten_set_touchcancel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, f_touch);
	};


}; // class ui_event_impl
