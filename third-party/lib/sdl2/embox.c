/**
 * @file embox.c
 * @brief Video bootstrap for SDL2
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.10.2018
 */

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <drivers/video/fb.h>
#include <drivers/keyboard.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/keymap.h>
#include <util/log.h>

#include <GL/osmesa.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define MESA_EGL_NO_X11_HEADERS
#define SDL_VIDEO_VULKAN 0
#define SDL_VIDEO_OPENGL_EGL 0
#define SDL_VIDEO_OPENGL_ES 0

#include "SDL_egl.h"
#include "../../SDL_internal.h"
#include "SDL_scancode.h"
#include "../SDL_sysvideo.h"
#include "../src/events/SDL_keyboard_c.h"
#include "../src/events/SDL_mouse_c.h"

#include <drivers/video/fb.h>

extern int glGetConfig(EGLConfig *pconf, int *pformat);
extern int glLoadLibrary(_THIS, const char *name);
void *glGetProcAddress(_THIS, const char *proc);
extern SDL_GLContext glCreateContext(_THIS, SDL_Window *window);
extern int glSetSwapInterval(_THIS, int interval);
extern int glSwapWindow(_THIS, SDL_Window *window);
extern int glMakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context);
extern void glDeleteContext(_THIS, SDL_GLContext context);
extern void glUnloadLibrary(_THIS);

typedef struct {
    int window;
    EGLSurface      surface;
    EGLConfig       conf;
} window_impl_t;

/**
 * Initializes the QNX video plugin.
 * Creates the Screen context and event handles used for all window operations
 * by the plugin.
 * @param   _THIS
 * @return  0 if successful, -1 on error
 */
static SDL_DisplayMode modes = { 0x17101803, 800, 600, 0};
static int videoInit(_THIS) {
    SDL_VideoDisplay display;

    SDL_zero(display);

    display.num_display_modes = 1;
    display.display_modes = &modes;
    if (SDL_AddVideoDisplay(&display) < 0) {
        return -1;
    }

    _this->num_displays = 1;
    return 0;
}

static void videoQuit(_THIS) {
	log_debug("is a stub");
}

/**
 * Creates a new native Screen window and associates it with the given SDL
 * window.
 * @param   _THIS
 * @param   window  SDL window to initialize
 * @return  0 if successful, -1 on error
 */
static int createWindow(_THIS, SDL_Window *window) {
	log_debug("is a stub");

	SDL_SetMouseFocus(window);

	window->flags |= SDL_WINDOW_INPUT_FOCUS;
	window->flags |= SDL_WINDOW_MOUSE_FOCUS;

	return 0;
}

/**
 * Gets a pointer to the Screen buffer associated with the given window. Note
 * that the buffer is actually created in createWindow().
 * @param       _THIS
 * @param       window  SDL window to get the buffer for
 * @param[out]  pixles  Holds a pointer to the window's buffer
 * @param[out]  format  Holds the pixel format for the buffer
 * @param[out]  pitch   Holds the number of bytes per line
 * @return  0 if successful, -1 on error
 */
static int
createWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format,
                        void ** pixels, int *pitch) {
	log_debug("is a stub");
	return 0;
}

/**
 * Informs the window manager that the window needs to be updated.
 * @param   _THIS
 * @param   window      The window to update
 * @param   rects       An array of reectangular areas to update
 * @param   numrects    Rect array length
 * @return  0 if successful, -1 on error
 */
static int updateWindowFramebuffer(_THIS, SDL_Window *window, const SDL_Rect *rects,
                        int numrects) {
	log_debug("is a stub");
	return 0;
}

