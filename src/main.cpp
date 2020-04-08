#define GL_GLEXT_PROTOTYPES

#include <cmath>

#ifdef __EMSCRIPTEN__
	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>

	#include <GLES3/gl3.h>

	#include "libs/ui_event_impl_web.hpp"
#else
	#define GLFW_INCLUDE_GLEXT
	#include <GLFW/glfw3.h>

	#include <GLES3/gl3.h>

	#include "libs/ui_event_impl_glfw.hpp"
#endif


#include "image.hpp"


ui_event_interface * ui__ = nullptr;

int ctx_{0};

int viewport_width_{0};
int viewport_height_{0};

int pixel_width_{0};
int pixel_height_{0};

double pixel_ratio_{1};

float iter1{1};
float iter2{1};
float iter3{1};
float iter4{1};
float iter5{0};

float s{0.0};
float r1{0};
float r2{0};
float i1{0};
float i2{0};

float mx{0};
float my{0};

float pattern{1};

james::image* im{nullptr};

float projectionMatrix_[16];

void set_projection_matrix(int width, int height)
{
	projectionMatrix_[0] = 2.0f / width;
	projectionMatrix_[1] = 0;
	projectionMatrix_[2] = 0;
	projectionMatrix_[3] = 0;

	projectionMatrix_[4] = 0;
	projectionMatrix_[5] = 2.0f / height;
	projectionMatrix_[6] = 0;
	projectionMatrix_[7] = 0;

	projectionMatrix_[8] = 0;
	projectionMatrix_[9] = 0;
	projectionMatrix_[10] = -0.8/(float)std::max(width, height);
	projectionMatrix_[11] = -1.0/(float)std::max(width, height);

	projectionMatrix_[12] = -1;
	projectionMatrix_[13] = -1;
	projectionMatrix_[14] = 0.3; //0.6;	// 0.5
	projectionMatrix_[15] = 1;
}


class ui_event : public ui_event_impl
{

public:

	ui_event(bool debug = false) : ui_event_impl(debug) {};


	void window_size(int width, int height)
	{
		viewport_width_ = width;
		viewport_height_ = height;

		pixel_width_	= round(viewport_width_	* pixel_ratio_);
		pixel_height_ = round(viewport_height_ * pixel_ratio_);

		glViewport(0, 0, pixel_width_, pixel_height_);
		set_projection_matrix(pixel_width_, pixel_height_);

		im->set_size(pixel_width_, pixel_height_);
		im->update(r1, r2, i1, i2);

		#ifdef __EMSCRIPTEN__
		emscripten_set_canvas_element_size("canvas", pixel_width_, pixel_height_);
		#endif
	};


	void key(int key, enum KeyEvent event, enum KeyMod mods)
	{
		switch (event)
		{
			case KeyEventPress:
			{
				if (key == 'W')
				{
					i1 += 0.2;
					i2 += 0.2;
				}
				break;
			}

			case KeyEventDown:
			{
				if (key == 'W')
				{
					auto ritemp = (i1 + i2) / 2;
					i1 -= 0.18*(i1 - ritemp);
					i2 += 0.18*(i2 - ritemp);
					//i1 += 0.2;
					//i2 += 0.2;
				} else if (key == 'S')
				{
					auto ritemp = (i1 + i2) / 2;
					i1 += 0.18*(i1 - ritemp);
					i2 -= 0.18*(i2 - ritemp);
				} else if (key == 'A')
				{
					auto ritemp = (r1 + r2) / 2;
					r1 += 0.18*(r1 - ritemp);
					r2 -= 0.18*(r2 - ritemp);
				} else if (key == 'D')
				{
					auto ritemp = (r1 + r2) / 2;
					r1 -= 0.18*(r1 - ritemp);
					r2 += 0.18*(r2 - ritemp);
				} else if (key == 'M')
				{
					iter5 = 1;
				} else if (key == 'F')
				{
					if (s <= -0.0008) {
						s /= 1.5;
					} else if (s < 0) {
						s = 0;
					} else if (s != 0) {
						s *= 1.5;
					} else {
						s = 0.001;
					}
				} else if (key == 'G')
				{
					if (s >= 0.0008) {
						s /= 1.5;
					} else if (s > 0) {
						s = 0;
					} else if (s != 0) {
						s *= 1.5;
					} else {
						s = -0.001;
					}
				} else if (key == 'P')
				{
					pattern = 1;
				} else if (key == 'O')
				{
					pattern = 0;
				}
				break;
			}

			default: break;
		}
	};


	void mouse_position(double x_pos, double y_pos)
	{
		mx = ((float)viewport_width_ - x_pos) / (float)viewport_width_;
		my = y_pos / (float)viewport_height_;
		iter3 = r1 * mx + r2 * (1 - mx);
		iter4 = i1 * my + i2 * (1 - my);
	};


	void mouse_button(enum MouseButton button, enum MouseButtonEvent event, enum KeyMod mods)
	{
		if (button == MouseButtonLeft)
		{
			switch (event)
			{
				case MouseButtonEventDown:
				{
					if (s == 0)
					{
						r1 = -2.75;
						r2 = 1.5;
						i1 = -1.5;
						i2 = 1.25;

						iter1 = 1;
						iter2 = 1;
						iter3 = 0;
						iter4 = 0;
						iter5 = 0;
					}
					s = 0.0;
					break;
				}

				default: break;
			}
		}

		if (button == MouseButtonRight && event == MouseButtonEventDown) to_fullscreen();
	};


