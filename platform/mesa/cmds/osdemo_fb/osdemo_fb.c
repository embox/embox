/**
 * @file
 * @brief
 *
 * @date Jun 21, 2017
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

#include <GL/osmesa.h>
#include <GL/glu.h>
#include <GL/gl.h>

extern void cxx_app_startup(void);
void *create_video_buffer(void) {
	long int screensize = 0;
	struct fb_info *fb_info;
	uint8_t *fbp = 0;

	fb_info = fb_lookup(0);

	printf("%dx%d, %dbpp\n", fb_info->var.xres, fb_info->var.yres,
			fb_info->var.bits_per_pixel);

	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres
			* fb_info->var.bits_per_pixel / 8;

	/* Map the device to memory */
	fbp = (uint8_t *) mmap_device_memory((void *) fb_info->screen_base,
			screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
			(uint64_t) ((uintptr_t) fb_info->screen_base));
	if ((int) fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	return fbp;
}

void destroy_video_buffer(void *buffer) {
	struct fb_info *fb_info;
	long int screensize = 0;

	fb_info = fb_lookup(0);
	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres
			* fb_info->var.bits_per_pixel / 8;

	munmap(buffer, screensize);
}


/*********************************
 *
 *
 *********************************/
static int Width = 400;
static int Height = 400;


static void
Sphere(float radius, int slices, int stacks)
{
   GLUquadric *q = gluNewQuadric();
   gluQuadricNormals(q, GLU_SMOOTH);
   gluSphere(q, radius, slices, stacks);
   gluDeleteQuadric(q);
}


static void
Cone(float base, float height, int slices, int stacks)
{
   GLUquadric *q = gluNewQuadric();
   gluQuadricDrawStyle(q, GLU_FILL);
   gluQuadricNormals(q, GLU_SMOOTH);
   gluCylinder(q, base, 0.0, height, slices, stacks);
   gluDeleteQuadric(q);
}

static void
Torus(float innerRadius, float outerRadius, int sides, int rings)
{
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


static void
render_image(void)
{
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
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

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glPushMatrix();
   glRotatef(20.0, 1.0, 0.0, 0.0);

   glPushMatrix();
   glTranslatef(-0.75, 0.5, 0.0);
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red_mat );
   Torus(0.275, 0.85, 20, 20);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-0.75, -0.5, 0.0);
   glRotatef(270.0, 1.0, 0.0, 0.0);
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green_mat );
   Cone(1.0, 2.0, 16, 1);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(0.75, 0.0, -1.0);
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue_mat );
   Sphere(1.0, 20, 20);
   glPopMatrix();

   glPopMatrix();

   /* This is very important!!!
    * Make sure buffered commands are finished!!!
    */
   glFinish();
}

int main() {
	OSMesaContext ctx;
	void *buffer;

	cxx_app_startup();
#if 0
	char *filename = NULL;

	if (argc < 2) {
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "  osdemo filename [width height]\n");
		return 0;
	}

	filename = argv[1];
	if (argc == 4) {
		Width = atoi(argv[2]);
		Height = atoi(argv[3]);
	}
#endif
	/* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
	/* specify Z, stencil, accum sizes */
	ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
#else
	ctx = OSMesaCreateContext(OSMESA_RGBA, NULL);
#endif
	if (!ctx) {
		printf("OSMesaCreateContext failed!\n");
		return 0;
	}
#if 0
	/* Allocate the image buffer */
	buffer = malloc(Width * Height * 4 * sizeof(GLubyte));
	if (!buffer) {
		printf("Alloc image buffer failed!\n");
		return 0;
	}
#endif
	buffer = create_video_buffer();
	/* Bind the buffer to the context and make it current */
	if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, Width, Height)) {
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
#if 0
	if (filename != NULL) {
#ifdef SAVE_TARGA
		write_targa(filename, buffer, Width, Height);
#else
		write_ppm(filename, buffer, Width, Height);
#endif
	} else {
		printf("Specify a filename if you want to make an image file\n");
	}
#endif
	printf("all done\n");
#if 0
	/* free the image buffer */
	free(buffer);
#endif
	/* destroy the context */
	OSMesaDestroyContext(ctx);
	destroy_video_buffer(buffer);

	return 0;
}