static int key_to_sdl[] = {
	[' '] = SDL_SCANCODE_SPACE,
	['\''] = SDL_SCANCODE_APOSTROPHE,
	[','] = SDL_SCANCODE_COMMA,
	['-'] = SDL_SCANCODE_MINUS,
	['.'] = SDL_SCANCODE_PERIOD,
	['/'] = SDL_SCANCODE_SLASH,
	['0'] = SDL_SCANCODE_0,
	['1'] = SDL_SCANCODE_1,
	['2'] = SDL_SCANCODE_2,
	['3'] = SDL_SCANCODE_3,
	['4'] = SDL_SCANCODE_4,
	['5'] = SDL_SCANCODE_5,
	['6'] = SDL_SCANCODE_6,
	['7'] = SDL_SCANCODE_7,
	['8'] = SDL_SCANCODE_8,
	['9'] = SDL_SCANCODE_9,
	[';'] = SDL_SCANCODE_SEMICOLON,
	['='] = SDL_SCANCODE_EQUALS,
	['\\'] = SDL_SCANCODE_BACKSLASH,
	['a'] = SDL_SCANCODE_A,
	['b'] = SDL_SCANCODE_B,
	['c'] = SDL_SCANCODE_C,
	['d'] = SDL_SCANCODE_D,
	['e'] = SDL_SCANCODE_E,
	['f'] = SDL_SCANCODE_F,
	['g'] = SDL_SCANCODE_G,
	['h'] = SDL_SCANCODE_H,
	['i'] = SDL_SCANCODE_I,
	['j'] = SDL_SCANCODE_J,
	['k'] = SDL_SCANCODE_K,
	['l'] = SDL_SCANCODE_L,
	['m'] = SDL_SCANCODE_M,
	['n'] = SDL_SCANCODE_N,
	['o'] = SDL_SCANCODE_O,
	['p'] = SDL_SCANCODE_P,
	['q'] = SDL_SCANCODE_Q,
	['r'] = SDL_SCANCODE_R,
	['s'] = SDL_SCANCODE_S,
	['t'] = SDL_SCANCODE_T,
	['u'] = SDL_SCANCODE_U,
	['v'] = SDL_SCANCODE_V,
	['w'] = SDL_SCANCODE_W,
	['x'] = SDL_SCANCODE_X,
	['y'] = SDL_SCANCODE_Y,
	['z'] = SDL_SCANCODE_Z,
	[KEY_UP] = SDL_SCANCODE_UP,
	[KEY_DOWN] = SDL_SCANCODE_DOWN,
	[KEY_LEFT] = SDL_SCANCODE_LEFT,
	[KEY_PGUP] = SDL_SCANCODE_PAGEUP,
	[KEY_PGDN] = SDL_SCANCODE_PAGEDOWN,
	[KEY_RGHT] = SDL_SCANCODE_RIGHT,
	['\r'] = SDL_SCANCODE_RETURN,
	[27] = SDL_SCANCODE_ESCAPE,
};

/**
 * Runs the main event loop.
 * @param   _THIS
 */

#define EVENT_NUM 1024

static struct input_event event_queue[EVENT_NUM];
static int cur_evt = 0, next_evt = 0;
static void pumpEvents(_THIS) {
	struct input_event *event;
	SDL_Scancode    scancode = 0;
	int keycode;


	while (cur_evt != next_evt) {
		event = &event_queue[cur_evt % EVENT_NUM];
		cur_evt++;

		log_debug("process %d\n", cur_evt - 1);
		if (event->devtype == INPUT_DEV_KBD) {
			keycode = keymap_kbd(event);
			if (keycode < 0) {
				return;
			}

			scancode = key_to_sdl[keycode];
			log_debug("scancode is %d\n", scancode);

			if (event->type & KEY_PRESSED) {
				SDL_SendKeyboardKey(SDL_PRESSED, scancode);
			} else {
				SDL_SendKeyboardKey(SDL_RELEASED, scancode);
			}
		} else if (event->devtype == INPUT_DEV_MOUSE) {
			if (event->type == 1) {
				/* Left putton press */
				log_debug("Send mouse left press");
				SDL_SendMouseButton(0, SDL_TOUCH_MOUSEID * 0, SDL_PRESSED, 1);
			} else if (event->type == 0 && event->value == 0) {
				/* Left button release */
				log_debug("Send mouse left release");
				SDL_SendMouseButton(0, SDL_TOUCH_MOUSEID * 0, SDL_RELEASED, 1);
			} else {
				/* Mouse motion */
				int16_t dx = (event->value >> 16) & 0xFFFF;
				int16_t dy = (event->value) & 0xFFFF;
				log_debug("Send mouse motion %d %d", dx, dy);
				SDL_SendMouseMotion(0, 0, 1, dx, -dy);
			}
		}

		event->devtype = 0xffff;
	}
}

/**
 * Updates the size of the native window using the geometry of the SDL window.
 * @param   _THIS
 * @param   window  SDL window to update
 */
static void setWindowSize(_THIS, SDL_Window *window) {
	log_debug("is a stub");
}