	void scroll_wheel(double x_delta, double y_delta)
	{
		if (y_delta < 0)
		{
			if (s <= -0.0008) {
				s /= 1.25;
			} else if (s < 0) {
				s = 0;
			} else if (s != 0) {
				s *= 1.25;
			} else {
				s = 0.001;
			}
		} else {
			if (s >= 0.0008) {
				s /= 1.25;
			} else if (s > 0) {
				s = 0;
			} else if (s != 0) {
				s *= 1.25;
			} else {
				s = -0.001;
			}
		}
	};


	void touch(int id, enum TouchEvent event, int x, int y)
	{
		if (event == TouchEventDown && id == 2)
		{
			printf("got touch down from id: %d\n", id);
			to_fullscreen();
		}
	};

}; // class inputs


ui_event * ui_event_ = nullptr;


void generate_frame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// james's draw
	im->display(0, 0, iter1++, iter2++);
	/*
	if (iter1 == 200) {
		iter2++;
		iter1 = 0;
	}*/
	/*
	r1 *= (1-s);
	r2 *= (1 - s);
	i1 *= (1 - s);
	i2 *= (1 - s);*/

	//ritemp = r1 + (-r1 * (1 - mx) + r2 * mx) / 2;

	auto ritemp = r1 * mx + r2 * (1 - mx);

	r1 -= s * (r1 - ritemp);
	r2 -= s * (r2 - ritemp);

	ritemp = i1 * my + i2 * (1 - my);
	i1 -= s * (i1 - ritemp);
	i2 -= s * (i2 - ritemp);

	im->update(r1, r2, i1, i2);
}


const char * quit(int eventType, const void *reserved, void *userData)
{
	if (ui_event_)
	{
		delete ui_event_;
		ui_event_ = nullptr;
	}

	return "";
}

 
int main(int argc, char *argv[])
{

	pixel_width_ = 3000;
	pixel_height_ = 1800;
	viewport_width_ = pixel_width_;
	viewport_height_ = pixel_height_;
	pixel_ratio_ = 1.0;

#ifdef __EMSCRIPTEN__

	double w, h;
	EMSCRIPTEN_RESULT r = emscripten_get_element_css_size("canvas", &w, &h);

	viewport_width_ = (int)w;
	viewport_height_ = (int)h;

	pixel_ratio_ = emscripten_get_device_pixel_ratio();

	pixel_width_	= round(w * pixel_ratio_);
	pixel_height_ = round(h * pixel_ratio_);

	emscripten_set_canvas_element_size("canvas", pixel_width_, pixel_height_);

	EmscriptenWebGLContextAttributes ctxAttrs;
	emscripten_webgl_init_context_attributes(&ctxAttrs);

	ctxAttrs.antialias = true;

	// try and create a webgl2 context first

	ctxAttrs.majorVersion = 2;
	ctxAttrs.minorVersion = 0;

	ctx_ = emscripten_webgl_create_context("canvas", &ctxAttrs);

	if (ctx_ <= 0) // unbale to create w webgl2 context so try a webgl1 context
	{
		ctxAttrs.majorVersion = 1;
		ctx_ = emscripten_webgl_create_context("canvas", &ctxAttrs);
	}

	emscripten_webgl_make_context_current(ctx_);

	printf("webgl context version: %s\n", glGetString(GL_VERSION));

	emscripten_set_beforeunload_callback(nullptr, quit);

	ui_event_ = new ui_event(true);

	ui_event_->set_context(ui_event::ContextWebGL, (float)ctxAttrs.majorVersion);
	ui_event_->start("canvas");

	ui__ = ui_event_;

#else

	if (!glfwInit()) {
		fprintf(stderr, "Error: GLFW Initialization failed.");
		return -1;
	}

	// used opengl 3.1 in order to be able to use webgl1 shaders..

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Create the display window

	GLFWwindow* window = glfwCreateWindow(pixel_width_, pixel_height_, "fractal1", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Error: GLFW Window Creation Failed");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLint major = 0, minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	ui_event i(true);

	// this pretends to be a webgl 1 or 2 instance
	i.set_context(ui_event::ContextWebGL, 2.0);

	i.start(window);

	ui__ = &i;

#endif

	set_projection_matrix(pixel_width_, pixel_height_);
	glViewport(0, 0, pixel_width_, pixel_height_);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// create an image object (pushed data to the gpu)
	im = new james::image();

	im->load(nullptr, pixel_width_, pixel_height_);

	r1 = -2.75;
	r2 = 1.5;
	i1 = -1.5;
	i2 = 1.25;

	iter1 = 1;
	iter2 = 1;
	iter3 = 0;
	iter4 = 0;
	iter5 = 0;

#ifdef __EMSCRIPTEN__

	emscripten_set_main_loop(generate_frame, 0, 0);

#else

	while (!glfwWindowShouldClose(window))
	{
		generate_frame();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();

#endif

	return 0;
};

