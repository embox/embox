/*
  Simple DirectMedia Layer
  Copyright (C) 2017 BlackBerry Limited

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#define SDL_VIDEO_VULKAN 0
#define SDL_VIDEO_OPENGL_EGL 0
#define SDL_VIDEO_OPENGL_ES 0

#define MESA_EGL_NO_X11_HEADERS

#include "SDL_egl.h"
#include "SDL_sysvideo.h"
#include "SDL_internal.h"

#include <drivers/video/fb.h>
#include <sys/mman.h>
#include <GL/osmesa.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <drivers/video/fb.h>
#include <drivers/video/fb_overlay.h>
#include "get.h"

#include <util/log.h>

/**
 * Enumerates the supported EGL configurations and chooses a suitable one.
 * @param[out]  pconf   The chosen configuration
 * @param[out]  pformat The chosen pixel format
 * @return 0 if successful, -1 on error
 */
int glGetConfig(EGLConfig *pconf, int *pformat) {
	log_debug("is a stub");
	return 0;
}

/**
 * Initializes the EGL library.
 * @param   _THIS
 * @param   name    unused
 * @return  0 if successful, -1 on error
 */
int glLoadLibrary(_THIS, const char *name) {
	log_debug("is a stub");
	return 0;
}

extern void glUnlockArraysEXT(void);
extern void glLockArraysEXT(void);

static struct {
	char *proc;
	void *fn;
} embox_sdl_tbl[] = {
	{ "glArrayElement",		glArrayElement },
	{ "glBindTexture",		glBindTexture },
	{ "glBlendFunc",		glBlendFunc },
	{ "glBegin",			glBegin },
	{ "glClear",			glClear },
	{ "glClearColor",		glClearColor },
	{ "glClearDepth",		glClearDepth },
	{ "glClearStencil",		glClearStencil },
	{ "glClipPlane",		glClipPlane },
	{ "glColor3f",			glColor3f },
	{ "glColor4f",			glColor4f },
	{ "glColor4ubv",		glColor4ubv },
	{ "glColorPointer",		glColorPointer },
	{ "glColorMask",		glColorMask },
	{ "glCopyTexSubImage2D",	glCopyTexSubImage2D },
	{ "glCullFace",			glCullFace },
	{ "glDeleteTextures",		glDeleteTextures },
	{ "glDepthFunc",		glDepthFunc },
	{ "glDepthMask",		glDepthMask },
	{ "glDepthRange",		glDepthRange },
	{ "glDisable",			glDisable },
	{ "glDrawArrays",		glDrawArrays },
	{ "glDrawBuffer",		glDrawBuffer },
	{ "glDrawElements",		glDrawElements },
	{ "glEnable",			glEnable },
	{ "glFinish",			glFinish },
	{ "glGenTextures",		glGenTextures },
	{ "glGetBooleanv",		glGetBooleanv },
	{ "glGetError",			glGetError },
	{ "glGetIntegerv",		glGetIntegerv },
	{ "glGetString",		_mesa_GetString },
	{ "glPolygonMode",		glPolygonMode },
	{ "glShadeModel",		glShadeModel },
	{ "glTexEnvf",			glTexEnvf },
	{ "glTexCoordPointer",		glTexCoordPointer },
	{ "glTexCoord2f",		glTexCoord2f },
	{ "glTexCoord2fv",		glTexCoord2fv },
	{ "glLineWidth",		glLineWidth },
	{ "glLoadIdentity",		glLoadIdentity },
	{ "glLoadMatrixf",		glLoadMatrixf },
	{ "glMatrixMode",		glMatrixMode },
	{ "glPolygonOffset",		glPolygonOffset },
	{ "glPopMatrix",		glPopMatrix },
	{ "glPushMatrix",		glPushMatrix },
	{ "glReadPixels",		glReadPixels },
	{ "glScissor",			glScissor },
	{ "glStencilFunc",		glStencilFunc },
	{ "glStencilMask",		glStencilMask },
	{ "glStencilOp",		glStencilOp },
	{ "glTexImage2D",		glTexImage2D },
	{ "glTexParameteri",		glTexParameteri },
	{ "glTexParameterf",		glTexParameterf },
	{ "glTexSubImage2D",		glTexSubImage2D },
	{ "glTranslatef",		glTranslatef },
	{ "glVertex2f",			glVertex2f },
	{ "glVertex3f",			glVertex3f },
	{ "glVertex3fv",		glVertex3fv },
	{ "glVertexPointer",		glVertexPointer },
	{ "glViewport",			glViewport },

	{ "glEnableClientState",	glEnableClientState },
	{ "glDisableClientState",	glDisableClientState },
	{ "glAlphaFunc",		glAlphaFunc },
	{ "glEnd",			glEnd },
	{ "glOrtho",			glOrtho },
	{ "glFrustum",			glFrustum },
	{ "glFlush",			glFlush },
	{ "glActiveTextureARB",		glActiveTextureARB },
	{ "glClientActiveTextureARB",	glClientActiveTextureARB },
	{ "glMultiTexCoord2fARB",	glMultiTexCoord2fARB },
	{ "glUnlockArraysEXT",		glUnlockArraysEXT },
	{ "glLockArraysEXT",		glLockArraysEXT },
	{ 0 },
};