/**
 * Makes the native window associated with the given SDL window visible.
 * @param   _THIS
 * @param   window  SDL window to update
 */
static void showWindow(_THIS, SDL_Window *window) {
	log_debug("is a stub");
}

/**
 * Makes the native window associated with the given SDL window invisible.
 * @param   _THIS
 * @param   window  SDL window to update
 */
static void hideWindow(_THIS, SDL_Window *window) {
	log_debug("is a stub");
}

/**
 * Destroys the native window associated with the given SDL window.
 * @param   _THIS
 * @param   window  SDL window that is being destroyed
 */
static void destroyWindow(_THIS, SDL_Window *window) {
	log_debug("is a stub");
}

/**
 * Frees the plugin object created by createDevice().
 * @param   device  Plugin object to free
 */
static void deleteDevice(SDL_VideoDevice *device) {
	SDL_free(device);
}

static int sdl_indev_eventhnd(struct input_dev *indev) {
	struct input_event event;

	while (0 == input_dev_event(indev, &event)) {
		log_debug("%s event #%d %x %x", indev->name, next_evt, event.type, event.value);
		memcpy(&event_queue[next_evt % EVENT_NUM], &event, sizeof(struct input_event));
		next_evt++;
	}

	return 0;
}

static SDL_Cursor * embox_CreateCursor(SDL_Surface * surface, int hot_x, int hot_y) {
	log_debug("is a stub");
	return 0;
}

static SDL_Cursor * embox_CreateDefaultCursor(void) {
	log_debug("is a stub");
	return 0;
}

static int embox_ShowCursor(SDL_Cursor * cursor) {
	log_debug("is a stub");
	return 0;
}

static void embox_FreeCursor(SDL_Cursor * cursor) {
	log_debug("is a stub");
}

static void embox_MoveCursor(SDL_Cursor * cursor) {
	log_debug("is a stub");
}

/**
 * Creates the QNX video plugin used by SDL.
 * @param   devindex    Unused
 * @return  Initialized device if successful, NULL otherwise
 */
static SDL_VideoDevice *createDevice(int devindex) {
	SDL_VideoDevice *device;

	device = (SDL_VideoDevice *)SDL_calloc(1, sizeof(SDL_VideoDevice));
	if (device == NULL) {
		return NULL;
	}

	device->driverdata = NULL;
	device->VideoInit = videoInit;
	device->VideoQuit = videoQuit;
	device->CreateSDLWindow = createWindow;
	device->CreateWindowFramebuffer = createWindowFramebuffer;
	device->UpdateWindowFramebuffer = updateWindowFramebuffer;
	device->SetWindowSize = setWindowSize;
	device->ShowWindow = showWindow;
	device->HideWindow = hideWindow;
	device->PumpEvents = pumpEvents;
	device->DestroyWindow = destroyWindow;

	device->GL_LoadLibrary = glLoadLibrary;
	device->GL_GetProcAddress = glGetProcAddress;
	device->GL_CreateContext = glCreateContext;
	device->GL_SetSwapInterval = glSetSwapInterval;
	device->GL_SwapWindow = glSwapWindow;
	device->GL_MakeCurrent = glMakeCurrent;
	device->GL_DeleteContext = glDeleteContext;
	device->GL_UnloadLibrary = glUnloadLibrary;

	device->free = deleteDevice;

	/* Initialize input */
	struct input_dev *dev;

	dev = input_dev_lookup("keyboard");
	if (dev) {
		input_dev_open(dev, &sdl_indev_eventhnd);
	} else {
		log_error("keyboard not found!");
	}

	dev = input_dev_lookup("mouse");
	if (dev) {
		input_dev_open(dev, &sdl_indev_eventhnd);
	} else {
		log_error("mouse not found!");
	}

	SDL_Mouse *mouse = SDL_GetMouse();

	mouse->CreateCursor = embox_CreateCursor;
	mouse->ShowCursor = embox_ShowCursor;
	mouse->MoveCursor = embox_MoveCursor;
	mouse->FreeCursor = embox_FreeCursor;
	SDL_SetDefaultCursor(embox_CreateDefaultCursor());


	return device;
}

static int available() {
	return 1;
}

VideoBootStrap EMBOX_bootstrap = {
	"embox", "EMBOX Screen",
	available, createDevice
};

#include <pthread.h>
#include <stddef.h>
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}
