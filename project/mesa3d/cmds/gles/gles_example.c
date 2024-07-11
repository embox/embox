/**
 * @file
 * @brief
 * @version 0.2
 * @date Jun 21, 2017
 * @author Anton Bondarev
 * @author Denis Deryugin <deryugin.denis@gmail.com>
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
#include <drivers/video/fb_overlay.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/osmesa.h>
#include <GL/glcorearb.h>
#include <GL/glu.h>
#include <GL/gl.h>

extern void cxx_app_startup(void);
extern void fb_overlay_init(struct fb_info *fbi, void *base);

static int Width;
static int Height;
static struct fb_info *mesa_fbi;
static void *hw_base = 0;
static void *sw_base = 0;

static int animated_scene = 1;

static const GLchar* vertex_shader_source =
    "attribute vec3 position;\n"
    "void main() {\n"
    "   gl_Position = vec4(position, 1.0);\n"
    "}\n";
static const GLchar* fragment_shader_source =
    "void main() {\n"
    "   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";
static const GLfloat vertices[] = {
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
};

GLint common_get_shader_program(const char *vertex_shader_source, const char *fragment_shader_source) {
    enum Consts {INFOLOG_LEN = 512};
    GLchar infoLog[INFOLOG_LEN];
    GLint fragment_shader;
    GLint shader_program;
    GLint success;
    GLint vertex_shader;

    /* Vertex shader */

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Fragment shader */
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    /* Link shaders */
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glBindAttribLocation(shader_program, 0, "position");

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

void init_buffers(void) {
	long int screensize = 0;

	mesa_fbi = fb_lookup(0);

	printf("%dx%d, %dbpp\n", mesa_fbi->var.xres, mesa_fbi->var.yres,
			mesa_fbi->var.bits_per_pixel);

	Width = mesa_fbi->var.xres;
	Height = mesa_fbi->var.yres;

	screensize = Width * Height * mesa_fbi->var.bits_per_pixel / 8;

	/* Map the device to memory */
	hw_base = (void *)mesa_fbi->screen_base;
	// hw_base = mmap_device_memory((void *)mesa_fbi->screen_base, screensize,
	//     PROT_READ | PROT_WRITE, MAP_SHARED, (uintptr_t)mesa_fbi->screen_base);

	if ((int) hw_base == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}

	printf("The framebuffer device was mapped to memory successfully.\n");

	sw_base = malloc(Width * Height * mesa_fbi->var.bits_per_pixel / 8);
	fb_overlay_init(mesa_fbi, sw_base);
}

static void swap_buffers(void) {
	static struct timespec time_prev;
	struct timespec time;
	static int fps = 0;
	static int counter = 0;

	static char fps_str[] = "FPS=??";

	if (animated_scene) {
		/* Refresh FPS counter every second */
		clock_gettime(CLOCK_REALTIME, &time);

		counter++;

		if (time.tv_sec != time_prev.tv_sec) {
			time_prev = time;
			fps = counter > 99 ? 99 : counter;
			counter = 0;
		}

		fps_str[4] = '0' + fps / 10;
		fps_str[5] = '0' + fps % 10;
		fb_overlay_put_string(0, 0, "Embox MESA demo v0.2");
		fb_overlay_put_string(0, 1, fps_str);
	}

	memcpy(hw_base,
		sw_base,
		mesa_fbi->var.bits_per_pixel / 8 * Width * Height);
}

static void destroy_video_buffer(void) {
	long int screensize = 0;

	/* Figure out the size of the screen in bytes */
	screensize = mesa_fbi->var.xres * mesa_fbi->var.yres
		* mesa_fbi->var.bits_per_pixel / 8;

	munmap(hw_base, screensize);

	free(sw_base);
}

static void render_image(void) {
	GLuint shader_program, vbo;
    GLint pos;
    printf("GL_VERSION  : %s\n", glGetString(GL_VERSION) );
    printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER) );


    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);
    pos = glGetAttribLocation(shader_program, "position");


    glClearColor(0.1, 0.1, 0.1, 1.);
    glViewport(0, 0, Width, Height);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    while(1) {
	    glClear(GL_COLOR_BUFFER_BIT);
	    glUseProgram(shader_program);
	    glDrawArrays(GL_TRIANGLES, 0, 3);
	    swap_buffers();

    }

    swap_buffers();

    return;
}

static void print_help(void) {
	printf("osdemofb: Test MESA 3D graphics.\n"
			"Usage: osdemofb [FLAGS]\n"
			"\t-h -- Print this message\n"
			"\t-s -- Show static scene instead of animation\n"
			"\t-d N -- Use depth N bits (otherwise try to autodetect,"
			"suppose 32 is RGB888 and 16 is RGB565)\n");
}

int main(int argc, char **argv) {
	OSMesaContext ctx;
	GLenum format;
	GLenum type;
	int opt, depth = -1;

	while (-1 != (opt = getopt(argc, argv, "hasd:"))) {
		switch(opt) {
			case 'h':
				print_help();
				return 0;
			case 's':
				animated_scene = 0;
				break;
			case 'd':
				depth = strtol(optarg, NULL, 0);


				printf("depth is %d\n", depth);
				break;
		}
	}

	init_buffers();
	assert(mesa_fbi);
	assert(sw_base);
	assert(hw_base);

	if (depth == -1) {
		/* Argument not given */
		depth = mesa_fbi->var.bits_per_pixel;
	}

	switch (depth) {
		case 16:
			format = OSMESA_RGB_565;
			type = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case 32:
			format = OSMESA_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		default:
			printf("Unsupported color depth: %d."
					"Try 16 or 32\n",
					depth);
			goto out_free;
	}

	cxx_app_startup();
	/* Create an RGBA-mode context */
	ctx = OSMesaCreateContextExt( format, 16, 0, 0, NULL );
	if (!ctx) {
		printf("OSMesaCreateContext failed!\n");
		goto out_free;
	}

	/* Bind the buffer to the context and make it current */
	if (!OSMesaMakeCurrent(ctx, sw_base, type, Width, Height)) {
		printf("OSMesaMakeCurrent failed!\n");
		return 0;
	}

	{
		int z, s, a;
		glGetIntegerv(GL_DEPTH_BITS, &z);
		glGetIntegerv(GL_STENCIL_BITS, &s);
		glGetIntegerv(GL_ACCUM_RED_BITS, &a);
		printf("Depth=%d Stencil=%d Accum=%d\n", z, s, a);
	}

	render_image();
	printf("all done\n");

	/* destroy the context */
	OSMesaDestroyContext(ctx);
out_free:
	destroy_video_buffer();

	return 0;
}