/**
 * Finds the address of an EGL extension function.
 * @param   proc    Function name
 * @return  Function address
 */
void *glGetProcAddress(_THIS, const char *proc) {
	for (int i = 0; embox_sdl_tbl[i].proc != 0; i++) {
		if (!strcmp(embox_sdl_tbl[i].proc, proc)) {
			return embox_sdl_tbl[i].fn;
		}
	}

	printf("embox/sdl: Failed to find %s\n", proc);
	return 0; //eglGetProcAddress(proc);
}

static int display_width;
static int display_height;

static void *hw_base = 0;
static void *sw_base = 0;
static void *res_base = 0;

static void sdl_init_buffers(struct fb_info *mesa_fbi, int xres, int yres) {
	long int screensize = 0;
	struct fb_var_screeninfo var;

	fb_get_var(mesa_fbi, &var);

	var.xres           = xres,
	var.yres           = yres,
	var.xres_virtual   = xres,
	var.yres_virtual   = yres,
	var.xoffset = 0,
	var.yoffset = 0,

	fb_set_var(mesa_fbi, &var);

	printf("%dx%d, %dbpp\n", mesa_fbi->var.xres, mesa_fbi->var.yres,
			mesa_fbi->var.bits_per_pixel);

	display_width = mesa_fbi->var.xres;
	display_height = mesa_fbi->var.yres;

	screensize = display_width * display_height * mesa_fbi->var.bits_per_pixel / 8;

	/* Map the device to memory */
	hw_base = (void *)mesa_fbi->screen_base;
	// hw_base = mmap_device_memory((void *)mesa_fbi->screen_base, screensize,
	//     PROT_READ | PROT_WRITE, MAP_SHARED, (uintptr_t)mesa_fbi->screen_base);

	if ((int) hw_base == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}

	printf("The framebuffer device was mapped to memory successfully.\n");

	sw_base = malloc(display_width * display_height * mesa_fbi->var.bits_per_pixel / 8);
	res_base = malloc(display_width * display_height * mesa_fbi->var.bits_per_pixel / 8);

	fb_overlay_init(mesa_fbi, res_base);
}

/**
 * Associates the given window with the necessary EGL structures for drawing and
 * displaying content.
 * @param   _THIS
 * @param   window  The SDL window to create the context for
 * @return  A pointer to the created context, if successful, NULL on error
 */
SDL_GLContext glCreateContext(_THIS, SDL_Window *window) {
	OSMesaContext ctx;
	struct fb_info *mesa_fbi;

	mesa_fbi = fb_lookup(0);
	sdl_init_buffers(mesa_fbi, window->fullscreen_mode.w, window->fullscreen_mode.h);

	GLenum format = OSMESA_BGRA, type = GL_UNSIGNED_BYTE;
	ctx = OSMesaCreateContextExt( format, 16, 0, 0, NULL );
	if (!ctx) {
		printf("OSMesaCreateContext failed!\n");
		return 0;
	}

	/* Bind the buffer to the context and make it current */
	if (!OSMesaMakeCurrent(ctx, sw_base, type, display_width, display_height)) {
		printf("OSMesaMakeCurrent failed!\n");
		return 0;
	}

	return ctx;
}

/**
 * Sets a new value for the number of frames to display before swapping buffers.
 * @param   _THIS
 * @param   interval    New interval value
 * @return  0 if successful, -1 on error
 */
int glSetSwapInterval(_THIS, int interval) {
	log_debug("is a stub");
	return 0;
}

/**
 * Swaps the EGL buffers associated with the given window
 * @param   _THIS
 * @param   window  Window to swap buffers for
 * @return  0 if successful, -1 on error
 */
int glSwapWindow(_THIS, SDL_Window *window) {
	for (int i = 0; i < display_height; i++) {
		memcpy(res_base + i * display_width * 4, sw_base + (display_height - i) * 4 * display_width, display_width * 4);
	}

	fb_overlay_put_string(0, 0, "Embox OS Quake3 Demo");

	memcpy(hw_base, res_base, 4 * display_width * display_height);

	return 0;
}

/**
 * Makes the given context the current one for drawing operations.
 * @param   _THIS
 * @param   window  SDL window associated with the context (maybe NULL)
 * @param   context The context to activate
 * @return  0 if successful, -1 on error
 */
int glMakeCurrent(_THIS, SDL_Window *window, SDL_GLContext context) {
	log_debug("is a stub");
	return 0;
}

/**
 * Destroys a context.
 * @param   _THIS
 * @param   context The context to destroy
 */
void glDeleteContext(_THIS, SDL_GLContext context) {
	log_debug("is a stub");
}

/**
 * Terminates access to the EGL library.
 * @param   _THIS
 */
void glUnloadLibrary(_THIS) {
	log_debug("is a stub");
}
