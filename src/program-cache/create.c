#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define VERTEX_SHADER \
	"                                            \
attribute vec4 vPosition;                                               \
                                                                        \
void main(void) {                                                       \
   gl_Position = vPosition;                                             \
}                                                                       \
"

#define FRAGMENT_SHADER \
	"                                            \
precision mediump float;                                                \
                                                                        \
void main(void) {                                                       \
   gl_FragColor = vec4(0.0, 0.75, 0.65, 1.0);                           \
}                                                                       \
"

typedef struct _ContextData {
	int width;
	int height;
	GLuint program;
	EGLSurface surface;
	EGLDisplay display;
	EGLContext context;
	EGLConfig config;
} ContextData;

bool setup_shader(ContextData *data, const char *shader_source,
		  GLint shader_type)
{
	GLenum glres;

	GLuint shader = glCreateShader(shader_type);
	glres = glGetError();
	if (shader == 0) {
		printf("%s %x\n", "Failed to create shader. GL error:", glres);
		goto ShaderExit;
	}

	glShaderSource(shader, 1, &shader_source, NULL);
	glres = glGetError();
	if (glres != GL_NO_ERROR) {
		printf("%s %x\n",
		       "Failed to create shader from source. GL error:", glres);
		goto ShaderExitDelete;
	}

	glCompileShader(shader);
	glres = glGetError();
	if (glres != GL_NO_ERROR) {
		printf("%s %x\n", "Failed to compile shader. GL error:", glres);
		goto ShaderExitDelete;
	}

	glAttachShader(data->program, shader);
	glres = glGetError();
	if (glres != GL_NO_ERROR) {
		printf("%s %x\n", "Failed to attach shader. GL error:", glres);
		goto ShaderExitDelete;
	}

ShaderExitDelete:
	/* mark shader to be deleted, it will only be deleted once it is
	 * detached */
	glDeleteShader(shader);
ShaderExit:
	return (glres == GL_NO_ERROR);
}

bool egl_create_surface(ContextData *data)
{
	// clang-format off
	EGLint surface_attribs[] = {
		EGL_WIDTH, data->width,
		EGL_HEIGHT, data->height,
		EGL_NONE,
	};
	// clang-format on

	data->surface = eglCreatePbufferSurface(data->display, data->config,
						surface_attribs);
	return (data->surface != EGL_NO_SURFACE);
}

bool egl_create_context(ContextData *data)
{
	// clang-format off
	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE,
	};
	// clang-format on

	data->context = eglCreateContext(data->display, data->config,
					 EGL_NO_CONTEXT, context_attribs);
	return (data->context != EGL_NO_CONTEXT);
}

bool egl_choose_config(ContextData *data)
{
	EGLint count;

	// clang-format off
	static const EGLint config_attribs[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE,
	};
	// clang-format on

	return eglChooseConfig(data->display, config_attribs, &data->config, 1,
			       &count);
}

bool egl_init(ContextData *data)
{
	/* setup egl for surfaceless rendering */
	data->display = eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA,
					      EGL_DEFAULT_DISPLAY, NULL);
	if (data->display == NULL)
		return false;

	return eglInitialize(data->display, NULL, NULL);
}

int32_t cache(ContextData *data)
{
	GLenum binary_format = 0;
	GLint binary_size = 0;
	void *binary;
	int fd;

	glGetProgramiv(data->program, GL_PROGRAM_BINARY_LENGTH, &binary_size);
	printf("binary_size: %i\n", binary_size);

	binary = malloc(binary_size);
	glGetProgramBinary(data->program, binary_size, NULL, &binary_format, binary);

	fd = open("program.bin", O_CREAT | O_WRONLY | O_TRUNC,
		  S_IRUSR | S_IWUSR);

	printf("binary_format: %i\n", binary_format);
	write(fd, (void *)&binary_format, sizeof(GLenum));
	write(fd, binary, binary_size);
	free(binary);

	return 0;
}

int32_t main(void)
{
	bool res = true;
	GLenum glres = GL_NO_ERROR;
	ContextData data = { .width = 1920, .height = 1080 };

	/* choose a display under the surfacless platform */
	res = egl_init(&data);
	if (!res) {
		printf("%s %x\n",
		       "Bad display parameter. EGL error:", eglGetError());
		goto ExitDisplay;
	}

	/* pick a known universal config to play with */
	res = egl_choose_config(&data);
	if (!res) {
		printf("%s %x\n", "Bad config. EGL error:", eglGetError());
		goto ExitDisplay;
	}

	/* bind the gles api to this thread */
	res = eglBindAPI(EGL_OPENGL_ES_API);
	if (!res) {
		printf("%s %x\n", "Bad API. EGL error:", eglGetError());
		goto ExitDisplay;
	}

	/* create a context for the given config */
	res = egl_create_context(&data);
	if (!res) {
		printf("%s %x\n", "Bad context. EGL error:", eglGetError());
		goto ExitContext;
	}

	/* create a pbuffer to render to */
	res = egl_create_surface(&data);
	if (!res) {
		printf("%s %x\n",
		       "Failed to create surface. EGL error:", eglGetError());
		goto ExitContext;
	}

	/* make the pbuffer the current surface */
	res = eglMakeCurrent(data.display, data.surface, data.surface,
			     data.context);
	if (!res) {
		printf("%s %x\n", "Failed to make surface current. EGL error:",
		       eglGetError());
		goto ExitContext;
	}

	/* create a program to attach shaders to */
	data.program = glCreateProgram();
	if (data.program == 0) {
		printf("%s %x\n",
		       "Failed to create program. GL error:", glGetError());
		goto ExitContext;
	}

	/* setup a vertex shader and register it with the program */
	res = setup_shader(&data, VERTEX_SHADER, GL_VERTEX_SHADER);
	if (!res) {
		printf("%s\n", "Failed to setup vertex shader");
		goto ExitProgram;
	}

	/* setup a fragment shader and register it with the program */
	res = setup_shader(&data, FRAGMENT_SHADER, GL_FRAGMENT_SHADER);
	if (!res) {
		printf("%s\n", "Failed to setup fragment shader");
		goto ExitProgram;
	}

	/* bind vPosition to index 0 for our vertex shader */
	glBindAttribLocation(data.program, 0, "vPosition");
	glres = glGetError();
	if (glres != GL_NO_ERROR) {
		printf("%s %x\n", "Failed to bind attribute. GL error:", glres);
		goto ExitProgram;
	}

	/* link the program to test shaders */
	glLinkProgram(data.program);
	glres = glGetError();
	if (glres != GL_NO_ERROR) {
		printf("%s %x\n", "Failed to link program. GL error:", glres);
		goto ExitProgram;
	}

	cache(&data);

	printf("Application cached successfully\n");

	/* free stuff */
ExitProgram:
	glDeleteProgram(data.program);
ExitContext:
	eglDestroyContext(data.display, data.context);
ExitDisplay:
	eglTerminate(data.display);
	return (glres != GL_NO_ERROR) || (!res);
}
