#pragma once


#include <string>


class ui_event_interface
{

public:


	ui_event_interface(bool debug_enabled = false) :
		debug_(debug_enabled)
	{
	};


	// For keyboard events

	enum KeyMod
	{
		KeyModShift		 = 1<<0,
		KeyModControl	 = 1<<1,
		KeyModAlt			 = 1<<2,
		KeyModSuper		 = 1<<3,
		KeyModCaps_Lock = 1<<4,
		KeyModNum_lock	= 1<<5
	};

	enum KeyEvent
	{
		KeyEventDown			= 1<<0,
		KeyEventUp				= 1<<1,
		KeyEventRepeat		= 1<<2,
		KeyEventPress		 = 1<<3
	};

	// For mouse events

	enum MouseButton
	{
		MouseButtonLeft		 = 1<<0,
		MouseButtonMiddle	 = 1<<1,
		MouseButtonRight		= 1<<2,
		MouseButtonBack		 = 1<<3,
		MouseButtonForward	= 1<<4
	};

	enum MouseButtonEvent
	{
		MouseButtonEventDown				= 1<<0,
		MouseButtonEventUp					= 1<<1,
		MouseButtonEventClick			 = 1<<2,
		MouseButtonEventDoubleClick = 1<<3
	};

	// For touch events

	enum TouchEvent
	{
		TouchEventDown	 = 1<<0,
		TouchEventUp		 = 1<<1,
		TouchEventMove	 = 1<<2,
		TouchEventCancel = 1<<3
	};

	// For device orientation

	enum Orientation
	{
		PortraitPrimary    = 1<<0,
		PortraitSecondary  = 1<<1,
		LandscapePrimary   = 1<<2,
		LandscapeSecondary = 1<<3
	};

	// For gpu context version

	enum Context
	{
		ContextNone        = 0,

		ContextWebGL       = 1<<0,		// web
		ContextWebGPU      = 1<<1,

		ContextOpenGL      = 1<<2,    // linux
		ContextVulkan      = 1<<3,

		ContextMetal       = 1<<4,    // apple

		ContextDirectx     = 1<<5     // microsoft
	};


	// setters

	virtual void set_context(enum Context context, float version)
	{
		if (debug_) printf("set_context(context: %d version: %f)\n", context, version);

		context_ = context;
		version_ = version;
	};


	virtual void to_fullscreen()
	{
		if (debug_) printf("to_fullscreen()\n");
	};


	// getters

  virtual void get_context(enum Context& context, float& version)
	{
		context = context_;
		version = version_;
	};


	// callbacks

	virtual void window_size(int width, int height)
	{
		if (debug_) printf("window_size(width: %d height: %d)\n", width, height);
	};


	virtual void window_focus(bool focus)
	{
		if (debug_) printf("window_focus(focus: %s)\n", focus ? "true" : "false");
	};


	virtual void window_iconified(bool iconified)
	{
		if (debug_) printf("window_iconified(iconified: %s)\n", iconified ? "true" : "false");
	};


	virtual void orientation(enum Orientation orientation)
	{
		if (debug_) printf("orientation(orientation: %d)\n", orientation);
	};


	virtual void key(int key, enum KeyEvent event, enum KeyMod mods)
	{
		if (debug_) printf("key(key: %d event: %d mods: %d)\n", key, event, mods);
	};


	virtual void mouse_position(double x_pos, double y_pos)
	{
		if (debug_) printf("mouse_position(x_pos: %f y_pos: %f)\n", x_pos, y_pos);
	};


	virtual void mouse_over(bool over)
	{
		if (debug_) printf("mouse_over(over: %s)\n", over ? "true" : "false");
	};


	virtual void mouse_button(enum MouseButton button, enum MouseButtonEvent event, enum KeyMod mods)
	{
		if (debug_) printf("mouse_button(button: %d event: %d mods: %d)\n", button, event, mods);
	};


	virtual void scroll_wheel(double x_delta, double y_delta)
	{
		if (debug_) printf("scroll_wheel(x_delta: %f y_delta: %f)\n", x_delta, y_delta);
	};


	virtual void touch(int id, enum TouchEvent event, int x, int y)
	{
		if (debug_) printf("touch(id: %d event: %d, x: %d y: %d)\n", id, event, x, y);
	};


private:


	bool debug_ = false;

	enum Context context_ = ContextNone;
	float version_ = 0;

}; // class ui_event_interface
