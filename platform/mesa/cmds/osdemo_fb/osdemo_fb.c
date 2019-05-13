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
#include <lib/fps.h>

#include <GL/osmesa.h>
#include <GL/glu.h>
#include <GL/gl.h>

extern void cxx_app_startup(void);
extern void fb_overlay_init(struct fb_info *fbi, void *base);

static int Width;
static int Height;
static struct fb_info *mesa_fbi;
static void *sw_base = 0;

static int animated_scene;

void init_buffers(void) {
	mesa_fbi = fb_lookup(0);

	printf("%"PRIu32"x%"PRIu32", %"PRIu32"bpp\n", mesa_fbi->var.xres, mesa_fbi->var.yres,
			mesa_fbi->var.bits_per_pixel);

	Width = mesa_fbi->var.xres;
	Height = mesa_fbi->var.yres;

	printf("The framebuffer device was mapped to memory successfully.\n");

	sw_base = fps_enable_swap(mesa_fbi);
	if (animated_scene) {
		fps_set_format("Embox MESA demo v0.2\nFPS=%2d");
	} else {
		fps_set_format("Embox MESA demo v0.2");
	}
}

static void swap_buffers() {
	fps_print(mesa_fbi);
	fps_swap(mesa_fbi);
}

static void destroy_video_buffer() {

}

/*********************************
 *
 *
 *********************************/

static void Sphere(float radius, int slices, int stacks) {
	GLUquadric *q = gluNewQuadric();
	gluQuadricNormals(q, GLU_SMOOTH);
	gluSphere(q, radius, slices, stacks);
	gluDeleteQuadric(q);
}

static void Cone(float base, float height, int slices, int stacks) {
	GLUquadric *q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	gluCylinder(q, base, 0.0, height, slices, stacks);
	gluDeleteQuadric(q);
}

static void Torus(float innerRadius, float outerRadius, int sides, int rings) {
	/* from GLUT... */
	int i, j;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	const GLfloat ringDelta = 2.0 * M_PI / rings;
	const GLfloat sideDelta = 2.0 * M_PI / sides;

	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	for (i = rings - 1; i >= 0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		glBegin(GL_QUAD_STRIP);
		phi = 0.0;
		for (j = sides; j >= 0; j--) {
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = outerRadius + innerRadius * cosPhi;

			glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, innerRadius * sinPhi);
			glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist,  innerRadius * sinPhi);
		}
		glEnd();
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
	}
}


static void render_image(void) {
	float view_angle = 1.;

	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, -1.0, 1.0, 0.0 };
	GLfloat red_mat[]   = { 1.0, 0.2, 0.2, 1.0 };
	GLfloat green_mat[] = { 0.2, 1.0, 0.2, 1.0 };
	GLfloat blue_mat[]  = { 0.2, 0.2, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.5, 2.5, -2.5, 2.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);

	while (1) {
		view_angle += 1.;
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glPushMatrix();
		glRotatef(-20.0, 1.0, 0.0, 0.0);
		glRotatef(180.0, 0.0, 0.0, 1.0);
		glRotatef(view_angle, 0.0, 1.0, 0.0);

		glPushMatrix();
		glTranslatef(-0.75, 0.5, 0.0);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red_mat );
		Torus(0.275, 0.85, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-0.75, -0.5, 0.0);
		glRotatef(270.0, 1.0, 0.0, 0.0);
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green_mat );
		Cone(1.0, 2.0, 8, 1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.75, 0.0, -1.0);
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue_mat );
		Sphere(1.0, 10, 10);
		glPopMatrix();

		glPopMatrix();

		/* This is very important!!!
		 * Make sure buffered commands are finished!!!
		 */
		glFinish();

		swap_buffers();

		if (!animated_scene)
			return;
	}
}

static void print_help() {
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

	animated_scene = 1;

	getopt_init();
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
