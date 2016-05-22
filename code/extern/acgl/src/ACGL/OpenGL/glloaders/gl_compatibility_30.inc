#include <cstdlib>
#include <cstring>
#include <cstddef>


#if defined(__APPLE__)
#include <dlfcn.h>

static void* AppleGLGetProcAddress (const char *name)
{
  static void* image = NULL;
  if (NULL == image)
  {
    image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  }
  return (image ? dlsym(image, name) : NULL);
}
#endif /* __APPLE__ */

#if defined(__sgi) || defined (__sun)
#include <dlfcn.h>
#include <stdio.h>

static void* SunGetProcAddress (const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}
#endif /* __sgi || __sun */

#if defined(_WIN32)

#ifdef _MSC_VER
#pragma warning(disable: 4055)
#pragma warning(disable: 4054)
#endif

static int TestPointer(const PROC pTest)
{
	ptrdiff_t iTest;
	if(!pTest) return 0;
	iTest = (ptrdiff_t)pTest;
	
	if(iTest == 1 || iTest == 2 || iTest == 3 || iTest == -1) return 0;
	
	return 1;
}

static PROC WinGetProcAddress(const char *name)
{
	HMODULE glMod = NULL;
	PROC pFunc = wglGetProcAddress((LPCSTR)name);
	if(TestPointer(pFunc))
	{
		return pFunc;
	}
	glMod = GetModuleHandleA("OpenGL32.dll");
	return (PROC)GetProcAddress(glMod, (LPCSTR)name);
}
	
#define IntGetProcAddress(name) WinGetProcAddress(name)
#else
	#if defined(__APPLE__)
		#define IntGetProcAddress(name) AppleGLGetProcAddress(name)
	#else
		#if defined(__sgi) || defined(__sun)
			#define IntGetProcAddress(name) SunGetProcAddress(name)
		#else /* GLX */
		    #include <GL/glx.h>

			#define IntGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
		#endif
	#endif
#endif

// forward declaration:
static void LoadExtByName(const char *extensionName);

int ogl_ext_EXT_geometry_shader4 = 0;
int ogl_ext_ARB_geometry_shader4 = 0;
int ogl_ext_ARB_tessellation_shader = 0;
int ogl_ext_ARB_compute_shader = 0;
int ogl_ext_ARB_debug_output = 0;
int ogl_ext_KHR_debug = 0;
int ogl_ext_EXT_texture_compression_s3tc = 0;
int ogl_ext_EXT_texture_sRGB = 0;
int ogl_ext_EXT_texture_filter_anisotropic = 0;
int ogl_ext_ARB_compressed_texture_pixel_storage = 0;
int ogl_ext_ARB_conservative_depth = 0;
int ogl_ext_ARB_ES2_compatibility = 0;
int ogl_ext_ARB_get_program_binary = 0;
int ogl_ext_ARB_explicit_uniform_location = 0;
int ogl_ext_ARB_internalformat_query = 0;
int ogl_ext_ARB_internalformat_query2 = 0;
int ogl_ext_ARB_map_buffer_alignment = 0;
int ogl_ext_ARB_program_interface_query = 0;
int ogl_ext_ARB_separate_shader_objects = 0;
int ogl_ext_ARB_shading_language_420pack = 0;
int ogl_ext_ARB_shading_language_packing = 0;
int ogl_ext_ARB_texture_buffer_range = 0;
int ogl_ext_ARB_texture_storage = 0;
int ogl_ext_ARB_texture_view = 0;
int ogl_ext_ARB_vertex_attrib_binding = 0;
int ogl_ext_ARB_viewport_array = 0;
int ogl_ext_ARB_arrays_of_arrays = 0;
int ogl_ext_ARB_clear_buffer_object = 0;
int ogl_ext_ARB_copy_image = 0;
int ogl_ext_ARB_ES3_compatibility = 0;
int ogl_ext_ARB_fragment_layer_viewport = 0;
int ogl_ext_ARB_framebuffer_no_attachments = 0;
int ogl_ext_ARB_invalidate_subdata = 0;
int ogl_ext_ARB_robust_buffer_access_behavior = 0;
int ogl_ext_ARB_stencil_texturing = 0;
int ogl_ext_ARB_texture_query_levels = 0;
int ogl_ext_ARB_texture_storage_multisample = 0;
int ogl_ext_NV_texture_barrier = 0;
int ogl_ext_NV_copy_image = 0;

// Extension: EXT_geometry_shader4
void (CODEGEN_FUNCPTR *_original_glProgramParameteriEXT)(GLuint, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteriEXT)(GLuint, GLenum, GLint) = NULL;

// Extension: ARB_geometry_shader4
void (CODEGEN_FUNCPTR *_original_glFramebufferTextureARB)(GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureARB)(GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTextureFaceARB)(GLenum, GLenum, GLuint, GLint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureFaceARB)(GLenum, GLenum, GLuint, GLint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTextureLayerARB)(GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureLayerARB)(GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramParameteriARB)(GLuint, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteriARB)(GLuint, GLenum, GLint) = NULL;

// Extension: ARB_tessellation_shader
void (CODEGEN_FUNCPTR *_original_glPatchParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPatchParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPatchParameteri)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPatchParameteri)(GLenum, GLint) = NULL;

// Extension: ARB_compute_shader
void (CODEGEN_FUNCPTR *_original_glDispatchCompute)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDispatchCompute)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDispatchComputeIndirect)(GLintptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDispatchComputeIndirect)(GLintptr) = NULL;

// Extension: ARB_debug_output
void (CODEGEN_FUNCPTR *_original_glDebugMessageCallbackARB)(GLDEBUGPROCARB, const void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallbackARB)(GLDEBUGPROCARB, const void *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDebugMessageControlARB)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControlARB)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glDebugMessageInsertARB)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsertARB)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGetDebugMessageLogARB)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLogARB)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;

// Extension: KHR_debug
void (CODEGEN_FUNCPTR *_original_glDebugMessageCallback)(GLDEBUGPROC, void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallback)(GLDEBUGPROC, void *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDebugMessageControl)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControl)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glDebugMessageInsert)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsert)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGetDebugMessageLog)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLog)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetObjectLabel)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectLabel)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetObjectPtrLabel)(const void *, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetObjectPtrLabel)(const void *, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetPointerv)(GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPointerv)(GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_original_glObjectLabel)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectLabel)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glObjectPtrLabel)(const void *, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glObjectPtrLabel)(const void *, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPopDebugGroup)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopDebugGroup)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPushDebugGroup)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushDebugGroup)(GLenum, GLuint, GLsizei, const GLchar *) = NULL;

// Extension: ARB_ES2_compatibility
void (CODEGEN_FUNCPTR *_original_glClearDepthf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearDepthf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glDepthRangef)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRangef)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetShaderPrecisionFormat)(GLenum, GLenum, GLint *, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderPrecisionFormat)(GLenum, GLenum, GLint *, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glReleaseShaderCompiler)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReleaseShaderCompiler)() = NULL;
void (CODEGEN_FUNCPTR *_original_glShaderBinary)(GLsizei, const GLuint *, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderBinary)(GLsizei, const GLuint *, GLenum, const GLvoid *, GLsizei) = NULL;

// Extension: ARB_get_program_binary
void (CODEGEN_FUNCPTR *_original_glGetProgramBinary)(GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramBinary)(GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramBinary)(GLuint, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramBinary)(GLuint, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramParameteri)(GLuint, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteri)(GLuint, GLenum, GLint) = NULL;

// Extension: ARB_internalformat_query
void (CODEGEN_FUNCPTR *_original_glGetInternalformativ)(GLenum, GLenum, GLenum, GLsizei, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformativ)(GLenum, GLenum, GLenum, GLsizei, GLint *) = NULL;

// Extension: ARB_internalformat_query2
void (CODEGEN_FUNCPTR *_original_glGetInternalformati64v)(GLenum, GLenum, GLenum, GLsizei, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformati64v)(GLenum, GLenum, GLenum, GLsizei, GLint64 *) = NULL;

// Extension: ARB_program_interface_query
void (CODEGEN_FUNCPTR *_original_glGetProgramInterfaceiv)(GLuint, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInterfaceiv)(GLuint, GLenum, GLenum, GLint *) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGetProgramResourceIndex)(GLuint, GLenum, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceIndex)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetProgramResourceLocation)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocation)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetProgramResourceLocationIndex)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocationIndex)(GLuint, GLenum, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramResourceName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramResourceiv)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceiv)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *) = NULL;

// Extension: ARB_separate_shader_objects
void (CODEGEN_FUNCPTR *_original_glActiveShaderProgram)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glActiveShaderProgram)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindProgramPipeline)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindProgramPipeline)(GLuint) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glCreateShaderProgramv)(GLenum, GLsizei, const GLchar *const*) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShaderProgramv)(GLenum, GLsizei, const GLchar *const*) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteProgramPipelines)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgramPipelines)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenProgramPipelines)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenProgramPipelines)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramPipelineInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramPipelineiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineiv)(GLuint, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsProgramPipeline)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgramPipeline)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1d)(GLuint, GLint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1d)(GLuint, GLint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1f)(GLuint, GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1f)(GLuint, GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1i)(GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1i)(GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1ui)(GLuint, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1ui)(GLuint, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform1uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2d)(GLuint, GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2d)(GLuint, GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2f)(GLuint, GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2f)(GLuint, GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2i)(GLuint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2i)(GLuint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2ui)(GLuint, GLint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2ui)(GLuint, GLint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform2uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3d)(GLuint, GLint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3d)(GLuint, GLint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3f)(GLuint, GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3f)(GLuint, GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3i)(GLuint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3i)(GLuint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3ui)(GLuint, GLint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3ui)(GLuint, GLint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform3uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4d)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4d)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4dv)(GLuint, GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4f)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4f)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4fv)(GLuint, GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4i)(GLuint, GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4i)(GLuint, GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4iv)(GLuint, GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4ui)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4ui)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniform4uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4uiv)(GLuint, GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix2x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix3x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProgramUniformMatrix4x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUseProgramStages)(GLuint, GLbitfield, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUseProgramStages)(GLuint, GLbitfield, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glValidateProgramPipeline)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glValidateProgramPipeline)(GLuint) = NULL;

// Extension: ARB_texture_buffer_range
void (CODEGEN_FUNCPTR *_original_glTexBufferRange)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexBufferRange)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr) = NULL;

// Extension: ARB_texture_storage
void (CODEGEN_FUNCPTR *_original_glTexStorage1D)(GLenum, GLsizei, GLenum, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage1D)(GLenum, GLsizei, GLenum, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexStorage2D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexStorage3D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei) = NULL;

// Extension: ARB_texture_view
void (CODEGEN_FUNCPTR *_original_glTextureView)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureView)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint) = NULL;

// Extension: ARB_vertex_attrib_binding
void (CODEGEN_FUNCPTR *_original_glBindVertexBuffer)(GLuint, GLuint, GLintptr, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindVertexBuffer)(GLuint, GLuint, GLintptr, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribBinding)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribBinding)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribFormat)(GLuint, GLint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribFormat)(GLuint, GLint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribIFormat)(GLuint, GLint, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIFormat)(GLuint, GLint, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribLFormat)(GLuint, GLint, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLFormat)(GLuint, GLint, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexBindingDivisor)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexBindingDivisor)(GLuint, GLuint) = NULL;

// Extension: ARB_viewport_array
void (CODEGEN_FUNCPTR *_original_glDepthRangeArrayv)(GLuint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeArrayv)(GLuint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDepthRangeIndexed)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeIndexed)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetDoublei_v)(GLenum, GLuint, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublei_v)(GLenum, GLuint, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetFloati_v)(GLenum, GLuint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloati_v)(GLenum, GLuint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glScissorArrayv)(GLuint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorArrayv)(GLuint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glScissorIndexed)(GLuint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexed)(GLuint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glScissorIndexedv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexedv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glViewportArrayv)(GLuint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportArrayv)(GLuint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glViewportIndexedf)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedf)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glViewportIndexedfv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedfv)(GLuint, const GLfloat *) = NULL;

// Extension: ARB_clear_buffer_object
void (CODEGEN_FUNCPTR *_original_glClearBufferData)(GLenum, GLenum, GLenum, GLenum, const void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferData)(GLenum, GLenum, GLenum, GLenum, const void *) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearBufferSubData)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferSubData)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *) = NULL;

// Extension: ARB_copy_image
void (CODEGEN_FUNCPTR *_original_glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;

// Extension: ARB_framebuffer_no_attachments
void (CODEGEN_FUNCPTR *_original_glFramebufferParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetFramebufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferParameteriv)(GLenum, GLenum, GLint *) = NULL;

// Extension: ARB_invalidate_subdata
void (CODEGEN_FUNCPTR *_original_glInvalidateBufferData)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferData)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glInvalidateBufferSubData)(GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferSubData)(GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_original_glInvalidateFramebuffer)(GLenum, GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateFramebuffer)(GLenum, GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *_original_glInvalidateSubFramebuffer)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateSubFramebuffer)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glInvalidateTexImage)(GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexImage)(GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glInvalidateTexSubImage)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexSubImage)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;

// Extension: ARB_texture_storage_multisample
void (CODEGEN_FUNCPTR *_original_glTexStorage2DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexStorage3DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean) = NULL;

// Extension: NV_texture_barrier
void (CODEGEN_FUNCPTR *_original_glTextureBarrierNV)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTextureBarrierNV)() = NULL;

// Extension: NV_copy_image
void (CODEGEN_FUNCPTR *_original_glCopyImageSubDataNV)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyImageSubDataNV)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = NULL;

// Extension: 1.0
void (CODEGEN_FUNCPTR *_original_glAccum)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glAccum)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glAlphaFunc)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glAlphaFunc)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glBegin)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBegin)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBitmap)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBitmap)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCallList)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCallList)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glCallLists)(GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCallLists)(GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearAccum)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearAccum)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearIndex)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearIndex)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glClipPlane)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClipPlane)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4b)(GLbyte, GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4b)(GLbyte, GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4ub)(GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ub)(GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4us)(GLushort, GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4us)(GLushort, GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_original_glColor4usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColor4usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glColorMaterial)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMaterial)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyPixels)(GLint, GLint, GLsizei, GLsizei, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyPixels)(GLint, GLint, GLsizei, GLsizei, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCullFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteLists)(GLuint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteLists)(GLuint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDepthFunc)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthFunc)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDepthMask)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthMask)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glDepthRange)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDepthRange)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glDisable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEdgeFlag)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlag)(GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glEdgeFlagv)(const GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagv)(const GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glEnd)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnd)() = NULL;
void (CODEGEN_FUNCPTR *_original_glEndList)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndList)() = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalMesh1)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh1)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalMesh2)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh2)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalPoint1)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint1)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEvalPoint2)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint2)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFeedbackBuffer)(GLsizei, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFeedbackBuffer)(GLsizei, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *_original_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *_original_glFogf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogiv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogiv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glFrustum)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrustum)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGenLists)(GLsizei) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGenLists)(GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetClipPlane)(GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetClipPlane)(GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetDoublev)(GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublev)(GLenum, GLdouble *) = NULL;
GLenum (CODEGEN_FUNCPTR *_original_glGetError)() = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glGetError)() = NULL;
void (CODEGEN_FUNCPTR *_original_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetIntegerv)(GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegerv)(GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetLightfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetLightfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetLightiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetLightiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMapdv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapdv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMapfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMapiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMapiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMaterialfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMaterialiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetPixelMapfv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapfv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetPixelMapuiv)(GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapuiv)(GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetPixelMapusv)(GLenum, GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapusv)(GLenum, GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetPolygonStipple)(GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetPolygonStipple)(GLubyte *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_original_glGetString)(GLenum) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexEnvfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnvfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexEnviv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnviv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexGendv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGendv)(GLenum, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexGenfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGenfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexGeniv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexGeniv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexImage)(GLenum, GLint, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexImage)(GLenum, GLint, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexParameterfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterfv)(GLenum, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glHint)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glHint)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexdv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexdv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexi)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexi)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexiv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexiv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexs)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexs)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexsv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexsv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glInitNames)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInitNames)() = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsEnabled)(GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsList)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsList)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightModelf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModelf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightModelfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModelfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightModeli)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModeli)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightModeliv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightModeliv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLighti)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLighti)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLightiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLightiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLineStipple)(GLint, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineStipple)(GLint, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_original_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glListBase)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glListBase)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadIdentity)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadIdentity)() = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glMap1d)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap1d)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMap1f)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap1f)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMap2d)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap2d)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMap2f)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMap2f)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMapGrid1d)(GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1d)(GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMapGrid1f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMapGrid2d)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2d)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMapGrid2f)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2f)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMaterialf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMaterialfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMateriali)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMateriali)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMaterialiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMaterialiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMatrixMode)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMatrixMode)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glNewList)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNewList)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormal3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormal3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glPassThrough)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPassThrough)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelMapfv)(GLenum, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapfv)(GLenum, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelMapuiv)(GLenum, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapuiv)(GLenum, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelMapusv)(GLenum, GLsizei, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelMapusv)(GLenum, GLsizei, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelTransferf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelTransferi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferi)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelZoom)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelZoom)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glPolygonStipple)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonStipple)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPopAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPopMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPopName)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopName)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPushAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glPushMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushMatrix)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPushName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushName)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glRasterPos4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectd)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectd)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectdv)(const GLdouble *, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectdv)(const GLdouble *, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectf)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectf)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectfv)(const GLfloat *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectfv)(const GLfloat *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRecti)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRecti)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectiv)(const GLint *, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectiv)(const GLint *, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glRects)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRects)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glRectsv)(const GLshort *, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRectsv)(const GLshort *, const GLshort *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glRenderMode)(GLenum) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glRenderMode)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glRotated)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRotated)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glRotatef)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRotatef)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glScaled)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScaled)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glScalef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScalef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glSelectBuffer)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSelectBuffer)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glShadeModel)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShadeModel)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1d)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1f)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1i)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1i)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1s)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1s)(GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord1sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoord4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexEnvf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexEnvfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexEnvi)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnvi)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexEnviv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexEnviv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGend)(GLenum, GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGend)(GLenum, GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGendv)(GLenum, GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGendv)(GLenum, GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGenf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGenf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGenfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGenfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGeni)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGeni)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexGeniv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexGeniv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexImage1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameterf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterf)(GLenum, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameterfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterfv)(GLenum, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteri)(GLenum, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameteriv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameteriv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTranslated)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTranslated)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glTranslatef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTranslatef)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4d)(GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4f)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4s)(GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertex4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertex4sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;

// Extension: 1.1
GLboolean (CODEGEN_FUNCPTR *_original_glAreTexturesResident)(GLsizei, const GLuint *, GLboolean *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glAreTexturesResident)(GLsizei, const GLuint *, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_original_glArrayElement)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glArrayElement)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindTexture)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyTexImage1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyTexSubImage1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteTextures)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteTextures)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDisableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEdgeFlagPointer)(GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagPointer)(GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEnableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableClientState)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenTextures)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexub)(GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexub)(GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glIndexubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glIndexubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glInterleavedArrays)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glInterleavedArrays)(GLenum, GLsizei, const GLvoid *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsTexture)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glNormalPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glNormalPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPopClientAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPopClientAttrib)() = NULL;
void (CODEGEN_FUNCPTR *_original_glPrioritizeTextures)(GLsizei, const GLuint *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPrioritizeTextures)(GLsizei, const GLuint *, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPushClientAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPushClientAttrib)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexCoordPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexCoordPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;

// Extension: 1.2
void (CODEGEN_FUNCPTR *_original_glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendEquation)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquation)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;

// Extension: 1.3
void (CODEGEN_FUNCPTR *_original_glActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glClientActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClientActiveTexture)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexImage1D)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage1D)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetCompressedTexImage)(GLenum, GLint, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTexImage)(GLenum, GLint, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glLoadTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixd)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixf)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1d)(GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1d)(GLenum, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1f)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1f)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1i)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1i)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1s)(GLenum, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1s)(GLenum, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord1sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2d)(GLenum, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2d)(GLenum, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2f)(GLenum, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2f)(GLenum, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2i)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2i)(GLenum, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2s)(GLenum, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2s)(GLenum, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord2sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3d)(GLenum, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3d)(GLenum, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3f)(GLenum, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3f)(GLenum, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3i)(GLenum, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3i)(GLenum, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3s)(GLenum, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3s)(GLenum, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord3sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4d)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4d)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4dv)(GLenum, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4f)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4f)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4fv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4i)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4i)(GLenum, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4iv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4s)(GLenum, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4s)(GLenum, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiTexCoord4sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4sv)(GLenum, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSampleCoverage)(GLfloat, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleCoverage)(GLfloat, GLboolean) = NULL;

// Extension: 1.4
void (CODEGEN_FUNCPTR *_original_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogCoordPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordPointer)(GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogCoordd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordd)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogCoorddv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoorddv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogCoordf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordf)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glFogCoordfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFogCoordfv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiDrawArrays)(GLenum, const GLint *, const GLsizei *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawArrays)(GLenum, const GLint *, const GLsizei *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiDrawElements)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElements)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointParameterf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterf)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameterfv)(GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointParameteri)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteri)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointParameteriv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointParameteriv)(GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3b)(GLbyte, GLbyte, GLbyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3bv)(const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ub)(GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ubv)(const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3uiv)(const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3us)(GLushort, GLushort, GLushort) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3usv)(const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSecondaryColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColorPointer)(GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2d)(GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2f)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2s)(GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3d)(GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3dv)(const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3f)(GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3fv)(const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3iv)(const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3s)(GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glWindowPos3sv)(const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3sv)(const GLshort *) = NULL;

// Extension: 1.5
void (CODEGEN_FUNCPTR *_original_glBeginQuery)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBeginQuery)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindBuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteBuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteBuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteQueries)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteQueries)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEndQuery)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndQuery)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenBuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenBuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenQueries)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenQueries)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBufferPointerv)(GLenum, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferPointerv)(GLenum, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBufferSubData)(GLenum, GLintptr, GLsizeiptr, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferSubData)(GLenum, GLintptr, GLsizeiptr, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryObjectiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryObjectuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryiv)(GLenum, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsBuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsBuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsQuery)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsQuery)(GLuint) = NULL;
void * (CODEGEN_FUNCPTR *_original_glMapBuffer)(GLenum, GLenum) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapBuffer)(GLenum, GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glUnmapBuffer)(GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glUnmapBuffer)(GLenum) = NULL;

// Extension: 2.0
void (CODEGEN_FUNCPTR *_original_glAttachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glAttachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindAttribLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindAttribLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendEquationSeparate)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparate)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glCompileShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCompileShader)(GLuint) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glCreateProgram)() = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateProgram)() = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glCreateShader)(GLenum) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShader)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDetachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDetachShader)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDisableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawBuffers)(GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffers)(GLsizei, const GLenum *) = NULL;
void (CODEGEN_FUNCPTR *_original_glEnableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnableVertexAttribArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetAttribLocation)(GLuint, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetAttribLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetShaderiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetShaderiv)(GLuint, GLenum, GLint *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetUniformLocation)(GLuint, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetUniformLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformfv)(GLuint, GLint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformfv)(GLuint, GLint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformiv)(GLuint, GLint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformiv)(GLuint, GLint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribPointerv)(GLuint, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribPointerv)(GLuint, GLenum, GLvoid **) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribiv)(GLuint, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsProgram)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgram)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsShader)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsShader)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glLinkProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLinkProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilMaskSeparate)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMaskSeparate)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1f)(GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1f)(GLint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1i)(GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2f)(GLint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2i)(GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3i)(GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4fv)(GLint, GLsizei, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4i)(GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4i)(GLint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4iv)(GLint, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUseProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUseProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glValidateProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glValidateProgram)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1f)(GLuint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1f)(GLuint, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1s)(GLuint, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1s)(GLuint, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib1sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2f)(GLuint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2f)(GLuint, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2s)(GLuint, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2s)(GLuint, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib2sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3s)(GLuint, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3s)(GLuint, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib3sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nbv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nbv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Niv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Niv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nsv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nsv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nub)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nub)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nuiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nuiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4Nusv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nusv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4fv)(GLuint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4s)(GLuint, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4s)(GLuint, GLshort, GLshort, GLshort, GLshort) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttrib4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) = NULL;

// Extension: 2.1
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = NULL;

// Extension: 3.0
void (CODEGEN_FUNCPTR *_original_glBeginConditionalRender)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBeginConditionalRender)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBeginTransformFeedback)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBeginTransformFeedback)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindBufferBase)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBufferBase)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindBufferRange)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindBufferRange)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindFragDataLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocation)(GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindFramebuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindFramebuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindRenderbuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindRenderbuffer)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindVertexArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindVertexArray)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = NULL;
GLenum (CODEGEN_FUNCPTR *_original_glCheckFramebufferStatus)(GLenum) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glCheckFramebufferStatus)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glClampColor)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClampColor)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearBufferfi)(GLenum, GLint, GLfloat, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfi)(GLenum, GLint, GLfloat, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearBufferfv)(GLenum, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfv)(GLenum, GLint, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearBufferiv)(GLenum, GLint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferiv)(GLenum, GLint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearBufferuiv)(GLenum, GLint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearBufferuiv)(GLenum, GLint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glColorMaski)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMaski)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteFramebuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteFramebuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteRenderbuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteRenderbuffers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteVertexArrays)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteVertexArrays)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDisablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDisablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEnablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnablei)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEndConditionalRender)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndConditionalRender)() = NULL;
void (CODEGEN_FUNCPTR *_original_glEndTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_original_glFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTexture1D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture1D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTexture3D)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture3D)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTextureLayer)(GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureLayer)(GLenum, GLenum, GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenFramebuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenFramebuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenRenderbuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenRenderbuffers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenVertexArrays)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenVertexArrays)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenerateMipmap)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenerateMipmap)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBooleani_v)(GLenum, GLuint, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleani_v)(GLenum, GLuint, GLboolean *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetFragDataLocation)(GLuint, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataLocation)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetIntegeri_v)(GLenum, GLuint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegeri_v)(GLenum, GLuint, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_original_glGetStringi)(GLenum, GLuint) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetStringi)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexParameterIiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIiv)(GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTexParameterIuiv)(GLenum, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIuiv)(GLenum, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetTransformFeedbackVarying)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbackVarying)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformuiv)(GLuint, GLint, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformuiv)(GLuint, GLint, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribIiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribIuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIuiv)(GLuint, GLenum, GLuint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsEnabledi)(GLenum, GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabledi)(GLenum, GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsFramebuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsFramebuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsRenderbuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsRenderbuffer)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsVertexArray)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsVertexArray)(GLuint) = NULL;
void * (CODEGEN_FUNCPTR *_original_glMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield) = NULL;
void * (CODEGEN_FUNCPTR *_ptrc_glMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glRenderbufferStorageMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorageMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameterIiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIiv)(GLenum, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexParameterIuiv)(GLenum, GLenum, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIuiv)(GLenum, GLenum, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTransformFeedbackVaryings)(GLuint, GLsizei, const GLchar *const*, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTransformFeedbackVaryings)(GLuint, GLsizei, const GLchar *const*, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1ui)(GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1ui)(GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2ui)(GLint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2ui)(GLint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3ui)(GLint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3ui)(GLint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4ui)(GLint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4ui)(GLint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4uiv)(GLint, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI1i)(GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1i)(GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI1iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI1ui)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1ui)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI1uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI2i)(GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2i)(GLuint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI2iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI2ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2ui)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI2uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI3i)(GLuint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3i)(GLuint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI3iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI3ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3ui)(GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI3uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4bv)(GLuint, const GLbyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4i)(GLuint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4i)(GLuint, GLint, GLint, GLint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4iv)(GLuint, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4sv)(GLuint, const GLshort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ubv)(GLuint, const GLubyte *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4ui)(GLuint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ui)(GLuint, GLuint, GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4uiv)(GLuint, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribI4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4usv)(GLuint, const GLushort *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *) = NULL;

static int Load_Version_3_0()
{
	int numFailed = 0;
	_original_glAccum = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glAccum");
	if(!_original_glAccum) numFailed++;
	_original_glAlphaFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glAlphaFunc");
	if(!_original_glAlphaFunc) numFailed++;
	_original_glBegin = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBegin");
	if(!_original_glBegin) numFailed++;
	_original_glBitmap = (void (CODEGEN_FUNCPTR *)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *))IntGetProcAddress("glBitmap");
	if(!_original_glBitmap) numFailed++;
	_original_glBlendFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendFunc");
	if(!_original_glBlendFunc) numFailed++;
	_original_glCallList = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCallList");
	if(!_original_glCallList) numFailed++;
	_original_glCallLists = (void (CODEGEN_FUNCPTR *)(GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glCallLists");
	if(!_original_glCallLists) numFailed++;
	_original_glClear = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glClear");
	if(!_original_glClear) numFailed++;
	_original_glClearAccum = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearAccum");
	if(!_original_glClearAccum) numFailed++;
	_original_glClearColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearColor");
	if(!_original_glClearColor) numFailed++;
	_original_glClearDepth = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glClearDepth");
	if(!_original_glClearDepth) numFailed++;
	_original_glClearIndex = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearIndex");
	if(!_original_glClearIndex) numFailed++;
	_original_glClearStencil = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glClearStencil");
	if(!_original_glClearStencil) numFailed++;
	_original_glClipPlane = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glClipPlane");
	if(!_original_glClipPlane) numFailed++;
	_original_glColor3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glColor3b");
	if(!_original_glColor3b) numFailed++;
	_original_glColor3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glColor3bv");
	if(!_original_glColor3bv) numFailed++;
	_original_glColor3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glColor3d");
	if(!_original_glColor3d) numFailed++;
	_original_glColor3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glColor3dv");
	if(!_original_glColor3dv) numFailed++;
	_original_glColor3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glColor3f");
	if(!_original_glColor3f) numFailed++;
	_original_glColor3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glColor3fv");
	if(!_original_glColor3fv) numFailed++;
	_original_glColor3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glColor3i");
	if(!_original_glColor3i) numFailed++;
	_original_glColor3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glColor3iv");
	if(!_original_glColor3iv) numFailed++;
	_original_glColor3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glColor3s");
	if(!_original_glColor3s) numFailed++;
	_original_glColor3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glColor3sv");
	if(!_original_glColor3sv) numFailed++;
	_original_glColor3ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte))IntGetProcAddress("glColor3ub");
	if(!_original_glColor3ub) numFailed++;
	_original_glColor3ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glColor3ubv");
	if(!_original_glColor3ubv) numFailed++;
	_original_glColor3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glColor3ui");
	if(!_original_glColor3ui) numFailed++;
	_original_glColor3uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glColor3uiv");
	if(!_original_glColor3uiv) numFailed++;
	_original_glColor3us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort))IntGetProcAddress("glColor3us");
	if(!_original_glColor3us) numFailed++;
	_original_glColor3usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glColor3usv");
	if(!_original_glColor3usv) numFailed++;
	_original_glColor4b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte, GLbyte))IntGetProcAddress("glColor4b");
	if(!_original_glColor4b) numFailed++;
	_original_glColor4bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glColor4bv");
	if(!_original_glColor4bv) numFailed++;
	_original_glColor4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glColor4d");
	if(!_original_glColor4d) numFailed++;
	_original_glColor4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glColor4dv");
	if(!_original_glColor4dv) numFailed++;
	_original_glColor4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glColor4f");
	if(!_original_glColor4f) numFailed++;
	_original_glColor4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glColor4fv");
	if(!_original_glColor4fv) numFailed++;
	_original_glColor4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glColor4i");
	if(!_original_glColor4i) numFailed++;
	_original_glColor4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glColor4iv");
	if(!_original_glColor4iv) numFailed++;
	_original_glColor4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glColor4s");
	if(!_original_glColor4s) numFailed++;
	_original_glColor4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glColor4sv");
	if(!_original_glColor4sv) numFailed++;
	_original_glColor4ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glColor4ub");
	if(!_original_glColor4ub) numFailed++;
	_original_glColor4ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glColor4ubv");
	if(!_original_glColor4ubv) numFailed++;
	_original_glColor4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glColor4ui");
	if(!_original_glColor4ui) numFailed++;
	_original_glColor4uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glColor4uiv");
	if(!_original_glColor4uiv) numFailed++;
	_original_glColor4us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort, GLushort))IntGetProcAddress("glColor4us");
	if(!_original_glColor4us) numFailed++;
	_original_glColor4usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glColor4usv");
	if(!_original_glColor4usv) numFailed++;
	_original_glColorMask = (void (CODEGEN_FUNCPTR *)(GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMask");
	if(!_original_glColorMask) numFailed++;
	_original_glColorMaterial = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glColorMaterial");
	if(!_original_glColorMaterial) numFailed++;
	_original_glCopyPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum))IntGetProcAddress("glCopyPixels");
	if(!_original_glCopyPixels) numFailed++;
	_original_glCullFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCullFace");
	if(!_original_glCullFace) numFailed++;
	_original_glDeleteLists = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei))IntGetProcAddress("glDeleteLists");
	if(!_original_glDeleteLists) numFailed++;
	_original_glDepthFunc = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDepthFunc");
	if(!_original_glDepthFunc) numFailed++;
	_original_glDepthMask = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glDepthMask");
	if(!_original_glDepthMask) numFailed++;
	_original_glDepthRange = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glDepthRange");
	if(!_original_glDepthRange) numFailed++;
	_original_glDisable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisable");
	if(!_original_glDisable) numFailed++;
	_original_glDrawBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDrawBuffer");
	if(!_original_glDrawBuffer) numFailed++;
	_original_glDrawPixels = (void (CODEGEN_FUNCPTR *)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glDrawPixels");
	if(!_original_glDrawPixels) numFailed++;
	_original_glEdgeFlag = (void (CODEGEN_FUNCPTR *)(GLboolean))IntGetProcAddress("glEdgeFlag");
	if(!_original_glEdgeFlag) numFailed++;
	_original_glEdgeFlagv = (void (CODEGEN_FUNCPTR *)(const GLboolean *))IntGetProcAddress("glEdgeFlagv");
	if(!_original_glEdgeFlagv) numFailed++;
	_original_glEnable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnable");
	if(!_original_glEnable) numFailed++;
	_original_glEnd = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEnd");
	if(!_original_glEnd) numFailed++;
	_original_glEndList = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndList");
	if(!_original_glEndList) numFailed++;
	_original_glEvalCoord1d = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glEvalCoord1d");
	if(!_original_glEvalCoord1d) numFailed++;
	_original_glEvalCoord1dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glEvalCoord1dv");
	if(!_original_glEvalCoord1dv) numFailed++;
	_original_glEvalCoord1f = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glEvalCoord1f");
	if(!_original_glEvalCoord1f) numFailed++;
	_original_glEvalCoord1fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glEvalCoord1fv");
	if(!_original_glEvalCoord1fv) numFailed++;
	_original_glEvalCoord2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glEvalCoord2d");
	if(!_original_glEvalCoord2d) numFailed++;
	_original_glEvalCoord2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glEvalCoord2dv");
	if(!_original_glEvalCoord2dv) numFailed++;
	_original_glEvalCoord2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glEvalCoord2f");
	if(!_original_glEvalCoord2f) numFailed++;
	_original_glEvalCoord2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glEvalCoord2fv");
	if(!_original_glEvalCoord2fv) numFailed++;
	_original_glEvalMesh1 = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint))IntGetProcAddress("glEvalMesh1");
	if(!_original_glEvalMesh1) numFailed++;
	_original_glEvalMesh2 = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint))IntGetProcAddress("glEvalMesh2");
	if(!_original_glEvalMesh2) numFailed++;
	_original_glEvalPoint1 = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glEvalPoint1");
	if(!_original_glEvalPoint1) numFailed++;
	_original_glEvalPoint2 = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glEvalPoint2");
	if(!_original_glEvalPoint2) numFailed++;
	_original_glFeedbackBuffer = (void (CODEGEN_FUNCPTR *)(GLsizei, GLenum, GLfloat *))IntGetProcAddress("glFeedbackBuffer");
	if(!_original_glFeedbackBuffer) numFailed++;
	_original_glFinish = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFinish");
	if(!_original_glFinish) numFailed++;
	_original_glFlush = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFlush");
	if(!_original_glFlush) numFailed++;
	_original_glFogf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glFogf");
	if(!_original_glFogf) numFailed++;
	_original_glFogfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glFogfv");
	if(!_original_glFogfv) numFailed++;
	_original_glFogi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glFogi");
	if(!_original_glFogi) numFailed++;
	_original_glFogiv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glFogiv");
	if(!_original_glFogiv) numFailed++;
	_original_glFrontFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glFrontFace");
	if(!_original_glFrontFace) numFailed++;
	_original_glFrustum = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glFrustum");
	if(!_original_glFrustum) numFailed++;
	_original_glGenLists = (GLuint (CODEGEN_FUNCPTR *)(GLsizei))IntGetProcAddress("glGenLists");
	if(!_original_glGenLists) numFailed++;
	_original_glGetBooleanv = (void (CODEGEN_FUNCPTR *)(GLenum, GLboolean *))IntGetProcAddress("glGetBooleanv");
	if(!_original_glGetBooleanv) numFailed++;
	_original_glGetClipPlane = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetClipPlane");
	if(!_original_glGetClipPlane) numFailed++;
	_original_glGetDoublev = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetDoublev");
	if(!_original_glGetDoublev) numFailed++;
	_original_glGetError = (GLenum (CODEGEN_FUNCPTR *)())IntGetProcAddress("glGetError");
	if(!_original_glGetError) numFailed++;
	_original_glGetFloatv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetFloatv");
	if(!_original_glGetFloatv) numFailed++;
	_original_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_original_glGetIntegerv) numFailed++;
	_original_glGetLightfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetLightfv");
	if(!_original_glGetLightfv) numFailed++;
	_original_glGetLightiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetLightiv");
	if(!_original_glGetLightiv) numFailed++;
	_original_glGetMapdv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble *))IntGetProcAddress("glGetMapdv");
	if(!_original_glGetMapdv) numFailed++;
	_original_glGetMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetMapfv");
	if(!_original_glGetMapfv) numFailed++;
	_original_glGetMapiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetMapiv");
	if(!_original_glGetMapiv) numFailed++;
	_original_glGetMaterialfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetMaterialfv");
	if(!_original_glGetMaterialfv) numFailed++;
	_original_glGetMaterialiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetMaterialiv");
	if(!_original_glGetMaterialiv) numFailed++;
	_original_glGetPixelMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetPixelMapfv");
	if(!_original_glGetPixelMapfv) numFailed++;
	_original_glGetPixelMapuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint *))IntGetProcAddress("glGetPixelMapuiv");
	if(!_original_glGetPixelMapuiv) numFailed++;
	_original_glGetPixelMapusv = (void (CODEGEN_FUNCPTR *)(GLenum, GLushort *))IntGetProcAddress("glGetPixelMapusv");
	if(!_original_glGetPixelMapusv) numFailed++;
	_original_glGetPolygonStipple = (void (CODEGEN_FUNCPTR *)(GLubyte *))IntGetProcAddress("glGetPolygonStipple");
	if(!_original_glGetPolygonStipple) numFailed++;
	_original_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!_original_glGetString) numFailed++;
	_original_glGetTexEnvfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexEnvfv");
	if(!_original_glGetTexEnvfv) numFailed++;
	_original_glGetTexEnviv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexEnviv");
	if(!_original_glGetTexEnviv) numFailed++;
	_original_glGetTexGendv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble *))IntGetProcAddress("glGetTexGendv");
	if(!_original_glGetTexGendv) numFailed++;
	_original_glGetTexGenfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexGenfv");
	if(!_original_glGetTexGenfv) numFailed++;
	_original_glGetTexGeniv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexGeniv");
	if(!_original_glGetTexGeniv) numFailed++;
	_original_glGetTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLenum, GLvoid *))IntGetProcAddress("glGetTexImage");
	if(!_original_glGetTexImage) numFailed++;
	_original_glGetTexLevelParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLfloat *))IntGetProcAddress("glGetTexLevelParameterfv");
	if(!_original_glGetTexLevelParameterfv) numFailed++;
	_original_glGetTexLevelParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint *))IntGetProcAddress("glGetTexLevelParameteriv");
	if(!_original_glGetTexLevelParameteriv) numFailed++;
	_original_glGetTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat *))IntGetProcAddress("glGetTexParameterfv");
	if(!_original_glGetTexParameterfv) numFailed++;
	_original_glGetTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameteriv");
	if(!_original_glGetTexParameteriv) numFailed++;
	_original_glHint = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glHint");
	if(!_original_glHint) numFailed++;
	_original_glIndexMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIndexMask");
	if(!_original_glIndexMask) numFailed++;
	_original_glIndexd = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glIndexd");
	if(!_original_glIndexd) numFailed++;
	_original_glIndexdv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glIndexdv");
	if(!_original_glIndexdv) numFailed++;
	_original_glIndexf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glIndexf");
	if(!_original_glIndexf) numFailed++;
	_original_glIndexfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glIndexfv");
	if(!_original_glIndexfv) numFailed++;
	_original_glIndexi = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glIndexi");
	if(!_original_glIndexi) numFailed++;
	_original_glIndexiv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glIndexiv");
	if(!_original_glIndexiv) numFailed++;
	_original_glIndexs = (void (CODEGEN_FUNCPTR *)(GLshort))IntGetProcAddress("glIndexs");
	if(!_original_glIndexs) numFailed++;
	_original_glIndexsv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glIndexsv");
	if(!_original_glIndexsv) numFailed++;
	_original_glInitNames = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glInitNames");
	if(!_original_glInitNames) numFailed++;
	_original_glIsEnabled = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glIsEnabled");
	if(!_original_glIsEnabled) numFailed++;
	_original_glIsList = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsList");
	if(!_original_glIsList) numFailed++;
	_original_glLightModelf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glLightModelf");
	if(!_original_glLightModelf) numFailed++;
	_original_glLightModelfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glLightModelfv");
	if(!_original_glLightModelfv) numFailed++;
	_original_glLightModeli = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glLightModeli");
	if(!_original_glLightModeli) numFailed++;
	_original_glLightModeliv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glLightModeliv");
	if(!_original_glLightModeliv) numFailed++;
	_original_glLightf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glLightf");
	if(!_original_glLightf) numFailed++;
	_original_glLightfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glLightfv");
	if(!_original_glLightfv) numFailed++;
	_original_glLighti = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glLighti");
	if(!_original_glLighti) numFailed++;
	_original_glLightiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glLightiv");
	if(!_original_glLightiv) numFailed++;
	_original_glLineStipple = (void (CODEGEN_FUNCPTR *)(GLint, GLushort))IntGetProcAddress("glLineStipple");
	if(!_original_glLineStipple) numFailed++;
	_original_glLineWidth = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glLineWidth");
	if(!_original_glLineWidth) numFailed++;
	_original_glListBase = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glListBase");
	if(!_original_glListBase) numFailed++;
	_original_glLoadIdentity = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glLoadIdentity");
	if(!_original_glLoadIdentity) numFailed++;
	_original_glLoadMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glLoadMatrixd");
	if(!_original_glLoadMatrixd) numFailed++;
	_original_glLoadMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glLoadMatrixf");
	if(!_original_glLoadMatrixf) numFailed++;
	_original_glLoadName = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLoadName");
	if(!_original_glLoadName) numFailed++;
	_original_glLogicOp = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glLogicOp");
	if(!_original_glLogicOp) numFailed++;
	_original_glMap1d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *))IntGetProcAddress("glMap1d");
	if(!_original_glMap1d) numFailed++;
	_original_glMap1f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *))IntGetProcAddress("glMap1f");
	if(!_original_glMap1f) numFailed++;
	_original_glMap2d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *))IntGetProcAddress("glMap2d");
	if(!_original_glMap2d) numFailed++;
	_original_glMap2f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *))IntGetProcAddress("glMap2f");
	if(!_original_glMap2f) numFailed++;
	_original_glMapGrid1d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble))IntGetProcAddress("glMapGrid1d");
	if(!_original_glMapGrid1d) numFailed++;
	_original_glMapGrid1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glMapGrid1f");
	if(!_original_glMapGrid1f) numFailed++;
	_original_glMapGrid2d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble))IntGetProcAddress("glMapGrid2d");
	if(!_original_glMapGrid2d) numFailed++;
	_original_glMapGrid2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat))IntGetProcAddress("glMapGrid2f");
	if(!_original_glMapGrid2f) numFailed++;
	_original_glMaterialf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glMaterialf");
	if(!_original_glMaterialf) numFailed++;
	_original_glMaterialfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glMaterialfv");
	if(!_original_glMaterialfv) numFailed++;
	_original_glMateriali = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glMateriali");
	if(!_original_glMateriali) numFailed++;
	_original_glMaterialiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glMaterialiv");
	if(!_original_glMaterialiv) numFailed++;
	_original_glMatrixMode = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glMatrixMode");
	if(!_original_glMatrixMode) numFailed++;
	_original_glMultMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glMultMatrixd");
	if(!_original_glMultMatrixd) numFailed++;
	_original_glMultMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glMultMatrixf");
	if(!_original_glMultMatrixf) numFailed++;
	_original_glNewList = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glNewList");
	if(!_original_glNewList) numFailed++;
	_original_glNormal3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glNormal3b");
	if(!_original_glNormal3b) numFailed++;
	_original_glNormal3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glNormal3bv");
	if(!_original_glNormal3bv) numFailed++;
	_original_glNormal3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glNormal3d");
	if(!_original_glNormal3d) numFailed++;
	_original_glNormal3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glNormal3dv");
	if(!_original_glNormal3dv) numFailed++;
	_original_glNormal3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glNormal3f");
	if(!_original_glNormal3f) numFailed++;
	_original_glNormal3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glNormal3fv");
	if(!_original_glNormal3fv) numFailed++;
	_original_glNormal3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glNormal3i");
	if(!_original_glNormal3i) numFailed++;
	_original_glNormal3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glNormal3iv");
	if(!_original_glNormal3iv) numFailed++;
	_original_glNormal3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glNormal3s");
	if(!_original_glNormal3s) numFailed++;
	_original_glNormal3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glNormal3sv");
	if(!_original_glNormal3sv) numFailed++;
	_original_glOrtho = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glOrtho");
	if(!_original_glOrtho) numFailed++;
	_original_glPassThrough = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPassThrough");
	if(!_original_glPassThrough) numFailed++;
	_original_glPixelMapfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLfloat *))IntGetProcAddress("glPixelMapfv");
	if(!_original_glPixelMapfv) numFailed++;
	_original_glPixelMapuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLuint *))IntGetProcAddress("glPixelMapuiv");
	if(!_original_glPixelMapuiv) numFailed++;
	_original_glPixelMapusv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLushort *))IntGetProcAddress("glPixelMapusv");
	if(!_original_glPixelMapusv) numFailed++;
	_original_glPixelStoref = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelStoref");
	if(!_original_glPixelStoref) numFailed++;
	_original_glPixelStorei = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelStorei");
	if(!_original_glPixelStorei) numFailed++;
	_original_glPixelTransferf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelTransferf");
	if(!_original_glPixelTransferf) numFailed++;
	_original_glPixelTransferi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelTransferi");
	if(!_original_glPixelTransferi) numFailed++;
	_original_glPixelZoom = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPixelZoom");
	if(!_original_glPixelZoom) numFailed++;
	_original_glPointSize = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPointSize");
	if(!_original_glPointSize) numFailed++;
	_original_glPolygonMode = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glPolygonMode");
	if(!_original_glPolygonMode) numFailed++;
	_original_glPolygonStipple = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glPolygonStipple");
	if(!_original_glPolygonStipple) numFailed++;
	_original_glPopAttrib = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopAttrib");
	if(!_original_glPopAttrib) numFailed++;
	_original_glPopMatrix = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopMatrix");
	if(!_original_glPopMatrix) numFailed++;
	_original_glPopName = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopName");
	if(!_original_glPopName) numFailed++;
	_original_glPushAttrib = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glPushAttrib");
	if(!_original_glPushAttrib) numFailed++;
	_original_glPushMatrix = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPushMatrix");
	if(!_original_glPushMatrix) numFailed++;
	_original_glPushName = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glPushName");
	if(!_original_glPushName) numFailed++;
	_original_glRasterPos2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glRasterPos2d");
	if(!_original_glRasterPos2d) numFailed++;
	_original_glRasterPos2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos2dv");
	if(!_original_glRasterPos2dv) numFailed++;
	_original_glRasterPos2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glRasterPos2f");
	if(!_original_glRasterPos2f) numFailed++;
	_original_glRasterPos2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos2fv");
	if(!_original_glRasterPos2fv) numFailed++;
	_original_glRasterPos2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glRasterPos2i");
	if(!_original_glRasterPos2i) numFailed++;
	_original_glRasterPos2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos2iv");
	if(!_original_glRasterPos2iv) numFailed++;
	_original_glRasterPos2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glRasterPos2s");
	if(!_original_glRasterPos2s) numFailed++;
	_original_glRasterPos2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos2sv");
	if(!_original_glRasterPos2sv) numFailed++;
	_original_glRasterPos3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRasterPos3d");
	if(!_original_glRasterPos3d) numFailed++;
	_original_glRasterPos3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos3dv");
	if(!_original_glRasterPos3dv) numFailed++;
	_original_glRasterPos3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRasterPos3f");
	if(!_original_glRasterPos3f) numFailed++;
	_original_glRasterPos3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos3fv");
	if(!_original_glRasterPos3fv) numFailed++;
	_original_glRasterPos3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glRasterPos3i");
	if(!_original_glRasterPos3i) numFailed++;
	_original_glRasterPos3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos3iv");
	if(!_original_glRasterPos3iv) numFailed++;
	_original_glRasterPos3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glRasterPos3s");
	if(!_original_glRasterPos3s) numFailed++;
	_original_glRasterPos3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos3sv");
	if(!_original_glRasterPos3sv) numFailed++;
	_original_glRasterPos4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRasterPos4d");
	if(!_original_glRasterPos4d) numFailed++;
	_original_glRasterPos4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glRasterPos4dv");
	if(!_original_glRasterPos4dv) numFailed++;
	_original_glRasterPos4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRasterPos4f");
	if(!_original_glRasterPos4f) numFailed++;
	_original_glRasterPos4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glRasterPos4fv");
	if(!_original_glRasterPos4fv) numFailed++;
	_original_glRasterPos4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glRasterPos4i");
	if(!_original_glRasterPos4i) numFailed++;
	_original_glRasterPos4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glRasterPos4iv");
	if(!_original_glRasterPos4iv) numFailed++;
	_original_glRasterPos4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glRasterPos4s");
	if(!_original_glRasterPos4s) numFailed++;
	_original_glRasterPos4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glRasterPos4sv");
	if(!_original_glRasterPos4sv) numFailed++;
	_original_glReadBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glReadBuffer");
	if(!_original_glReadBuffer) numFailed++;
	_original_glReadPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))IntGetProcAddress("glReadPixels");
	if(!_original_glReadPixels) numFailed++;
	_original_glRectd = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRectd");
	if(!_original_glRectd) numFailed++;
	_original_glRectdv = (void (CODEGEN_FUNCPTR *)(const GLdouble *, const GLdouble *))IntGetProcAddress("glRectdv");
	if(!_original_glRectdv) numFailed++;
	_original_glRectf = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRectf");
	if(!_original_glRectf) numFailed++;
	_original_glRectfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *, const GLfloat *))IntGetProcAddress("glRectfv");
	if(!_original_glRectfv) numFailed++;
	_original_glRecti = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glRecti");
	if(!_original_glRecti) numFailed++;
	_original_glRectiv = (void (CODEGEN_FUNCPTR *)(const GLint *, const GLint *))IntGetProcAddress("glRectiv");
	if(!_original_glRectiv) numFailed++;
	_original_glRects = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glRects");
	if(!_original_glRects) numFailed++;
	_original_glRectsv = (void (CODEGEN_FUNCPTR *)(const GLshort *, const GLshort *))IntGetProcAddress("glRectsv");
	if(!_original_glRectsv) numFailed++;
	_original_glRenderMode = (GLint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glRenderMode");
	if(!_original_glRenderMode) numFailed++;
	_original_glRotated = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glRotated");
	if(!_original_glRotated) numFailed++;
	_original_glRotatef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glRotatef");
	if(!_original_glRotatef) numFailed++;
	_original_glScaled = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glScaled");
	if(!_original_glScaled) numFailed++;
	_original_glScalef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glScalef");
	if(!_original_glScalef) numFailed++;
	_original_glScissor = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissor");
	if(!_original_glScissor) numFailed++;
	_original_glSelectBuffer = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glSelectBuffer");
	if(!_original_glSelectBuffer) numFailed++;
	_original_glShadeModel = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glShadeModel");
	if(!_original_glShadeModel) numFailed++;
	_original_glStencilFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint))IntGetProcAddress("glStencilFunc");
	if(!_original_glStencilFunc) numFailed++;
	_original_glStencilMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glStencilMask");
	if(!_original_glStencilMask) numFailed++;
	_original_glStencilOp = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOp");
	if(!_original_glStencilOp) numFailed++;
	_original_glTexCoord1d = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glTexCoord1d");
	if(!_original_glTexCoord1d) numFailed++;
	_original_glTexCoord1dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord1dv");
	if(!_original_glTexCoord1dv) numFailed++;
	_original_glTexCoord1f = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glTexCoord1f");
	if(!_original_glTexCoord1f) numFailed++;
	_original_glTexCoord1fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord1fv");
	if(!_original_glTexCoord1fv) numFailed++;
	_original_glTexCoord1i = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glTexCoord1i");
	if(!_original_glTexCoord1i) numFailed++;
	_original_glTexCoord1iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord1iv");
	if(!_original_glTexCoord1iv) numFailed++;
	_original_glTexCoord1s = (void (CODEGEN_FUNCPTR *)(GLshort))IntGetProcAddress("glTexCoord1s");
	if(!_original_glTexCoord1s) numFailed++;
	_original_glTexCoord1sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord1sv");
	if(!_original_glTexCoord1sv) numFailed++;
	_original_glTexCoord2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glTexCoord2d");
	if(!_original_glTexCoord2d) numFailed++;
	_original_glTexCoord2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord2dv");
	if(!_original_glTexCoord2dv) numFailed++;
	_original_glTexCoord2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glTexCoord2f");
	if(!_original_glTexCoord2f) numFailed++;
	_original_glTexCoord2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord2fv");
	if(!_original_glTexCoord2fv) numFailed++;
	_original_glTexCoord2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glTexCoord2i");
	if(!_original_glTexCoord2i) numFailed++;
	_original_glTexCoord2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord2iv");
	if(!_original_glTexCoord2iv) numFailed++;
	_original_glTexCoord2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glTexCoord2s");
	if(!_original_glTexCoord2s) numFailed++;
	_original_glTexCoord2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord2sv");
	if(!_original_glTexCoord2sv) numFailed++;
	_original_glTexCoord3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTexCoord3d");
	if(!_original_glTexCoord3d) numFailed++;
	_original_glTexCoord3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord3dv");
	if(!_original_glTexCoord3dv) numFailed++;
	_original_glTexCoord3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTexCoord3f");
	if(!_original_glTexCoord3f) numFailed++;
	_original_glTexCoord3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord3fv");
	if(!_original_glTexCoord3fv) numFailed++;
	_original_glTexCoord3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glTexCoord3i");
	if(!_original_glTexCoord3i) numFailed++;
	_original_glTexCoord3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord3iv");
	if(!_original_glTexCoord3iv) numFailed++;
	_original_glTexCoord3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glTexCoord3s");
	if(!_original_glTexCoord3s) numFailed++;
	_original_glTexCoord3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord3sv");
	if(!_original_glTexCoord3sv) numFailed++;
	_original_glTexCoord4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTexCoord4d");
	if(!_original_glTexCoord4d) numFailed++;
	_original_glTexCoord4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glTexCoord4dv");
	if(!_original_glTexCoord4dv) numFailed++;
	_original_glTexCoord4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTexCoord4f");
	if(!_original_glTexCoord4f) numFailed++;
	_original_glTexCoord4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glTexCoord4fv");
	if(!_original_glTexCoord4fv) numFailed++;
	_original_glTexCoord4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glTexCoord4i");
	if(!_original_glTexCoord4i) numFailed++;
	_original_glTexCoord4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glTexCoord4iv");
	if(!_original_glTexCoord4iv) numFailed++;
	_original_glTexCoord4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glTexCoord4s");
	if(!_original_glTexCoord4s) numFailed++;
	_original_glTexCoord4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glTexCoord4sv");
	if(!_original_glTexCoord4sv) numFailed++;
	_original_glTexEnvf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexEnvf");
	if(!_original_glTexEnvf) numFailed++;
	_original_glTexEnvfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexEnvfv");
	if(!_original_glTexEnvfv) numFailed++;
	_original_glTexEnvi = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexEnvi");
	if(!_original_glTexEnvi) numFailed++;
	_original_glTexEnviv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexEnviv");
	if(!_original_glTexEnviv) numFailed++;
	_original_glTexGend = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLdouble))IntGetProcAddress("glTexGend");
	if(!_original_glTexGend) numFailed++;
	_original_glTexGendv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLdouble *))IntGetProcAddress("glTexGendv");
	if(!_original_glTexGendv) numFailed++;
	_original_glTexGenf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexGenf");
	if(!_original_glTexGenf) numFailed++;
	_original_glTexGenfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexGenfv");
	if(!_original_glTexGenfv) numFailed++;
	_original_glTexGeni = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexGeni");
	if(!_original_glTexGeni) numFailed++;
	_original_glTexGeniv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexGeniv");
	if(!_original_glTexGeniv) numFailed++;
	_original_glTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage1D");
	if(!_original_glTexImage1D) numFailed++;
	_original_glTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage2D");
	if(!_original_glTexImage2D) numFailed++;
	_original_glTexParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLfloat))IntGetProcAddress("glTexParameterf");
	if(!_original_glTexParameterf) numFailed++;
	_original_glTexParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLfloat *))IntGetProcAddress("glTexParameterfv");
	if(!_original_glTexParameterfv) numFailed++;
	_original_glTexParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glTexParameteri");
	if(!_original_glTexParameteri) numFailed++;
	_original_glTexParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameteriv");
	if(!_original_glTexParameteriv) numFailed++;
	_original_glTranslated = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glTranslated");
	if(!_original_glTranslated) numFailed++;
	_original_glTranslatef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glTranslatef");
	if(!_original_glTranslatef) numFailed++;
	_original_glVertex2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glVertex2d");
	if(!_original_glVertex2d) numFailed++;
	_original_glVertex2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex2dv");
	if(!_original_glVertex2dv) numFailed++;
	_original_glVertex2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glVertex2f");
	if(!_original_glVertex2f) numFailed++;
	_original_glVertex2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex2fv");
	if(!_original_glVertex2fv) numFailed++;
	_original_glVertex2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glVertex2i");
	if(!_original_glVertex2i) numFailed++;
	_original_glVertex2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex2iv");
	if(!_original_glVertex2iv) numFailed++;
	_original_glVertex2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glVertex2s");
	if(!_original_glVertex2s) numFailed++;
	_original_glVertex2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex2sv");
	if(!_original_glVertex2sv) numFailed++;
	_original_glVertex3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertex3d");
	if(!_original_glVertex3d) numFailed++;
	_original_glVertex3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex3dv");
	if(!_original_glVertex3dv) numFailed++;
	_original_glVertex3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertex3f");
	if(!_original_glVertex3f) numFailed++;
	_original_glVertex3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex3fv");
	if(!_original_glVertex3fv) numFailed++;
	_original_glVertex3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glVertex3i");
	if(!_original_glVertex3i) numFailed++;
	_original_glVertex3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex3iv");
	if(!_original_glVertex3iv) numFailed++;
	_original_glVertex3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glVertex3s");
	if(!_original_glVertex3s) numFailed++;
	_original_glVertex3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex3sv");
	if(!_original_glVertex3sv) numFailed++;
	_original_glVertex4d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertex4d");
	if(!_original_glVertex4d) numFailed++;
	_original_glVertex4dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glVertex4dv");
	if(!_original_glVertex4dv) numFailed++;
	_original_glVertex4f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertex4f");
	if(!_original_glVertex4f) numFailed++;
	_original_glVertex4fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glVertex4fv");
	if(!_original_glVertex4fv) numFailed++;
	_original_glVertex4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glVertex4i");
	if(!_original_glVertex4i) numFailed++;
	_original_glVertex4iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glVertex4iv");
	if(!_original_glVertex4iv) numFailed++;
	_original_glVertex4s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertex4s");
	if(!_original_glVertex4s) numFailed++;
	_original_glVertex4sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glVertex4sv");
	if(!_original_glVertex4sv) numFailed++;
	_original_glViewport = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glViewport");
	if(!_original_glViewport) numFailed++;
	_original_glAreTexturesResident = (GLboolean (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLboolean *))IntGetProcAddress("glAreTexturesResident");
	if(!_original_glAreTexturesResident) numFailed++;
	_original_glArrayElement = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glArrayElement");
	if(!_original_glArrayElement) numFailed++;
	_original_glBindTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTexture");
	if(!_original_glBindTexture) numFailed++;
	_original_glColorPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glColorPointer");
	if(!_original_glColorPointer) numFailed++;
	_original_glCopyTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint))IntGetProcAddress("glCopyTexImage1D");
	if(!_original_glCopyTexImage1D) numFailed++;
	_original_glCopyTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint))IntGetProcAddress("glCopyTexImage2D");
	if(!_original_glCopyTexImage2D) numFailed++;
	_original_glCopyTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei))IntGetProcAddress("glCopyTexSubImage1D");
	if(!_original_glCopyTexSubImage1D) numFailed++;
	_original_glCopyTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage2D");
	if(!_original_glCopyTexSubImage2D) numFailed++;
	_original_glDeleteTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTextures");
	if(!_original_glDeleteTextures) numFailed++;
	_original_glDisableClientState = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glDisableClientState");
	if(!_original_glDisableClientState) numFailed++;
	_original_glDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei))IntGetProcAddress("glDrawArrays");
	if(!_original_glDrawArrays) numFailed++;
	_original_glDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawElements");
	if(!_original_glDrawElements) numFailed++;
	_original_glEdgeFlagPointer = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLvoid *))IntGetProcAddress("glEdgeFlagPointer");
	if(!_original_glEdgeFlagPointer) numFailed++;
	_original_glEnableClientState = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnableClientState");
	if(!_original_glEnableClientState) numFailed++;
	_original_glGenTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTextures");
	if(!_original_glGenTextures) numFailed++;
	_original_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLvoid **))IntGetProcAddress("glGetPointerv");
	if(!_original_glGetPointerv) numFailed++;
	_original_glIndexPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glIndexPointer");
	if(!_original_glIndexPointer) numFailed++;
	_original_glIndexub = (void (CODEGEN_FUNCPTR *)(GLubyte))IntGetProcAddress("glIndexub");
	if(!_original_glIndexub) numFailed++;
	_original_glIndexubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glIndexubv");
	if(!_original_glIndexubv) numFailed++;
	_original_glInterleavedArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glInterleavedArrays");
	if(!_original_glInterleavedArrays) numFailed++;
	_original_glIsTexture = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTexture");
	if(!_original_glIsTexture) numFailed++;
	_original_glNormalPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glNormalPointer");
	if(!_original_glNormalPointer) numFailed++;
	_original_glPolygonOffset = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPolygonOffset");
	if(!_original_glPolygonOffset) numFailed++;
	_original_glPopClientAttrib = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopClientAttrib");
	if(!_original_glPopClientAttrib) numFailed++;
	_original_glPrioritizeTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, const GLfloat *))IntGetProcAddress("glPrioritizeTextures");
	if(!_original_glPrioritizeTextures) numFailed++;
	_original_glPushClientAttrib = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glPushClientAttrib");
	if(!_original_glPushClientAttrib) numFailed++;
	_original_glTexCoordPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glTexCoordPointer");
	if(!_original_glTexCoordPointer) numFailed++;
	_original_glTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage1D");
	if(!_original_glTexSubImage1D) numFailed++;
	_original_glTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage2D");
	if(!_original_glTexSubImage2D) numFailed++;
	_original_glVertexPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glVertexPointer");
	if(!_original_glVertexPointer) numFailed++;
	_original_glBlendColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glBlendColor");
	if(!_original_glBlendColor) numFailed++;
	_original_glBlendEquation = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBlendEquation");
	if(!_original_glBlendEquation) numFailed++;
	_original_glCopyTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glCopyTexSubImage3D");
	if(!_original_glCopyTexSubImage3D) numFailed++;
	_original_glDrawRangeElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawRangeElements");
	if(!_original_glDrawRangeElements) numFailed++;
	_original_glTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexImage3D");
	if(!_original_glTexImage3D) numFailed++;
	_original_glTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage3D");
	if(!_original_glTexSubImage3D) numFailed++;
	_original_glActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glActiveTexture");
	if(!_original_glActiveTexture) numFailed++;
	_original_glClientActiveTexture = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glClientActiveTexture");
	if(!_original_glClientActiveTexture) numFailed++;
	_original_glCompressedTexImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage1D");
	if(!_original_glCompressedTexImage1D) numFailed++;
	_original_glCompressedTexImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage2D");
	if(!_original_glCompressedTexImage2D) numFailed++;
	_original_glCompressedTexImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexImage3D");
	if(!_original_glCompressedTexImage3D) numFailed++;
	_original_glCompressedTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage1D");
	if(!_original_glCompressedTexSubImage1D) numFailed++;
	_original_glCompressedTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage2D");
	if(!_original_glCompressedTexSubImage2D) numFailed++;
	_original_glCompressedTexSubImage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glCompressedTexSubImage3D");
	if(!_original_glCompressedTexSubImage3D) numFailed++;
	_original_glGetCompressedTexImage = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLvoid *))IntGetProcAddress("glGetCompressedTexImage");
	if(!_original_glGetCompressedTexImage) numFailed++;
	_original_glLoadTransposeMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glLoadTransposeMatrixd");
	if(!_original_glLoadTransposeMatrixd) numFailed++;
	_original_glLoadTransposeMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glLoadTransposeMatrixf");
	if(!_original_glLoadTransposeMatrixf) numFailed++;
	_original_glMultTransposeMatrixd = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glMultTransposeMatrixd");
	if(!_original_glMultTransposeMatrixd) numFailed++;
	_original_glMultTransposeMatrixf = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glMultTransposeMatrixf");
	if(!_original_glMultTransposeMatrixf) numFailed++;
	_original_glMultiTexCoord1d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble))IntGetProcAddress("glMultiTexCoord1d");
	if(!_original_glMultiTexCoord1d) numFailed++;
	_original_glMultiTexCoord1dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord1dv");
	if(!_original_glMultiTexCoord1dv) numFailed++;
	_original_glMultiTexCoord1f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glMultiTexCoord1f");
	if(!_original_glMultiTexCoord1f) numFailed++;
	_original_glMultiTexCoord1fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord1fv");
	if(!_original_glMultiTexCoord1fv) numFailed++;
	_original_glMultiTexCoord1i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glMultiTexCoord1i");
	if(!_original_glMultiTexCoord1i) numFailed++;
	_original_glMultiTexCoord1iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord1iv");
	if(!_original_glMultiTexCoord1iv) numFailed++;
	_original_glMultiTexCoord1s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort))IntGetProcAddress("glMultiTexCoord1s");
	if(!_original_glMultiTexCoord1s) numFailed++;
	_original_glMultiTexCoord1sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord1sv");
	if(!_original_glMultiTexCoord1sv) numFailed++;
	_original_glMultiTexCoord2d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord2d");
	if(!_original_glMultiTexCoord2d) numFailed++;
	_original_glMultiTexCoord2dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord2dv");
	if(!_original_glMultiTexCoord2dv) numFailed++;
	_original_glMultiTexCoord2f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord2f");
	if(!_original_glMultiTexCoord2f) numFailed++;
	_original_glMultiTexCoord2fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord2fv");
	if(!_original_glMultiTexCoord2fv) numFailed++;
	_original_glMultiTexCoord2i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint))IntGetProcAddress("glMultiTexCoord2i");
	if(!_original_glMultiTexCoord2i) numFailed++;
	_original_glMultiTexCoord2iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord2iv");
	if(!_original_glMultiTexCoord2iv) numFailed++;
	_original_glMultiTexCoord2s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord2s");
	if(!_original_glMultiTexCoord2s) numFailed++;
	_original_glMultiTexCoord2sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord2sv");
	if(!_original_glMultiTexCoord2sv) numFailed++;
	_original_glMultiTexCoord3d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord3d");
	if(!_original_glMultiTexCoord3d) numFailed++;
	_original_glMultiTexCoord3dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord3dv");
	if(!_original_glMultiTexCoord3dv) numFailed++;
	_original_glMultiTexCoord3f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord3f");
	if(!_original_glMultiTexCoord3f) numFailed++;
	_original_glMultiTexCoord3fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord3fv");
	if(!_original_glMultiTexCoord3fv) numFailed++;
	_original_glMultiTexCoord3i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint))IntGetProcAddress("glMultiTexCoord3i");
	if(!_original_glMultiTexCoord3i) numFailed++;
	_original_glMultiTexCoord3iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord3iv");
	if(!_original_glMultiTexCoord3iv) numFailed++;
	_original_glMultiTexCoord3s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord3s");
	if(!_original_glMultiTexCoord3s) numFailed++;
	_original_glMultiTexCoord3sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord3sv");
	if(!_original_glMultiTexCoord3sv) numFailed++;
	_original_glMultiTexCoord4d = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glMultiTexCoord4d");
	if(!_original_glMultiTexCoord4d) numFailed++;
	_original_glMultiTexCoord4dv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLdouble *))IntGetProcAddress("glMultiTexCoord4dv");
	if(!_original_glMultiTexCoord4dv) numFailed++;
	_original_glMultiTexCoord4f = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glMultiTexCoord4f");
	if(!_original_glMultiTexCoord4f) numFailed++;
	_original_glMultiTexCoord4fv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glMultiTexCoord4fv");
	if(!_original_glMultiTexCoord4fv) numFailed++;
	_original_glMultiTexCoord4i = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLint))IntGetProcAddress("glMultiTexCoord4i");
	if(!_original_glMultiTexCoord4i) numFailed++;
	_original_glMultiTexCoord4iv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glMultiTexCoord4iv");
	if(!_original_glMultiTexCoord4iv) numFailed++;
	_original_glMultiTexCoord4s = (void (CODEGEN_FUNCPTR *)(GLenum, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glMultiTexCoord4s");
	if(!_original_glMultiTexCoord4s) numFailed++;
	_original_glMultiTexCoord4sv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLshort *))IntGetProcAddress("glMultiTexCoord4sv");
	if(!_original_glMultiTexCoord4sv) numFailed++;
	_original_glSampleCoverage = (void (CODEGEN_FUNCPTR *)(GLfloat, GLboolean))IntGetProcAddress("glSampleCoverage");
	if(!_original_glSampleCoverage) numFailed++;
	_original_glBlendFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparate");
	if(!_original_glBlendFuncSeparate) numFailed++;
	_original_glFogCoordPointer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glFogCoordPointer");
	if(!_original_glFogCoordPointer) numFailed++;
	_original_glFogCoordd = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glFogCoordd");
	if(!_original_glFogCoordd) numFailed++;
	_original_glFogCoorddv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glFogCoorddv");
	if(!_original_glFogCoorddv) numFailed++;
	_original_glFogCoordf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glFogCoordf");
	if(!_original_glFogCoordf) numFailed++;
	_original_glFogCoordfv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glFogCoordfv");
	if(!_original_glFogCoordfv) numFailed++;
	_original_glMultiDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *, const GLsizei *, GLsizei))IntGetProcAddress("glMultiDrawArrays");
	if(!_original_glMultiDrawArrays) numFailed++;
	_original_glMultiDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei))IntGetProcAddress("glMultiDrawElements");
	if(!_original_glMultiDrawElements) numFailed++;
	_original_glPointParameterf = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPointParameterf");
	if(!_original_glPointParameterf) numFailed++;
	_original_glPointParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPointParameterfv");
	if(!_original_glPointParameterfv) numFailed++;
	_original_glPointParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPointParameteri");
	if(!_original_glPointParameteri) numFailed++;
	_original_glPointParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLint *))IntGetProcAddress("glPointParameteriv");
	if(!_original_glPointParameteriv) numFailed++;
	_original_glSecondaryColor3b = (void (CODEGEN_FUNCPTR *)(GLbyte, GLbyte, GLbyte))IntGetProcAddress("glSecondaryColor3b");
	if(!_original_glSecondaryColor3b) numFailed++;
	_original_glSecondaryColor3bv = (void (CODEGEN_FUNCPTR *)(const GLbyte *))IntGetProcAddress("glSecondaryColor3bv");
	if(!_original_glSecondaryColor3bv) numFailed++;
	_original_glSecondaryColor3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glSecondaryColor3d");
	if(!_original_glSecondaryColor3d) numFailed++;
	_original_glSecondaryColor3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glSecondaryColor3dv");
	if(!_original_glSecondaryColor3dv) numFailed++;
	_original_glSecondaryColor3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glSecondaryColor3f");
	if(!_original_glSecondaryColor3f) numFailed++;
	_original_glSecondaryColor3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glSecondaryColor3fv");
	if(!_original_glSecondaryColor3fv) numFailed++;
	_original_glSecondaryColor3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glSecondaryColor3i");
	if(!_original_glSecondaryColor3i) numFailed++;
	_original_glSecondaryColor3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glSecondaryColor3iv");
	if(!_original_glSecondaryColor3iv) numFailed++;
	_original_glSecondaryColor3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glSecondaryColor3s");
	if(!_original_glSecondaryColor3s) numFailed++;
	_original_glSecondaryColor3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glSecondaryColor3sv");
	if(!_original_glSecondaryColor3sv) numFailed++;
	_original_glSecondaryColor3ub = (void (CODEGEN_FUNCPTR *)(GLubyte, GLubyte, GLubyte))IntGetProcAddress("glSecondaryColor3ub");
	if(!_original_glSecondaryColor3ub) numFailed++;
	_original_glSecondaryColor3ubv = (void (CODEGEN_FUNCPTR *)(const GLubyte *))IntGetProcAddress("glSecondaryColor3ubv");
	if(!_original_glSecondaryColor3ubv) numFailed++;
	_original_glSecondaryColor3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glSecondaryColor3ui");
	if(!_original_glSecondaryColor3ui) numFailed++;
	_original_glSecondaryColor3uiv = (void (CODEGEN_FUNCPTR *)(const GLuint *))IntGetProcAddress("glSecondaryColor3uiv");
	if(!_original_glSecondaryColor3uiv) numFailed++;
	_original_glSecondaryColor3us = (void (CODEGEN_FUNCPTR *)(GLushort, GLushort, GLushort))IntGetProcAddress("glSecondaryColor3us");
	if(!_original_glSecondaryColor3us) numFailed++;
	_original_glSecondaryColor3usv = (void (CODEGEN_FUNCPTR *)(const GLushort *))IntGetProcAddress("glSecondaryColor3usv");
	if(!_original_glSecondaryColor3usv) numFailed++;
	_original_glSecondaryColorPointer = (void (CODEGEN_FUNCPTR *)(GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glSecondaryColorPointer");
	if(!_original_glSecondaryColorPointer) numFailed++;
	_original_glWindowPos2d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble))IntGetProcAddress("glWindowPos2d");
	if(!_original_glWindowPos2d) numFailed++;
	_original_glWindowPos2dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glWindowPos2dv");
	if(!_original_glWindowPos2dv) numFailed++;
	_original_glWindowPos2f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glWindowPos2f");
	if(!_original_glWindowPos2f) numFailed++;
	_original_glWindowPos2fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glWindowPos2fv");
	if(!_original_glWindowPos2fv) numFailed++;
	_original_glWindowPos2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glWindowPos2i");
	if(!_original_glWindowPos2i) numFailed++;
	_original_glWindowPos2iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glWindowPos2iv");
	if(!_original_glWindowPos2iv) numFailed++;
	_original_glWindowPos2s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort))IntGetProcAddress("glWindowPos2s");
	if(!_original_glWindowPos2s) numFailed++;
	_original_glWindowPos2sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glWindowPos2sv");
	if(!_original_glWindowPos2sv) numFailed++;
	_original_glWindowPos3d = (void (CODEGEN_FUNCPTR *)(GLdouble, GLdouble, GLdouble))IntGetProcAddress("glWindowPos3d");
	if(!_original_glWindowPos3d) numFailed++;
	_original_glWindowPos3dv = (void (CODEGEN_FUNCPTR *)(const GLdouble *))IntGetProcAddress("glWindowPos3dv");
	if(!_original_glWindowPos3dv) numFailed++;
	_original_glWindowPos3f = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat))IntGetProcAddress("glWindowPos3f");
	if(!_original_glWindowPos3f) numFailed++;
	_original_glWindowPos3fv = (void (CODEGEN_FUNCPTR *)(const GLfloat *))IntGetProcAddress("glWindowPos3fv");
	if(!_original_glWindowPos3fv) numFailed++;
	_original_glWindowPos3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glWindowPos3i");
	if(!_original_glWindowPos3i) numFailed++;
	_original_glWindowPos3iv = (void (CODEGEN_FUNCPTR *)(const GLint *))IntGetProcAddress("glWindowPos3iv");
	if(!_original_glWindowPos3iv) numFailed++;
	_original_glWindowPos3s = (void (CODEGEN_FUNCPTR *)(GLshort, GLshort, GLshort))IntGetProcAddress("glWindowPos3s");
	if(!_original_glWindowPos3s) numFailed++;
	_original_glWindowPos3sv = (void (CODEGEN_FUNCPTR *)(const GLshort *))IntGetProcAddress("glWindowPos3sv");
	if(!_original_glWindowPos3sv) numFailed++;
	_original_glBeginQuery = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBeginQuery");
	if(!_original_glBeginQuery) numFailed++;
	_original_glBindBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindBuffer");
	if(!_original_glBindBuffer) numFailed++;
	_original_glBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizeiptr, const GLvoid *, GLenum))IntGetProcAddress("glBufferData");
	if(!_original_glBufferData) numFailed++;
	_original_glBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, const GLvoid *))IntGetProcAddress("glBufferSubData");
	if(!_original_glBufferSubData) numFailed++;
	_original_glDeleteBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteBuffers");
	if(!_original_glDeleteBuffers) numFailed++;
	_original_glDeleteQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteQueries");
	if(!_original_glDeleteQueries) numFailed++;
	_original_glEndQuery = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEndQuery");
	if(!_original_glEndQuery) numFailed++;
	_original_glGenBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenBuffers");
	if(!_original_glGenBuffers) numFailed++;
	_original_glGenQueries = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenQueries");
	if(!_original_glGenQueries) numFailed++;
	_original_glGetBufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetBufferParameteriv");
	if(!_original_glGetBufferParameteriv) numFailed++;
	_original_glGetBufferPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLvoid **))IntGetProcAddress("glGetBufferPointerv");
	if(!_original_glGetBufferPointerv) numFailed++;
	_original_glGetBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLvoid *))IntGetProcAddress("glGetBufferSubData");
	if(!_original_glGetBufferSubData) numFailed++;
	_original_glGetQueryObjectiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryObjectiv");
	if(!_original_glGetQueryObjectiv) numFailed++;
	_original_glGetQueryObjectuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetQueryObjectuiv");
	if(!_original_glGetQueryObjectuiv) numFailed++;
	_original_glGetQueryiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetQueryiv");
	if(!_original_glGetQueryiv) numFailed++;
	_original_glIsBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsBuffer");
	if(!_original_glIsBuffer) numFailed++;
	_original_glIsQuery = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsQuery");
	if(!_original_glIsQuery) numFailed++;
	_original_glMapBuffer = (void * (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glMapBuffer");
	if(!_original_glMapBuffer) numFailed++;
	_original_glUnmapBuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glUnmapBuffer");
	if(!_original_glUnmapBuffer) numFailed++;
	_original_glAttachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glAttachShader");
	if(!_original_glAttachShader) numFailed++;
	_original_glBindAttribLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindAttribLocation");
	if(!_original_glBindAttribLocation) numFailed++;
	_original_glBlendEquationSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparate");
	if(!_original_glBlendEquationSeparate) numFailed++;
	_original_glCompileShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glCompileShader");
	if(!_original_glCompileShader) numFailed++;
	_original_glCreateProgram = (GLuint (CODEGEN_FUNCPTR *)())IntGetProcAddress("glCreateProgram");
	if(!_original_glCreateProgram) numFailed++;
	_original_glCreateShader = (GLuint (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCreateShader");
	if(!_original_glCreateShader) numFailed++;
	_original_glDeleteProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteProgram");
	if(!_original_glDeleteProgram) numFailed++;
	_original_glDeleteShader = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDeleteShader");
	if(!_original_glDeleteShader) numFailed++;
	_original_glDetachShader = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glDetachShader");
	if(!_original_glDetachShader) numFailed++;
	_original_glDisableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glDisableVertexAttribArray");
	if(!_original_glDisableVertexAttribArray) numFailed++;
	_original_glDrawBuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLenum *))IntGetProcAddress("glDrawBuffers");
	if(!_original_glDrawBuffers) numFailed++;
	_original_glEnableVertexAttribArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glEnableVertexAttribArray");
	if(!_original_glEnableVertexAttribArray) numFailed++;
	_original_glGetActiveAttrib = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveAttrib");
	if(!_original_glGetActiveAttrib) numFailed++;
	_original_glGetActiveUniform = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))IntGetProcAddress("glGetActiveUniform");
	if(!_original_glGetActiveUniform) numFailed++;
	_original_glGetAttachedShaders = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLuint *))IntGetProcAddress("glGetAttachedShaders");
	if(!_original_glGetAttachedShaders) numFailed++;
	_original_glGetAttribLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetAttribLocation");
	if(!_original_glGetAttribLocation) numFailed++;
	_original_glGetProgramInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramInfoLog");
	if(!_original_glGetProgramInfoLog) numFailed++;
	_original_glGetProgramiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramiv");
	if(!_original_glGetProgramiv) numFailed++;
	_original_glGetShaderInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderInfoLog");
	if(!_original_glGetShaderInfoLog) numFailed++;
	_original_glGetShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetShaderSource");
	if(!_original_glGetShaderSource) numFailed++;
	_original_glGetShaderiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetShaderiv");
	if(!_original_glGetShaderiv) numFailed++;
	_original_glGetUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformLocation");
	if(!_original_glGetUniformLocation) numFailed++;
	_original_glGetUniformfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat *))IntGetProcAddress("glGetUniformfv");
	if(!_original_glGetUniformfv) numFailed++;
	_original_glGetUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint *))IntGetProcAddress("glGetUniformiv");
	if(!_original_glGetUniformiv) numFailed++;
	_original_glGetVertexAttribPointerv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLvoid **))IntGetProcAddress("glGetVertexAttribPointerv");
	if(!_original_glGetVertexAttribPointerv) numFailed++;
	_original_glGetVertexAttribdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribdv");
	if(!_original_glGetVertexAttribdv) numFailed++;
	_original_glGetVertexAttribfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetVertexAttribfv");
	if(!_original_glGetVertexAttribfv) numFailed++;
	_original_glGetVertexAttribiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribiv");
	if(!_original_glGetVertexAttribiv) numFailed++;
	_original_glIsProgram = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgram");
	if(!_original_glIsProgram) numFailed++;
	_original_glIsShader = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsShader");
	if(!_original_glIsShader) numFailed++;
	_original_glLinkProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glLinkProgram");
	if(!_original_glLinkProgram) numFailed++;
	_original_glShaderSource = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, const GLint *))IntGetProcAddress("glShaderSource");
	if(!_original_glShaderSource) numFailed++;
	_original_glStencilFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint, GLuint))IntGetProcAddress("glStencilFuncSeparate");
	if(!_original_glStencilFuncSeparate) numFailed++;
	_original_glStencilMaskSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glStencilMaskSeparate");
	if(!_original_glStencilMaskSeparate) numFailed++;
	_original_glStencilOpSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOpSeparate");
	if(!_original_glStencilOpSeparate) numFailed++;
	_original_glUniform1f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat))IntGetProcAddress("glUniform1f");
	if(!_original_glUniform1f) numFailed++;
	_original_glUniform1fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform1fv");
	if(!_original_glUniform1fv) numFailed++;
	_original_glUniform1i = (void (CODEGEN_FUNCPTR *)(GLint, GLint))IntGetProcAddress("glUniform1i");
	if(!_original_glUniform1i) numFailed++;
	_original_glUniform1iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform1iv");
	if(!_original_glUniform1iv) numFailed++;
	_original_glUniform2f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat))IntGetProcAddress("glUniform2f");
	if(!_original_glUniform2f) numFailed++;
	_original_glUniform2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform2fv");
	if(!_original_glUniform2fv) numFailed++;
	_original_glUniform2i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint))IntGetProcAddress("glUniform2i");
	if(!_original_glUniform2i) numFailed++;
	_original_glUniform2iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform2iv");
	if(!_original_glUniform2iv) numFailed++;
	_original_glUniform3f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform3f");
	if(!_original_glUniform3f) numFailed++;
	_original_glUniform3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform3fv");
	if(!_original_glUniform3fv) numFailed++;
	_original_glUniform3i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform3i");
	if(!_original_glUniform3i) numFailed++;
	_original_glUniform3iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform3iv");
	if(!_original_glUniform3iv) numFailed++;
	_original_glUniform4f = (void (CODEGEN_FUNCPTR *)(GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glUniform4f");
	if(!_original_glUniform4f) numFailed++;
	_original_glUniform4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLfloat *))IntGetProcAddress("glUniform4fv");
	if(!_original_glUniform4fv) numFailed++;
	_original_glUniform4i = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glUniform4i");
	if(!_original_glUniform4i) numFailed++;
	_original_glUniform4iv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLint *))IntGetProcAddress("glUniform4iv");
	if(!_original_glUniform4iv) numFailed++;
	_original_glUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2fv");
	if(!_original_glUniformMatrix2fv) numFailed++;
	_original_glUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3fv");
	if(!_original_glUniformMatrix3fv) numFailed++;
	_original_glUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4fv");
	if(!_original_glUniformMatrix4fv) numFailed++;
	_original_glUseProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glUseProgram");
	if(!_original_glUseProgram) numFailed++;
	_original_glValidateProgram = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgram");
	if(!_original_glValidateProgram) numFailed++;
	_original_glVertexAttrib1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttrib1d");
	if(!_original_glVertexAttrib1d) numFailed++;
	_original_glVertexAttrib1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib1dv");
	if(!_original_glVertexAttrib1dv) numFailed++;
	_original_glVertexAttrib1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat))IntGetProcAddress("glVertexAttrib1f");
	if(!_original_glVertexAttrib1f) numFailed++;
	_original_glVertexAttrib1fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib1fv");
	if(!_original_glVertexAttrib1fv) numFailed++;
	_original_glVertexAttrib1s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort))IntGetProcAddress("glVertexAttrib1s");
	if(!_original_glVertexAttrib1s) numFailed++;
	_original_glVertexAttrib1sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib1sv");
	if(!_original_glVertexAttrib1sv) numFailed++;
	_original_glVertexAttrib2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib2d");
	if(!_original_glVertexAttrib2d) numFailed++;
	_original_glVertexAttrib2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib2dv");
	if(!_original_glVertexAttrib2dv) numFailed++;
	_original_glVertexAttrib2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib2f");
	if(!_original_glVertexAttrib2f) numFailed++;
	_original_glVertexAttrib2fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib2fv");
	if(!_original_glVertexAttrib2fv) numFailed++;
	_original_glVertexAttrib2s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort))IntGetProcAddress("glVertexAttrib2s");
	if(!_original_glVertexAttrib2s) numFailed++;
	_original_glVertexAttrib2sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib2sv");
	if(!_original_glVertexAttrib2sv) numFailed++;
	_original_glVertexAttrib3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib3d");
	if(!_original_glVertexAttrib3d) numFailed++;
	_original_glVertexAttrib3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib3dv");
	if(!_original_glVertexAttrib3dv) numFailed++;
	_original_glVertexAttrib3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib3f");
	if(!_original_glVertexAttrib3f) numFailed++;
	_original_glVertexAttrib3fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib3fv");
	if(!_original_glVertexAttrib3fv) numFailed++;
	_original_glVertexAttrib3s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib3s");
	if(!_original_glVertexAttrib3s) numFailed++;
	_original_glVertexAttrib3sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib3sv");
	if(!_original_glVertexAttrib3sv) numFailed++;
	_original_glVertexAttrib4Nbv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4Nbv");
	if(!_original_glVertexAttrib4Nbv) numFailed++;
	_original_glVertexAttrib4Niv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4Niv");
	if(!_original_glVertexAttrib4Niv) numFailed++;
	_original_glVertexAttrib4Nsv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4Nsv");
	if(!_original_glVertexAttrib4Nsv) numFailed++;
	_original_glVertexAttrib4Nub = (void (CODEGEN_FUNCPTR *)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte))IntGetProcAddress("glVertexAttrib4Nub");
	if(!_original_glVertexAttrib4Nub) numFailed++;
	_original_glVertexAttrib4Nubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4Nubv");
	if(!_original_glVertexAttrib4Nubv) numFailed++;
	_original_glVertexAttrib4Nuiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4Nuiv");
	if(!_original_glVertexAttrib4Nuiv) numFailed++;
	_original_glVertexAttrib4Nusv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4Nusv");
	if(!_original_glVertexAttrib4Nusv) numFailed++;
	_original_glVertexAttrib4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttrib4bv");
	if(!_original_glVertexAttrib4bv) numFailed++;
	_original_glVertexAttrib4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttrib4d");
	if(!_original_glVertexAttrib4d) numFailed++;
	_original_glVertexAttrib4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttrib4dv");
	if(!_original_glVertexAttrib4dv) numFailed++;
	_original_glVertexAttrib4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glVertexAttrib4f");
	if(!_original_glVertexAttrib4f) numFailed++;
	_original_glVertexAttrib4fv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glVertexAttrib4fv");
	if(!_original_glVertexAttrib4fv) numFailed++;
	_original_glVertexAttrib4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttrib4iv");
	if(!_original_glVertexAttrib4iv) numFailed++;
	_original_glVertexAttrib4s = (void (CODEGEN_FUNCPTR *)(GLuint, GLshort, GLshort, GLshort, GLshort))IntGetProcAddress("glVertexAttrib4s");
	if(!_original_glVertexAttrib4s) numFailed++;
	_original_glVertexAttrib4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttrib4sv");
	if(!_original_glVertexAttrib4sv) numFailed++;
	_original_glVertexAttrib4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttrib4ubv");
	if(!_original_glVertexAttrib4ubv) numFailed++;
	_original_glVertexAttrib4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttrib4uiv");
	if(!_original_glVertexAttrib4uiv) numFailed++;
	_original_glVertexAttrib4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttrib4usv");
	if(!_original_glVertexAttrib4usv) numFailed++;
	_original_glVertexAttribPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribPointer");
	if(!_original_glVertexAttribPointer) numFailed++;
	_original_glUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x3fv");
	if(!_original_glUniformMatrix2x3fv) numFailed++;
	_original_glUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix2x4fv");
	if(!_original_glUniformMatrix2x4fv) numFailed++;
	_original_glUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x2fv");
	if(!_original_glUniformMatrix3x2fv) numFailed++;
	_original_glUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix3x4fv");
	if(!_original_glUniformMatrix3x4fv) numFailed++;
	_original_glUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x2fv");
	if(!_original_glUniformMatrix4x2fv) numFailed++;
	_original_glUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glUniformMatrix4x3fv");
	if(!_original_glUniformMatrix4x3fv) numFailed++;
	_original_glBeginConditionalRender = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glBeginConditionalRender");
	if(!_original_glBeginConditionalRender) numFailed++;
	_original_glBeginTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glBeginTransformFeedback");
	if(!_original_glBeginTransformFeedback) numFailed++;
	_original_glBindBufferBase = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glBindBufferBase");
	if(!_original_glBindBufferBase) numFailed++;
	_original_glBindBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glBindBufferRange");
	if(!_original_glBindBufferRange) numFailed++;
	_original_glBindFragDataLocation = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindFragDataLocation");
	if(!_original_glBindFragDataLocation) numFailed++;
	_original_glBindFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindFramebuffer");
	if(!_original_glBindFramebuffer) numFailed++;
	_original_glBindRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindRenderbuffer");
	if(!_original_glBindRenderbuffer) numFailed++;
	_original_glBindVertexArray = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindVertexArray");
	if(!_original_glBindVertexArray) numFailed++;
	_original_glBlitFramebuffer = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))IntGetProcAddress("glBlitFramebuffer");
	if(!_original_glBlitFramebuffer) numFailed++;
	_original_glCheckFramebufferStatus = (GLenum (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCheckFramebufferStatus");
	if(!_original_glCheckFramebufferStatus) numFailed++;
	_original_glClampColor = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glClampColor");
	if(!_original_glClampColor) numFailed++;
	_original_glClearBufferfi = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLfloat, GLint))IntGetProcAddress("glClearBufferfi");
	if(!_original_glClearBufferfi) numFailed++;
	_original_glClearBufferfv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLfloat *))IntGetProcAddress("glClearBufferfv");
	if(!_original_glClearBufferfv) numFailed++;
	_original_glClearBufferiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLint *))IntGetProcAddress("glClearBufferiv");
	if(!_original_glClearBufferiv) numFailed++;
	_original_glClearBufferuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, const GLuint *))IntGetProcAddress("glClearBufferuiv");
	if(!_original_glClearBufferuiv) numFailed++;
	_original_glColorMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMaski");
	if(!_original_glColorMaski) numFailed++;
	_original_glDeleteFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteFramebuffers");
	if(!_original_glDeleteFramebuffers) numFailed++;
	_original_glDeleteRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteRenderbuffers");
	if(!_original_glDeleteRenderbuffers) numFailed++;
	_original_glDeleteVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteVertexArrays");
	if(!_original_glDeleteVertexArrays) numFailed++;
	_original_glDisablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glDisablei");
	if(!_original_glDisablei) numFailed++;
	_original_glEnablei = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glEnablei");
	if(!_original_glEnablei) numFailed++;
	_original_glEndConditionalRender = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndConditionalRender");
	if(!_original_glEndConditionalRender) numFailed++;
	_original_glEndTransformFeedback = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glEndTransformFeedback");
	if(!_original_glEndTransformFeedback) numFailed++;
	_original_glFlushMappedBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr))IntGetProcAddress("glFlushMappedBufferRange");
	if(!_original_glFlushMappedBufferRange) numFailed++;
	_original_glFramebufferRenderbuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint))IntGetProcAddress("glFramebufferRenderbuffer");
	if(!_original_glFramebufferRenderbuffer) numFailed++;
	_original_glFramebufferTexture1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture1D");
	if(!_original_glFramebufferTexture1D) numFailed++;
	_original_glFramebufferTexture2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture2D");
	if(!_original_glFramebufferTexture2D) numFailed++;
	_original_glFramebufferTexture3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTexture3D");
	if(!_original_glFramebufferTexture3D) numFailed++;
	_original_glFramebufferTextureLayer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTextureLayer");
	if(!_original_glFramebufferTextureLayer) numFailed++;
	_original_glGenFramebuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenFramebuffers");
	if(!_original_glGenFramebuffers) numFailed++;
	_original_glGenRenderbuffers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenRenderbuffers");
	if(!_original_glGenRenderbuffers) numFailed++;
	_original_glGenVertexArrays = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenVertexArrays");
	if(!_original_glGenVertexArrays) numFailed++;
	_original_glGenerateMipmap = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGenerateMipmap");
	if(!_original_glGenerateMipmap) numFailed++;
	_original_glGetBooleani_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLboolean *))IntGetProcAddress("glGetBooleani_v");
	if(!_original_glGetBooleani_v) numFailed++;
	_original_glGetFragDataLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetFragDataLocation");
	if(!_original_glGetFragDataLocation) numFailed++;
	_original_glGetFramebufferAttachmentParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferAttachmentParameteriv");
	if(!_original_glGetFramebufferAttachmentParameteriv) numFailed++;
	_original_glGetIntegeri_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint *))IntGetProcAddress("glGetIntegeri_v");
	if(!_original_glGetIntegeri_v) numFailed++;
	_original_glGetRenderbufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetRenderbufferParameteriv");
	if(!_original_glGetRenderbufferParameteriv) numFailed++;
	_original_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!_original_glGetStringi) numFailed++;
	_original_glGetTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetTexParameterIiv");
	if(!_original_glGetTexParameterIiv) numFailed++;
	_original_glGetTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint *))IntGetProcAddress("glGetTexParameterIuiv");
	if(!_original_glGetTexParameterIuiv) numFailed++;
	_original_glGetTransformFeedbackVarying = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *))IntGetProcAddress("glGetTransformFeedbackVarying");
	if(!_original_glGetTransformFeedbackVarying) numFailed++;
	_original_glGetUniformuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint *))IntGetProcAddress("glGetUniformuiv");
	if(!_original_glGetUniformuiv) numFailed++;
	_original_glGetVertexAttribIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetVertexAttribIiv");
	if(!_original_glGetVertexAttribIiv) numFailed++;
	_original_glGetVertexAttribIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetVertexAttribIuiv");
	if(!_original_glGetVertexAttribIuiv) numFailed++;
	_original_glIsEnabledi = (GLboolean (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glIsEnabledi");
	if(!_original_glIsEnabledi) numFailed++;
	_original_glIsFramebuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsFramebuffer");
	if(!_original_glIsFramebuffer) numFailed++;
	_original_glIsRenderbuffer = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsRenderbuffer");
	if(!_original_glIsRenderbuffer) numFailed++;
	_original_glIsVertexArray = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsVertexArray");
	if(!_original_glIsVertexArray) numFailed++;
	_original_glMapBufferRange = (void * (CODEGEN_FUNCPTR *)(GLenum, GLintptr, GLsizeiptr, GLbitfield))IntGetProcAddress("glMapBufferRange");
	if(!_original_glMapBufferRange) numFailed++;
	_original_glRenderbufferStorage = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorage");
	if(!_original_glRenderbufferStorage) numFailed++;
	_original_glRenderbufferStorageMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glRenderbufferStorageMultisample");
	if(!_original_glRenderbufferStorageMultisample) numFailed++;
	_original_glTexParameterIiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLint *))IntGetProcAddress("glTexParameterIiv");
	if(!_original_glTexParameterIiv) numFailed++;
	_original_glTexParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLuint *))IntGetProcAddress("glTexParameterIuiv");
	if(!_original_glTexParameterIuiv) numFailed++;
	_original_glTransformFeedbackVaryings = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLenum))IntGetProcAddress("glTransformFeedbackVaryings");
	if(!_original_glTransformFeedbackVaryings) numFailed++;
	_original_glUniform1ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint))IntGetProcAddress("glUniform1ui");
	if(!_original_glUniform1ui) numFailed++;
	_original_glUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform1uiv");
	if(!_original_glUniform1uiv) numFailed++;
	_original_glUniform2ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint))IntGetProcAddress("glUniform2ui");
	if(!_original_glUniform2ui) numFailed++;
	_original_glUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform2uiv");
	if(!_original_glUniform2uiv) numFailed++;
	_original_glUniform3ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform3ui");
	if(!_original_glUniform3ui) numFailed++;
	_original_glUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform3uiv");
	if(!_original_glUniform3uiv) numFailed++;
	_original_glUniform4ui = (void (CODEGEN_FUNCPTR *)(GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glUniform4ui");
	if(!_original_glUniform4ui) numFailed++;
	_original_glUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLuint *))IntGetProcAddress("glUniform4uiv");
	if(!_original_glUniform4uiv) numFailed++;
	_original_glVertexAttribI1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glVertexAttribI1i");
	if(!_original_glVertexAttribI1i) numFailed++;
	_original_glVertexAttribI1iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI1iv");
	if(!_original_glVertexAttribI1iv) numFailed++;
	_original_glVertexAttribI1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribI1ui");
	if(!_original_glVertexAttribI1ui) numFailed++;
	_original_glVertexAttribI1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI1uiv");
	if(!_original_glVertexAttribI1uiv) numFailed++;
	_original_glVertexAttribI2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glVertexAttribI2i");
	if(!_original_glVertexAttribI2i) numFailed++;
	_original_glVertexAttribI2iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI2iv");
	if(!_original_glVertexAttribI2iv) numFailed++;
	_original_glVertexAttribI2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI2ui");
	if(!_original_glVertexAttribI2ui) numFailed++;
	_original_glVertexAttribI2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI2uiv");
	if(!_original_glVertexAttribI2uiv) numFailed++;
	_original_glVertexAttribI3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI3i");
	if(!_original_glVertexAttribI3i) numFailed++;
	_original_glVertexAttribI3iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI3iv");
	if(!_original_glVertexAttribI3iv) numFailed++;
	_original_glVertexAttribI3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI3ui");
	if(!_original_glVertexAttribI3ui) numFailed++;
	_original_glVertexAttribI3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI3uiv");
	if(!_original_glVertexAttribI3uiv) numFailed++;
	_original_glVertexAttribI4bv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLbyte *))IntGetProcAddress("glVertexAttribI4bv");
	if(!_original_glVertexAttribI4bv) numFailed++;
	_original_glVertexAttribI4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glVertexAttribI4i");
	if(!_original_glVertexAttribI4i) numFailed++;
	_original_glVertexAttribI4iv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glVertexAttribI4iv");
	if(!_original_glVertexAttribI4iv) numFailed++;
	_original_glVertexAttribI4sv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLshort *))IntGetProcAddress("glVertexAttribI4sv");
	if(!_original_glVertexAttribI4sv) numFailed++;
	_original_glVertexAttribI4ubv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLubyte *))IntGetProcAddress("glVertexAttribI4ubv");
	if(!_original_glVertexAttribI4ubv) numFailed++;
	_original_glVertexAttribI4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glVertexAttribI4ui");
	if(!_original_glVertexAttribI4ui) numFailed++;
	_original_glVertexAttribI4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLuint *))IntGetProcAddress("glVertexAttribI4uiv");
	if(!_original_glVertexAttribI4uiv) numFailed++;
	_original_glVertexAttribI4usv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLushort *))IntGetProcAddress("glVertexAttribI4usv");
	if(!_original_glVertexAttribI4usv) numFailed++;
	_original_glVertexAttribIPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribIPointer");
	if(!_original_glVertexAttribIPointer) numFailed++;
	return numFailed;
}
static int Load_EXT_geometry_shader4()
{
	int numFailed = 0;
	_original_glProgramParameteriEXT = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteriEXT");
	if(!_original_glProgramParameteriEXT) numFailed++;
	return numFailed;
}

static int Load_ARB_geometry_shader4()
{
	int numFailed = 0;
	_original_glFramebufferTextureARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTextureARB");
	if(!_original_glFramebufferTextureARB) numFailed++;
	_original_glFramebufferTextureFaceARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint, GLenum))IntGetProcAddress("glFramebufferTextureFaceARB");
	if(!_original_glFramebufferTextureFaceARB) numFailed++;
	_original_glFramebufferTextureLayerARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint, GLint))IntGetProcAddress("glFramebufferTextureLayerARB");
	if(!_original_glFramebufferTextureLayerARB) numFailed++;
	_original_glProgramParameteriARB = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteriARB");
	if(!_original_glProgramParameteriARB) numFailed++;
	return numFailed;
}

static int Load_ARB_tessellation_shader()
{
	int numFailed = 0;
	_original_glPatchParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPatchParameterfv");
	if(!_original_glPatchParameterfv) numFailed++;
	_original_glPatchParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPatchParameteri");
	if(!_original_glPatchParameteri) numFailed++;
	return numFailed;
}

static int Load_ARB_compute_shader()
{
	int numFailed = 0;
	_original_glDispatchCompute = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glDispatchCompute");
	if(!_original_glDispatchCompute) numFailed++;
	_original_glDispatchComputeIndirect = (void (CODEGEN_FUNCPTR *)(GLintptr))IntGetProcAddress("glDispatchComputeIndirect");
	if(!_original_glDispatchComputeIndirect) numFailed++;
	return numFailed;
}

static int Load_ARB_debug_output()
{
	int numFailed = 0;
	_original_glDebugMessageCallbackARB = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROCARB, const void *))IntGetProcAddress("glDebugMessageCallbackARB");
	if(!_original_glDebugMessageCallbackARB) numFailed++;
	_original_glDebugMessageControlARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControlARB");
	if(!_original_glDebugMessageControlARB) numFailed++;
	_original_glDebugMessageInsertARB = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsertARB");
	if(!_original_glDebugMessageInsertARB) numFailed++;
	_original_glGetDebugMessageLogARB = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLogARB");
	if(!_original_glGetDebugMessageLogARB) numFailed++;
	return numFailed;
}

static int Load_KHR_debug()
{
	int numFailed = 0;
	_original_glDebugMessageCallback = (void (CODEGEN_FUNCPTR *)(GLDEBUGPROC, void *))IntGetProcAddress("glDebugMessageCallback");
	if(!_original_glDebugMessageCallback) numFailed++;
	_original_glDebugMessageControl = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))IntGetProcAddress("glDebugMessageControl");
	if(!_original_glDebugMessageControl) numFailed++;
	_original_glDebugMessageInsert = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))IntGetProcAddress("glDebugMessageInsert");
	if(!_original_glDebugMessageInsert) numFailed++;
	_original_glGetDebugMessageLog = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))IntGetProcAddress("glGetDebugMessageLog");
	if(!_original_glGetDebugMessageLog) numFailed++;
	_original_glGetObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectLabel");
	if(!_original_glGetObjectLabel) numFailed++;
	_original_glGetObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetObjectPtrLabel");
	if(!_original_glGetObjectPtrLabel) numFailed++;
	_original_glGetPointerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLvoid **))IntGetProcAddress("glGetPointerv");
	if(!_original_glGetPointerv) numFailed++;
	_original_glObjectLabel = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glObjectLabel");
	if(!_original_glObjectLabel) numFailed++;
	_original_glObjectPtrLabel = (void (CODEGEN_FUNCPTR *)(const void *, GLsizei, const GLchar *))IntGetProcAddress("glObjectPtrLabel");
	if(!_original_glObjectPtrLabel) numFailed++;
	_original_glPopDebugGroup = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPopDebugGroup");
	if(!_original_glPopDebugGroup) numFailed++;
	_original_glPushDebugGroup = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLsizei, const GLchar *))IntGetProcAddress("glPushDebugGroup");
	if(!_original_glPushDebugGroup) numFailed++;
	return numFailed;
}

static int Load_ARB_ES2_compatibility()
{
	int numFailed = 0;
	_original_glClearDepthf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearDepthf");
	if(!_original_glClearDepthf) numFailed++;
	_original_glDepthRangef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glDepthRangef");
	if(!_original_glDepthRangef) numFailed++;
	_original_glGetShaderPrecisionFormat = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *, GLint *))IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_original_glGetShaderPrecisionFormat) numFailed++;
	_original_glReleaseShaderCompiler = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glReleaseShaderCompiler");
	if(!_original_glReleaseShaderCompiler) numFailed++;
	_original_glShaderBinary = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glShaderBinary");
	if(!_original_glShaderBinary) numFailed++;
	return numFailed;
}

static int Load_ARB_get_program_binary()
{
	int numFailed = 0;
	_original_glGetProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *))IntGetProcAddress("glGetProgramBinary");
	if(!_original_glGetProgramBinary) numFailed++;
	_original_glProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glProgramBinary");
	if(!_original_glProgramBinary) numFailed++;
	_original_glProgramParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteri");
	if(!_original_glProgramParameteri) numFailed++;
	return numFailed;
}

static int Load_ARB_internalformat_query()
{
	int numFailed = 0;
	_original_glGetInternalformativ = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint *))IntGetProcAddress("glGetInternalformativ");
	if(!_original_glGetInternalformativ) numFailed++;
	return numFailed;
}

static int Load_ARB_internalformat_query2()
{
	int numFailed = 0;
	_original_glGetInternalformati64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLsizei, GLint64 *))IntGetProcAddress("glGetInternalformati64v");
	if(!_original_glGetInternalformati64v) numFailed++;
	return numFailed;
}

static int Load_ARB_program_interface_query()
{
	int numFailed = 0;
	_original_glGetProgramInterfaceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetProgramInterfaceiv");
	if(!_original_glGetProgramInterfaceiv) numFailed++;
	_original_glGetProgramResourceIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceIndex");
	if(!_original_glGetProgramResourceIndex) numFailed++;
	_original_glGetProgramResourceLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocation");
	if(!_original_glGetProgramResourceLocation) numFailed++;
	_original_glGetProgramResourceLocationIndex = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetProgramResourceLocationIndex");
	if(!_original_glGetProgramResourceLocationIndex) numFailed++;
	_original_glGetProgramResourceName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramResourceName");
	if(!_original_glGetProgramResourceName) numFailed++;
	_original_glGetProgramResourceiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetProgramResourceiv");
	if(!_original_glGetProgramResourceiv) numFailed++;
	return numFailed;
}

static int Load_ARB_separate_shader_objects()
{
	int numFailed = 0;
	_original_glActiveShaderProgram = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glActiveShaderProgram");
	if(!_original_glActiveShaderProgram) numFailed++;
	_original_glBindProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindProgramPipeline");
	if(!_original_glBindProgramPipeline) numFailed++;
	_original_glCreateShaderProgramv = (GLuint (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLchar *const*))IntGetProcAddress("glCreateShaderProgramv");
	if(!_original_glCreateShaderProgramv) numFailed++;
	_original_glDeleteProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteProgramPipelines");
	if(!_original_glDeleteProgramPipelines) numFailed++;
	_original_glGenProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenProgramPipelines");
	if(!_original_glGenProgramPipelines) numFailed++;
	_original_glGetProgramPipelineInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_original_glGetProgramPipelineInfoLog) numFailed++;
	_original_glGetProgramPipelineiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramPipelineiv");
	if(!_original_glGetProgramPipelineiv) numFailed++;
	_original_glIsProgramPipeline = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgramPipeline");
	if(!_original_glIsProgramPipeline) numFailed++;
	_original_glProgramUniform1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble))IntGetProcAddress("glProgramUniform1d");
	if(!_original_glProgramUniform1d) numFailed++;
	_original_glProgramUniform1dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform1dv");
	if(!_original_glProgramUniform1dv) numFailed++;
	_original_glProgramUniform1f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat))IntGetProcAddress("glProgramUniform1f");
	if(!_original_glProgramUniform1f) numFailed++;
	_original_glProgramUniform1fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform1fv");
	if(!_original_glProgramUniform1fv) numFailed++;
	_original_glProgramUniform1i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint))IntGetProcAddress("glProgramUniform1i");
	if(!_original_glProgramUniform1i) numFailed++;
	_original_glProgramUniform1iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform1iv");
	if(!_original_glProgramUniform1iv) numFailed++;
	_original_glProgramUniform1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint))IntGetProcAddress("glProgramUniform1ui");
	if(!_original_glProgramUniform1ui) numFailed++;
	_original_glProgramUniform1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform1uiv");
	if(!_original_glProgramUniform1uiv) numFailed++;
	_original_glProgramUniform2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform2d");
	if(!_original_glProgramUniform2d) numFailed++;
	_original_glProgramUniform2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform2dv");
	if(!_original_glProgramUniform2dv) numFailed++;
	_original_glProgramUniform2f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform2f");
	if(!_original_glProgramUniform2f) numFailed++;
	_original_glProgramUniform2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform2fv");
	if(!_original_glProgramUniform2fv) numFailed++;
	_original_glProgramUniform2i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform2i");
	if(!_original_glProgramUniform2i) numFailed++;
	_original_glProgramUniform2iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform2iv");
	if(!_original_glProgramUniform2iv) numFailed++;
	_original_glProgramUniform2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint))IntGetProcAddress("glProgramUniform2ui");
	if(!_original_glProgramUniform2ui) numFailed++;
	_original_glProgramUniform2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform2uiv");
	if(!_original_glProgramUniform2uiv) numFailed++;
	_original_glProgramUniform3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform3d");
	if(!_original_glProgramUniform3d) numFailed++;
	_original_glProgramUniform3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform3dv");
	if(!_original_glProgramUniform3dv) numFailed++;
	_original_glProgramUniform3f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform3f");
	if(!_original_glProgramUniform3f) numFailed++;
	_original_glProgramUniform3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform3fv");
	if(!_original_glProgramUniform3fv) numFailed++;
	_original_glProgramUniform3i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform3i");
	if(!_original_glProgramUniform3i) numFailed++;
	_original_glProgramUniform3iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform3iv");
	if(!_original_glProgramUniform3iv) numFailed++;
	_original_glProgramUniform3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform3ui");
	if(!_original_glProgramUniform3ui) numFailed++;
	_original_glProgramUniform3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform3uiv");
	if(!_original_glProgramUniform3uiv) numFailed++;
	_original_glProgramUniform4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glProgramUniform4d");
	if(!_original_glProgramUniform4d) numFailed++;
	_original_glProgramUniform4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLdouble *))IntGetProcAddress("glProgramUniform4dv");
	if(!_original_glProgramUniform4dv) numFailed++;
	_original_glProgramUniform4f = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glProgramUniform4f");
	if(!_original_glProgramUniform4f) numFailed++;
	_original_glProgramUniform4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLfloat *))IntGetProcAddress("glProgramUniform4fv");
	if(!_original_glProgramUniform4fv) numFailed++;
	_original_glProgramUniform4i = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLint))IntGetProcAddress("glProgramUniform4i");
	if(!_original_glProgramUniform4i) numFailed++;
	_original_glProgramUniform4iv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLint *))IntGetProcAddress("glProgramUniform4iv");
	if(!_original_glProgramUniform4iv) numFailed++;
	_original_glProgramUniform4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glProgramUniform4ui");
	if(!_original_glProgramUniform4ui) numFailed++;
	_original_glProgramUniform4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, const GLuint *))IntGetProcAddress("glProgramUniform4uiv");
	if(!_original_glProgramUniform4uiv) numFailed++;
	_original_glProgramUniformMatrix2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2dv");
	if(!_original_glProgramUniformMatrix2dv) numFailed++;
	_original_glProgramUniformMatrix2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2fv");
	if(!_original_glProgramUniformMatrix2fv) numFailed++;
	_original_glProgramUniformMatrix2x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x3dv");
	if(!_original_glProgramUniformMatrix2x3dv) numFailed++;
	_original_glProgramUniformMatrix2x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x3fv");
	if(!_original_glProgramUniformMatrix2x3fv) numFailed++;
	_original_glProgramUniformMatrix2x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix2x4dv");
	if(!_original_glProgramUniformMatrix2x4dv) numFailed++;
	_original_glProgramUniformMatrix2x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix2x4fv");
	if(!_original_glProgramUniformMatrix2x4fv) numFailed++;
	_original_glProgramUniformMatrix3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3dv");
	if(!_original_glProgramUniformMatrix3dv) numFailed++;
	_original_glProgramUniformMatrix3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3fv");
	if(!_original_glProgramUniformMatrix3fv) numFailed++;
	_original_glProgramUniformMatrix3x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x2dv");
	if(!_original_glProgramUniformMatrix3x2dv) numFailed++;
	_original_glProgramUniformMatrix3x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x2fv");
	if(!_original_glProgramUniformMatrix3x2fv) numFailed++;
	_original_glProgramUniformMatrix3x4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix3x4dv");
	if(!_original_glProgramUniformMatrix3x4dv) numFailed++;
	_original_glProgramUniformMatrix3x4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix3x4fv");
	if(!_original_glProgramUniformMatrix3x4fv) numFailed++;
	_original_glProgramUniformMatrix4dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4dv");
	if(!_original_glProgramUniformMatrix4dv) numFailed++;
	_original_glProgramUniformMatrix4fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4fv");
	if(!_original_glProgramUniformMatrix4fv) numFailed++;
	_original_glProgramUniformMatrix4x2dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x2dv");
	if(!_original_glProgramUniformMatrix4x2dv) numFailed++;
	_original_glProgramUniformMatrix4x2fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x2fv");
	if(!_original_glProgramUniformMatrix4x2fv) numFailed++;
	_original_glProgramUniformMatrix4x3dv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glProgramUniformMatrix4x3dv");
	if(!_original_glProgramUniformMatrix4x3dv) numFailed++;
	_original_glProgramUniformMatrix4x3fv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *))IntGetProcAddress("glProgramUniformMatrix4x3fv");
	if(!_original_glProgramUniformMatrix4x3fv) numFailed++;
	_original_glUseProgramStages = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield, GLuint))IntGetProcAddress("glUseProgramStages");
	if(!_original_glUseProgramStages) numFailed++;
	_original_glValidateProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgramPipeline");
	if(!_original_glValidateProgramPipeline) numFailed++;
	return numFailed;
}

static int Load_ARB_texture_buffer_range()
{
	int numFailed = 0;
	_original_glTexBufferRange = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glTexBufferRange");
	if(!_original_glTexBufferRange) numFailed++;
	return numFailed;
}

static int Load_ARB_texture_storage()
{
	int numFailed = 0;
	_original_glTexStorage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei))IntGetProcAddress("glTexStorage1D");
	if(!_original_glTexStorage1D) numFailed++;
	_original_glTexStorage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei))IntGetProcAddress("glTexStorage2D");
	if(!_original_glTexStorage2D) numFailed++;
	_original_glTexStorage3D = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glTexStorage3D");
	if(!_original_glTexStorage3D) numFailed++;
	return numFailed;
}

static int Load_ARB_texture_view()
{
	int numFailed = 0;
	_original_glTextureView = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint))IntGetProcAddress("glTextureView");
	if(!_original_glTextureView) numFailed++;
	return numFailed;
}

static int Load_ARB_vertex_attrib_binding()
{
	int numFailed = 0;
	_original_glBindVertexBuffer = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLintptr, GLsizei))IntGetProcAddress("glBindVertexBuffer");
	if(!_original_glBindVertexBuffer) numFailed++;
	_original_glVertexAttribBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribBinding");
	if(!_original_glVertexAttribBinding) numFailed++;
	_original_glVertexAttribFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribFormat");
	if(!_original_glVertexAttribFormat) numFailed++;
	_original_glVertexAttribIFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribIFormat");
	if(!_original_glVertexAttribIFormat) numFailed++;
	_original_glVertexAttribLFormat = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLuint))IntGetProcAddress("glVertexAttribLFormat");
	if(!_original_glVertexAttribLFormat) numFailed++;
	_original_glVertexBindingDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexBindingDivisor");
	if(!_original_glVertexBindingDivisor) numFailed++;
	return numFailed;
}

static int Load_ARB_viewport_array()
{
	int numFailed = 0;
	_original_glDepthRangeArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLdouble *))IntGetProcAddress("glDepthRangeArrayv");
	if(!_original_glDepthRangeArrayv) numFailed++;
	_original_glDepthRangeIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glDepthRangeIndexed");
	if(!_original_glDepthRangeIndexed) numFailed++;
	_original_glGetDoublei_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLdouble *))IntGetProcAddress("glGetDoublei_v");
	if(!_original_glGetDoublei_v) numFailed++;
	_original_glGetFloati_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetFloati_v");
	if(!_original_glGetFloati_v) numFailed++;
	_original_glScissorArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLint *))IntGetProcAddress("glScissorArrayv");
	if(!_original_glScissorArrayv) numFailed++;
	_original_glScissorIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissorIndexed");
	if(!_original_glScissorIndexed) numFailed++;
	_original_glScissorIndexedv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glScissorIndexedv");
	if(!_original_glScissorIndexedv) numFailed++;
	_original_glViewportArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLfloat *))IntGetProcAddress("glViewportArrayv");
	if(!_original_glViewportArrayv) numFailed++;
	_original_glViewportIndexedf = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glViewportIndexedf");
	if(!_original_glViewportIndexedf) numFailed++;
	_original_glViewportIndexedfv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glViewportIndexedfv");
	if(!_original_glViewportIndexedfv) numFailed++;
	return numFailed;
}

static int Load_ARB_clear_buffer_object()
{
	int numFailed = 0;
	_original_glClearBufferData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferData");
	if(!_original_glClearBufferData) numFailed++;
	_original_glClearBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *))IntGetProcAddress("glClearBufferSubData");
	if(!_original_glClearBufferSubData) numFailed++;
	return numFailed;
}

static int Load_ARB_copy_image()
{
	int numFailed = 0;
	_original_glCopyImageSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glCopyImageSubData");
	if(!_original_glCopyImageSubData) numFailed++;
	return numFailed;
}

static int Load_ARB_framebuffer_no_attachments()
{
	int numFailed = 0;
	_original_glFramebufferParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint))IntGetProcAddress("glFramebufferParameteri");
	if(!_original_glFramebufferParameteri) numFailed++;
	_original_glGetFramebufferParameteriv = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *))IntGetProcAddress("glGetFramebufferParameteriv");
	if(!_original_glGetFramebufferParameteriv) numFailed++;
	return numFailed;
}

static int Load_ARB_invalidate_subdata()
{
	int numFailed = 0;
	_original_glInvalidateBufferData = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glInvalidateBufferData");
	if(!_original_glInvalidateBufferData) numFailed++;
	_original_glInvalidateBufferSubData = (void (CODEGEN_FUNCPTR *)(GLuint, GLintptr, GLsizeiptr))IntGetProcAddress("glInvalidateBufferSubData");
	if(!_original_glInvalidateBufferSubData) numFailed++;
	_original_glInvalidateFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *))IntGetProcAddress("glInvalidateFramebuffer");
	if(!_original_glInvalidateFramebuffer) numFailed++;
	_original_glInvalidateSubFramebuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glInvalidateSubFramebuffer");
	if(!_original_glInvalidateSubFramebuffer) numFailed++;
	_original_glInvalidateTexImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint))IntGetProcAddress("glInvalidateTexImage");
	if(!_original_glInvalidateTexImage) numFailed++;
	_original_glInvalidateTexSubImage = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glInvalidateTexSubImage");
	if(!_original_glInvalidateTexSubImage) numFailed++;
	return numFailed;
}

static int Load_ARB_texture_storage_multisample()
{
	int numFailed = 0;
	_original_glTexStorage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage2DMultisample");
	if(!_original_glTexStorage2DMultisample) numFailed++;
	_original_glTexStorage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexStorage3DMultisample");
	if(!_original_glTexStorage3DMultisample) numFailed++;
	return numFailed;
}

static int Load_NV_texture_barrier()
{
	int numFailed = 0;
	_original_glTextureBarrierNV = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glTextureBarrierNV");
	if(!_original_glTextureBarrierNV) numFailed++;
	return numFailed;
}

static int Load_NV_copy_image()
{
	int numFailed = 0;
	_original_glCopyImageSubDataNV = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei))IntGetProcAddress("glCopyImageSubDataNV");
	if(!_original_glCopyImageSubDataNV) numFailed++;
	return numFailed;
}



void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf);

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageCallback(GLDEBUGPROC callback, void * userParam);

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled);

GLuint APIENTRY KHR_DEBUG_EMULATOR_GetDebugMessageLog(GLuint count, GLsizei bufsize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog);

void APIENTRY KHR_DEBUG_EMULATOR_GetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label);

void APIENTRY KHR_DEBUG_EMULATOR_GetObjectPtrLabel(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label);

void APIENTRY KHR_DEBUG_EMULATOR_ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label);

void APIENTRY KHR_DEBUG_EMULATOR_ObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label);

void APIENTRY KHR_DEBUG_EMULATOR_PopDebugGroup();

void APIENTRY KHR_DEBUG_EMULATOR_PushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar * message);

// internal error check that gets triggered after every GL call
void APIENTRY KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();

GLenum APIENTRY KHR_DEBUG_EMULATOR_GetError();

void APIENTRY KHR_DEBUG_EMULATOR_Disable(GLenum cap);

void APIENTRY KHR_DEBUG_EMULATOR_Enable(GLenum cap);

GLboolean APIENTRY KHR_DEBUG_EMULATOR_IsEnabled(GLenum cap);

void APIENTRY KHR_DEBUG_EMULATOR_GetIntegerv(GLenum pname, GLint * params);

void APIENTRY KHR_DEBUG_EMULATOR_GetPointerv( GLenum pname, GLvoid ** params );

// Extension: EXT_geometry_shader4

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteriEXT)(GLuint, GLenum, GLint)
void APIENTRY wrapperglProgramParameteriEXT(GLuint program, GLenum pname, GLint value){
   _original_glProgramParameteriEXT(program, pname, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_geometry_shader4

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureARB)(GLenum, GLenum, GLuint, GLint)
void APIENTRY wrapperglFramebufferTextureARB(GLenum target, GLenum attachment, GLuint texture, GLint level){
   _original_glFramebufferTextureARB(target, attachment, texture, level); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureFaceARB)(GLenum, GLenum, GLuint, GLint, GLenum)
void APIENTRY wrapperglFramebufferTextureFaceARB(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face){
   _original_glFramebufferTextureFaceARB(target, attachment, texture, level, face); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureLayerARB)(GLenum, GLenum, GLuint, GLint, GLint)
void APIENTRY wrapperglFramebufferTextureLayerARB(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){
   _original_glFramebufferTextureLayerARB(target, attachment, texture, level, layer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteriARB)(GLuint, GLenum, GLint)
void APIENTRY wrapperglProgramParameteriARB(GLuint program, GLenum pname, GLint value){
   _original_glProgramParameteriARB(program, pname, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_tessellation_shader

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPatchParameterfv)(GLenum, const GLfloat *)
void APIENTRY wrapperglPatchParameterfv(GLenum pname, const GLfloat * values){
   _original_glPatchParameterfv(pname, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPatchParameteri)(GLenum, GLint)
void APIENTRY wrapperglPatchParameteri(GLenum pname, GLint value){
   _original_glPatchParameteri(pname, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_compute_shader

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDispatchCompute)(GLuint, GLuint, GLuint)
void APIENTRY wrapperglDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z){
   _original_glDispatchCompute(num_groups_x, num_groups_y, num_groups_z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDispatchComputeIndirect)(GLintptr)
void APIENTRY wrapperglDispatchComputeIndirect(GLintptr indirect){
   _original_glDispatchComputeIndirect(indirect); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_debug_output

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallbackARB)(GLDEBUGPROCARB, const void *)
void APIENTRY wrapperglDebugMessageCallbackARB(GLDEBUGPROCARB callback, const void * userParam){
   _original_glDebugMessageCallbackARB(callback, userParam); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControlARB)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)
void APIENTRY wrapperglDebugMessageControlARB(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled){
   _original_glDebugMessageControlARB(source, type, severity, count, ids, enabled); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsertARB)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)
void APIENTRY wrapperglDebugMessageInsertARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf){
   _original_glDebugMessageInsertARB(source, type, id, severity, length, buf); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLogARB)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)
GLuint APIENTRY wrapperglGetDebugMessageLogARB(GLuint count, GLsizei bufsize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog){
   GLuint returnValue = _original_glGetDebugMessageLogARB(count, bufsize, sources, types, ids, severities, lengths, messageLog);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


// Extension: KHR_debug

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageCallback)(GLDEBUGPROC, void *)
void APIENTRY wrapperglDebugMessageCallback(GLDEBUGPROC callback, void * userParam){
   _original_glDebugMessageCallback(callback, userParam); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageControl)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)
void APIENTRY wrapperglDebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled){
   _original_glDebugMessageControl(source, type, severity, count, ids, enabled); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDebugMessageInsert)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)
void APIENTRY wrapperglDebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf){
   _original_glDebugMessageInsert(source, type, id, severity, length, buf); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGetDebugMessageLog)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)
GLuint APIENTRY wrapperglGetDebugMessageLog(GLuint count, GLsizei bufsize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog){
   GLuint returnValue = _original_glGetDebugMessageLog(count, bufsize, sources, types, ids, severities, lengths, messageLog);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetObjectLabel)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label){
   _original_glGetObjectLabel(identifier, name, bufSize, length, label); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetObjectPtrLabel)(const void *, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetObjectPtrLabel(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label){
   _original_glGetObjectPtrLabel(ptr, bufSize, length, label); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetPointerv)(GLenum, GLvoid **)
void APIENTRY wrapperglGetPointerv(GLenum pname, GLvoid ** params){
   _original_glGetPointerv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glObjectLabel)(GLenum, GLuint, GLsizei, const GLchar *)
void APIENTRY wrapperglObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label){
   _original_glObjectLabel(identifier, name, length, label); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glObjectPtrLabel)(const void *, GLsizei, const GLchar *)
void APIENTRY wrapperglObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label){
   _original_glObjectPtrLabel(ptr, length, label); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPopDebugGroup)()
void APIENTRY wrapperglPopDebugGroup(){
   _original_glPopDebugGroup(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPushDebugGroup)(GLenum, GLuint, GLsizei, const GLchar *)
void APIENTRY wrapperglPushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar * message){
   _original_glPushDebugGroup(source, id, length, message); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_ES2_compatibility

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearDepthf)(GLfloat)
void APIENTRY wrapperglClearDepthf(GLfloat d){
   _original_glClearDepthf(d); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthRangef)(GLfloat, GLfloat)
void APIENTRY wrapperglDepthRangef(GLfloat n, GLfloat f){
   _original_glDepthRangef(n, f); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetShaderPrecisionFormat)(GLenum, GLenum, GLint *, GLint *)
void APIENTRY wrapperglGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision){
   _original_glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glReleaseShaderCompiler)()
void APIENTRY wrapperglReleaseShaderCompiler(){
   _original_glReleaseShaderCompiler(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glShaderBinary)(GLsizei, const GLuint *, GLenum, const GLvoid *, GLsizei)
void APIENTRY wrapperglShaderBinary(GLsizei count, const GLuint * shaders, GLenum binaryformat, const GLvoid * binary, GLsizei length){
   _original_glShaderBinary(count, shaders, binaryformat, binary, length); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_get_program_binary

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramBinary)(GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *)
void APIENTRY wrapperglGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei * length, GLenum * binaryFormat, GLvoid * binary){
   _original_glGetProgramBinary(program, bufSize, length, binaryFormat, binary); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramBinary)(GLuint, GLenum, const GLvoid *, GLsizei)
void APIENTRY wrapperglProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid * binary, GLsizei length){
   _original_glProgramBinary(program, binaryFormat, binary, length); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramParameteri)(GLuint, GLenum, GLint)
void APIENTRY wrapperglProgramParameteri(GLuint program, GLenum pname, GLint value){
   _original_glProgramParameteri(program, pname, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_internalformat_query

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformativ)(GLenum, GLenum, GLenum, GLsizei, GLint *)
void APIENTRY wrapperglGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint * params){
   _original_glGetInternalformativ(target, internalformat, pname, bufSize, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_internalformat_query2

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetInternalformati64v)(GLenum, GLenum, GLenum, GLsizei, GLint64 *)
void APIENTRY wrapperglGetInternalformati64v(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 * params){
   _original_glGetInternalformati64v(target, internalformat, pname, bufSize, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_program_interface_query

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInterfaceiv)(GLuint, GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint * params){
   _original_glGetProgramInterfaceiv(program, programInterface, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceIndex)(GLuint, GLenum, const GLchar *)
GLuint APIENTRY wrapperglGetProgramResourceIndex(GLuint program, GLenum programInterface, const GLchar * name){
   GLuint returnValue = _original_glGetProgramResourceIndex(program, programInterface, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocation)(GLuint, GLenum, const GLchar *)
GLint APIENTRY wrapperglGetProgramResourceLocation(GLuint program, GLenum programInterface, const GLchar * name){
   GLint returnValue = _original_glGetProgramResourceLocation(program, programInterface, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceLocationIndex)(GLuint, GLenum, const GLchar *)
GLint APIENTRY wrapperglGetProgramResourceLocationIndex(GLuint program, GLenum programInterface, const GLchar * name){
   GLint returnValue = _original_glGetProgramResourceLocationIndex(program, programInterface, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei * length, GLchar * name){
   _original_glGetProgramResourceName(program, programInterface, index, bufSize, length, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramResourceiv)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)
void APIENTRY wrapperglGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum * props, GLsizei bufSize, GLsizei * length, GLint * params){
   _original_glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_separate_shader_objects

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glActiveShaderProgram)(GLuint, GLuint)
void APIENTRY wrapperglActiveShaderProgram(GLuint pipeline, GLuint program){
   _original_glActiveShaderProgram(pipeline, program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindProgramPipeline)(GLuint)
void APIENTRY wrapperglBindProgramPipeline(GLuint pipeline){
   _original_glBindProgramPipeline(pipeline); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShaderProgramv)(GLenum, GLsizei, const GLchar *const*)
GLuint APIENTRY wrapperglCreateShaderProgramv(GLenum type, GLsizei count, const GLchar *const* strings){
   GLuint returnValue = _original_glCreateShaderProgramv(type, count, strings);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgramPipelines)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteProgramPipelines(GLsizei n, const GLuint * pipelines){
   _original_glDeleteProgramPipelines(n, pipelines); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenProgramPipelines)(GLsizei, GLuint *)
void APIENTRY wrapperglGenProgramPipelines(GLsizei n, GLuint * pipelines){
   _original_glGenProgramPipelines(n, pipelines); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetProgramPipelineInfoLog(GLuint pipeline, GLsizei bufSize, GLsizei * length, GLchar * infoLog){
   _original_glGetProgramPipelineInfoLog(pipeline, bufSize, length, infoLog); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramPipelineiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetProgramPipelineiv(GLuint pipeline, GLenum pname, GLint * params){
   _original_glGetProgramPipelineiv(pipeline, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgramPipeline)(GLuint)
GLboolean APIENTRY wrapperglIsProgramPipeline(GLuint pipeline){
   GLboolean returnValue = _original_glIsProgramPipeline(pipeline);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1d)(GLuint, GLint, GLdouble)
void APIENTRY wrapperglProgramUniform1d(GLuint program, GLint location, GLdouble v0){
   _original_glProgramUniform1d(program, location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1dv)(GLuint, GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglProgramUniform1dv(GLuint program, GLint location, GLsizei count, const GLdouble * value){
   _original_glProgramUniform1dv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1f)(GLuint, GLint, GLfloat)
void APIENTRY wrapperglProgramUniform1f(GLuint program, GLint location, GLfloat v0){
   _original_glProgramUniform1f(program, location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1fv)(GLuint, GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat * value){
   _original_glProgramUniform1fv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1i)(GLuint, GLint, GLint)
void APIENTRY wrapperglProgramUniform1i(GLuint program, GLint location, GLint v0){
   _original_glProgramUniform1i(program, location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1iv)(GLuint, GLint, GLsizei, const GLint *)
void APIENTRY wrapperglProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint * value){
   _original_glProgramUniform1iv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1ui)(GLuint, GLint, GLuint)
void APIENTRY wrapperglProgramUniform1ui(GLuint program, GLint location, GLuint v0){
   _original_glProgramUniform1ui(program, location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform1uiv)(GLuint, GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglProgramUniform1uiv(GLuint program, GLint location, GLsizei count, const GLuint * value){
   _original_glProgramUniform1uiv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2d)(GLuint, GLint, GLdouble, GLdouble)
void APIENTRY wrapperglProgramUniform2d(GLuint program, GLint location, GLdouble v0, GLdouble v1){
   _original_glProgramUniform2d(program, location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2dv)(GLuint, GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglProgramUniform2dv(GLuint program, GLint location, GLsizei count, const GLdouble * value){
   _original_glProgramUniform2dv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2f)(GLuint, GLint, GLfloat, GLfloat)
void APIENTRY wrapperglProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1){
   _original_glProgramUniform2f(program, location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2fv)(GLuint, GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat * value){
   _original_glProgramUniform2fv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2i)(GLuint, GLint, GLint, GLint)
void APIENTRY wrapperglProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1){
   _original_glProgramUniform2i(program, location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2iv)(GLuint, GLint, GLsizei, const GLint *)
void APIENTRY wrapperglProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint * value){
   _original_glProgramUniform2iv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2ui)(GLuint, GLint, GLuint, GLuint)
void APIENTRY wrapperglProgramUniform2ui(GLuint program, GLint location, GLuint v0, GLuint v1){
   _original_glProgramUniform2ui(program, location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform2uiv)(GLuint, GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglProgramUniform2uiv(GLuint program, GLint location, GLsizei count, const GLuint * value){
   _original_glProgramUniform2uiv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3d)(GLuint, GLint, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglProgramUniform3d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2){
   _original_glProgramUniform3d(program, location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3dv)(GLuint, GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglProgramUniform3dv(GLuint program, GLint location, GLsizei count, const GLdouble * value){
   _original_glProgramUniform3dv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3f)(GLuint, GLint, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2){
   _original_glProgramUniform3f(program, location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3fv)(GLuint, GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat * value){
   _original_glProgramUniform3fv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3i)(GLuint, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2){
   _original_glProgramUniform3i(program, location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3iv)(GLuint, GLint, GLsizei, const GLint *)
void APIENTRY wrapperglProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint * value){
   _original_glProgramUniform3iv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3ui)(GLuint, GLint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglProgramUniform3ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2){
   _original_glProgramUniform3ui(program, location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform3uiv)(GLuint, GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglProgramUniform3uiv(GLuint program, GLint location, GLsizei count, const GLuint * value){
   _original_glProgramUniform3uiv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4d)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglProgramUniform4d(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3){
   _original_glProgramUniform4d(program, location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4dv)(GLuint, GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglProgramUniform4dv(GLuint program, GLint location, GLsizei count, const GLdouble * value){
   _original_glProgramUniform4dv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4f)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){
   _original_glProgramUniform4f(program, location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4fv)(GLuint, GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat * value){
   _original_glProgramUniform4fv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4i)(GLuint, GLint, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3){
   _original_glProgramUniform4i(program, location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4iv)(GLuint, GLint, GLsizei, const GLint *)
void APIENTRY wrapperglProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint * value){
   _original_glProgramUniform4iv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4ui)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglProgramUniform4ui(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){
   _original_glProgramUniform4ui(program, location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniform4uiv)(GLuint, GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglProgramUniform4uiv(GLuint program, GLint location, GLsizei count, const GLuint * value){
   _original_glProgramUniform4uiv(program, location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix2dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix2fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix2x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix2x3dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix2x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix2x3fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix2x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix2x4dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix2x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix2x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix2x4fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix3dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix3fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix3x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix3x2dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix3x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix3x2fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix3x4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix3x4dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix3x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix3x4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix3x4fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix4dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix4dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix4fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix4x2dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix4x2dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix4x2fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix4x2fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3dv)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglProgramUniformMatrix4x3dv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glProgramUniformMatrix4x3dv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProgramUniformMatrix4x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglProgramUniformMatrix4x3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glProgramUniformMatrix4x3fv(program, location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUseProgramStages)(GLuint, GLbitfield, GLuint)
void APIENTRY wrapperglUseProgramStages(GLuint pipeline, GLbitfield stages, GLuint program){
   _original_glUseProgramStages(pipeline, stages, program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glValidateProgramPipeline)(GLuint)
void APIENTRY wrapperglValidateProgramPipeline(GLuint pipeline){
   _original_glValidateProgramPipeline(pipeline); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_texture_buffer_range

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexBufferRange)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)
void APIENTRY wrapperglTexBufferRange(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size){
   _original_glTexBufferRange(target, internalformat, buffer, offset, size); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_texture_storage

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexStorage1D)(GLenum, GLsizei, GLenum, GLsizei)
void APIENTRY wrapperglTexStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width){
   _original_glTexStorage1D(target, levels, internalformat, width); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)
void APIENTRY wrapperglTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height){
   _original_glTexStorage2D(target, levels, internalformat, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)
void APIENTRY wrapperglTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth){
   _original_glTexStorage3D(target, levels, internalformat, width, height, depth); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_texture_view

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTextureView)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglTextureView(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers){
   _original_glTextureView(texture, target, origtexture, internalformat, minlevel, numlevels, minlayer, numlayers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_vertex_attrib_binding

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindVertexBuffer)(GLuint, GLuint, GLintptr, GLsizei)
void APIENTRY wrapperglBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride){
   _original_glBindVertexBuffer(bindingindex, buffer, offset, stride); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribBinding)(GLuint, GLuint)
void APIENTRY wrapperglVertexAttribBinding(GLuint attribindex, GLuint bindingindex){
   _original_glVertexAttribBinding(attribindex, bindingindex); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribFormat)(GLuint, GLint, GLenum, GLboolean, GLuint)
void APIENTRY wrapperglVertexAttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset){
   _original_glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIFormat)(GLuint, GLint, GLenum, GLuint)
void APIENTRY wrapperglVertexAttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){
   _original_glVertexAttribIFormat(attribindex, size, type, relativeoffset); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLFormat)(GLuint, GLint, GLenum, GLuint)
void APIENTRY wrapperglVertexAttribLFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){
   _original_glVertexAttribLFormat(attribindex, size, type, relativeoffset); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexBindingDivisor)(GLuint, GLuint)
void APIENTRY wrapperglVertexBindingDivisor(GLuint bindingindex, GLuint divisor){
   _original_glVertexBindingDivisor(bindingindex, divisor); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_viewport_array

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeArrayv)(GLuint, GLsizei, const GLdouble *)
void APIENTRY wrapperglDepthRangeArrayv(GLuint first, GLsizei count, const GLdouble * v){
   _original_glDepthRangeArrayv(first, count, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthRangeIndexed)(GLuint, GLdouble, GLdouble)
void APIENTRY wrapperglDepthRangeIndexed(GLuint index, GLdouble n, GLdouble f){
   _original_glDepthRangeIndexed(index, n, f); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetDoublei_v)(GLenum, GLuint, GLdouble *)
void APIENTRY wrapperglGetDoublei_v(GLenum target, GLuint index, GLdouble * data){
   _original_glGetDoublei_v(target, index, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetFloati_v)(GLenum, GLuint, GLfloat *)
void APIENTRY wrapperglGetFloati_v(GLenum target, GLuint index, GLfloat * data){
   _original_glGetFloati_v(target, index, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScissorArrayv)(GLuint, GLsizei, const GLint *)
void APIENTRY wrapperglScissorArrayv(GLuint first, GLsizei count, const GLint * v){
   _original_glScissorArrayv(first, count, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexed)(GLuint, GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglScissorIndexed(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height){
   _original_glScissorIndexed(index, left, bottom, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScissorIndexedv)(GLuint, const GLint *)
void APIENTRY wrapperglScissorIndexedv(GLuint index, const GLint * v){
   _original_glScissorIndexedv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glViewportArrayv)(GLuint, GLsizei, const GLfloat *)
void APIENTRY wrapperglViewportArrayv(GLuint first, GLsizei count, const GLfloat * v){
   _original_glViewportArrayv(first, count, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedf)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglViewportIndexedf(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h){
   _original_glViewportIndexedf(index, x, y, w, h); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glViewportIndexedfv)(GLuint, const GLfloat *)
void APIENTRY wrapperglViewportIndexedfv(GLuint index, const GLfloat * v){
   _original_glViewportIndexedfv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_clear_buffer_object

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferData)(GLenum, GLenum, GLenum, GLenum, const void *)
void APIENTRY wrapperglClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void * data){
   _original_glClearBufferData(target, internalformat, format, type, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferSubData)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *)
void APIENTRY wrapperglClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data){
   _original_glClearBufferSubData(target, internalformat, offset, size, format, type, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_copy_image

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyImageSubData)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)
void APIENTRY wrapperglCopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth){
   _original_glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_framebuffer_no_attachments

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferParameteri)(GLenum, GLenum, GLint)
void APIENTRY wrapperglFramebufferParameteri(GLenum target, GLenum pname, GLint param){
   _original_glFramebufferParameteri(target, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferParameteriv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetFramebufferParameteriv(GLenum target, GLenum pname, GLint * params){
   _original_glGetFramebufferParameteriv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_invalidate_subdata

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferData)(GLuint)
void APIENTRY wrapperglInvalidateBufferData(GLuint buffer){
   _original_glInvalidateBufferData(buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateBufferSubData)(GLuint, GLintptr, GLsizeiptr)
void APIENTRY wrapperglInvalidateBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr length){
   _original_glInvalidateBufferSubData(buffer, offset, length); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateFramebuffer)(GLenum, GLsizei, const GLenum *)
void APIENTRY wrapperglInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum * attachments){
   _original_glInvalidateFramebuffer(target, numAttachments, attachments); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateSubFramebuffer)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexImage)(GLuint, GLint)
void APIENTRY wrapperglInvalidateTexImage(GLuint texture, GLint level){
   _original_glInvalidateTexImage(texture, level); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInvalidateTexSubImage)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)
void APIENTRY wrapperglInvalidateTexSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth){
   _original_glInvalidateTexSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: ARB_texture_storage_multisample

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexStorage2DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)
void APIENTRY wrapperglTexStorage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations){
   _original_glTexStorage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexStorage3DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)
void APIENTRY wrapperglTexStorage3DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations){
   _original_glTexStorage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: NV_texture_barrier

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTextureBarrierNV)()
void APIENTRY wrapperglTextureBarrierNV(){
   _original_glTextureBarrierNV(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: NV_copy_image

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyImageSubDataNV)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)
void APIENTRY wrapperglCopyImageSubDataNV(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth){
   _original_glCopyImageSubDataNV(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.0

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glAccum)(GLenum, GLfloat)
void APIENTRY wrapperglAccum(GLenum op, GLfloat value){
   _original_glAccum(op, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glAlphaFunc)(GLenum, GLfloat)
void APIENTRY wrapperglAlphaFunc(GLenum func, GLfloat ref){
   _original_glAlphaFunc(func, ref); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBegin)(GLenum)
void APIENTRY wrapperglBegin(GLenum mode){
   _original_glBegin(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBitmap)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte *)
void APIENTRY wrapperglBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap){
   _original_glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum, GLenum)
void APIENTRY wrapperglBlendFunc(GLenum sfactor, GLenum dfactor){
   _original_glBlendFunc(sfactor, dfactor); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCallList)(GLuint)
void APIENTRY wrapperglCallList(GLuint list){
   _original_glCallList(list); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCallLists)(GLsizei, GLenum, const GLvoid *)
void APIENTRY wrapperglCallLists(GLsizei n, GLenum type, const GLvoid * lists){
   _original_glCallLists(n, type, lists); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield)
void APIENTRY wrapperglClear(GLbitfield mask){
   _original_glClear(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearAccum)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
   _original_glClearAccum(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
   _original_glClearColor(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearDepth)(GLdouble)
void APIENTRY wrapperglClearDepth(GLdouble depth){
   _original_glClearDepth(depth); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearIndex)(GLfloat)
void APIENTRY wrapperglClearIndex(GLfloat c){
   _original_glClearIndex(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint)
void APIENTRY wrapperglClearStencil(GLint s){
   _original_glClearStencil(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClipPlane)(GLenum, const GLdouble *)
void APIENTRY wrapperglClipPlane(GLenum plane, const GLdouble * equation){
   _original_glClipPlane(plane, equation); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3b)(GLbyte, GLbyte, GLbyte)
void APIENTRY wrapperglColor3b(GLbyte red, GLbyte green, GLbyte blue){
   _original_glColor3b(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3bv)(const GLbyte *)
void APIENTRY wrapperglColor3bv(const GLbyte * v){
   _original_glColor3bv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglColor3d(GLdouble red, GLdouble green, GLdouble blue){
   _original_glColor3d(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3dv)(const GLdouble *)
void APIENTRY wrapperglColor3dv(const GLdouble * v){
   _original_glColor3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglColor3f(GLfloat red, GLfloat green, GLfloat blue){
   _original_glColor3f(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3fv)(const GLfloat *)
void APIENTRY wrapperglColor3fv(const GLfloat * v){
   _original_glColor3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3i)(GLint, GLint, GLint)
void APIENTRY wrapperglColor3i(GLint red, GLint green, GLint blue){
   _original_glColor3i(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3iv)(const GLint *)
void APIENTRY wrapperglColor3iv(const GLint * v){
   _original_glColor3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglColor3s(GLshort red, GLshort green, GLshort blue){
   _original_glColor3s(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3sv)(const GLshort *)
void APIENTRY wrapperglColor3sv(const GLshort * v){
   _original_glColor3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3ub)(GLubyte, GLubyte, GLubyte)
void APIENTRY wrapperglColor3ub(GLubyte red, GLubyte green, GLubyte blue){
   _original_glColor3ub(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3ubv)(const GLubyte *)
void APIENTRY wrapperglColor3ubv(const GLubyte * v){
   _original_glColor3ubv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3ui)(GLuint, GLuint, GLuint)
void APIENTRY wrapperglColor3ui(GLuint red, GLuint green, GLuint blue){
   _original_glColor3ui(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3uiv)(const GLuint *)
void APIENTRY wrapperglColor3uiv(const GLuint * v){
   _original_glColor3uiv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3us)(GLushort, GLushort, GLushort)
void APIENTRY wrapperglColor3us(GLushort red, GLushort green, GLushort blue){
   _original_glColor3us(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor3usv)(const GLushort *)
void APIENTRY wrapperglColor3usv(const GLushort * v){
   _original_glColor3usv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4b)(GLbyte, GLbyte, GLbyte, GLbyte)
void APIENTRY wrapperglColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha){
   _original_glColor4b(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4bv)(const GLbyte *)
void APIENTRY wrapperglColor4bv(const GLbyte * v){
   _original_glColor4bv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4d)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha){
   _original_glColor4d(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4dv)(const GLdouble *)
void APIENTRY wrapperglColor4dv(const GLdouble * v){
   _original_glColor4dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4f)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
   _original_glColor4f(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4fv)(const GLfloat *)
void APIENTRY wrapperglColor4fv(const GLfloat * v){
   _original_glColor4fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4i)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglColor4i(GLint red, GLint green, GLint blue, GLint alpha){
   _original_glColor4i(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4iv)(const GLint *)
void APIENTRY wrapperglColor4iv(const GLint * v){
   _original_glColor4iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4s)(GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha){
   _original_glColor4s(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4sv)(const GLshort *)
void APIENTRY wrapperglColor4sv(const GLshort * v){
   _original_glColor4sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4ub)(GLubyte, GLubyte, GLubyte, GLubyte)
void APIENTRY wrapperglColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha){
   _original_glColor4ub(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4ubv)(const GLubyte *)
void APIENTRY wrapperglColor4ubv(const GLubyte * v){
   _original_glColor4ubv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4ui)(GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha){
   _original_glColor4ui(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4uiv)(const GLuint *)
void APIENTRY wrapperglColor4uiv(const GLuint * v){
   _original_glColor4uiv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4us)(GLushort, GLushort, GLushort, GLushort)
void APIENTRY wrapperglColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha){
   _original_glColor4us(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColor4usv)(const GLushort *)
void APIENTRY wrapperglColor4usv(const GLushort * v){
   _original_glColor4usv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean)
void APIENTRY wrapperglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha){
   _original_glColorMask(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColorMaterial)(GLenum, GLenum)
void APIENTRY wrapperglColorMaterial(GLenum face, GLenum mode){
   _original_glColorMaterial(face, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyPixels)(GLint, GLint, GLsizei, GLsizei, GLenum)
void APIENTRY wrapperglCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type){
   _original_glCopyPixels(x, y, width, height, type); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum)
void APIENTRY wrapperglCullFace(GLenum mode){
   _original_glCullFace(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteLists)(GLuint, GLsizei)
void APIENTRY wrapperglDeleteLists(GLuint list, GLsizei range){
   _original_glDeleteLists(list, range); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthFunc)(GLenum)
void APIENTRY wrapperglDepthFunc(GLenum func){
   _original_glDepthFunc(func); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthMask)(GLboolean)
void APIENTRY wrapperglDepthMask(GLboolean flag){
   _original_glDepthMask(flag); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDepthRange)(GLdouble, GLdouble)
void APIENTRY wrapperglDepthRange(GLdouble ren_near, GLdouble ren_far){
   _original_glDepthRange(ren_near, ren_far); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDisable)(GLenum)
void APIENTRY wrapperglDisable(GLenum cap){
   _original_glDisable(cap); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffer)(GLenum)
void APIENTRY wrapperglDrawBuffer(GLenum mode){
   _original_glDrawBuffer(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glDrawPixels(width, height, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlag)(GLboolean)
void APIENTRY wrapperglEdgeFlag(GLboolean flag){
   _original_glEdgeFlag(flag); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagv)(const GLboolean *)
void APIENTRY wrapperglEdgeFlagv(const GLboolean * flag){
   _original_glEdgeFlagv(flag); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum)
void APIENTRY wrapperglEnable(GLenum cap){
   _original_glEnable(cap); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnd)()
void APIENTRY wrapperglEnd(){
   _original_glEnd(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEndList)()
void APIENTRY wrapperglEndList(){
   _original_glEndList(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1d)(GLdouble)
void APIENTRY wrapperglEvalCoord1d(GLdouble u){
   _original_glEvalCoord1d(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1dv)(const GLdouble *)
void APIENTRY wrapperglEvalCoord1dv(const GLdouble * u){
   _original_glEvalCoord1dv(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1f)(GLfloat)
void APIENTRY wrapperglEvalCoord1f(GLfloat u){
   _original_glEvalCoord1f(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord1fv)(const GLfloat *)
void APIENTRY wrapperglEvalCoord1fv(const GLfloat * u){
   _original_glEvalCoord1fv(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2d)(GLdouble, GLdouble)
void APIENTRY wrapperglEvalCoord2d(GLdouble u, GLdouble v){
   _original_glEvalCoord2d(u, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2dv)(const GLdouble *)
void APIENTRY wrapperglEvalCoord2dv(const GLdouble * u){
   _original_glEvalCoord2dv(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2f)(GLfloat, GLfloat)
void APIENTRY wrapperglEvalCoord2f(GLfloat u, GLfloat v){
   _original_glEvalCoord2f(u, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalCoord2fv)(const GLfloat *)
void APIENTRY wrapperglEvalCoord2fv(const GLfloat * u){
   _original_glEvalCoord2fv(u); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh1)(GLenum, GLint, GLint)
void APIENTRY wrapperglEvalMesh1(GLenum mode, GLint i1, GLint i2){
   _original_glEvalMesh1(mode, i1, i2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalMesh2)(GLenum, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2){
   _original_glEvalMesh2(mode, i1, i2, j1, j2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint1)(GLint)
void APIENTRY wrapperglEvalPoint1(GLint i){
   _original_glEvalPoint1(i); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEvalPoint2)(GLint, GLint)
void APIENTRY wrapperglEvalPoint2(GLint i, GLint j){
   _original_glEvalPoint2(i, j); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFeedbackBuffer)(GLsizei, GLenum, GLfloat *)
void APIENTRY wrapperglFeedbackBuffer(GLsizei size, GLenum type, GLfloat * buffer){
   _original_glFeedbackBuffer(size, type, buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFinish)()
void APIENTRY wrapperglFinish(){
   _original_glFinish(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFlush)()
void APIENTRY wrapperglFlush(){
   _original_glFlush(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogf)(GLenum, GLfloat)
void APIENTRY wrapperglFogf(GLenum pname, GLfloat param){
   _original_glFogf(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogfv)(GLenum, const GLfloat *)
void APIENTRY wrapperglFogfv(GLenum pname, const GLfloat * params){
   _original_glFogfv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogi)(GLenum, GLint)
void APIENTRY wrapperglFogi(GLenum pname, GLint param){
   _original_glFogi(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogiv)(GLenum, const GLint *)
void APIENTRY wrapperglFogiv(GLenum pname, const GLint * params){
   _original_glFogiv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum)
void APIENTRY wrapperglFrontFace(GLenum mode){
   _original_glFrontFace(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFrustum)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
   _original_glFrustum(left, right, bottom, top, zNear, zFar); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGenLists)(GLsizei)
GLuint APIENTRY wrapperglGenLists(GLsizei range){
   GLuint returnValue = _original_glGenLists(range);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum, GLboolean *)
void APIENTRY wrapperglGetBooleanv(GLenum pname, GLboolean * params){
   _original_glGetBooleanv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetClipPlane)(GLenum, GLdouble *)
void APIENTRY wrapperglGetClipPlane(GLenum plane, GLdouble * equation){
   _original_glGetClipPlane(plane, equation); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetDoublev)(GLenum, GLdouble *)
void APIENTRY wrapperglGetDoublev(GLenum pname, GLdouble * params){
   _original_glGetDoublev(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLenum (CODEGEN_FUNCPTR *_ptrc_glGetError)()
// no wrapper for glGetError

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetFloatv)(GLenum, GLfloat *)
void APIENTRY wrapperglGetFloatv(GLenum pname, GLfloat * params){
   _original_glGetFloatv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetIntegerv)(GLenum, GLint *)
void APIENTRY wrapperglGetIntegerv(GLenum pname, GLint * params){
   _original_glGetIntegerv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetLightfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetLightfv(GLenum light, GLenum pname, GLfloat * params){
   _original_glGetLightfv(light, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetLightiv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetLightiv(GLenum light, GLenum pname, GLint * params){
   _original_glGetLightiv(light, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMapdv)(GLenum, GLenum, GLdouble *)
void APIENTRY wrapperglGetMapdv(GLenum target, GLenum query, GLdouble * v){
   _original_glGetMapdv(target, query, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMapfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetMapfv(GLenum target, GLenum query, GLfloat * v){
   _original_glGetMapfv(target, query, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMapiv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetMapiv(GLenum target, GLenum query, GLint * v){
   _original_glGetMapiv(target, query, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetMaterialfv(GLenum face, GLenum pname, GLfloat * params){
   _original_glGetMaterialfv(face, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMaterialiv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetMaterialiv(GLenum face, GLenum pname, GLint * params){
   _original_glGetMaterialiv(face, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapfv)(GLenum, GLfloat *)
void APIENTRY wrapperglGetPixelMapfv(GLenum map, GLfloat * values){
   _original_glGetPixelMapfv(map, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapuiv)(GLenum, GLuint *)
void APIENTRY wrapperglGetPixelMapuiv(GLenum map, GLuint * values){
   _original_glGetPixelMapuiv(map, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetPixelMapusv)(GLenum, GLushort *)
void APIENTRY wrapperglGetPixelMapusv(GLenum map, GLushort * values){
   _original_glGetPixelMapusv(map, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetPolygonStipple)(GLubyte *)
void APIENTRY wrapperglGetPolygonStipple(GLubyte * mask){
   _original_glGetPolygonStipple(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum)
const GLubyte * APIENTRY wrapperglGetString(GLenum name){
   const GLubyte * returnValue = _original_glGetString(name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnvfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetTexEnvfv(GLenum target, GLenum pname, GLfloat * params){
   _original_glGetTexEnvfv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexEnviv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetTexEnviv(GLenum target, GLenum pname, GLint * params){
   _original_glGetTexEnviv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexGendv)(GLenum, GLenum, GLdouble *)
void APIENTRY wrapperglGetTexGendv(GLenum coord, GLenum pname, GLdouble * params){
   _original_glGetTexGendv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexGenfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetTexGenfv(GLenum coord, GLenum pname, GLfloat * params){
   _original_glGetTexGenfv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexGeniv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetTexGeniv(GLenum coord, GLenum pname, GLint * params){
   _original_glGetTexGeniv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexImage)(GLenum, GLint, GLenum, GLenum, GLvoid *)
void APIENTRY wrapperglGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels){
   _original_glGetTexImage(target, level, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *)
void APIENTRY wrapperglGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat * params){
   _original_glGetTexLevelParameterfv(target, level, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *)
void APIENTRY wrapperglGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint * params){
   _original_glGetTexLevelParameteriv(target, level, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterfv)(GLenum, GLenum, GLfloat *)
void APIENTRY wrapperglGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params){
   _original_glGetTexParameterfv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameteriv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetTexParameteriv(GLenum target, GLenum pname, GLint * params){
   _original_glGetTexParameteriv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glHint)(GLenum, GLenum)
void APIENTRY wrapperglHint(GLenum target, GLenum mode){
   _original_glHint(target, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexMask)(GLuint)
void APIENTRY wrapperglIndexMask(GLuint mask){
   _original_glIndexMask(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexd)(GLdouble)
void APIENTRY wrapperglIndexd(GLdouble c){
   _original_glIndexd(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexdv)(const GLdouble *)
void APIENTRY wrapperglIndexdv(const GLdouble * c){
   _original_glIndexdv(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexf)(GLfloat)
void APIENTRY wrapperglIndexf(GLfloat c){
   _original_glIndexf(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexfv)(const GLfloat *)
void APIENTRY wrapperglIndexfv(const GLfloat * c){
   _original_glIndexfv(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexi)(GLint)
void APIENTRY wrapperglIndexi(GLint c){
   _original_glIndexi(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexiv)(const GLint *)
void APIENTRY wrapperglIndexiv(const GLint * c){
   _original_glIndexiv(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexs)(GLshort)
void APIENTRY wrapperglIndexs(GLshort c){
   _original_glIndexs(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexsv)(const GLshort *)
void APIENTRY wrapperglIndexsv(const GLshort * c){
   _original_glIndexsv(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInitNames)()
void APIENTRY wrapperglInitNames(){
   _original_glInitNames(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum)
GLboolean APIENTRY wrapperglIsEnabled(GLenum cap){
   GLboolean returnValue = _original_glIsEnabled(cap);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsList)(GLuint)
GLboolean APIENTRY wrapperglIsList(GLuint list){
   GLboolean returnValue = _original_glIsList(list);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightModelf)(GLenum, GLfloat)
void APIENTRY wrapperglLightModelf(GLenum pname, GLfloat param){
   _original_glLightModelf(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightModelfv)(GLenum, const GLfloat *)
void APIENTRY wrapperglLightModelfv(GLenum pname, const GLfloat * params){
   _original_glLightModelfv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightModeli)(GLenum, GLint)
void APIENTRY wrapperglLightModeli(GLenum pname, GLint param){
   _original_glLightModeli(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightModeliv)(GLenum, const GLint *)
void APIENTRY wrapperglLightModeliv(GLenum pname, const GLint * params){
   _original_glLightModeliv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightf)(GLenum, GLenum, GLfloat)
void APIENTRY wrapperglLightf(GLenum light, GLenum pname, GLfloat param){
   _original_glLightf(light, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightfv)(GLenum, GLenum, const GLfloat *)
void APIENTRY wrapperglLightfv(GLenum light, GLenum pname, const GLfloat * params){
   _original_glLightfv(light, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLighti)(GLenum, GLenum, GLint)
void APIENTRY wrapperglLighti(GLenum light, GLenum pname, GLint param){
   _original_glLighti(light, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLightiv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglLightiv(GLenum light, GLenum pname, const GLint * params){
   _original_glLightiv(light, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLineStipple)(GLint, GLushort)
void APIENTRY wrapperglLineStipple(GLint factor, GLushort pattern){
   _original_glLineStipple(factor, pattern); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat)
void APIENTRY wrapperglLineWidth(GLfloat width){
   _original_glLineWidth(width); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glListBase)(GLuint)
void APIENTRY wrapperglListBase(GLuint base){
   _original_glListBase(base); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadIdentity)()
void APIENTRY wrapperglLoadIdentity(){
   _original_glLoadIdentity(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixd)(const GLdouble *)
void APIENTRY wrapperglLoadMatrixd(const GLdouble * m){
   _original_glLoadMatrixd(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadMatrixf)(const GLfloat *)
void APIENTRY wrapperglLoadMatrixf(const GLfloat * m){
   _original_glLoadMatrixf(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadName)(GLuint)
void APIENTRY wrapperglLoadName(GLuint name){
   _original_glLoadName(name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum)
void APIENTRY wrapperglLogicOp(GLenum opcode){
   _original_glLogicOp(opcode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMap1d)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble *)
void APIENTRY wrapperglMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points){
   _original_glMap1d(target, u1, u2, stride, order, points); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMap1f)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat *)
void APIENTRY wrapperglMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points){
   _original_glMap1f(target, u1, u2, stride, order, points); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMap2d)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble *)
void APIENTRY wrapperglMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points){
   _original_glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMap2f)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat *)
void APIENTRY wrapperglMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points){
   _original_glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1d)(GLint, GLdouble, GLdouble)
void APIENTRY wrapperglMapGrid1d(GLint un, GLdouble u1, GLdouble u2){
   _original_glMapGrid1d(un, u1, u2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMapGrid1f)(GLint, GLfloat, GLfloat)
void APIENTRY wrapperglMapGrid1f(GLint un, GLfloat u1, GLfloat u2){
   _original_glMapGrid1f(un, u1, u2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2d)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble)
void APIENTRY wrapperglMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2){
   _original_glMapGrid2d(un, u1, u2, vn, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMapGrid2f)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat)
void APIENTRY wrapperglMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2){
   _original_glMapGrid2f(un, u1, u2, vn, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMaterialf)(GLenum, GLenum, GLfloat)
void APIENTRY wrapperglMaterialf(GLenum face, GLenum pname, GLfloat param){
   _original_glMaterialf(face, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMaterialfv)(GLenum, GLenum, const GLfloat *)
void APIENTRY wrapperglMaterialfv(GLenum face, GLenum pname, const GLfloat * params){
   _original_glMaterialfv(face, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMateriali)(GLenum, GLenum, GLint)
void APIENTRY wrapperglMateriali(GLenum face, GLenum pname, GLint param){
   _original_glMateriali(face, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMaterialiv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglMaterialiv(GLenum face, GLenum pname, const GLint * params){
   _original_glMaterialiv(face, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMatrixMode)(GLenum)
void APIENTRY wrapperglMatrixMode(GLenum mode){
   _original_glMatrixMode(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixd)(const GLdouble *)
void APIENTRY wrapperglMultMatrixd(const GLdouble * m){
   _original_glMultMatrixd(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultMatrixf)(const GLfloat *)
void APIENTRY wrapperglMultMatrixf(const GLfloat * m){
   _original_glMultMatrixf(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNewList)(GLuint, GLenum)
void APIENTRY wrapperglNewList(GLuint list, GLenum mode){
   _original_glNewList(list, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3b)(GLbyte, GLbyte, GLbyte)
void APIENTRY wrapperglNormal3b(GLbyte nx, GLbyte ny, GLbyte nz){
   _original_glNormal3b(nx, ny, nz); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3bv)(const GLbyte *)
void APIENTRY wrapperglNormal3bv(const GLbyte * v){
   _original_glNormal3bv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglNormal3d(GLdouble nx, GLdouble ny, GLdouble nz){
   _original_glNormal3d(nx, ny, nz); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3dv)(const GLdouble *)
void APIENTRY wrapperglNormal3dv(const GLdouble * v){
   _original_glNormal3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglNormal3f(GLfloat nx, GLfloat ny, GLfloat nz){
   _original_glNormal3f(nx, ny, nz); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3fv)(const GLfloat *)
void APIENTRY wrapperglNormal3fv(const GLfloat * v){
   _original_glNormal3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3i)(GLint, GLint, GLint)
void APIENTRY wrapperglNormal3i(GLint nx, GLint ny, GLint nz){
   _original_glNormal3i(nx, ny, nz); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3iv)(const GLint *)
void APIENTRY wrapperglNormal3iv(const GLint * v){
   _original_glNormal3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglNormal3s(GLshort nx, GLshort ny, GLshort nz){
   _original_glNormal3s(nx, ny, nz); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormal3sv)(const GLshort *)
void APIENTRY wrapperglNormal3sv(const GLshort * v){
   _original_glNormal3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){
   _original_glOrtho(left, right, bottom, top, zNear, zFar); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPassThrough)(GLfloat)
void APIENTRY wrapperglPassThrough(GLfloat token){
   _original_glPassThrough(token); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelMapfv)(GLenum, GLsizei, const GLfloat *)
void APIENTRY wrapperglPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat * values){
   _original_glPixelMapfv(map, mapsize, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelMapuiv)(GLenum, GLsizei, const GLuint *)
void APIENTRY wrapperglPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint * values){
   _original_glPixelMapuiv(map, mapsize, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelMapusv)(GLenum, GLsizei, const GLushort *)
void APIENTRY wrapperglPixelMapusv(GLenum map, GLsizei mapsize, const GLushort * values){
   _original_glPixelMapusv(map, mapsize, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelStoref)(GLenum, GLfloat)
void APIENTRY wrapperglPixelStoref(GLenum pname, GLfloat param){
   _original_glPixelStoref(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelStorei)(GLenum, GLint)
void APIENTRY wrapperglPixelStorei(GLenum pname, GLint param){
   _original_glPixelStorei(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferf)(GLenum, GLfloat)
void APIENTRY wrapperglPixelTransferf(GLenum pname, GLfloat param){
   _original_glPixelTransferf(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelTransferi)(GLenum, GLint)
void APIENTRY wrapperglPixelTransferi(GLenum pname, GLint param){
   _original_glPixelTransferi(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPixelZoom)(GLfloat, GLfloat)
void APIENTRY wrapperglPixelZoom(GLfloat xfactor, GLfloat yfactor){
   _original_glPixelZoom(xfactor, yfactor); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPointSize)(GLfloat)
void APIENTRY wrapperglPointSize(GLfloat size){
   _original_glPointSize(size); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPolygonMode)(GLenum, GLenum)
void APIENTRY wrapperglPolygonMode(GLenum face, GLenum mode){
   _original_glPolygonMode(face, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPolygonStipple)(const GLubyte *)
void APIENTRY wrapperglPolygonStipple(const GLubyte * mask){
   _original_glPolygonStipple(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPopAttrib)()
void APIENTRY wrapperglPopAttrib(){
   _original_glPopAttrib(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPopMatrix)()
void APIENTRY wrapperglPopMatrix(){
   _original_glPopMatrix(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPopName)()
void APIENTRY wrapperglPopName(){
   _original_glPopName(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPushAttrib)(GLbitfield)
void APIENTRY wrapperglPushAttrib(GLbitfield mask){
   _original_glPushAttrib(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPushMatrix)()
void APIENTRY wrapperglPushMatrix(){
   _original_glPushMatrix(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPushName)(GLuint)
void APIENTRY wrapperglPushName(GLuint name){
   _original_glPushName(name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2d)(GLdouble, GLdouble)
void APIENTRY wrapperglRasterPos2d(GLdouble x, GLdouble y){
   _original_glRasterPos2d(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2dv)(const GLdouble *)
void APIENTRY wrapperglRasterPos2dv(const GLdouble * v){
   _original_glRasterPos2dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2f)(GLfloat, GLfloat)
void APIENTRY wrapperglRasterPos2f(GLfloat x, GLfloat y){
   _original_glRasterPos2f(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2fv)(const GLfloat *)
void APIENTRY wrapperglRasterPos2fv(const GLfloat * v){
   _original_glRasterPos2fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2i)(GLint, GLint)
void APIENTRY wrapperglRasterPos2i(GLint x, GLint y){
   _original_glRasterPos2i(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2iv)(const GLint *)
void APIENTRY wrapperglRasterPos2iv(const GLint * v){
   _original_glRasterPos2iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2s)(GLshort, GLshort)
void APIENTRY wrapperglRasterPos2s(GLshort x, GLshort y){
   _original_glRasterPos2s(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos2sv)(const GLshort *)
void APIENTRY wrapperglRasterPos2sv(const GLshort * v){
   _original_glRasterPos2sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglRasterPos3d(GLdouble x, GLdouble y, GLdouble z){
   _original_glRasterPos3d(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3dv)(const GLdouble *)
void APIENTRY wrapperglRasterPos3dv(const GLdouble * v){
   _original_glRasterPos3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglRasterPos3f(GLfloat x, GLfloat y, GLfloat z){
   _original_glRasterPos3f(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3fv)(const GLfloat *)
void APIENTRY wrapperglRasterPos3fv(const GLfloat * v){
   _original_glRasterPos3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3i)(GLint, GLint, GLint)
void APIENTRY wrapperglRasterPos3i(GLint x, GLint y, GLint z){
   _original_glRasterPos3i(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3iv)(const GLint *)
void APIENTRY wrapperglRasterPos3iv(const GLint * v){
   _original_glRasterPos3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglRasterPos3s(GLshort x, GLshort y, GLshort z){
   _original_glRasterPos3s(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos3sv)(const GLshort *)
void APIENTRY wrapperglRasterPos3sv(const GLshort * v){
   _original_glRasterPos3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w){
   _original_glRasterPos4d(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4dv)(const GLdouble *)
void APIENTRY wrapperglRasterPos4dv(const GLdouble * v){
   _original_glRasterPos4dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4f)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w){
   _original_glRasterPos4f(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4fv)(const GLfloat *)
void APIENTRY wrapperglRasterPos4fv(const GLfloat * v){
   _original_glRasterPos4fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4i)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglRasterPos4i(GLint x, GLint y, GLint z, GLint w){
   _original_glRasterPos4i(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4iv)(const GLint *)
void APIENTRY wrapperglRasterPos4iv(const GLint * v){
   _original_glRasterPos4iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4s)(GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w){
   _original_glRasterPos4s(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRasterPos4sv)(const GLshort *)
void APIENTRY wrapperglRasterPos4sv(const GLshort * v){
   _original_glRasterPos4sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glReadBuffer)(GLenum)
void APIENTRY wrapperglReadBuffer(GLenum mode){
   _original_glReadBuffer(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *)
void APIENTRY wrapperglReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels){
   _original_glReadPixels(x, y, width, height, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectd)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2){
   _original_glRectd(x1, y1, x2, y2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectdv)(const GLdouble *, const GLdouble *)
void APIENTRY wrapperglRectdv(const GLdouble * v1, const GLdouble * v2){
   _original_glRectdv(v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectf)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2){
   _original_glRectf(x1, y1, x2, y2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectfv)(const GLfloat *, const GLfloat *)
void APIENTRY wrapperglRectfv(const GLfloat * v1, const GLfloat * v2){
   _original_glRectfv(v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRecti)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglRecti(GLint x1, GLint y1, GLint x2, GLint y2){
   _original_glRecti(x1, y1, x2, y2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectiv)(const GLint *, const GLint *)
void APIENTRY wrapperglRectiv(const GLint * v1, const GLint * v2){
   _original_glRectiv(v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRects)(GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2){
   _original_glRects(x1, y1, x2, y2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRectsv)(const GLshort *, const GLshort *)
void APIENTRY wrapperglRectsv(const GLshort * v1, const GLshort * v2){
   _original_glRectsv(v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glRenderMode)(GLenum)
GLint APIENTRY wrapperglRenderMode(GLenum mode){
   GLint returnValue = _original_glRenderMode(mode);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRotated)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z){
   _original_glRotated(angle, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRotatef)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z){
   _original_glRotatef(angle, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScaled)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglScaled(GLdouble x, GLdouble y, GLdouble z){
   _original_glScaled(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScalef)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglScalef(GLfloat x, GLfloat y, GLfloat z){
   _original_glScalef(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglScissor(GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glScissor(x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSelectBuffer)(GLsizei, GLuint *)
void APIENTRY wrapperglSelectBuffer(GLsizei size, GLuint * buffer){
   _original_glSelectBuffer(size, buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glShadeModel)(GLenum)
void APIENTRY wrapperglShadeModel(GLenum mode){
   _original_glShadeModel(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilFunc)(GLenum, GLint, GLuint)
void APIENTRY wrapperglStencilFunc(GLenum func, GLint ref, GLuint mask){
   _original_glStencilFunc(func, ref, mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilMask)(GLuint)
void APIENTRY wrapperglStencilMask(GLuint mask){
   _original_glStencilMask(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilOp)(GLenum, GLenum, GLenum)
void APIENTRY wrapperglStencilOp(GLenum fail, GLenum zfail, GLenum zpass){
   _original_glStencilOp(fail, zfail, zpass); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1d)(GLdouble)
void APIENTRY wrapperglTexCoord1d(GLdouble s){
   _original_glTexCoord1d(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1dv)(const GLdouble *)
void APIENTRY wrapperglTexCoord1dv(const GLdouble * v){
   _original_glTexCoord1dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1f)(GLfloat)
void APIENTRY wrapperglTexCoord1f(GLfloat s){
   _original_glTexCoord1f(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1fv)(const GLfloat *)
void APIENTRY wrapperglTexCoord1fv(const GLfloat * v){
   _original_glTexCoord1fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1i)(GLint)
void APIENTRY wrapperglTexCoord1i(GLint s){
   _original_glTexCoord1i(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1iv)(const GLint *)
void APIENTRY wrapperglTexCoord1iv(const GLint * v){
   _original_glTexCoord1iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1s)(GLshort)
void APIENTRY wrapperglTexCoord1s(GLshort s){
   _original_glTexCoord1s(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord1sv)(const GLshort *)
void APIENTRY wrapperglTexCoord1sv(const GLshort * v){
   _original_glTexCoord1sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2d)(GLdouble, GLdouble)
void APIENTRY wrapperglTexCoord2d(GLdouble s, GLdouble t){
   _original_glTexCoord2d(s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2dv)(const GLdouble *)
void APIENTRY wrapperglTexCoord2dv(const GLdouble * v){
   _original_glTexCoord2dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2f)(GLfloat, GLfloat)
void APIENTRY wrapperglTexCoord2f(GLfloat s, GLfloat t){
   _original_glTexCoord2f(s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2fv)(const GLfloat *)
void APIENTRY wrapperglTexCoord2fv(const GLfloat * v){
   _original_glTexCoord2fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2i)(GLint, GLint)
void APIENTRY wrapperglTexCoord2i(GLint s, GLint t){
   _original_glTexCoord2i(s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2iv)(const GLint *)
void APIENTRY wrapperglTexCoord2iv(const GLint * v){
   _original_glTexCoord2iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2s)(GLshort, GLshort)
void APIENTRY wrapperglTexCoord2s(GLshort s, GLshort t){
   _original_glTexCoord2s(s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord2sv)(const GLshort *)
void APIENTRY wrapperglTexCoord2sv(const GLshort * v){
   _original_glTexCoord2sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglTexCoord3d(GLdouble s, GLdouble t, GLdouble r){
   _original_glTexCoord3d(s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3dv)(const GLdouble *)
void APIENTRY wrapperglTexCoord3dv(const GLdouble * v){
   _original_glTexCoord3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglTexCoord3f(GLfloat s, GLfloat t, GLfloat r){
   _original_glTexCoord3f(s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3fv)(const GLfloat *)
void APIENTRY wrapperglTexCoord3fv(const GLfloat * v){
   _original_glTexCoord3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3i)(GLint, GLint, GLint)
void APIENTRY wrapperglTexCoord3i(GLint s, GLint t, GLint r){
   _original_glTexCoord3i(s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3iv)(const GLint *)
void APIENTRY wrapperglTexCoord3iv(const GLint * v){
   _original_glTexCoord3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglTexCoord3s(GLshort s, GLshort t, GLshort r){
   _original_glTexCoord3s(s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord3sv)(const GLshort *)
void APIENTRY wrapperglTexCoord3sv(const GLshort * v){
   _original_glTexCoord3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4d)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q){
   _original_glTexCoord4d(s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4dv)(const GLdouble *)
void APIENTRY wrapperglTexCoord4dv(const GLdouble * v){
   _original_glTexCoord4dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4f)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q){
   _original_glTexCoord4f(s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4fv)(const GLfloat *)
void APIENTRY wrapperglTexCoord4fv(const GLfloat * v){
   _original_glTexCoord4fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4i)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglTexCoord4i(GLint s, GLint t, GLint r, GLint q){
   _original_glTexCoord4i(s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4iv)(const GLint *)
void APIENTRY wrapperglTexCoord4iv(const GLint * v){
   _original_glTexCoord4iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4s)(GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q){
   _original_glTexCoord4s(s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoord4sv)(const GLshort *)
void APIENTRY wrapperglTexCoord4sv(const GLshort * v){
   _original_glTexCoord4sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexEnvf)(GLenum, GLenum, GLfloat)
void APIENTRY wrapperglTexEnvf(GLenum target, GLenum pname, GLfloat param){
   _original_glTexEnvf(target, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexEnvfv)(GLenum, GLenum, const GLfloat *)
void APIENTRY wrapperglTexEnvfv(GLenum target, GLenum pname, const GLfloat * params){
   _original_glTexEnvfv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexEnvi)(GLenum, GLenum, GLint)
void APIENTRY wrapperglTexEnvi(GLenum target, GLenum pname, GLint param){
   _original_glTexEnvi(target, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexEnviv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglTexEnviv(GLenum target, GLenum pname, const GLint * params){
   _original_glTexEnviv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGend)(GLenum, GLenum, GLdouble)
void APIENTRY wrapperglTexGend(GLenum coord, GLenum pname, GLdouble param){
   _original_glTexGend(coord, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGendv)(GLenum, GLenum, const GLdouble *)
void APIENTRY wrapperglTexGendv(GLenum coord, GLenum pname, const GLdouble * params){
   _original_glTexGendv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGenf)(GLenum, GLenum, GLfloat)
void APIENTRY wrapperglTexGenf(GLenum coord, GLenum pname, GLfloat param){
   _original_glTexGenf(coord, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGenfv)(GLenum, GLenum, const GLfloat *)
void APIENTRY wrapperglTexGenfv(GLenum coord, GLenum pname, const GLfloat * params){
   _original_glTexGenfv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGeni)(GLenum, GLenum, GLint)
void APIENTRY wrapperglTexGeni(GLenum coord, GLenum pname, GLint param){
   _original_glTexGeni(coord, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexGeniv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglTexGeniv(GLenum coord, GLenum pname, const GLint * params){
   _original_glTexGeniv(coord, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexImage1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexImage1D(target, level, internalformat, width, border, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameterf)(GLenum, GLenum, GLfloat)
void APIENTRY wrapperglTexParameterf(GLenum target, GLenum pname, GLfloat param){
   _original_glTexParameterf(target, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameterfv)(GLenum, GLenum, const GLfloat *)
void APIENTRY wrapperglTexParameterfv(GLenum target, GLenum pname, const GLfloat * params){
   _original_glTexParameterfv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameteri)(GLenum, GLenum, GLint)
void APIENTRY wrapperglTexParameteri(GLenum target, GLenum pname, GLint param){
   _original_glTexParameteri(target, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameteriv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglTexParameteriv(GLenum target, GLenum pname, const GLint * params){
   _original_glTexParameteriv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTranslated)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglTranslated(GLdouble x, GLdouble y, GLdouble z){
   _original_glTranslated(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTranslatef)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglTranslatef(GLfloat x, GLfloat y, GLfloat z){
   _original_glTranslatef(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2d)(GLdouble, GLdouble)
void APIENTRY wrapperglVertex2d(GLdouble x, GLdouble y){
   _original_glVertex2d(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2dv)(const GLdouble *)
void APIENTRY wrapperglVertex2dv(const GLdouble * v){
   _original_glVertex2dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2f)(GLfloat, GLfloat)
void APIENTRY wrapperglVertex2f(GLfloat x, GLfloat y){
   _original_glVertex2f(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2fv)(const GLfloat *)
void APIENTRY wrapperglVertex2fv(const GLfloat * v){
   _original_glVertex2fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2i)(GLint, GLint)
void APIENTRY wrapperglVertex2i(GLint x, GLint y){
   _original_glVertex2i(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2iv)(const GLint *)
void APIENTRY wrapperglVertex2iv(const GLint * v){
   _original_glVertex2iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2s)(GLshort, GLshort)
void APIENTRY wrapperglVertex2s(GLshort x, GLshort y){
   _original_glVertex2s(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex2sv)(const GLshort *)
void APIENTRY wrapperglVertex2sv(const GLshort * v){
   _original_glVertex2sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertex3d(GLdouble x, GLdouble y, GLdouble z){
   _original_glVertex3d(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3dv)(const GLdouble *)
void APIENTRY wrapperglVertex3dv(const GLdouble * v){
   _original_glVertex3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglVertex3f(GLfloat x, GLfloat y, GLfloat z){
   _original_glVertex3f(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3fv)(const GLfloat *)
void APIENTRY wrapperglVertex3fv(const GLfloat * v){
   _original_glVertex3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3i)(GLint, GLint, GLint)
void APIENTRY wrapperglVertex3i(GLint x, GLint y, GLint z){
   _original_glVertex3i(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3iv)(const GLint *)
void APIENTRY wrapperglVertex3iv(const GLint * v){
   _original_glVertex3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglVertex3s(GLshort x, GLshort y, GLshort z){
   _original_glVertex3s(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex3sv)(const GLshort *)
void APIENTRY wrapperglVertex3sv(const GLshort * v){
   _original_glVertex3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4d)(GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w){
   _original_glVertex4d(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4dv)(const GLdouble *)
void APIENTRY wrapperglVertex4dv(const GLdouble * v){
   _original_glVertex4dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4f)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w){
   _original_glVertex4f(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4fv)(const GLfloat *)
void APIENTRY wrapperglVertex4fv(const GLfloat * v){
   _original_glVertex4fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4i)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglVertex4i(GLint x, GLint y, GLint z, GLint w){
   _original_glVertex4i(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4iv)(const GLint *)
void APIENTRY wrapperglVertex4iv(const GLint * v){
   _original_glVertex4iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4s)(GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglVertex4s(GLshort x, GLshort y, GLshort z, GLshort w){
   _original_glVertex4s(x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertex4sv)(const GLshort *)
void APIENTRY wrapperglVertex4sv(const GLshort * v){
   _original_glVertex4sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglViewport(GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glViewport(x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.1

 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glAreTexturesResident)(GLsizei, const GLuint *, GLboolean *)
GLboolean APIENTRY wrapperglAreTexturesResident(GLsizei n, const GLuint * textures, GLboolean * residences){
   GLboolean returnValue = _original_glAreTexturesResident(n, textures, residences);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glArrayElement)(GLint)
void APIENTRY wrapperglArrayElement(GLint i){
   _original_glArrayElement(i); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum, GLuint)
void APIENTRY wrapperglBindTexture(GLenum target, GLuint texture){
   _original_glBindTexture(target, texture); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColorPointer)(GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glColorPointer(size, type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint)
void APIENTRY wrapperglCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border){
   _original_glCopyTexImage1D(target, level, internalformat, x, y, width, border); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint)
void APIENTRY wrapperglCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border){
   _original_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei)
void APIENTRY wrapperglCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width){
   _original_glCopyTexSubImage1D(target, level, xoffset, x, y, width); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteTextures)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteTextures(GLsizei n, const GLuint * textures){
   _original_glDeleteTextures(n, textures); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDisableClientState)(GLenum)
void APIENTRY wrapperglDisableClientState(GLenum ren_array){
   _original_glDisableClientState(ren_array); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawArrays)(GLenum, GLint, GLsizei)
void APIENTRY wrapperglDrawArrays(GLenum mode, GLint first, GLsizei count){
   _original_glDrawArrays(mode, first, count); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *)
void APIENTRY wrapperglDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices){
   _original_glDrawElements(mode, count, type, indices); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEdgeFlagPointer)(GLsizei, const GLvoid *)
void APIENTRY wrapperglEdgeFlagPointer(GLsizei stride, const GLvoid * pointer){
   _original_glEdgeFlagPointer(stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnableClientState)(GLenum)
void APIENTRY wrapperglEnableClientState(GLenum ren_array){
   _original_glEnableClientState(ren_array); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei, GLuint *)
void APIENTRY wrapperglGenTextures(GLsizei n, GLuint * textures){
   _original_glGenTextures(n, textures); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexPointer)(GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglIndexPointer(GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glIndexPointer(type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexub)(GLubyte)
void APIENTRY wrapperglIndexub(GLubyte c){
   _original_glIndexub(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glIndexubv)(const GLubyte *)
void APIENTRY wrapperglIndexubv(const GLubyte * c){
   _original_glIndexubv(c); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glInterleavedArrays)(GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglInterleavedArrays(GLenum format, GLsizei stride, const GLvoid * pointer){
   _original_glInterleavedArrays(format, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint)
GLboolean APIENTRY wrapperglIsTexture(GLuint texture){
   GLboolean returnValue = _original_glIsTexture(texture);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glNormalPointer)(GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glNormalPointer(type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat, GLfloat)
void APIENTRY wrapperglPolygonOffset(GLfloat factor, GLfloat units){
   _original_glPolygonOffset(factor, units); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPopClientAttrib)()
void APIENTRY wrapperglPopClientAttrib(){
   _original_glPopClientAttrib(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPrioritizeTextures)(GLsizei, const GLuint *, const GLfloat *)
void APIENTRY wrapperglPrioritizeTextures(GLsizei n, const GLuint * textures, const GLfloat * priorities){
   _original_glPrioritizeTextures(n, textures, priorities); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPushClientAttrib)(GLbitfield)
void APIENTRY wrapperglPushClientAttrib(GLbitfield mask){
   _original_glPushClientAttrib(mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexCoordPointer)(GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glTexCoordPointer(size, type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexSubImage1D(target, level, xoffset, width, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexPointer)(GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glVertexPointer(size, type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.2

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
   _original_glBlendColor(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendEquation)(GLenum)
void APIENTRY wrapperglBlendEquation(GLenum mode){
   _original_glBlendEquation(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *)
void APIENTRY wrapperglDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices){
   _original_glDrawRangeElements(mode, start, end, count, type, indices); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels){
   _original_glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.3

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glActiveTexture)(GLenum)
void APIENTRY wrapperglActiveTexture(GLenum texture){
   _original_glActiveTexture(texture); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClientActiveTexture)(GLenum)
void APIENTRY wrapperglClientActiveTexture(GLenum texture){
   _original_glClientActiveTexture(texture); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage1D)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data){
   _original_glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetCompressedTexImage)(GLenum, GLint, GLvoid *)
void APIENTRY wrapperglGetCompressedTexImage(GLenum target, GLint level, GLvoid * img){
   _original_glGetCompressedTexImage(target, level, img); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixd)(const GLdouble *)
void APIENTRY wrapperglLoadTransposeMatrixd(const GLdouble * m){
   _original_glLoadTransposeMatrixd(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLoadTransposeMatrixf)(const GLfloat *)
void APIENTRY wrapperglLoadTransposeMatrixf(const GLfloat * m){
   _original_glLoadTransposeMatrixf(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixd)(const GLdouble *)
void APIENTRY wrapperglMultTransposeMatrixd(const GLdouble * m){
   _original_glMultTransposeMatrixd(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultTransposeMatrixf)(const GLfloat *)
void APIENTRY wrapperglMultTransposeMatrixf(const GLfloat * m){
   _original_glMultTransposeMatrixf(m); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1d)(GLenum, GLdouble)
void APIENTRY wrapperglMultiTexCoord1d(GLenum target, GLdouble s){
   _original_glMultiTexCoord1d(target, s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1dv)(GLenum, const GLdouble *)
void APIENTRY wrapperglMultiTexCoord1dv(GLenum target, const GLdouble * v){
   _original_glMultiTexCoord1dv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1f)(GLenum, GLfloat)
void APIENTRY wrapperglMultiTexCoord1f(GLenum target, GLfloat s){
   _original_glMultiTexCoord1f(target, s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1fv)(GLenum, const GLfloat *)
void APIENTRY wrapperglMultiTexCoord1fv(GLenum target, const GLfloat * v){
   _original_glMultiTexCoord1fv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1i)(GLenum, GLint)
void APIENTRY wrapperglMultiTexCoord1i(GLenum target, GLint s){
   _original_glMultiTexCoord1i(target, s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1iv)(GLenum, const GLint *)
void APIENTRY wrapperglMultiTexCoord1iv(GLenum target, const GLint * v){
   _original_glMultiTexCoord1iv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1s)(GLenum, GLshort)
void APIENTRY wrapperglMultiTexCoord1s(GLenum target, GLshort s){
   _original_glMultiTexCoord1s(target, s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord1sv)(GLenum, const GLshort *)
void APIENTRY wrapperglMultiTexCoord1sv(GLenum target, const GLshort * v){
   _original_glMultiTexCoord1sv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2d)(GLenum, GLdouble, GLdouble)
void APIENTRY wrapperglMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t){
   _original_glMultiTexCoord2d(target, s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2dv)(GLenum, const GLdouble *)
void APIENTRY wrapperglMultiTexCoord2dv(GLenum target, const GLdouble * v){
   _original_glMultiTexCoord2dv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2f)(GLenum, GLfloat, GLfloat)
void APIENTRY wrapperglMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t){
   _original_glMultiTexCoord2f(target, s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2fv)(GLenum, const GLfloat *)
void APIENTRY wrapperglMultiTexCoord2fv(GLenum target, const GLfloat * v){
   _original_glMultiTexCoord2fv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2i)(GLenum, GLint, GLint)
void APIENTRY wrapperglMultiTexCoord2i(GLenum target, GLint s, GLint t){
   _original_glMultiTexCoord2i(target, s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2iv)(GLenum, const GLint *)
void APIENTRY wrapperglMultiTexCoord2iv(GLenum target, const GLint * v){
   _original_glMultiTexCoord2iv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2s)(GLenum, GLshort, GLshort)
void APIENTRY wrapperglMultiTexCoord2s(GLenum target, GLshort s, GLshort t){
   _original_glMultiTexCoord2s(target, s, t); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord2sv)(GLenum, const GLshort *)
void APIENTRY wrapperglMultiTexCoord2sv(GLenum target, const GLshort * v){
   _original_glMultiTexCoord2sv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3d)(GLenum, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r){
   _original_glMultiTexCoord3d(target, s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3dv)(GLenum, const GLdouble *)
void APIENTRY wrapperglMultiTexCoord3dv(GLenum target, const GLdouble * v){
   _original_glMultiTexCoord3dv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3f)(GLenum, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r){
   _original_glMultiTexCoord3f(target, s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3fv)(GLenum, const GLfloat *)
void APIENTRY wrapperglMultiTexCoord3fv(GLenum target, const GLfloat * v){
   _original_glMultiTexCoord3fv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3i)(GLenum, GLint, GLint, GLint)
void APIENTRY wrapperglMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r){
   _original_glMultiTexCoord3i(target, s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3iv)(GLenum, const GLint *)
void APIENTRY wrapperglMultiTexCoord3iv(GLenum target, const GLint * v){
   _original_glMultiTexCoord3iv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3s)(GLenum, GLshort, GLshort, GLshort)
void APIENTRY wrapperglMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r){
   _original_glMultiTexCoord3s(target, s, t, r); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord3sv)(GLenum, const GLshort *)
void APIENTRY wrapperglMultiTexCoord3sv(GLenum target, const GLshort * v){
   _original_glMultiTexCoord3sv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4d)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q){
   _original_glMultiTexCoord4d(target, s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4dv)(GLenum, const GLdouble *)
void APIENTRY wrapperglMultiTexCoord4dv(GLenum target, const GLdouble * v){
   _original_glMultiTexCoord4dv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4f)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q){
   _original_glMultiTexCoord4f(target, s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4fv)(GLenum, const GLfloat *)
void APIENTRY wrapperglMultiTexCoord4fv(GLenum target, const GLfloat * v){
   _original_glMultiTexCoord4fv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4i)(GLenum, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglMultiTexCoord4i(GLenum target, GLint s, GLint t, GLint r, GLint q){
   _original_glMultiTexCoord4i(target, s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4iv)(GLenum, const GLint *)
void APIENTRY wrapperglMultiTexCoord4iv(GLenum target, const GLint * v){
   _original_glMultiTexCoord4iv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4s)(GLenum, GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q){
   _original_glMultiTexCoord4s(target, s, t, r, q); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiTexCoord4sv)(GLenum, const GLshort *)
void APIENTRY wrapperglMultiTexCoord4sv(GLenum target, const GLshort * v){
   _original_glMultiTexCoord4sv(target, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSampleCoverage)(GLfloat, GLboolean)
void APIENTRY wrapperglSampleCoverage(GLfloat value, GLboolean invert){
   _original_glSampleCoverage(value, invert); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.4

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum)
void APIENTRY wrapperglBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha){
   _original_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogCoordPointer)(GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglFogCoordPointer(GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glFogCoordPointer(type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogCoordd)(GLdouble)
void APIENTRY wrapperglFogCoordd(GLdouble coord){
   _original_glFogCoordd(coord); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogCoorddv)(const GLdouble *)
void APIENTRY wrapperglFogCoorddv(const GLdouble * coord){
   _original_glFogCoorddv(coord); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogCoordf)(GLfloat)
void APIENTRY wrapperglFogCoordf(GLfloat coord){
   _original_glFogCoordf(coord); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFogCoordfv)(const GLfloat *)
void APIENTRY wrapperglFogCoordfv(const GLfloat * coord){
   _original_glFogCoordfv(coord); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawArrays)(GLenum, const GLint *, const GLsizei *, GLsizei)
void APIENTRY wrapperglMultiDrawArrays(GLenum mode, const GLint * first, const GLsizei * count, GLsizei drawcount){
   _original_glMultiDrawArrays(mode, first, count, drawcount); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElements)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei)
void APIENTRY wrapperglMultiDrawElements(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount){
   _original_glMultiDrawElements(mode, count, type, indices, drawcount); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPointParameterf)(GLenum, GLfloat)
void APIENTRY wrapperglPointParameterf(GLenum pname, GLfloat param){
   _original_glPointParameterf(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPointParameterfv)(GLenum, const GLfloat *)
void APIENTRY wrapperglPointParameterfv(GLenum pname, const GLfloat * params){
   _original_glPointParameterfv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPointParameteri)(GLenum, GLint)
void APIENTRY wrapperglPointParameteri(GLenum pname, GLint param){
   _original_glPointParameteri(pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPointParameteriv)(GLenum, const GLint *)
void APIENTRY wrapperglPointParameteriv(GLenum pname, const GLint * params){
   _original_glPointParameteriv(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3b)(GLbyte, GLbyte, GLbyte)
void APIENTRY wrapperglSecondaryColor3b(GLbyte red, GLbyte green, GLbyte blue){
   _original_glSecondaryColor3b(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3bv)(const GLbyte *)
void APIENTRY wrapperglSecondaryColor3bv(const GLbyte * v){
   _original_glSecondaryColor3bv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglSecondaryColor3d(GLdouble red, GLdouble green, GLdouble blue){
   _original_glSecondaryColor3d(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3dv)(const GLdouble *)
void APIENTRY wrapperglSecondaryColor3dv(const GLdouble * v){
   _original_glSecondaryColor3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglSecondaryColor3f(GLfloat red, GLfloat green, GLfloat blue){
   _original_glSecondaryColor3f(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3fv)(const GLfloat *)
void APIENTRY wrapperglSecondaryColor3fv(const GLfloat * v){
   _original_glSecondaryColor3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3i)(GLint, GLint, GLint)
void APIENTRY wrapperglSecondaryColor3i(GLint red, GLint green, GLint blue){
   _original_glSecondaryColor3i(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3iv)(const GLint *)
void APIENTRY wrapperglSecondaryColor3iv(const GLint * v){
   _original_glSecondaryColor3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglSecondaryColor3s(GLshort red, GLshort green, GLshort blue){
   _original_glSecondaryColor3s(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3sv)(const GLshort *)
void APIENTRY wrapperglSecondaryColor3sv(const GLshort * v){
   _original_glSecondaryColor3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ub)(GLubyte, GLubyte, GLubyte)
void APIENTRY wrapperglSecondaryColor3ub(GLubyte red, GLubyte green, GLubyte blue){
   _original_glSecondaryColor3ub(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ubv)(const GLubyte *)
void APIENTRY wrapperglSecondaryColor3ubv(const GLubyte * v){
   _original_glSecondaryColor3ubv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3ui)(GLuint, GLuint, GLuint)
void APIENTRY wrapperglSecondaryColor3ui(GLuint red, GLuint green, GLuint blue){
   _original_glSecondaryColor3ui(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3uiv)(const GLuint *)
void APIENTRY wrapperglSecondaryColor3uiv(const GLuint * v){
   _original_glSecondaryColor3uiv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3us)(GLushort, GLushort, GLushort)
void APIENTRY wrapperglSecondaryColor3us(GLushort red, GLushort green, GLushort blue){
   _original_glSecondaryColor3us(red, green, blue); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColor3usv)(const GLushort *)
void APIENTRY wrapperglSecondaryColor3usv(const GLushort * v){
   _original_glSecondaryColor3usv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSecondaryColorPointer)(GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glSecondaryColorPointer(size, type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2d)(GLdouble, GLdouble)
void APIENTRY wrapperglWindowPos2d(GLdouble x, GLdouble y){
   _original_glWindowPos2d(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2dv)(const GLdouble *)
void APIENTRY wrapperglWindowPos2dv(const GLdouble * v){
   _original_glWindowPos2dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2f)(GLfloat, GLfloat)
void APIENTRY wrapperglWindowPos2f(GLfloat x, GLfloat y){
   _original_glWindowPos2f(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2fv)(const GLfloat *)
void APIENTRY wrapperglWindowPos2fv(const GLfloat * v){
   _original_glWindowPos2fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2i)(GLint, GLint)
void APIENTRY wrapperglWindowPos2i(GLint x, GLint y){
   _original_glWindowPos2i(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2iv)(const GLint *)
void APIENTRY wrapperglWindowPos2iv(const GLint * v){
   _original_glWindowPos2iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2s)(GLshort, GLshort)
void APIENTRY wrapperglWindowPos2s(GLshort x, GLshort y){
   _original_glWindowPos2s(x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos2sv)(const GLshort *)
void APIENTRY wrapperglWindowPos2sv(const GLshort * v){
   _original_glWindowPos2sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3d)(GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglWindowPos3d(GLdouble x, GLdouble y, GLdouble z){
   _original_glWindowPos3d(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3dv)(const GLdouble *)
void APIENTRY wrapperglWindowPos3dv(const GLdouble * v){
   _original_glWindowPos3dv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3f)(GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglWindowPos3f(GLfloat x, GLfloat y, GLfloat z){
   _original_glWindowPos3f(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3fv)(const GLfloat *)
void APIENTRY wrapperglWindowPos3fv(const GLfloat * v){
   _original_glWindowPos3fv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3i)(GLint, GLint, GLint)
void APIENTRY wrapperglWindowPos3i(GLint x, GLint y, GLint z){
   _original_glWindowPos3i(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3iv)(const GLint *)
void APIENTRY wrapperglWindowPos3iv(const GLint * v){
   _original_glWindowPos3iv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3s)(GLshort, GLshort, GLshort)
void APIENTRY wrapperglWindowPos3s(GLshort x, GLshort y, GLshort z){
   _original_glWindowPos3s(x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWindowPos3sv)(const GLshort *)
void APIENTRY wrapperglWindowPos3sv(const GLshort * v){
   _original_glWindowPos3sv(v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.5

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBeginQuery)(GLenum, GLuint)
void APIENTRY wrapperglBeginQuery(GLenum target, GLuint id){
   _original_glBeginQuery(target, id); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindBuffer)(GLenum, GLuint)
void APIENTRY wrapperglBindBuffer(GLenum target, GLuint buffer){
   _original_glBindBuffer(target, buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum)
void APIENTRY wrapperglBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage){
   _original_glBufferData(target, size, data, usage); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *)
void APIENTRY wrapperglBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data){
   _original_glBufferSubData(target, offset, size, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteBuffers)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteBuffers(GLsizei n, const GLuint * buffers){
   _original_glDeleteBuffers(n, buffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteQueries)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteQueries(GLsizei n, const GLuint * ids){
   _original_glDeleteQueries(n, ids); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEndQuery)(GLenum)
void APIENTRY wrapperglEndQuery(GLenum target){
   _original_glEndQuery(target); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenBuffers)(GLsizei, GLuint *)
void APIENTRY wrapperglGenBuffers(GLsizei n, GLuint * buffers){
   _original_glGenBuffers(n, buffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenQueries)(GLsizei, GLuint *)
void APIENTRY wrapperglGenQueries(GLsizei n, GLuint * ids){
   _original_glGenQueries(n, ids); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteriv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetBufferParameteriv(GLenum target, GLenum pname, GLint * params){
   _original_glGetBufferParameteriv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBufferPointerv)(GLenum, GLenum, GLvoid **)
void APIENTRY wrapperglGetBufferPointerv(GLenum target, GLenum pname, GLvoid ** params){
   _original_glGetBufferPointerv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBufferSubData)(GLenum, GLintptr, GLsizeiptr, GLvoid *)
void APIENTRY wrapperglGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data){
   _original_glGetBufferSubData(target, offset, size, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetQueryObjectiv(GLuint id, GLenum pname, GLint * params){
   _original_glGetQueryObjectiv(id, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectuiv)(GLuint, GLenum, GLuint *)
void APIENTRY wrapperglGetQueryObjectuiv(GLuint id, GLenum pname, GLuint * params){
   _original_glGetQueryObjectuiv(id, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryiv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetQueryiv(GLenum target, GLenum pname, GLint * params){
   _original_glGetQueryiv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsBuffer)(GLuint)
GLboolean APIENTRY wrapperglIsBuffer(GLuint buffer){
   GLboolean returnValue = _original_glIsBuffer(buffer);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsQuery)(GLuint)
GLboolean APIENTRY wrapperglIsQuery(GLuint id){
   GLboolean returnValue = _original_glIsQuery(id);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void * (CODEGEN_FUNCPTR *_ptrc_glMapBuffer)(GLenum, GLenum)
void * APIENTRY wrapperglMapBuffer(GLenum target, GLenum access){
   void * returnValue = _original_glMapBuffer(target, access);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glUnmapBuffer)(GLenum)
GLboolean APIENTRY wrapperglUnmapBuffer(GLenum target){
   GLboolean returnValue = _original_glUnmapBuffer(target);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


// Extension: 2.0

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glAttachShader)(GLuint, GLuint)
void APIENTRY wrapperglAttachShader(GLuint program, GLuint shader){
   _original_glAttachShader(program, shader); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindAttribLocation)(GLuint, GLuint, const GLchar *)
void APIENTRY wrapperglBindAttribLocation(GLuint program, GLuint index, const GLchar * name){
   _original_glBindAttribLocation(program, index, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparate)(GLenum, GLenum)
void APIENTRY wrapperglBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha){
   _original_glBlendEquationSeparate(modeRGB, modeAlpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCompileShader)(GLuint)
void APIENTRY wrapperglCompileShader(GLuint shader){
   _original_glCompileShader(shader); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateProgram)()
GLuint APIENTRY wrapperglCreateProgram(){
   GLuint returnValue = _original_glCreateProgram();
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glCreateShader)(GLenum)
GLuint APIENTRY wrapperglCreateShader(GLenum type){
   GLuint returnValue = _original_glCreateShader(type);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteProgram)(GLuint)
void APIENTRY wrapperglDeleteProgram(GLuint program){
   _original_glDeleteProgram(program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteShader)(GLuint)
void APIENTRY wrapperglDeleteShader(GLuint shader){
   _original_glDeleteShader(shader); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDetachShader)(GLuint, GLuint)
void APIENTRY wrapperglDetachShader(GLuint program, GLuint shader){
   _original_glDetachShader(program, shader); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDisableVertexAttribArray)(GLuint)
void APIENTRY wrapperglDisableVertexAttribArray(GLuint index){
   _original_glDisableVertexAttribArray(index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawBuffers)(GLsizei, const GLenum *)
void APIENTRY wrapperglDrawBuffers(GLsizei n, const GLenum * bufs){
   _original_glDrawBuffers(n, bufs); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnableVertexAttribArray)(GLuint)
void APIENTRY wrapperglEnableVertexAttribArray(GLuint index){
   _original_glEnableVertexAttribArray(index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)
void APIENTRY wrapperglGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name){
   _original_glGetActiveAttrib(program, index, bufSize, length, size, type, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)
void APIENTRY wrapperglGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name){
   _original_glGetActiveUniform(program, index, bufSize, length, size, type, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *)
void APIENTRY wrapperglGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders){
   _original_glGetAttachedShaders(program, maxCount, count, shaders); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetAttribLocation)(GLuint, const GLchar *)
GLint APIENTRY wrapperglGetAttribLocation(GLuint program, const GLchar * name){
   GLint returnValue = _original_glGetAttribLocation(program, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog){
   _original_glGetProgramInfoLog(program, bufSize, length, infoLog); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetProgramiv(GLuint program, GLenum pname, GLint * params){
   _original_glGetProgramiv(program, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog){
   _original_glGetShaderInfoLog(shader, bufSize, length, infoLog); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source){
   _original_glGetShaderSource(shader, bufSize, length, source); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetShaderiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetShaderiv(GLuint shader, GLenum pname, GLint * params){
   _original_glGetShaderiv(shader, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetUniformLocation)(GLuint, const GLchar *)
GLint APIENTRY wrapperglGetUniformLocation(GLuint program, const GLchar * name){
   GLint returnValue = _original_glGetUniformLocation(program, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformfv)(GLuint, GLint, GLfloat *)
void APIENTRY wrapperglGetUniformfv(GLuint program, GLint location, GLfloat * params){
   _original_glGetUniformfv(program, location, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformiv)(GLuint, GLint, GLint *)
void APIENTRY wrapperglGetUniformiv(GLuint program, GLint location, GLint * params){
   _original_glGetUniformiv(program, location, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribPointerv)(GLuint, GLenum, GLvoid **)
void APIENTRY wrapperglGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid ** pointer){
   _original_glGetVertexAttribPointerv(index, pname, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribdv)(GLuint, GLenum, GLdouble *)
void APIENTRY wrapperglGetVertexAttribdv(GLuint index, GLenum pname, GLdouble * params){
   _original_glGetVertexAttribdv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribfv)(GLuint, GLenum, GLfloat *)
void APIENTRY wrapperglGetVertexAttribfv(GLuint index, GLenum pname, GLfloat * params){
   _original_glGetVertexAttribfv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetVertexAttribiv(GLuint index, GLenum pname, GLint * params){
   _original_glGetVertexAttribiv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsProgram)(GLuint)
GLboolean APIENTRY wrapperglIsProgram(GLuint program){
   GLboolean returnValue = _original_glIsProgram(program);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsShader)(GLuint)
GLboolean APIENTRY wrapperglIsShader(GLuint shader){
   GLboolean returnValue = _original_glIsShader(shader);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLinkProgram)(GLuint)
void APIENTRY wrapperglLinkProgram(GLuint program){
   _original_glLinkProgram(program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *)
void APIENTRY wrapperglShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length){
   _original_glShaderSource(shader, count, string, length); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint)
void APIENTRY wrapperglStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask){
   _original_glStencilFuncSeparate(face, func, ref, mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilMaskSeparate)(GLenum, GLuint)
void APIENTRY wrapperglStencilMaskSeparate(GLenum face, GLuint mask){
   _original_glStencilMaskSeparate(face, mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum)
void APIENTRY wrapperglStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass){
   _original_glStencilOpSeparate(face, sfail, dpfail, dppass); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1f)(GLint, GLfloat)
void APIENTRY wrapperglUniform1f(GLint location, GLfloat v0){
   _original_glUniform1f(location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1fv)(GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglUniform1fv(GLint location, GLsizei count, const GLfloat * value){
   _original_glUniform1fv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1i)(GLint, GLint)
void APIENTRY wrapperglUniform1i(GLint location, GLint v0){
   _original_glUniform1i(location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1iv)(GLint, GLsizei, const GLint *)
void APIENTRY wrapperglUniform1iv(GLint location, GLsizei count, const GLint * value){
   _original_glUniform1iv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2f)(GLint, GLfloat, GLfloat)
void APIENTRY wrapperglUniform2f(GLint location, GLfloat v0, GLfloat v1){
   _original_glUniform2f(location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2fv)(GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglUniform2fv(GLint location, GLsizei count, const GLfloat * value){
   _original_glUniform2fv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2i)(GLint, GLint, GLint)
void APIENTRY wrapperglUniform2i(GLint location, GLint v0, GLint v1){
   _original_glUniform2i(location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2iv)(GLint, GLsizei, const GLint *)
void APIENTRY wrapperglUniform2iv(GLint location, GLsizei count, const GLint * value){
   _original_glUniform2iv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){
   _original_glUniform3f(location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3fv)(GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglUniform3fv(GLint location, GLsizei count, const GLfloat * value){
   _original_glUniform3fv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3i)(GLint, GLint, GLint, GLint)
void APIENTRY wrapperglUniform3i(GLint location, GLint v0, GLint v1, GLint v2){
   _original_glUniform3i(location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3iv)(GLint, GLsizei, const GLint *)
void APIENTRY wrapperglUniform3iv(GLint location, GLsizei count, const GLint * value){
   _original_glUniform3iv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){
   _original_glUniform4f(location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4fv)(GLint, GLsizei, const GLfloat *)
void APIENTRY wrapperglUniform4fv(GLint location, GLsizei count, const GLfloat * value){
   _original_glUniform4fv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4i)(GLint, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3){
   _original_glUniform4i(location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4iv)(GLint, GLsizei, const GLint *)
void APIENTRY wrapperglUniform4iv(GLint location, GLsizei count, const GLint * value){
   _original_glUniform4iv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix2fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix3fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix4fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUseProgram)(GLuint)
void APIENTRY wrapperglUseProgram(GLuint program){
   _original_glUseProgram(program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glValidateProgram)(GLuint)
void APIENTRY wrapperglValidateProgram(GLuint program){
   _original_glValidateProgram(program); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1d)(GLuint, GLdouble)
void APIENTRY wrapperglVertexAttrib1d(GLuint index, GLdouble x){
   _original_glVertexAttrib1d(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttrib1dv(GLuint index, const GLdouble * v){
   _original_glVertexAttrib1dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1f)(GLuint, GLfloat)
void APIENTRY wrapperglVertexAttrib1f(GLuint index, GLfloat x){
   _original_glVertexAttrib1f(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1fv)(GLuint, const GLfloat *)
void APIENTRY wrapperglVertexAttrib1fv(GLuint index, const GLfloat * v){
   _original_glVertexAttrib1fv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1s)(GLuint, GLshort)
void APIENTRY wrapperglVertexAttrib1s(GLuint index, GLshort x){
   _original_glVertexAttrib1s(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib1sv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttrib1sv(GLuint index, const GLshort * v){
   _original_glVertexAttrib1sv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2d)(GLuint, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttrib2d(GLuint index, GLdouble x, GLdouble y){
   _original_glVertexAttrib2d(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttrib2dv(GLuint index, const GLdouble * v){
   _original_glVertexAttrib2dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2f)(GLuint, GLfloat, GLfloat)
void APIENTRY wrapperglVertexAttrib2f(GLuint index, GLfloat x, GLfloat y){
   _original_glVertexAttrib2f(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2fv)(GLuint, const GLfloat *)
void APIENTRY wrapperglVertexAttrib2fv(GLuint index, const GLfloat * v){
   _original_glVertexAttrib2fv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2s)(GLuint, GLshort, GLshort)
void APIENTRY wrapperglVertexAttrib2s(GLuint index, GLshort x, GLshort y){
   _original_glVertexAttrib2s(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib2sv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttrib2sv(GLuint index, const GLshort * v){
   _original_glVertexAttrib2sv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3d)(GLuint, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z){
   _original_glVertexAttrib3d(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttrib3dv(GLuint index, const GLdouble * v){
   _original_glVertexAttrib3dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z){
   _original_glVertexAttrib3f(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3fv)(GLuint, const GLfloat *)
void APIENTRY wrapperglVertexAttrib3fv(GLuint index, const GLfloat * v){
   _original_glVertexAttrib3fv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3s)(GLuint, GLshort, GLshort, GLshort)
void APIENTRY wrapperglVertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z){
   _original_glVertexAttrib3s(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib3sv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttrib3sv(GLuint index, const GLshort * v){
   _original_glVertexAttrib3sv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nbv)(GLuint, const GLbyte *)
void APIENTRY wrapperglVertexAttrib4Nbv(GLuint index, const GLbyte * v){
   _original_glVertexAttrib4Nbv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Niv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttrib4Niv(GLuint index, const GLint * v){
   _original_glVertexAttrib4Niv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nsv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttrib4Nsv(GLuint index, const GLshort * v){
   _original_glVertexAttrib4Nsv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nub)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte)
void APIENTRY wrapperglVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w){
   _original_glVertexAttrib4Nub(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nubv)(GLuint, const GLubyte *)
void APIENTRY wrapperglVertexAttrib4Nubv(GLuint index, const GLubyte * v){
   _original_glVertexAttrib4Nubv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nuiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttrib4Nuiv(GLuint index, const GLuint * v){
   _original_glVertexAttrib4Nuiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4Nusv)(GLuint, const GLushort *)
void APIENTRY wrapperglVertexAttrib4Nusv(GLuint index, const GLushort * v){
   _original_glVertexAttrib4Nusv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4bv)(GLuint, const GLbyte *)
void APIENTRY wrapperglVertexAttrib4bv(GLuint index, const GLbyte * v){
   _original_glVertexAttrib4bv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){
   _original_glVertexAttrib4d(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttrib4dv(GLuint index, const GLdouble * v){
   _original_glVertexAttrib4dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)
void APIENTRY wrapperglVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){
   _original_glVertexAttrib4f(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4fv)(GLuint, const GLfloat *)
void APIENTRY wrapperglVertexAttrib4fv(GLuint index, const GLfloat * v){
   _original_glVertexAttrib4fv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4iv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttrib4iv(GLuint index, const GLint * v){
   _original_glVertexAttrib4iv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4s)(GLuint, GLshort, GLshort, GLshort, GLshort)
void APIENTRY wrapperglVertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w){
   _original_glVertexAttrib4s(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4sv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttrib4sv(GLuint index, const GLshort * v){
   _original_glVertexAttrib4sv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4ubv)(GLuint, const GLubyte *)
void APIENTRY wrapperglVertexAttrib4ubv(GLuint index, const GLubyte * v){
   _original_glVertexAttrib4ubv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4uiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttrib4uiv(GLuint index, const GLuint * v){
   _original_glVertexAttrib4uiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttrib4usv)(GLuint, const GLushort *)
void APIENTRY wrapperglVertexAttrib4usv(GLuint index, const GLushort * v){
   _original_glVertexAttrib4usv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)
void APIENTRY wrapperglVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer){
   _original_glVertexAttribPointer(index, size, type, normalized, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 2.1

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix2x3fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix2x4fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix3x2fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix3x4fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix4x2fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *)
void APIENTRY wrapperglUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value){
   _original_glUniformMatrix4x3fv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 3.0

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBeginConditionalRender)(GLuint, GLenum)
void APIENTRY wrapperglBeginConditionalRender(GLuint id, GLenum mode){
   _original_glBeginConditionalRender(id, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBeginTransformFeedback)(GLenum)
void APIENTRY wrapperglBeginTransformFeedback(GLenum primitiveMode){
   _original_glBeginTransformFeedback(primitiveMode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindBufferBase)(GLenum, GLuint, GLuint)
void APIENTRY wrapperglBindBufferBase(GLenum target, GLuint index, GLuint buffer){
   _original_glBindBufferBase(target, index, buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindBufferRange)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)
void APIENTRY wrapperglBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){
   _original_glBindBufferRange(target, index, buffer, offset, size); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocation)(GLuint, GLuint, const GLchar *)
void APIENTRY wrapperglBindFragDataLocation(GLuint program, GLuint color, const GLchar * name){
   _original_glBindFragDataLocation(program, color, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindFramebuffer)(GLenum, GLuint)
void APIENTRY wrapperglBindFramebuffer(GLenum target, GLuint framebuffer){
   _original_glBindFramebuffer(target, framebuffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindRenderbuffer)(GLenum, GLuint)
void APIENTRY wrapperglBindRenderbuffer(GLenum target, GLuint renderbuffer){
   _original_glBindRenderbuffer(target, renderbuffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindVertexArray)(GLuint)
void APIENTRY wrapperglBindVertexArray(GLuint ren_array){
   _original_glBindVertexArray(ren_array); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)
void APIENTRY wrapperglBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){
   _original_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLenum (CODEGEN_FUNCPTR *_ptrc_glCheckFramebufferStatus)(GLenum)
GLenum APIENTRY wrapperglCheckFramebufferStatus(GLenum target){
   GLenum returnValue = _original_glCheckFramebufferStatus(target);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClampColor)(GLenum, GLenum)
void APIENTRY wrapperglClampColor(GLenum target, GLenum clamp){
   _original_glClampColor(target, clamp); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfi)(GLenum, GLint, GLfloat, GLint)
void APIENTRY wrapperglClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil){
   _original_glClearBufferfi(buffer, drawbuffer, depth, stencil); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferfv)(GLenum, GLint, const GLfloat *)
void APIENTRY wrapperglClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat * value){
   _original_glClearBufferfv(buffer, drawbuffer, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferiv)(GLenum, GLint, const GLint *)
void APIENTRY wrapperglClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint * value){
   _original_glClearBufferiv(buffer, drawbuffer, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearBufferuiv)(GLenum, GLint, const GLuint *)
void APIENTRY wrapperglClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint * value){
   _original_glClearBufferuiv(buffer, drawbuffer, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColorMaski)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)
void APIENTRY wrapperglColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a){
   _original_glColorMaski(index, r, g, b, a); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteFramebuffers)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteFramebuffers(GLsizei n, const GLuint * framebuffers){
   _original_glDeleteFramebuffers(n, framebuffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteRenderbuffers)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteRenderbuffers(GLsizei n, const GLuint * renderbuffers){
   _original_glDeleteRenderbuffers(n, renderbuffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteVertexArrays)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteVertexArrays(GLsizei n, const GLuint * arrays){
   _original_glDeleteVertexArrays(n, arrays); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDisablei)(GLenum, GLuint)
void APIENTRY wrapperglDisablei(GLenum target, GLuint index){
   _original_glDisablei(target, index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnablei)(GLenum, GLuint)
void APIENTRY wrapperglEnablei(GLenum target, GLuint index){
   _original_glEnablei(target, index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEndConditionalRender)()
void APIENTRY wrapperglEndConditionalRender(){
   _original_glEndConditionalRender(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEndTransformFeedback)()
void APIENTRY wrapperglEndTransformFeedback(){
   _original_glEndTransformFeedback(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr)
void APIENTRY wrapperglFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length){
   _original_glFlushMappedBufferRange(target, offset, length); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint)
void APIENTRY wrapperglFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){
   _original_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture1D)(GLenum, GLenum, GLenum, GLuint, GLint)
void APIENTRY wrapperglFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){
   _original_glFramebufferTexture1D(target, attachment, textarget, texture, level); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint)
void APIENTRY wrapperglFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){
   _original_glFramebufferTexture2D(target, attachment, textarget, texture, level); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture3D)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)
void APIENTRY wrapperglFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset){
   _original_glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTextureLayer)(GLenum, GLenum, GLuint, GLint, GLint)
void APIENTRY wrapperglFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){
   _original_glFramebufferTextureLayer(target, attachment, texture, level, layer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenFramebuffers)(GLsizei, GLuint *)
void APIENTRY wrapperglGenFramebuffers(GLsizei n, GLuint * framebuffers){
   _original_glGenFramebuffers(n, framebuffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenRenderbuffers)(GLsizei, GLuint *)
void APIENTRY wrapperglGenRenderbuffers(GLsizei n, GLuint * renderbuffers){
   _original_glGenRenderbuffers(n, renderbuffers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenVertexArrays)(GLsizei, GLuint *)
void APIENTRY wrapperglGenVertexArrays(GLsizei n, GLuint * arrays){
   _original_glGenVertexArrays(n, arrays); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenerateMipmap)(GLenum)
void APIENTRY wrapperglGenerateMipmap(GLenum target){
   _original_glGenerateMipmap(target); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBooleani_v)(GLenum, GLuint, GLboolean *)
void APIENTRY wrapperglGetBooleani_v(GLenum target, GLuint index, GLboolean * data){
   _original_glGetBooleani_v(target, index, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataLocation)(GLuint, const GLchar *)
GLint APIENTRY wrapperglGetFragDataLocation(GLuint program, const GLchar * name){
   GLint returnValue = _original_glGetFragDataLocation(program, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint * params){
   _original_glGetFramebufferAttachmentParameteriv(target, attachment, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetIntegeri_v)(GLenum, GLuint, GLint *)
void APIENTRY wrapperglGetIntegeri_v(GLenum target, GLuint index, GLint * data){
   _original_glGetIntegeri_v(target, index, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint * params){
   _original_glGetRenderbufferParameteriv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetStringi)(GLenum, GLuint)
const GLubyte * APIENTRY wrapperglGetStringi(GLenum name, GLuint index){
   const GLubyte * returnValue = _original_glGetStringi(name, index);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIiv)(GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetTexParameterIiv(GLenum target, GLenum pname, GLint * params){
   _original_glGetTexParameterIiv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTexParameterIuiv)(GLenum, GLenum, GLuint *)
void APIENTRY wrapperglGetTexParameterIuiv(GLenum target, GLenum pname, GLuint * params){
   _original_glGetTexParameterIuiv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetTransformFeedbackVarying)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *)
void APIENTRY wrapperglGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name){
   _original_glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformuiv)(GLuint, GLint, GLuint *)
void APIENTRY wrapperglGetUniformuiv(GLuint program, GLint location, GLuint * params){
   _original_glGetUniformuiv(program, location, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetVertexAttribIiv(GLuint index, GLenum pname, GLint * params){
   _original_glGetVertexAttribIiv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribIuiv)(GLuint, GLenum, GLuint *)
void APIENTRY wrapperglGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint * params){
   _original_glGetVertexAttribIuiv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabledi)(GLenum, GLuint)
GLboolean APIENTRY wrapperglIsEnabledi(GLenum target, GLuint index){
   GLboolean returnValue = _original_glIsEnabledi(target, index);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsFramebuffer)(GLuint)
GLboolean APIENTRY wrapperglIsFramebuffer(GLuint framebuffer){
   GLboolean returnValue = _original_glIsFramebuffer(framebuffer);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsRenderbuffer)(GLuint)
GLboolean APIENTRY wrapperglIsRenderbuffer(GLuint renderbuffer){
   GLboolean returnValue = _original_glIsRenderbuffer(renderbuffer);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsVertexArray)(GLuint)
GLboolean APIENTRY wrapperglIsVertexArray(GLuint ren_array){
   GLboolean returnValue = _original_glIsVertexArray(ren_array);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void * (CODEGEN_FUNCPTR *_ptrc_glMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield)
void * APIENTRY wrapperglMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access){
   void * returnValue = _original_glMapBufferRange(target, offset, length, access);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei)
void APIENTRY wrapperglRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height){
   _original_glRenderbufferStorage(target, internalformat, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glRenderbufferStorageMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)
void APIENTRY wrapperglRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height){
   _original_glRenderbufferStorageMultisample(target, samples, internalformat, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIiv)(GLenum, GLenum, const GLint *)
void APIENTRY wrapperglTexParameterIiv(GLenum target, GLenum pname, const GLint * params){
   _original_glTexParameterIiv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexParameterIuiv)(GLenum, GLenum, const GLuint *)
void APIENTRY wrapperglTexParameterIuiv(GLenum target, GLenum pname, const GLuint * params){
   _original_glTexParameterIuiv(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTransformFeedbackVaryings)(GLuint, GLsizei, const GLchar *const*, GLenum)
void APIENTRY wrapperglTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode){
   _original_glTransformFeedbackVaryings(program, count, varyings, bufferMode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1ui)(GLint, GLuint)
void APIENTRY wrapperglUniform1ui(GLint location, GLuint v0){
   _original_glUniform1ui(location, v0); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1uiv)(GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglUniform1uiv(GLint location, GLsizei count, const GLuint * value){
   _original_glUniform1uiv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2ui)(GLint, GLuint, GLuint)
void APIENTRY wrapperglUniform2ui(GLint location, GLuint v0, GLuint v1){
   _original_glUniform2ui(location, v0, v1); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2uiv)(GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglUniform2uiv(GLint location, GLsizei count, const GLuint * value){
   _original_glUniform2uiv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3ui)(GLint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2){
   _original_glUniform3ui(location, v0, v1, v2); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3uiv)(GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglUniform3uiv(GLint location, GLsizei count, const GLuint * value){
   _original_glUniform3uiv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4ui)(GLint, GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){
   _original_glUniform4ui(location, v0, v1, v2, v3); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4uiv)(GLint, GLsizei, const GLuint *)
void APIENTRY wrapperglUniform4uiv(GLint location, GLsizei count, const GLuint * value){
   _original_glUniform4uiv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1i)(GLuint, GLint)
void APIENTRY wrapperglVertexAttribI1i(GLuint index, GLint x){
   _original_glVertexAttribI1i(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1iv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttribI1iv(GLuint index, const GLint * v){
   _original_glVertexAttribI1iv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1ui)(GLuint, GLuint)
void APIENTRY wrapperglVertexAttribI1ui(GLuint index, GLuint x){
   _original_glVertexAttribI1ui(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI1uiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttribI1uiv(GLuint index, const GLuint * v){
   _original_glVertexAttribI1uiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2i)(GLuint, GLint, GLint)
void APIENTRY wrapperglVertexAttribI2i(GLuint index, GLint x, GLint y){
   _original_glVertexAttribI2i(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2iv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttribI2iv(GLuint index, const GLint * v){
   _original_glVertexAttribI2iv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2ui)(GLuint, GLuint, GLuint)
void APIENTRY wrapperglVertexAttribI2ui(GLuint index, GLuint x, GLuint y){
   _original_glVertexAttribI2ui(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI2uiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttribI2uiv(GLuint index, const GLuint * v){
   _original_glVertexAttribI2uiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3i)(GLuint, GLint, GLint, GLint)
void APIENTRY wrapperglVertexAttribI3i(GLuint index, GLint x, GLint y, GLint z){
   _original_glVertexAttribI3i(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3iv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttribI3iv(GLuint index, const GLint * v){
   _original_glVertexAttribI3iv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3ui)(GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z){
   _original_glVertexAttribI3ui(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI3uiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttribI3uiv(GLuint index, const GLuint * v){
   _original_glVertexAttribI3uiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4bv)(GLuint, const GLbyte *)
void APIENTRY wrapperglVertexAttribI4bv(GLuint index, const GLbyte * v){
   _original_glVertexAttribI4bv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4i)(GLuint, GLint, GLint, GLint, GLint)
void APIENTRY wrapperglVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w){
   _original_glVertexAttribI4i(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4iv)(GLuint, const GLint *)
void APIENTRY wrapperglVertexAttribI4iv(GLuint index, const GLint * v){
   _original_glVertexAttribI4iv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4sv)(GLuint, const GLshort *)
void APIENTRY wrapperglVertexAttribI4sv(GLuint index, const GLshort * v){
   _original_glVertexAttribI4sv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ubv)(GLuint, const GLubyte *)
void APIENTRY wrapperglVertexAttribI4ubv(GLuint index, const GLubyte * v){
   _original_glVertexAttribI4ubv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4ui)(GLuint, GLuint, GLuint, GLuint, GLuint)
void APIENTRY wrapperglVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){
   _original_glVertexAttribI4ui(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4uiv)(GLuint, const GLuint *)
void APIENTRY wrapperglVertexAttribI4uiv(GLuint index, const GLuint * v){
   _original_glVertexAttribI4uiv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribI4usv)(GLuint, const GLushort *)
void APIENTRY wrapperglVertexAttribI4usv(GLuint index, const GLushort * v){
   _original_glVertexAttribI4usv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glVertexAttribIPointer(index, size, type, stride, pointer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


void mapOriginalGLFunctions() {
        _ptrc_glProgramParameteriEXT = _original_glProgramParameteriEXT;

        _ptrc_glFramebufferTextureARB = _original_glFramebufferTextureARB;
        _ptrc_glFramebufferTextureFaceARB = _original_glFramebufferTextureFaceARB;
        _ptrc_glFramebufferTextureLayerARB = _original_glFramebufferTextureLayerARB;
        _ptrc_glProgramParameteriARB = _original_glProgramParameteriARB;

        _ptrc_glPatchParameterfv = _original_glPatchParameterfv;
        _ptrc_glPatchParameteri = _original_glPatchParameteri;

        _ptrc_glDispatchCompute = _original_glDispatchCompute;
        _ptrc_glDispatchComputeIndirect = _original_glDispatchComputeIndirect;

        _ptrc_glDebugMessageCallbackARB = _original_glDebugMessageCallbackARB;
        _ptrc_glDebugMessageControlARB = _original_glDebugMessageControlARB;
        _ptrc_glDebugMessageInsertARB = _original_glDebugMessageInsertARB;
        _ptrc_glGetDebugMessageLogARB = _original_glGetDebugMessageLogARB;

        _ptrc_glDebugMessageCallback = _original_glDebugMessageCallback;
        _ptrc_glDebugMessageControl = _original_glDebugMessageControl;
        _ptrc_glDebugMessageInsert = _original_glDebugMessageInsert;
        _ptrc_glGetDebugMessageLog = _original_glGetDebugMessageLog;
        _ptrc_glGetObjectLabel = _original_glGetObjectLabel;
        _ptrc_glGetObjectPtrLabel = _original_glGetObjectPtrLabel;
        _ptrc_glGetPointerv = _original_glGetPointerv;
        _ptrc_glObjectLabel = _original_glObjectLabel;
        _ptrc_glObjectPtrLabel = _original_glObjectPtrLabel;
        _ptrc_glPopDebugGroup = _original_glPopDebugGroup;
        _ptrc_glPushDebugGroup = _original_glPushDebugGroup;

        _ptrc_glClearDepthf = _original_glClearDepthf;
        _ptrc_glDepthRangef = _original_glDepthRangef;
        _ptrc_glGetShaderPrecisionFormat = _original_glGetShaderPrecisionFormat;
        _ptrc_glReleaseShaderCompiler = _original_glReleaseShaderCompiler;
        _ptrc_glShaderBinary = _original_glShaderBinary;

        _ptrc_glGetProgramBinary = _original_glGetProgramBinary;
        _ptrc_glProgramBinary = _original_glProgramBinary;
        _ptrc_glProgramParameteri = _original_glProgramParameteri;

        _ptrc_glGetInternalformativ = _original_glGetInternalformativ;

        _ptrc_glGetInternalformati64v = _original_glGetInternalformati64v;

        _ptrc_glGetProgramInterfaceiv = _original_glGetProgramInterfaceiv;
        _ptrc_glGetProgramResourceIndex = _original_glGetProgramResourceIndex;
        _ptrc_glGetProgramResourceLocation = _original_glGetProgramResourceLocation;
        _ptrc_glGetProgramResourceLocationIndex = _original_glGetProgramResourceLocationIndex;
        _ptrc_glGetProgramResourceName = _original_glGetProgramResourceName;
        _ptrc_glGetProgramResourceiv = _original_glGetProgramResourceiv;

        _ptrc_glActiveShaderProgram = _original_glActiveShaderProgram;
        _ptrc_glBindProgramPipeline = _original_glBindProgramPipeline;
        _ptrc_glCreateShaderProgramv = _original_glCreateShaderProgramv;
        _ptrc_glDeleteProgramPipelines = _original_glDeleteProgramPipelines;
        _ptrc_glGenProgramPipelines = _original_glGenProgramPipelines;
        _ptrc_glGetProgramPipelineInfoLog = _original_glGetProgramPipelineInfoLog;
        _ptrc_glGetProgramPipelineiv = _original_glGetProgramPipelineiv;
        _ptrc_glIsProgramPipeline = _original_glIsProgramPipeline;
        _ptrc_glProgramUniform1d = _original_glProgramUniform1d;
        _ptrc_glProgramUniform1dv = _original_glProgramUniform1dv;
        _ptrc_glProgramUniform1f = _original_glProgramUniform1f;
        _ptrc_glProgramUniform1fv = _original_glProgramUniform1fv;
        _ptrc_glProgramUniform1i = _original_glProgramUniform1i;
        _ptrc_glProgramUniform1iv = _original_glProgramUniform1iv;
        _ptrc_glProgramUniform1ui = _original_glProgramUniform1ui;
        _ptrc_glProgramUniform1uiv = _original_glProgramUniform1uiv;
        _ptrc_glProgramUniform2d = _original_glProgramUniform2d;
        _ptrc_glProgramUniform2dv = _original_glProgramUniform2dv;
        _ptrc_glProgramUniform2f = _original_glProgramUniform2f;
        _ptrc_glProgramUniform2fv = _original_glProgramUniform2fv;
        _ptrc_glProgramUniform2i = _original_glProgramUniform2i;
        _ptrc_glProgramUniform2iv = _original_glProgramUniform2iv;
        _ptrc_glProgramUniform2ui = _original_glProgramUniform2ui;
        _ptrc_glProgramUniform2uiv = _original_glProgramUniform2uiv;
        _ptrc_glProgramUniform3d = _original_glProgramUniform3d;
        _ptrc_glProgramUniform3dv = _original_glProgramUniform3dv;
        _ptrc_glProgramUniform3f = _original_glProgramUniform3f;
        _ptrc_glProgramUniform3fv = _original_glProgramUniform3fv;
        _ptrc_glProgramUniform3i = _original_glProgramUniform3i;
        _ptrc_glProgramUniform3iv = _original_glProgramUniform3iv;
        _ptrc_glProgramUniform3ui = _original_glProgramUniform3ui;
        _ptrc_glProgramUniform3uiv = _original_glProgramUniform3uiv;
        _ptrc_glProgramUniform4d = _original_glProgramUniform4d;
        _ptrc_glProgramUniform4dv = _original_glProgramUniform4dv;
        _ptrc_glProgramUniform4f = _original_glProgramUniform4f;
        _ptrc_glProgramUniform4fv = _original_glProgramUniform4fv;
        _ptrc_glProgramUniform4i = _original_glProgramUniform4i;
        _ptrc_glProgramUniform4iv = _original_glProgramUniform4iv;
        _ptrc_glProgramUniform4ui = _original_glProgramUniform4ui;
        _ptrc_glProgramUniform4uiv = _original_glProgramUniform4uiv;
        _ptrc_glProgramUniformMatrix2dv = _original_glProgramUniformMatrix2dv;
        _ptrc_glProgramUniformMatrix2fv = _original_glProgramUniformMatrix2fv;
        _ptrc_glProgramUniformMatrix2x3dv = _original_glProgramUniformMatrix2x3dv;
        _ptrc_glProgramUniformMatrix2x3fv = _original_glProgramUniformMatrix2x3fv;
        _ptrc_glProgramUniformMatrix2x4dv = _original_glProgramUniformMatrix2x4dv;
        _ptrc_glProgramUniformMatrix2x4fv = _original_glProgramUniformMatrix2x4fv;
        _ptrc_glProgramUniformMatrix3dv = _original_glProgramUniformMatrix3dv;
        _ptrc_glProgramUniformMatrix3fv = _original_glProgramUniformMatrix3fv;
        _ptrc_glProgramUniformMatrix3x2dv = _original_glProgramUniformMatrix3x2dv;
        _ptrc_glProgramUniformMatrix3x2fv = _original_glProgramUniformMatrix3x2fv;
        _ptrc_glProgramUniformMatrix3x4dv = _original_glProgramUniformMatrix3x4dv;
        _ptrc_glProgramUniformMatrix3x4fv = _original_glProgramUniformMatrix3x4fv;
        _ptrc_glProgramUniformMatrix4dv = _original_glProgramUniformMatrix4dv;
        _ptrc_glProgramUniformMatrix4fv = _original_glProgramUniformMatrix4fv;
        _ptrc_glProgramUniformMatrix4x2dv = _original_glProgramUniformMatrix4x2dv;
        _ptrc_glProgramUniformMatrix4x2fv = _original_glProgramUniformMatrix4x2fv;
        _ptrc_glProgramUniformMatrix4x3dv = _original_glProgramUniformMatrix4x3dv;
        _ptrc_glProgramUniformMatrix4x3fv = _original_glProgramUniformMatrix4x3fv;
        _ptrc_glUseProgramStages = _original_glUseProgramStages;
        _ptrc_glValidateProgramPipeline = _original_glValidateProgramPipeline;

        _ptrc_glTexBufferRange = _original_glTexBufferRange;

        _ptrc_glTexStorage1D = _original_glTexStorage1D;
        _ptrc_glTexStorage2D = _original_glTexStorage2D;
        _ptrc_glTexStorage3D = _original_glTexStorage3D;

        _ptrc_glTextureView = _original_glTextureView;

        _ptrc_glBindVertexBuffer = _original_glBindVertexBuffer;
        _ptrc_glVertexAttribBinding = _original_glVertexAttribBinding;
        _ptrc_glVertexAttribFormat = _original_glVertexAttribFormat;
        _ptrc_glVertexAttribIFormat = _original_glVertexAttribIFormat;
        _ptrc_glVertexAttribLFormat = _original_glVertexAttribLFormat;
        _ptrc_glVertexBindingDivisor = _original_glVertexBindingDivisor;

        _ptrc_glDepthRangeArrayv = _original_glDepthRangeArrayv;
        _ptrc_glDepthRangeIndexed = _original_glDepthRangeIndexed;
        _ptrc_glGetDoublei_v = _original_glGetDoublei_v;
        _ptrc_glGetFloati_v = _original_glGetFloati_v;
        _ptrc_glScissorArrayv = _original_glScissorArrayv;
        _ptrc_glScissorIndexed = _original_glScissorIndexed;
        _ptrc_glScissorIndexedv = _original_glScissorIndexedv;
        _ptrc_glViewportArrayv = _original_glViewportArrayv;
        _ptrc_glViewportIndexedf = _original_glViewportIndexedf;
        _ptrc_glViewportIndexedfv = _original_glViewportIndexedfv;

        _ptrc_glClearBufferData = _original_glClearBufferData;
        _ptrc_glClearBufferSubData = _original_glClearBufferSubData;

        _ptrc_glCopyImageSubData = _original_glCopyImageSubData;

        _ptrc_glFramebufferParameteri = _original_glFramebufferParameteri;
        _ptrc_glGetFramebufferParameteriv = _original_glGetFramebufferParameteriv;

        _ptrc_glInvalidateBufferData = _original_glInvalidateBufferData;
        _ptrc_glInvalidateBufferSubData = _original_glInvalidateBufferSubData;
        _ptrc_glInvalidateFramebuffer = _original_glInvalidateFramebuffer;
        _ptrc_glInvalidateSubFramebuffer = _original_glInvalidateSubFramebuffer;
        _ptrc_glInvalidateTexImage = _original_glInvalidateTexImage;
        _ptrc_glInvalidateTexSubImage = _original_glInvalidateTexSubImage;

        _ptrc_glTexStorage2DMultisample = _original_glTexStorage2DMultisample;
        _ptrc_glTexStorage3DMultisample = _original_glTexStorage3DMultisample;

        _ptrc_glTextureBarrierNV = _original_glTextureBarrierNV;

        _ptrc_glCopyImageSubDataNV = _original_glCopyImageSubDataNV;

        _ptrc_glAccum = _original_glAccum;
        _ptrc_glAlphaFunc = _original_glAlphaFunc;
        _ptrc_glBegin = _original_glBegin;
        _ptrc_glBitmap = _original_glBitmap;
        _ptrc_glBlendFunc = _original_glBlendFunc;
        _ptrc_glCallList = _original_glCallList;
        _ptrc_glCallLists = _original_glCallLists;
        _ptrc_glClear = _original_glClear;
        _ptrc_glClearAccum = _original_glClearAccum;
        _ptrc_glClearColor = _original_glClearColor;
        _ptrc_glClearDepth = _original_glClearDepth;
        _ptrc_glClearIndex = _original_glClearIndex;
        _ptrc_glClearStencil = _original_glClearStencil;
        _ptrc_glClipPlane = _original_glClipPlane;
        _ptrc_glColor3b = _original_glColor3b;
        _ptrc_glColor3bv = _original_glColor3bv;
        _ptrc_glColor3d = _original_glColor3d;
        _ptrc_glColor3dv = _original_glColor3dv;
        _ptrc_glColor3f = _original_glColor3f;
        _ptrc_glColor3fv = _original_glColor3fv;
        _ptrc_glColor3i = _original_glColor3i;
        _ptrc_glColor3iv = _original_glColor3iv;
        _ptrc_glColor3s = _original_glColor3s;
        _ptrc_glColor3sv = _original_glColor3sv;
        _ptrc_glColor3ub = _original_glColor3ub;
        _ptrc_glColor3ubv = _original_glColor3ubv;
        _ptrc_glColor3ui = _original_glColor3ui;
        _ptrc_glColor3uiv = _original_glColor3uiv;
        _ptrc_glColor3us = _original_glColor3us;
        _ptrc_glColor3usv = _original_glColor3usv;
        _ptrc_glColor4b = _original_glColor4b;
        _ptrc_glColor4bv = _original_glColor4bv;
        _ptrc_glColor4d = _original_glColor4d;
        _ptrc_glColor4dv = _original_glColor4dv;
        _ptrc_glColor4f = _original_glColor4f;
        _ptrc_glColor4fv = _original_glColor4fv;
        _ptrc_glColor4i = _original_glColor4i;
        _ptrc_glColor4iv = _original_glColor4iv;
        _ptrc_glColor4s = _original_glColor4s;
        _ptrc_glColor4sv = _original_glColor4sv;
        _ptrc_glColor4ub = _original_glColor4ub;
        _ptrc_glColor4ubv = _original_glColor4ubv;
        _ptrc_glColor4ui = _original_glColor4ui;
        _ptrc_glColor4uiv = _original_glColor4uiv;
        _ptrc_glColor4us = _original_glColor4us;
        _ptrc_glColor4usv = _original_glColor4usv;
        _ptrc_glColorMask = _original_glColorMask;
        _ptrc_glColorMaterial = _original_glColorMaterial;
        _ptrc_glCopyPixels = _original_glCopyPixels;
        _ptrc_glCullFace = _original_glCullFace;
        _ptrc_glDeleteLists = _original_glDeleteLists;
        _ptrc_glDepthFunc = _original_glDepthFunc;
        _ptrc_glDepthMask = _original_glDepthMask;
        _ptrc_glDepthRange = _original_glDepthRange;
        _ptrc_glDisable = _original_glDisable;
        _ptrc_glDrawBuffer = _original_glDrawBuffer;
        _ptrc_glDrawPixels = _original_glDrawPixels;
        _ptrc_glEdgeFlag = _original_glEdgeFlag;
        _ptrc_glEdgeFlagv = _original_glEdgeFlagv;
        _ptrc_glEnable = _original_glEnable;
        _ptrc_glEnd = _original_glEnd;
        _ptrc_glEndList = _original_glEndList;
        _ptrc_glEvalCoord1d = _original_glEvalCoord1d;
        _ptrc_glEvalCoord1dv = _original_glEvalCoord1dv;
        _ptrc_glEvalCoord1f = _original_glEvalCoord1f;
        _ptrc_glEvalCoord1fv = _original_glEvalCoord1fv;
        _ptrc_glEvalCoord2d = _original_glEvalCoord2d;
        _ptrc_glEvalCoord2dv = _original_glEvalCoord2dv;
        _ptrc_glEvalCoord2f = _original_glEvalCoord2f;
        _ptrc_glEvalCoord2fv = _original_glEvalCoord2fv;
        _ptrc_glEvalMesh1 = _original_glEvalMesh1;
        _ptrc_glEvalMesh2 = _original_glEvalMesh2;
        _ptrc_glEvalPoint1 = _original_glEvalPoint1;
        _ptrc_glEvalPoint2 = _original_glEvalPoint2;
        _ptrc_glFeedbackBuffer = _original_glFeedbackBuffer;
        _ptrc_glFinish = _original_glFinish;
        _ptrc_glFlush = _original_glFlush;
        _ptrc_glFogf = _original_glFogf;
        _ptrc_glFogfv = _original_glFogfv;
        _ptrc_glFogi = _original_glFogi;
        _ptrc_glFogiv = _original_glFogiv;
        _ptrc_glFrontFace = _original_glFrontFace;
        _ptrc_glFrustum = _original_glFrustum;
        _ptrc_glGenLists = _original_glGenLists;
        _ptrc_glGetBooleanv = _original_glGetBooleanv;
        _ptrc_glGetClipPlane = _original_glGetClipPlane;
        _ptrc_glGetDoublev = _original_glGetDoublev;
        _ptrc_glGetError = _original_glGetError;
        _ptrc_glGetFloatv = _original_glGetFloatv;
        _ptrc_glGetIntegerv = _original_glGetIntegerv;
        _ptrc_glGetLightfv = _original_glGetLightfv;
        _ptrc_glGetLightiv = _original_glGetLightiv;
        _ptrc_glGetMapdv = _original_glGetMapdv;
        _ptrc_glGetMapfv = _original_glGetMapfv;
        _ptrc_glGetMapiv = _original_glGetMapiv;
        _ptrc_glGetMaterialfv = _original_glGetMaterialfv;
        _ptrc_glGetMaterialiv = _original_glGetMaterialiv;
        _ptrc_glGetPixelMapfv = _original_glGetPixelMapfv;
        _ptrc_glGetPixelMapuiv = _original_glGetPixelMapuiv;
        _ptrc_glGetPixelMapusv = _original_glGetPixelMapusv;
        _ptrc_glGetPolygonStipple = _original_glGetPolygonStipple;
        _ptrc_glGetString = _original_glGetString;
        _ptrc_glGetTexEnvfv = _original_glGetTexEnvfv;
        _ptrc_glGetTexEnviv = _original_glGetTexEnviv;
        _ptrc_glGetTexGendv = _original_glGetTexGendv;
        _ptrc_glGetTexGenfv = _original_glGetTexGenfv;
        _ptrc_glGetTexGeniv = _original_glGetTexGeniv;
        _ptrc_glGetTexImage = _original_glGetTexImage;
        _ptrc_glGetTexLevelParameterfv = _original_glGetTexLevelParameterfv;
        _ptrc_glGetTexLevelParameteriv = _original_glGetTexLevelParameteriv;
        _ptrc_glGetTexParameterfv = _original_glGetTexParameterfv;
        _ptrc_glGetTexParameteriv = _original_glGetTexParameteriv;
        _ptrc_glHint = _original_glHint;
        _ptrc_glIndexMask = _original_glIndexMask;
        _ptrc_glIndexd = _original_glIndexd;
        _ptrc_glIndexdv = _original_glIndexdv;
        _ptrc_glIndexf = _original_glIndexf;
        _ptrc_glIndexfv = _original_glIndexfv;
        _ptrc_glIndexi = _original_glIndexi;
        _ptrc_glIndexiv = _original_glIndexiv;
        _ptrc_glIndexs = _original_glIndexs;
        _ptrc_glIndexsv = _original_glIndexsv;
        _ptrc_glInitNames = _original_glInitNames;
        _ptrc_glIsEnabled = _original_glIsEnabled;
        _ptrc_glIsList = _original_glIsList;
        _ptrc_glLightModelf = _original_glLightModelf;
        _ptrc_glLightModelfv = _original_glLightModelfv;
        _ptrc_glLightModeli = _original_glLightModeli;
        _ptrc_glLightModeliv = _original_glLightModeliv;
        _ptrc_glLightf = _original_glLightf;
        _ptrc_glLightfv = _original_glLightfv;
        _ptrc_glLighti = _original_glLighti;
        _ptrc_glLightiv = _original_glLightiv;
        _ptrc_glLineStipple = _original_glLineStipple;
        _ptrc_glLineWidth = _original_glLineWidth;
        _ptrc_glListBase = _original_glListBase;
        _ptrc_glLoadIdentity = _original_glLoadIdentity;
        _ptrc_glLoadMatrixd = _original_glLoadMatrixd;
        _ptrc_glLoadMatrixf = _original_glLoadMatrixf;
        _ptrc_glLoadName = _original_glLoadName;
        _ptrc_glLogicOp = _original_glLogicOp;
        _ptrc_glMap1d = _original_glMap1d;
        _ptrc_glMap1f = _original_glMap1f;
        _ptrc_glMap2d = _original_glMap2d;
        _ptrc_glMap2f = _original_glMap2f;
        _ptrc_glMapGrid1d = _original_glMapGrid1d;
        _ptrc_glMapGrid1f = _original_glMapGrid1f;
        _ptrc_glMapGrid2d = _original_glMapGrid2d;
        _ptrc_glMapGrid2f = _original_glMapGrid2f;
        _ptrc_glMaterialf = _original_glMaterialf;
        _ptrc_glMaterialfv = _original_glMaterialfv;
        _ptrc_glMateriali = _original_glMateriali;
        _ptrc_glMaterialiv = _original_glMaterialiv;
        _ptrc_glMatrixMode = _original_glMatrixMode;
        _ptrc_glMultMatrixd = _original_glMultMatrixd;
        _ptrc_glMultMatrixf = _original_glMultMatrixf;
        _ptrc_glNewList = _original_glNewList;
        _ptrc_glNormal3b = _original_glNormal3b;
        _ptrc_glNormal3bv = _original_glNormal3bv;
        _ptrc_glNormal3d = _original_glNormal3d;
        _ptrc_glNormal3dv = _original_glNormal3dv;
        _ptrc_glNormal3f = _original_glNormal3f;
        _ptrc_glNormal3fv = _original_glNormal3fv;
        _ptrc_glNormal3i = _original_glNormal3i;
        _ptrc_glNormal3iv = _original_glNormal3iv;
        _ptrc_glNormal3s = _original_glNormal3s;
        _ptrc_glNormal3sv = _original_glNormal3sv;
        _ptrc_glOrtho = _original_glOrtho;
        _ptrc_glPassThrough = _original_glPassThrough;
        _ptrc_glPixelMapfv = _original_glPixelMapfv;
        _ptrc_glPixelMapuiv = _original_glPixelMapuiv;
        _ptrc_glPixelMapusv = _original_glPixelMapusv;
        _ptrc_glPixelStoref = _original_glPixelStoref;
        _ptrc_glPixelStorei = _original_glPixelStorei;
        _ptrc_glPixelTransferf = _original_glPixelTransferf;
        _ptrc_glPixelTransferi = _original_glPixelTransferi;
        _ptrc_glPixelZoom = _original_glPixelZoom;
        _ptrc_glPointSize = _original_glPointSize;
        _ptrc_glPolygonMode = _original_glPolygonMode;
        _ptrc_glPolygonStipple = _original_glPolygonStipple;
        _ptrc_glPopAttrib = _original_glPopAttrib;
        _ptrc_glPopMatrix = _original_glPopMatrix;
        _ptrc_glPopName = _original_glPopName;
        _ptrc_glPushAttrib = _original_glPushAttrib;
        _ptrc_glPushMatrix = _original_glPushMatrix;
        _ptrc_glPushName = _original_glPushName;
        _ptrc_glRasterPos2d = _original_glRasterPos2d;
        _ptrc_glRasterPos2dv = _original_glRasterPos2dv;
        _ptrc_glRasterPos2f = _original_glRasterPos2f;
        _ptrc_glRasterPos2fv = _original_glRasterPos2fv;
        _ptrc_glRasterPos2i = _original_glRasterPos2i;
        _ptrc_glRasterPos2iv = _original_glRasterPos2iv;
        _ptrc_glRasterPos2s = _original_glRasterPos2s;
        _ptrc_glRasterPos2sv = _original_glRasterPos2sv;
        _ptrc_glRasterPos3d = _original_glRasterPos3d;
        _ptrc_glRasterPos3dv = _original_glRasterPos3dv;
        _ptrc_glRasterPos3f = _original_glRasterPos3f;
        _ptrc_glRasterPos3fv = _original_glRasterPos3fv;
        _ptrc_glRasterPos3i = _original_glRasterPos3i;
        _ptrc_glRasterPos3iv = _original_glRasterPos3iv;
        _ptrc_glRasterPos3s = _original_glRasterPos3s;
        _ptrc_glRasterPos3sv = _original_glRasterPos3sv;
        _ptrc_glRasterPos4d = _original_glRasterPos4d;
        _ptrc_glRasterPos4dv = _original_glRasterPos4dv;
        _ptrc_glRasterPos4f = _original_glRasterPos4f;
        _ptrc_glRasterPos4fv = _original_glRasterPos4fv;
        _ptrc_glRasterPos4i = _original_glRasterPos4i;
        _ptrc_glRasterPos4iv = _original_glRasterPos4iv;
        _ptrc_glRasterPos4s = _original_glRasterPos4s;
        _ptrc_glRasterPos4sv = _original_glRasterPos4sv;
        _ptrc_glReadBuffer = _original_glReadBuffer;
        _ptrc_glReadPixels = _original_glReadPixels;
        _ptrc_glRectd = _original_glRectd;
        _ptrc_glRectdv = _original_glRectdv;
        _ptrc_glRectf = _original_glRectf;
        _ptrc_glRectfv = _original_glRectfv;
        _ptrc_glRecti = _original_glRecti;
        _ptrc_glRectiv = _original_glRectiv;
        _ptrc_glRects = _original_glRects;
        _ptrc_glRectsv = _original_glRectsv;
        _ptrc_glRenderMode = _original_glRenderMode;
        _ptrc_glRotated = _original_glRotated;
        _ptrc_glRotatef = _original_glRotatef;
        _ptrc_glScaled = _original_glScaled;
        _ptrc_glScalef = _original_glScalef;
        _ptrc_glScissor = _original_glScissor;
        _ptrc_glSelectBuffer = _original_glSelectBuffer;
        _ptrc_glShadeModel = _original_glShadeModel;
        _ptrc_glStencilFunc = _original_glStencilFunc;
        _ptrc_glStencilMask = _original_glStencilMask;
        _ptrc_glStencilOp = _original_glStencilOp;
        _ptrc_glTexCoord1d = _original_glTexCoord1d;
        _ptrc_glTexCoord1dv = _original_glTexCoord1dv;
        _ptrc_glTexCoord1f = _original_glTexCoord1f;
        _ptrc_glTexCoord1fv = _original_glTexCoord1fv;
        _ptrc_glTexCoord1i = _original_glTexCoord1i;
        _ptrc_glTexCoord1iv = _original_glTexCoord1iv;
        _ptrc_glTexCoord1s = _original_glTexCoord1s;
        _ptrc_glTexCoord1sv = _original_glTexCoord1sv;
        _ptrc_glTexCoord2d = _original_glTexCoord2d;
        _ptrc_glTexCoord2dv = _original_glTexCoord2dv;
        _ptrc_glTexCoord2f = _original_glTexCoord2f;
        _ptrc_glTexCoord2fv = _original_glTexCoord2fv;
        _ptrc_glTexCoord2i = _original_glTexCoord2i;
        _ptrc_glTexCoord2iv = _original_glTexCoord2iv;
        _ptrc_glTexCoord2s = _original_glTexCoord2s;
        _ptrc_glTexCoord2sv = _original_glTexCoord2sv;
        _ptrc_glTexCoord3d = _original_glTexCoord3d;
        _ptrc_glTexCoord3dv = _original_glTexCoord3dv;
        _ptrc_glTexCoord3f = _original_glTexCoord3f;
        _ptrc_glTexCoord3fv = _original_glTexCoord3fv;
        _ptrc_glTexCoord3i = _original_glTexCoord3i;
        _ptrc_glTexCoord3iv = _original_glTexCoord3iv;
        _ptrc_glTexCoord3s = _original_glTexCoord3s;
        _ptrc_glTexCoord3sv = _original_glTexCoord3sv;
        _ptrc_glTexCoord4d = _original_glTexCoord4d;
        _ptrc_glTexCoord4dv = _original_glTexCoord4dv;
        _ptrc_glTexCoord4f = _original_glTexCoord4f;
        _ptrc_glTexCoord4fv = _original_glTexCoord4fv;
        _ptrc_glTexCoord4i = _original_glTexCoord4i;
        _ptrc_glTexCoord4iv = _original_glTexCoord4iv;
        _ptrc_glTexCoord4s = _original_glTexCoord4s;
        _ptrc_glTexCoord4sv = _original_glTexCoord4sv;
        _ptrc_glTexEnvf = _original_glTexEnvf;
        _ptrc_glTexEnvfv = _original_glTexEnvfv;
        _ptrc_glTexEnvi = _original_glTexEnvi;
        _ptrc_glTexEnviv = _original_glTexEnviv;
        _ptrc_glTexGend = _original_glTexGend;
        _ptrc_glTexGendv = _original_glTexGendv;
        _ptrc_glTexGenf = _original_glTexGenf;
        _ptrc_glTexGenfv = _original_glTexGenfv;
        _ptrc_glTexGeni = _original_glTexGeni;
        _ptrc_glTexGeniv = _original_glTexGeniv;
        _ptrc_glTexImage1D = _original_glTexImage1D;
        _ptrc_glTexImage2D = _original_glTexImage2D;
        _ptrc_glTexParameterf = _original_glTexParameterf;
        _ptrc_glTexParameterfv = _original_glTexParameterfv;
        _ptrc_glTexParameteri = _original_glTexParameteri;
        _ptrc_glTexParameteriv = _original_glTexParameteriv;
        _ptrc_glTranslated = _original_glTranslated;
        _ptrc_glTranslatef = _original_glTranslatef;
        _ptrc_glVertex2d = _original_glVertex2d;
        _ptrc_glVertex2dv = _original_glVertex2dv;
        _ptrc_glVertex2f = _original_glVertex2f;
        _ptrc_glVertex2fv = _original_glVertex2fv;
        _ptrc_glVertex2i = _original_glVertex2i;
        _ptrc_glVertex2iv = _original_glVertex2iv;
        _ptrc_glVertex2s = _original_glVertex2s;
        _ptrc_glVertex2sv = _original_glVertex2sv;
        _ptrc_glVertex3d = _original_glVertex3d;
        _ptrc_glVertex3dv = _original_glVertex3dv;
        _ptrc_glVertex3f = _original_glVertex3f;
        _ptrc_glVertex3fv = _original_glVertex3fv;
        _ptrc_glVertex3i = _original_glVertex3i;
        _ptrc_glVertex3iv = _original_glVertex3iv;
        _ptrc_glVertex3s = _original_glVertex3s;
        _ptrc_glVertex3sv = _original_glVertex3sv;
        _ptrc_glVertex4d = _original_glVertex4d;
        _ptrc_glVertex4dv = _original_glVertex4dv;
        _ptrc_glVertex4f = _original_glVertex4f;
        _ptrc_glVertex4fv = _original_glVertex4fv;
        _ptrc_glVertex4i = _original_glVertex4i;
        _ptrc_glVertex4iv = _original_glVertex4iv;
        _ptrc_glVertex4s = _original_glVertex4s;
        _ptrc_glVertex4sv = _original_glVertex4sv;
        _ptrc_glViewport = _original_glViewport;

        _ptrc_glAreTexturesResident = _original_glAreTexturesResident;
        _ptrc_glArrayElement = _original_glArrayElement;
        _ptrc_glBindTexture = _original_glBindTexture;
        _ptrc_glColorPointer = _original_glColorPointer;
        _ptrc_glCopyTexImage1D = _original_glCopyTexImage1D;
        _ptrc_glCopyTexImage2D = _original_glCopyTexImage2D;
        _ptrc_glCopyTexSubImage1D = _original_glCopyTexSubImage1D;
        _ptrc_glCopyTexSubImage2D = _original_glCopyTexSubImage2D;
        _ptrc_glDeleteTextures = _original_glDeleteTextures;
        _ptrc_glDisableClientState = _original_glDisableClientState;
        _ptrc_glDrawArrays = _original_glDrawArrays;
        _ptrc_glDrawElements = _original_glDrawElements;
        _ptrc_glEdgeFlagPointer = _original_glEdgeFlagPointer;
        _ptrc_glEnableClientState = _original_glEnableClientState;
        _ptrc_glGenTextures = _original_glGenTextures;
        _ptrc_glIndexPointer = _original_glIndexPointer;
        _ptrc_glIndexub = _original_glIndexub;
        _ptrc_glIndexubv = _original_glIndexubv;
        _ptrc_glInterleavedArrays = _original_glInterleavedArrays;
        _ptrc_glIsTexture = _original_glIsTexture;
        _ptrc_glNormalPointer = _original_glNormalPointer;
        _ptrc_glPolygonOffset = _original_glPolygonOffset;
        _ptrc_glPopClientAttrib = _original_glPopClientAttrib;
        _ptrc_glPrioritizeTextures = _original_glPrioritizeTextures;
        _ptrc_glPushClientAttrib = _original_glPushClientAttrib;
        _ptrc_glTexCoordPointer = _original_glTexCoordPointer;
        _ptrc_glTexSubImage1D = _original_glTexSubImage1D;
        _ptrc_glTexSubImage2D = _original_glTexSubImage2D;
        _ptrc_glVertexPointer = _original_glVertexPointer;

        _ptrc_glBlendColor = _original_glBlendColor;
        _ptrc_glBlendEquation = _original_glBlendEquation;
        _ptrc_glCopyTexSubImage3D = _original_glCopyTexSubImage3D;
        _ptrc_glDrawRangeElements = _original_glDrawRangeElements;
        _ptrc_glTexImage3D = _original_glTexImage3D;
        _ptrc_glTexSubImage3D = _original_glTexSubImage3D;

        _ptrc_glActiveTexture = _original_glActiveTexture;
        _ptrc_glClientActiveTexture = _original_glClientActiveTexture;
        _ptrc_glCompressedTexImage1D = _original_glCompressedTexImage1D;
        _ptrc_glCompressedTexImage2D = _original_glCompressedTexImage2D;
        _ptrc_glCompressedTexImage3D = _original_glCompressedTexImage3D;
        _ptrc_glCompressedTexSubImage1D = _original_glCompressedTexSubImage1D;
        _ptrc_glCompressedTexSubImage2D = _original_glCompressedTexSubImage2D;
        _ptrc_glCompressedTexSubImage3D = _original_glCompressedTexSubImage3D;
        _ptrc_glGetCompressedTexImage = _original_glGetCompressedTexImage;
        _ptrc_glLoadTransposeMatrixd = _original_glLoadTransposeMatrixd;
        _ptrc_glLoadTransposeMatrixf = _original_glLoadTransposeMatrixf;
        _ptrc_glMultTransposeMatrixd = _original_glMultTransposeMatrixd;
        _ptrc_glMultTransposeMatrixf = _original_glMultTransposeMatrixf;
        _ptrc_glMultiTexCoord1d = _original_glMultiTexCoord1d;
        _ptrc_glMultiTexCoord1dv = _original_glMultiTexCoord1dv;
        _ptrc_glMultiTexCoord1f = _original_glMultiTexCoord1f;
        _ptrc_glMultiTexCoord1fv = _original_glMultiTexCoord1fv;
        _ptrc_glMultiTexCoord1i = _original_glMultiTexCoord1i;
        _ptrc_glMultiTexCoord1iv = _original_glMultiTexCoord1iv;
        _ptrc_glMultiTexCoord1s = _original_glMultiTexCoord1s;
        _ptrc_glMultiTexCoord1sv = _original_glMultiTexCoord1sv;
        _ptrc_glMultiTexCoord2d = _original_glMultiTexCoord2d;
        _ptrc_glMultiTexCoord2dv = _original_glMultiTexCoord2dv;
        _ptrc_glMultiTexCoord2f = _original_glMultiTexCoord2f;
        _ptrc_glMultiTexCoord2fv = _original_glMultiTexCoord2fv;
        _ptrc_glMultiTexCoord2i = _original_glMultiTexCoord2i;
        _ptrc_glMultiTexCoord2iv = _original_glMultiTexCoord2iv;
        _ptrc_glMultiTexCoord2s = _original_glMultiTexCoord2s;
        _ptrc_glMultiTexCoord2sv = _original_glMultiTexCoord2sv;
        _ptrc_glMultiTexCoord3d = _original_glMultiTexCoord3d;
        _ptrc_glMultiTexCoord3dv = _original_glMultiTexCoord3dv;
        _ptrc_glMultiTexCoord3f = _original_glMultiTexCoord3f;
        _ptrc_glMultiTexCoord3fv = _original_glMultiTexCoord3fv;
        _ptrc_glMultiTexCoord3i = _original_glMultiTexCoord3i;
        _ptrc_glMultiTexCoord3iv = _original_glMultiTexCoord3iv;
        _ptrc_glMultiTexCoord3s = _original_glMultiTexCoord3s;
        _ptrc_glMultiTexCoord3sv = _original_glMultiTexCoord3sv;
        _ptrc_glMultiTexCoord4d = _original_glMultiTexCoord4d;
        _ptrc_glMultiTexCoord4dv = _original_glMultiTexCoord4dv;
        _ptrc_glMultiTexCoord4f = _original_glMultiTexCoord4f;
        _ptrc_glMultiTexCoord4fv = _original_glMultiTexCoord4fv;
        _ptrc_glMultiTexCoord4i = _original_glMultiTexCoord4i;
        _ptrc_glMultiTexCoord4iv = _original_glMultiTexCoord4iv;
        _ptrc_glMultiTexCoord4s = _original_glMultiTexCoord4s;
        _ptrc_glMultiTexCoord4sv = _original_glMultiTexCoord4sv;
        _ptrc_glSampleCoverage = _original_glSampleCoverage;

        _ptrc_glBlendFuncSeparate = _original_glBlendFuncSeparate;
        _ptrc_glFogCoordPointer = _original_glFogCoordPointer;
        _ptrc_glFogCoordd = _original_glFogCoordd;
        _ptrc_glFogCoorddv = _original_glFogCoorddv;
        _ptrc_glFogCoordf = _original_glFogCoordf;
        _ptrc_glFogCoordfv = _original_glFogCoordfv;
        _ptrc_glMultiDrawArrays = _original_glMultiDrawArrays;
        _ptrc_glMultiDrawElements = _original_glMultiDrawElements;
        _ptrc_glPointParameterf = _original_glPointParameterf;
        _ptrc_glPointParameterfv = _original_glPointParameterfv;
        _ptrc_glPointParameteri = _original_glPointParameteri;
        _ptrc_glPointParameteriv = _original_glPointParameteriv;
        _ptrc_glSecondaryColor3b = _original_glSecondaryColor3b;
        _ptrc_glSecondaryColor3bv = _original_glSecondaryColor3bv;
        _ptrc_glSecondaryColor3d = _original_glSecondaryColor3d;
        _ptrc_glSecondaryColor3dv = _original_glSecondaryColor3dv;
        _ptrc_glSecondaryColor3f = _original_glSecondaryColor3f;
        _ptrc_glSecondaryColor3fv = _original_glSecondaryColor3fv;
        _ptrc_glSecondaryColor3i = _original_glSecondaryColor3i;
        _ptrc_glSecondaryColor3iv = _original_glSecondaryColor3iv;
        _ptrc_glSecondaryColor3s = _original_glSecondaryColor3s;
        _ptrc_glSecondaryColor3sv = _original_glSecondaryColor3sv;
        _ptrc_glSecondaryColor3ub = _original_glSecondaryColor3ub;
        _ptrc_glSecondaryColor3ubv = _original_glSecondaryColor3ubv;
        _ptrc_glSecondaryColor3ui = _original_glSecondaryColor3ui;
        _ptrc_glSecondaryColor3uiv = _original_glSecondaryColor3uiv;
        _ptrc_glSecondaryColor3us = _original_glSecondaryColor3us;
        _ptrc_glSecondaryColor3usv = _original_glSecondaryColor3usv;
        _ptrc_glSecondaryColorPointer = _original_glSecondaryColorPointer;
        _ptrc_glWindowPos2d = _original_glWindowPos2d;
        _ptrc_glWindowPos2dv = _original_glWindowPos2dv;
        _ptrc_glWindowPos2f = _original_glWindowPos2f;
        _ptrc_glWindowPos2fv = _original_glWindowPos2fv;
        _ptrc_glWindowPos2i = _original_glWindowPos2i;
        _ptrc_glWindowPos2iv = _original_glWindowPos2iv;
        _ptrc_glWindowPos2s = _original_glWindowPos2s;
        _ptrc_glWindowPos2sv = _original_glWindowPos2sv;
        _ptrc_glWindowPos3d = _original_glWindowPos3d;
        _ptrc_glWindowPos3dv = _original_glWindowPos3dv;
        _ptrc_glWindowPos3f = _original_glWindowPos3f;
        _ptrc_glWindowPos3fv = _original_glWindowPos3fv;
        _ptrc_glWindowPos3i = _original_glWindowPos3i;
        _ptrc_glWindowPos3iv = _original_glWindowPos3iv;
        _ptrc_glWindowPos3s = _original_glWindowPos3s;
        _ptrc_glWindowPos3sv = _original_glWindowPos3sv;

        _ptrc_glBeginQuery = _original_glBeginQuery;
        _ptrc_glBindBuffer = _original_glBindBuffer;
        _ptrc_glBufferData = _original_glBufferData;
        _ptrc_glBufferSubData = _original_glBufferSubData;
        _ptrc_glDeleteBuffers = _original_glDeleteBuffers;
        _ptrc_glDeleteQueries = _original_glDeleteQueries;
        _ptrc_glEndQuery = _original_glEndQuery;
        _ptrc_glGenBuffers = _original_glGenBuffers;
        _ptrc_glGenQueries = _original_glGenQueries;
        _ptrc_glGetBufferParameteriv = _original_glGetBufferParameteriv;
        _ptrc_glGetBufferPointerv = _original_glGetBufferPointerv;
        _ptrc_glGetBufferSubData = _original_glGetBufferSubData;
        _ptrc_glGetQueryObjectiv = _original_glGetQueryObjectiv;
        _ptrc_glGetQueryObjectuiv = _original_glGetQueryObjectuiv;
        _ptrc_glGetQueryiv = _original_glGetQueryiv;
        _ptrc_glIsBuffer = _original_glIsBuffer;
        _ptrc_glIsQuery = _original_glIsQuery;
        _ptrc_glMapBuffer = _original_glMapBuffer;
        _ptrc_glUnmapBuffer = _original_glUnmapBuffer;

        _ptrc_glAttachShader = _original_glAttachShader;
        _ptrc_glBindAttribLocation = _original_glBindAttribLocation;
        _ptrc_glBlendEquationSeparate = _original_glBlendEquationSeparate;
        _ptrc_glCompileShader = _original_glCompileShader;
        _ptrc_glCreateProgram = _original_glCreateProgram;
        _ptrc_glCreateShader = _original_glCreateShader;
        _ptrc_glDeleteProgram = _original_glDeleteProgram;
        _ptrc_glDeleteShader = _original_glDeleteShader;
        _ptrc_glDetachShader = _original_glDetachShader;
        _ptrc_glDisableVertexAttribArray = _original_glDisableVertexAttribArray;
        _ptrc_glDrawBuffers = _original_glDrawBuffers;
        _ptrc_glEnableVertexAttribArray = _original_glEnableVertexAttribArray;
        _ptrc_glGetActiveAttrib = _original_glGetActiveAttrib;
        _ptrc_glGetActiveUniform = _original_glGetActiveUniform;
        _ptrc_glGetAttachedShaders = _original_glGetAttachedShaders;
        _ptrc_glGetAttribLocation = _original_glGetAttribLocation;
        _ptrc_glGetProgramInfoLog = _original_glGetProgramInfoLog;
        _ptrc_glGetProgramiv = _original_glGetProgramiv;
        _ptrc_glGetShaderInfoLog = _original_glGetShaderInfoLog;
        _ptrc_glGetShaderSource = _original_glGetShaderSource;
        _ptrc_glGetShaderiv = _original_glGetShaderiv;
        _ptrc_glGetUniformLocation = _original_glGetUniformLocation;
        _ptrc_glGetUniformfv = _original_glGetUniformfv;
        _ptrc_glGetUniformiv = _original_glGetUniformiv;
        _ptrc_glGetVertexAttribPointerv = _original_glGetVertexAttribPointerv;
        _ptrc_glGetVertexAttribdv = _original_glGetVertexAttribdv;
        _ptrc_glGetVertexAttribfv = _original_glGetVertexAttribfv;
        _ptrc_glGetVertexAttribiv = _original_glGetVertexAttribiv;
        _ptrc_glIsProgram = _original_glIsProgram;
        _ptrc_glIsShader = _original_glIsShader;
        _ptrc_glLinkProgram = _original_glLinkProgram;
        _ptrc_glShaderSource = _original_glShaderSource;
        _ptrc_glStencilFuncSeparate = _original_glStencilFuncSeparate;
        _ptrc_glStencilMaskSeparate = _original_glStencilMaskSeparate;
        _ptrc_glStencilOpSeparate = _original_glStencilOpSeparate;
        _ptrc_glUniform1f = _original_glUniform1f;
        _ptrc_glUniform1fv = _original_glUniform1fv;
        _ptrc_glUniform1i = _original_glUniform1i;
        _ptrc_glUniform1iv = _original_glUniform1iv;
        _ptrc_glUniform2f = _original_glUniform2f;
        _ptrc_glUniform2fv = _original_glUniform2fv;
        _ptrc_glUniform2i = _original_glUniform2i;
        _ptrc_glUniform2iv = _original_glUniform2iv;
        _ptrc_glUniform3f = _original_glUniform3f;
        _ptrc_glUniform3fv = _original_glUniform3fv;
        _ptrc_glUniform3i = _original_glUniform3i;
        _ptrc_glUniform3iv = _original_glUniform3iv;
        _ptrc_glUniform4f = _original_glUniform4f;
        _ptrc_glUniform4fv = _original_glUniform4fv;
        _ptrc_glUniform4i = _original_glUniform4i;
        _ptrc_glUniform4iv = _original_glUniform4iv;
        _ptrc_glUniformMatrix2fv = _original_glUniformMatrix2fv;
        _ptrc_glUniformMatrix3fv = _original_glUniformMatrix3fv;
        _ptrc_glUniformMatrix4fv = _original_glUniformMatrix4fv;
        _ptrc_glUseProgram = _original_glUseProgram;
        _ptrc_glValidateProgram = _original_glValidateProgram;
        _ptrc_glVertexAttrib1d = _original_glVertexAttrib1d;
        _ptrc_glVertexAttrib1dv = _original_glVertexAttrib1dv;
        _ptrc_glVertexAttrib1f = _original_glVertexAttrib1f;
        _ptrc_glVertexAttrib1fv = _original_glVertexAttrib1fv;
        _ptrc_glVertexAttrib1s = _original_glVertexAttrib1s;
        _ptrc_glVertexAttrib1sv = _original_glVertexAttrib1sv;
        _ptrc_glVertexAttrib2d = _original_glVertexAttrib2d;
        _ptrc_glVertexAttrib2dv = _original_glVertexAttrib2dv;
        _ptrc_glVertexAttrib2f = _original_glVertexAttrib2f;
        _ptrc_glVertexAttrib2fv = _original_glVertexAttrib2fv;
        _ptrc_glVertexAttrib2s = _original_glVertexAttrib2s;
        _ptrc_glVertexAttrib2sv = _original_glVertexAttrib2sv;
        _ptrc_glVertexAttrib3d = _original_glVertexAttrib3d;
        _ptrc_glVertexAttrib3dv = _original_glVertexAttrib3dv;
        _ptrc_glVertexAttrib3f = _original_glVertexAttrib3f;
        _ptrc_glVertexAttrib3fv = _original_glVertexAttrib3fv;
        _ptrc_glVertexAttrib3s = _original_glVertexAttrib3s;
        _ptrc_glVertexAttrib3sv = _original_glVertexAttrib3sv;
        _ptrc_glVertexAttrib4Nbv = _original_glVertexAttrib4Nbv;
        _ptrc_glVertexAttrib4Niv = _original_glVertexAttrib4Niv;
        _ptrc_glVertexAttrib4Nsv = _original_glVertexAttrib4Nsv;
        _ptrc_glVertexAttrib4Nub = _original_glVertexAttrib4Nub;
        _ptrc_glVertexAttrib4Nubv = _original_glVertexAttrib4Nubv;
        _ptrc_glVertexAttrib4Nuiv = _original_glVertexAttrib4Nuiv;
        _ptrc_glVertexAttrib4Nusv = _original_glVertexAttrib4Nusv;
        _ptrc_glVertexAttrib4bv = _original_glVertexAttrib4bv;
        _ptrc_glVertexAttrib4d = _original_glVertexAttrib4d;
        _ptrc_glVertexAttrib4dv = _original_glVertexAttrib4dv;
        _ptrc_glVertexAttrib4f = _original_glVertexAttrib4f;
        _ptrc_glVertexAttrib4fv = _original_glVertexAttrib4fv;
        _ptrc_glVertexAttrib4iv = _original_glVertexAttrib4iv;
        _ptrc_glVertexAttrib4s = _original_glVertexAttrib4s;
        _ptrc_glVertexAttrib4sv = _original_glVertexAttrib4sv;
        _ptrc_glVertexAttrib4ubv = _original_glVertexAttrib4ubv;
        _ptrc_glVertexAttrib4uiv = _original_glVertexAttrib4uiv;
        _ptrc_glVertexAttrib4usv = _original_glVertexAttrib4usv;
        _ptrc_glVertexAttribPointer = _original_glVertexAttribPointer;

        _ptrc_glUniformMatrix2x3fv = _original_glUniformMatrix2x3fv;
        _ptrc_glUniformMatrix2x4fv = _original_glUniformMatrix2x4fv;
        _ptrc_glUniformMatrix3x2fv = _original_glUniformMatrix3x2fv;
        _ptrc_glUniformMatrix3x4fv = _original_glUniformMatrix3x4fv;
        _ptrc_glUniformMatrix4x2fv = _original_glUniformMatrix4x2fv;
        _ptrc_glUniformMatrix4x3fv = _original_glUniformMatrix4x3fv;

        _ptrc_glBeginConditionalRender = _original_glBeginConditionalRender;
        _ptrc_glBeginTransformFeedback = _original_glBeginTransformFeedback;
        _ptrc_glBindBufferBase = _original_glBindBufferBase;
        _ptrc_glBindBufferRange = _original_glBindBufferRange;
        _ptrc_glBindFragDataLocation = _original_glBindFragDataLocation;
        _ptrc_glBindFramebuffer = _original_glBindFramebuffer;
        _ptrc_glBindRenderbuffer = _original_glBindRenderbuffer;
        _ptrc_glBindVertexArray = _original_glBindVertexArray;
        _ptrc_glBlitFramebuffer = _original_glBlitFramebuffer;
        _ptrc_glCheckFramebufferStatus = _original_glCheckFramebufferStatus;
        _ptrc_glClampColor = _original_glClampColor;
        _ptrc_glClearBufferfi = _original_glClearBufferfi;
        _ptrc_glClearBufferfv = _original_glClearBufferfv;
        _ptrc_glClearBufferiv = _original_glClearBufferiv;
        _ptrc_glClearBufferuiv = _original_glClearBufferuiv;
        _ptrc_glColorMaski = _original_glColorMaski;
        _ptrc_glDeleteFramebuffers = _original_glDeleteFramebuffers;
        _ptrc_glDeleteRenderbuffers = _original_glDeleteRenderbuffers;
        _ptrc_glDeleteVertexArrays = _original_glDeleteVertexArrays;
        _ptrc_glDisablei = _original_glDisablei;
        _ptrc_glEnablei = _original_glEnablei;
        _ptrc_glEndConditionalRender = _original_glEndConditionalRender;
        _ptrc_glEndTransformFeedback = _original_glEndTransformFeedback;
        _ptrc_glFlushMappedBufferRange = _original_glFlushMappedBufferRange;
        _ptrc_glFramebufferRenderbuffer = _original_glFramebufferRenderbuffer;
        _ptrc_glFramebufferTexture1D = _original_glFramebufferTexture1D;
        _ptrc_glFramebufferTexture2D = _original_glFramebufferTexture2D;
        _ptrc_glFramebufferTexture3D = _original_glFramebufferTexture3D;
        _ptrc_glFramebufferTextureLayer = _original_glFramebufferTextureLayer;
        _ptrc_glGenFramebuffers = _original_glGenFramebuffers;
        _ptrc_glGenRenderbuffers = _original_glGenRenderbuffers;
        _ptrc_glGenVertexArrays = _original_glGenVertexArrays;
        _ptrc_glGenerateMipmap = _original_glGenerateMipmap;
        _ptrc_glGetBooleani_v = _original_glGetBooleani_v;
        _ptrc_glGetFragDataLocation = _original_glGetFragDataLocation;
        _ptrc_glGetFramebufferAttachmentParameteriv = _original_glGetFramebufferAttachmentParameteriv;
        _ptrc_glGetIntegeri_v = _original_glGetIntegeri_v;
        _ptrc_glGetRenderbufferParameteriv = _original_glGetRenderbufferParameteriv;
        _ptrc_glGetStringi = _original_glGetStringi;
        _ptrc_glGetTexParameterIiv = _original_glGetTexParameterIiv;
        _ptrc_glGetTexParameterIuiv = _original_glGetTexParameterIuiv;
        _ptrc_glGetTransformFeedbackVarying = _original_glGetTransformFeedbackVarying;
        _ptrc_glGetUniformuiv = _original_glGetUniformuiv;
        _ptrc_glGetVertexAttribIiv = _original_glGetVertexAttribIiv;
        _ptrc_glGetVertexAttribIuiv = _original_glGetVertexAttribIuiv;
        _ptrc_glIsEnabledi = _original_glIsEnabledi;
        _ptrc_glIsFramebuffer = _original_glIsFramebuffer;
        _ptrc_glIsRenderbuffer = _original_glIsRenderbuffer;
        _ptrc_glIsVertexArray = _original_glIsVertexArray;
        _ptrc_glMapBufferRange = _original_glMapBufferRange;
        _ptrc_glRenderbufferStorage = _original_glRenderbufferStorage;
        _ptrc_glRenderbufferStorageMultisample = _original_glRenderbufferStorageMultisample;
        _ptrc_glTexParameterIiv = _original_glTexParameterIiv;
        _ptrc_glTexParameterIuiv = _original_glTexParameterIuiv;
        _ptrc_glTransformFeedbackVaryings = _original_glTransformFeedbackVaryings;
        _ptrc_glUniform1ui = _original_glUniform1ui;
        _ptrc_glUniform1uiv = _original_glUniform1uiv;
        _ptrc_glUniform2ui = _original_glUniform2ui;
        _ptrc_glUniform2uiv = _original_glUniform2uiv;
        _ptrc_glUniform3ui = _original_glUniform3ui;
        _ptrc_glUniform3uiv = _original_glUniform3uiv;
        _ptrc_glUniform4ui = _original_glUniform4ui;
        _ptrc_glUniform4uiv = _original_glUniform4uiv;
        _ptrc_glVertexAttribI1i = _original_glVertexAttribI1i;
        _ptrc_glVertexAttribI1iv = _original_glVertexAttribI1iv;
        _ptrc_glVertexAttribI1ui = _original_glVertexAttribI1ui;
        _ptrc_glVertexAttribI1uiv = _original_glVertexAttribI1uiv;
        _ptrc_glVertexAttribI2i = _original_glVertexAttribI2i;
        _ptrc_glVertexAttribI2iv = _original_glVertexAttribI2iv;
        _ptrc_glVertexAttribI2ui = _original_glVertexAttribI2ui;
        _ptrc_glVertexAttribI2uiv = _original_glVertexAttribI2uiv;
        _ptrc_glVertexAttribI3i = _original_glVertexAttribI3i;
        _ptrc_glVertexAttribI3iv = _original_glVertexAttribI3iv;
        _ptrc_glVertexAttribI3ui = _original_glVertexAttribI3ui;
        _ptrc_glVertexAttribI3uiv = _original_glVertexAttribI3uiv;
        _ptrc_glVertexAttribI4bv = _original_glVertexAttribI4bv;
        _ptrc_glVertexAttribI4i = _original_glVertexAttribI4i;
        _ptrc_glVertexAttribI4iv = _original_glVertexAttribI4iv;
        _ptrc_glVertexAttribI4sv = _original_glVertexAttribI4sv;
        _ptrc_glVertexAttribI4ubv = _original_glVertexAttribI4ubv;
        _ptrc_glVertexAttribI4ui = _original_glVertexAttribI4ui;
        _ptrc_glVertexAttribI4uiv = _original_glVertexAttribI4uiv;
        _ptrc_glVertexAttribI4usv = _original_glVertexAttribI4usv;
        _ptrc_glVertexAttribIPointer = _original_glVertexAttribIPointer;

}

void simulateKHR_debug() {
	ogl_ext_KHR_debug = ogl_LOAD_SUCCEEDED;
	
	_ptrc_glDebugMessageCallback = KHR_DEBUG_EMULATOR_DebugMessageCallback;
	_ptrc_glDebugMessageControl = KHR_DEBUG_EMULATOR_DebugMessageControl;
	_ptrc_glDebugMessageInsert = KHR_DEBUG_EMULATOR_DebugMessageInsert;
	_ptrc_glGetDebugMessageLog = KHR_DEBUG_EMULATOR_GetDebugMessageLog;
	_ptrc_glGetObjectLabel = KHR_DEBUG_EMULATOR_GetObjectLabel;
	_ptrc_glGetObjectPtrLabel = KHR_DEBUG_EMULATOR_GetObjectPtrLabel;
	_ptrc_glGetPointerv = KHR_DEBUG_EMULATOR_GetPointerv;
	_ptrc_glObjectLabel = KHR_DEBUG_EMULATOR_ObjectLabel;
	_ptrc_glObjectPtrLabel = KHR_DEBUG_EMULATOR_ObjectPtrLabel;
	_ptrc_glPopDebugGroup = KHR_DEBUG_EMULATOR_PopDebugGroup;
	_ptrc_glPushDebugGroup = KHR_DEBUG_EMULATOR_PushDebugGroup;
	
	_ptrc_glIsEnabled = KHR_DEBUG_EMULATOR_IsEnabled;
	_ptrc_glGetError = KHR_DEBUG_EMULATOR_GetError;
	_ptrc_glGetIntegerv = KHR_DEBUG_EMULATOR_GetIntegerv;
	_ptrc_glDisable = KHR_DEBUG_EMULATOR_Disable;
	_ptrc_glEnable = KHR_DEBUG_EMULATOR_Enable;
}
	
void simulateDebugContext() {
    simulateKHR_debug();
        _ptrc_glProgramParameteriEXT = wrapperglProgramParameteriEXT;

        _ptrc_glFramebufferTextureARB = wrapperglFramebufferTextureARB;
        _ptrc_glFramebufferTextureFaceARB = wrapperglFramebufferTextureFaceARB;
        _ptrc_glFramebufferTextureLayerARB = wrapperglFramebufferTextureLayerARB;
        _ptrc_glProgramParameteriARB = wrapperglProgramParameteriARB;

        _ptrc_glPatchParameterfv = wrapperglPatchParameterfv;
        _ptrc_glPatchParameteri = wrapperglPatchParameteri;

        _ptrc_glDispatchCompute = wrapperglDispatchCompute;
        _ptrc_glDispatchComputeIndirect = wrapperglDispatchComputeIndirect;

        _ptrc_glDebugMessageCallbackARB = wrapperglDebugMessageCallbackARB;
        _ptrc_glDebugMessageControlARB = wrapperglDebugMessageControlARB;
        _ptrc_glDebugMessageInsertARB = wrapperglDebugMessageInsertARB;
        _ptrc_glGetDebugMessageLogARB = wrapperglGetDebugMessageLogARB;


        _ptrc_glClearDepthf = wrapperglClearDepthf;
        _ptrc_glDepthRangef = wrapperglDepthRangef;
        _ptrc_glGetShaderPrecisionFormat = wrapperglGetShaderPrecisionFormat;
        _ptrc_glReleaseShaderCompiler = wrapperglReleaseShaderCompiler;
        _ptrc_glShaderBinary = wrapperglShaderBinary;

        _ptrc_glGetProgramBinary = wrapperglGetProgramBinary;
        _ptrc_glProgramBinary = wrapperglProgramBinary;
        _ptrc_glProgramParameteri = wrapperglProgramParameteri;

        _ptrc_glGetInternalformativ = wrapperglGetInternalformativ;

        _ptrc_glGetInternalformati64v = wrapperglGetInternalformati64v;

        _ptrc_glGetProgramInterfaceiv = wrapperglGetProgramInterfaceiv;
        _ptrc_glGetProgramResourceIndex = wrapperglGetProgramResourceIndex;
        _ptrc_glGetProgramResourceLocation = wrapperglGetProgramResourceLocation;
        _ptrc_glGetProgramResourceLocationIndex = wrapperglGetProgramResourceLocationIndex;
        _ptrc_glGetProgramResourceName = wrapperglGetProgramResourceName;
        _ptrc_glGetProgramResourceiv = wrapperglGetProgramResourceiv;

        _ptrc_glActiveShaderProgram = wrapperglActiveShaderProgram;
        _ptrc_glBindProgramPipeline = wrapperglBindProgramPipeline;
        _ptrc_glCreateShaderProgramv = wrapperglCreateShaderProgramv;
        _ptrc_glDeleteProgramPipelines = wrapperglDeleteProgramPipelines;
        _ptrc_glGenProgramPipelines = wrapperglGenProgramPipelines;
        _ptrc_glGetProgramPipelineInfoLog = wrapperglGetProgramPipelineInfoLog;
        _ptrc_glGetProgramPipelineiv = wrapperglGetProgramPipelineiv;
        _ptrc_glIsProgramPipeline = wrapperglIsProgramPipeline;
        _ptrc_glProgramUniform1d = wrapperglProgramUniform1d;
        _ptrc_glProgramUniform1dv = wrapperglProgramUniform1dv;
        _ptrc_glProgramUniform1f = wrapperglProgramUniform1f;
        _ptrc_glProgramUniform1fv = wrapperglProgramUniform1fv;
        _ptrc_glProgramUniform1i = wrapperglProgramUniform1i;
        _ptrc_glProgramUniform1iv = wrapperglProgramUniform1iv;
        _ptrc_glProgramUniform1ui = wrapperglProgramUniform1ui;
        _ptrc_glProgramUniform1uiv = wrapperglProgramUniform1uiv;
        _ptrc_glProgramUniform2d = wrapperglProgramUniform2d;
        _ptrc_glProgramUniform2dv = wrapperglProgramUniform2dv;
        _ptrc_glProgramUniform2f = wrapperglProgramUniform2f;
        _ptrc_glProgramUniform2fv = wrapperglProgramUniform2fv;
        _ptrc_glProgramUniform2i = wrapperglProgramUniform2i;
        _ptrc_glProgramUniform2iv = wrapperglProgramUniform2iv;
        _ptrc_glProgramUniform2ui = wrapperglProgramUniform2ui;
        _ptrc_glProgramUniform2uiv = wrapperglProgramUniform2uiv;
        _ptrc_glProgramUniform3d = wrapperglProgramUniform3d;
        _ptrc_glProgramUniform3dv = wrapperglProgramUniform3dv;
        _ptrc_glProgramUniform3f = wrapperglProgramUniform3f;
        _ptrc_glProgramUniform3fv = wrapperglProgramUniform3fv;
        _ptrc_glProgramUniform3i = wrapperglProgramUniform3i;
        _ptrc_glProgramUniform3iv = wrapperglProgramUniform3iv;
        _ptrc_glProgramUniform3ui = wrapperglProgramUniform3ui;
        _ptrc_glProgramUniform3uiv = wrapperglProgramUniform3uiv;
        _ptrc_glProgramUniform4d = wrapperglProgramUniform4d;
        _ptrc_glProgramUniform4dv = wrapperglProgramUniform4dv;
        _ptrc_glProgramUniform4f = wrapperglProgramUniform4f;
        _ptrc_glProgramUniform4fv = wrapperglProgramUniform4fv;
        _ptrc_glProgramUniform4i = wrapperglProgramUniform4i;
        _ptrc_glProgramUniform4iv = wrapperglProgramUniform4iv;
        _ptrc_glProgramUniform4ui = wrapperglProgramUniform4ui;
        _ptrc_glProgramUniform4uiv = wrapperglProgramUniform4uiv;
        _ptrc_glProgramUniformMatrix2dv = wrapperglProgramUniformMatrix2dv;
        _ptrc_glProgramUniformMatrix2fv = wrapperglProgramUniformMatrix2fv;
        _ptrc_glProgramUniformMatrix2x3dv = wrapperglProgramUniformMatrix2x3dv;
        _ptrc_glProgramUniformMatrix2x3fv = wrapperglProgramUniformMatrix2x3fv;
        _ptrc_glProgramUniformMatrix2x4dv = wrapperglProgramUniformMatrix2x4dv;
        _ptrc_glProgramUniformMatrix2x4fv = wrapperglProgramUniformMatrix2x4fv;
        _ptrc_glProgramUniformMatrix3dv = wrapperglProgramUniformMatrix3dv;
        _ptrc_glProgramUniformMatrix3fv = wrapperglProgramUniformMatrix3fv;
        _ptrc_glProgramUniformMatrix3x2dv = wrapperglProgramUniformMatrix3x2dv;
        _ptrc_glProgramUniformMatrix3x2fv = wrapperglProgramUniformMatrix3x2fv;
        _ptrc_glProgramUniformMatrix3x4dv = wrapperglProgramUniformMatrix3x4dv;
        _ptrc_glProgramUniformMatrix3x4fv = wrapperglProgramUniformMatrix3x4fv;
        _ptrc_glProgramUniformMatrix4dv = wrapperglProgramUniformMatrix4dv;
        _ptrc_glProgramUniformMatrix4fv = wrapperglProgramUniformMatrix4fv;
        _ptrc_glProgramUniformMatrix4x2dv = wrapperglProgramUniformMatrix4x2dv;
        _ptrc_glProgramUniformMatrix4x2fv = wrapperglProgramUniformMatrix4x2fv;
        _ptrc_glProgramUniformMatrix4x3dv = wrapperglProgramUniformMatrix4x3dv;
        _ptrc_glProgramUniformMatrix4x3fv = wrapperglProgramUniformMatrix4x3fv;
        _ptrc_glUseProgramStages = wrapperglUseProgramStages;
        _ptrc_glValidateProgramPipeline = wrapperglValidateProgramPipeline;

        _ptrc_glTexBufferRange = wrapperglTexBufferRange;

        _ptrc_glTexStorage1D = wrapperglTexStorage1D;
        _ptrc_glTexStorage2D = wrapperglTexStorage2D;
        _ptrc_glTexStorage3D = wrapperglTexStorage3D;

        _ptrc_glTextureView = wrapperglTextureView;

        _ptrc_glBindVertexBuffer = wrapperglBindVertexBuffer;
        _ptrc_glVertexAttribBinding = wrapperglVertexAttribBinding;
        _ptrc_glVertexAttribFormat = wrapperglVertexAttribFormat;
        _ptrc_glVertexAttribIFormat = wrapperglVertexAttribIFormat;
        _ptrc_glVertexAttribLFormat = wrapperglVertexAttribLFormat;
        _ptrc_glVertexBindingDivisor = wrapperglVertexBindingDivisor;

        _ptrc_glDepthRangeArrayv = wrapperglDepthRangeArrayv;
        _ptrc_glDepthRangeIndexed = wrapperglDepthRangeIndexed;
        _ptrc_glGetDoublei_v = wrapperglGetDoublei_v;
        _ptrc_glGetFloati_v = wrapperglGetFloati_v;
        _ptrc_glScissorArrayv = wrapperglScissorArrayv;
        _ptrc_glScissorIndexed = wrapperglScissorIndexed;
        _ptrc_glScissorIndexedv = wrapperglScissorIndexedv;
        _ptrc_glViewportArrayv = wrapperglViewportArrayv;
        _ptrc_glViewportIndexedf = wrapperglViewportIndexedf;
        _ptrc_glViewportIndexedfv = wrapperglViewportIndexedfv;

        _ptrc_glClearBufferData = wrapperglClearBufferData;
        _ptrc_glClearBufferSubData = wrapperglClearBufferSubData;

        _ptrc_glCopyImageSubData = wrapperglCopyImageSubData;

        _ptrc_glFramebufferParameteri = wrapperglFramebufferParameteri;
        _ptrc_glGetFramebufferParameteriv = wrapperglGetFramebufferParameteriv;

        _ptrc_glInvalidateBufferData = wrapperglInvalidateBufferData;
        _ptrc_glInvalidateBufferSubData = wrapperglInvalidateBufferSubData;
        _ptrc_glInvalidateFramebuffer = wrapperglInvalidateFramebuffer;
        _ptrc_glInvalidateSubFramebuffer = wrapperglInvalidateSubFramebuffer;
        _ptrc_glInvalidateTexImage = wrapperglInvalidateTexImage;
        _ptrc_glInvalidateTexSubImage = wrapperglInvalidateTexSubImage;

        _ptrc_glTexStorage2DMultisample = wrapperglTexStorage2DMultisample;
        _ptrc_glTexStorage3DMultisample = wrapperglTexStorage3DMultisample;

        _ptrc_glTextureBarrierNV = wrapperglTextureBarrierNV;

        _ptrc_glCopyImageSubDataNV = wrapperglCopyImageSubDataNV;

        _ptrc_glAccum = wrapperglAccum;
        _ptrc_glAlphaFunc = wrapperglAlphaFunc;
        _ptrc_glBegin = wrapperglBegin;
        _ptrc_glBitmap = wrapperglBitmap;
        _ptrc_glBlendFunc = wrapperglBlendFunc;
        _ptrc_glCallList = wrapperglCallList;
        _ptrc_glCallLists = wrapperglCallLists;
        _ptrc_glClear = wrapperglClear;
        _ptrc_glClearAccum = wrapperglClearAccum;
        _ptrc_glClearColor = wrapperglClearColor;
        _ptrc_glClearDepth = wrapperglClearDepth;
        _ptrc_glClearIndex = wrapperglClearIndex;
        _ptrc_glClearStencil = wrapperglClearStencil;
        _ptrc_glClipPlane = wrapperglClipPlane;
        _ptrc_glColor3b = wrapperglColor3b;
        _ptrc_glColor3bv = wrapperglColor3bv;
        _ptrc_glColor3d = wrapperglColor3d;
        _ptrc_glColor3dv = wrapperglColor3dv;
        _ptrc_glColor3f = wrapperglColor3f;
        _ptrc_glColor3fv = wrapperglColor3fv;
        _ptrc_glColor3i = wrapperglColor3i;
        _ptrc_glColor3iv = wrapperglColor3iv;
        _ptrc_glColor3s = wrapperglColor3s;
        _ptrc_glColor3sv = wrapperglColor3sv;
        _ptrc_glColor3ub = wrapperglColor3ub;
        _ptrc_glColor3ubv = wrapperglColor3ubv;
        _ptrc_glColor3ui = wrapperglColor3ui;
        _ptrc_glColor3uiv = wrapperglColor3uiv;
        _ptrc_glColor3us = wrapperglColor3us;
        _ptrc_glColor3usv = wrapperglColor3usv;
        _ptrc_glColor4b = wrapperglColor4b;
        _ptrc_glColor4bv = wrapperglColor4bv;
        _ptrc_glColor4d = wrapperglColor4d;
        _ptrc_glColor4dv = wrapperglColor4dv;
        _ptrc_glColor4f = wrapperglColor4f;
        _ptrc_glColor4fv = wrapperglColor4fv;
        _ptrc_glColor4i = wrapperglColor4i;
        _ptrc_glColor4iv = wrapperglColor4iv;
        _ptrc_glColor4s = wrapperglColor4s;
        _ptrc_glColor4sv = wrapperglColor4sv;
        _ptrc_glColor4ub = wrapperglColor4ub;
        _ptrc_glColor4ubv = wrapperglColor4ubv;
        _ptrc_glColor4ui = wrapperglColor4ui;
        _ptrc_glColor4uiv = wrapperglColor4uiv;
        _ptrc_glColor4us = wrapperglColor4us;
        _ptrc_glColor4usv = wrapperglColor4usv;
        _ptrc_glColorMask = wrapperglColorMask;
        _ptrc_glColorMaterial = wrapperglColorMaterial;
        _ptrc_glCopyPixels = wrapperglCopyPixels;
        _ptrc_glCullFace = wrapperglCullFace;
        _ptrc_glDeleteLists = wrapperglDeleteLists;
        _ptrc_glDepthFunc = wrapperglDepthFunc;
        _ptrc_glDepthMask = wrapperglDepthMask;
        _ptrc_glDepthRange = wrapperglDepthRange;
        _ptrc_glDrawBuffer = wrapperglDrawBuffer;
        _ptrc_glDrawPixels = wrapperglDrawPixels;
        _ptrc_glEdgeFlag = wrapperglEdgeFlag;
        _ptrc_glEdgeFlagv = wrapperglEdgeFlagv;
        _ptrc_glEnd = wrapperglEnd;
        _ptrc_glEndList = wrapperglEndList;
        _ptrc_glEvalCoord1d = wrapperglEvalCoord1d;
        _ptrc_glEvalCoord1dv = wrapperglEvalCoord1dv;
        _ptrc_glEvalCoord1f = wrapperglEvalCoord1f;
        _ptrc_glEvalCoord1fv = wrapperglEvalCoord1fv;
        _ptrc_glEvalCoord2d = wrapperglEvalCoord2d;
        _ptrc_glEvalCoord2dv = wrapperglEvalCoord2dv;
        _ptrc_glEvalCoord2f = wrapperglEvalCoord2f;
        _ptrc_glEvalCoord2fv = wrapperglEvalCoord2fv;
        _ptrc_glEvalMesh1 = wrapperglEvalMesh1;
        _ptrc_glEvalMesh2 = wrapperglEvalMesh2;
        _ptrc_glEvalPoint1 = wrapperglEvalPoint1;
        _ptrc_glEvalPoint2 = wrapperglEvalPoint2;
        _ptrc_glFeedbackBuffer = wrapperglFeedbackBuffer;
        _ptrc_glFinish = wrapperglFinish;
        _ptrc_glFlush = wrapperglFlush;
        _ptrc_glFogf = wrapperglFogf;
        _ptrc_glFogfv = wrapperglFogfv;
        _ptrc_glFogi = wrapperglFogi;
        _ptrc_glFogiv = wrapperglFogiv;
        _ptrc_glFrontFace = wrapperglFrontFace;
        _ptrc_glFrustum = wrapperglFrustum;
        _ptrc_glGenLists = wrapperglGenLists;
        _ptrc_glGetBooleanv = wrapperglGetBooleanv;
        _ptrc_glGetClipPlane = wrapperglGetClipPlane;
        _ptrc_glGetDoublev = wrapperglGetDoublev;
        _ptrc_glGetFloatv = wrapperglGetFloatv;
        _ptrc_glGetLightfv = wrapperglGetLightfv;
        _ptrc_glGetLightiv = wrapperglGetLightiv;
        _ptrc_glGetMapdv = wrapperglGetMapdv;
        _ptrc_glGetMapfv = wrapperglGetMapfv;
        _ptrc_glGetMapiv = wrapperglGetMapiv;
        _ptrc_glGetMaterialfv = wrapperglGetMaterialfv;
        _ptrc_glGetMaterialiv = wrapperglGetMaterialiv;
        _ptrc_glGetPixelMapfv = wrapperglGetPixelMapfv;
        _ptrc_glGetPixelMapuiv = wrapperglGetPixelMapuiv;
        _ptrc_glGetPixelMapusv = wrapperglGetPixelMapusv;
        _ptrc_glGetPolygonStipple = wrapperglGetPolygonStipple;
        _ptrc_glGetString = wrapperglGetString;
        _ptrc_glGetTexEnvfv = wrapperglGetTexEnvfv;
        _ptrc_glGetTexEnviv = wrapperglGetTexEnviv;
        _ptrc_glGetTexGendv = wrapperglGetTexGendv;
        _ptrc_glGetTexGenfv = wrapperglGetTexGenfv;
        _ptrc_glGetTexGeniv = wrapperglGetTexGeniv;
        _ptrc_glGetTexImage = wrapperglGetTexImage;
        _ptrc_glGetTexLevelParameterfv = wrapperglGetTexLevelParameterfv;
        _ptrc_glGetTexLevelParameteriv = wrapperglGetTexLevelParameteriv;
        _ptrc_glGetTexParameterfv = wrapperglGetTexParameterfv;
        _ptrc_glGetTexParameteriv = wrapperglGetTexParameteriv;
        _ptrc_glHint = wrapperglHint;
        _ptrc_glIndexMask = wrapperglIndexMask;
        _ptrc_glIndexd = wrapperglIndexd;
        _ptrc_glIndexdv = wrapperglIndexdv;
        _ptrc_glIndexf = wrapperglIndexf;
        _ptrc_glIndexfv = wrapperglIndexfv;
        _ptrc_glIndexi = wrapperglIndexi;
        _ptrc_glIndexiv = wrapperglIndexiv;
        _ptrc_glIndexs = wrapperglIndexs;
        _ptrc_glIndexsv = wrapperglIndexsv;
        _ptrc_glInitNames = wrapperglInitNames;
        _ptrc_glIsList = wrapperglIsList;
        _ptrc_glLightModelf = wrapperglLightModelf;
        _ptrc_glLightModelfv = wrapperglLightModelfv;
        _ptrc_glLightModeli = wrapperglLightModeli;
        _ptrc_glLightModeliv = wrapperglLightModeliv;
        _ptrc_glLightf = wrapperglLightf;
        _ptrc_glLightfv = wrapperglLightfv;
        _ptrc_glLighti = wrapperglLighti;
        _ptrc_glLightiv = wrapperglLightiv;
        _ptrc_glLineStipple = wrapperglLineStipple;
        _ptrc_glLineWidth = wrapperglLineWidth;
        _ptrc_glListBase = wrapperglListBase;
        _ptrc_glLoadIdentity = wrapperglLoadIdentity;
        _ptrc_glLoadMatrixd = wrapperglLoadMatrixd;
        _ptrc_glLoadMatrixf = wrapperglLoadMatrixf;
        _ptrc_glLoadName = wrapperglLoadName;
        _ptrc_glLogicOp = wrapperglLogicOp;
        _ptrc_glMap1d = wrapperglMap1d;
        _ptrc_glMap1f = wrapperglMap1f;
        _ptrc_glMap2d = wrapperglMap2d;
        _ptrc_glMap2f = wrapperglMap2f;
        _ptrc_glMapGrid1d = wrapperglMapGrid1d;
        _ptrc_glMapGrid1f = wrapperglMapGrid1f;
        _ptrc_glMapGrid2d = wrapperglMapGrid2d;
        _ptrc_glMapGrid2f = wrapperglMapGrid2f;
        _ptrc_glMaterialf = wrapperglMaterialf;
        _ptrc_glMaterialfv = wrapperglMaterialfv;
        _ptrc_glMateriali = wrapperglMateriali;
        _ptrc_glMaterialiv = wrapperglMaterialiv;
        _ptrc_glMatrixMode = wrapperglMatrixMode;
        _ptrc_glMultMatrixd = wrapperglMultMatrixd;
        _ptrc_glMultMatrixf = wrapperglMultMatrixf;
        _ptrc_glNewList = wrapperglNewList;
        _ptrc_glNormal3b = wrapperglNormal3b;
        _ptrc_glNormal3bv = wrapperglNormal3bv;
        _ptrc_glNormal3d = wrapperglNormal3d;
        _ptrc_glNormal3dv = wrapperglNormal3dv;
        _ptrc_glNormal3f = wrapperglNormal3f;
        _ptrc_glNormal3fv = wrapperglNormal3fv;
        _ptrc_glNormal3i = wrapperglNormal3i;
        _ptrc_glNormal3iv = wrapperglNormal3iv;
        _ptrc_glNormal3s = wrapperglNormal3s;
        _ptrc_glNormal3sv = wrapperglNormal3sv;
        _ptrc_glOrtho = wrapperglOrtho;
        _ptrc_glPassThrough = wrapperglPassThrough;
        _ptrc_glPixelMapfv = wrapperglPixelMapfv;
        _ptrc_glPixelMapuiv = wrapperglPixelMapuiv;
        _ptrc_glPixelMapusv = wrapperglPixelMapusv;
        _ptrc_glPixelStoref = wrapperglPixelStoref;
        _ptrc_glPixelStorei = wrapperglPixelStorei;
        _ptrc_glPixelTransferf = wrapperglPixelTransferf;
        _ptrc_glPixelTransferi = wrapperglPixelTransferi;
        _ptrc_glPixelZoom = wrapperglPixelZoom;
        _ptrc_glPointSize = wrapperglPointSize;
        _ptrc_glPolygonMode = wrapperglPolygonMode;
        _ptrc_glPolygonStipple = wrapperglPolygonStipple;
        _ptrc_glPopAttrib = wrapperglPopAttrib;
        _ptrc_glPopMatrix = wrapperglPopMatrix;
        _ptrc_glPopName = wrapperglPopName;
        _ptrc_glPushAttrib = wrapperglPushAttrib;
        _ptrc_glPushMatrix = wrapperglPushMatrix;
        _ptrc_glPushName = wrapperglPushName;
        _ptrc_glRasterPos2d = wrapperglRasterPos2d;
        _ptrc_glRasterPos2dv = wrapperglRasterPos2dv;
        _ptrc_glRasterPos2f = wrapperglRasterPos2f;
        _ptrc_glRasterPos2fv = wrapperglRasterPos2fv;
        _ptrc_glRasterPos2i = wrapperglRasterPos2i;
        _ptrc_glRasterPos2iv = wrapperglRasterPos2iv;
        _ptrc_glRasterPos2s = wrapperglRasterPos2s;
        _ptrc_glRasterPos2sv = wrapperglRasterPos2sv;
        _ptrc_glRasterPos3d = wrapperglRasterPos3d;
        _ptrc_glRasterPos3dv = wrapperglRasterPos3dv;
        _ptrc_glRasterPos3f = wrapperglRasterPos3f;
        _ptrc_glRasterPos3fv = wrapperglRasterPos3fv;
        _ptrc_glRasterPos3i = wrapperglRasterPos3i;
        _ptrc_glRasterPos3iv = wrapperglRasterPos3iv;
        _ptrc_glRasterPos3s = wrapperglRasterPos3s;
        _ptrc_glRasterPos3sv = wrapperglRasterPos3sv;
        _ptrc_glRasterPos4d = wrapperglRasterPos4d;
        _ptrc_glRasterPos4dv = wrapperglRasterPos4dv;
        _ptrc_glRasterPos4f = wrapperglRasterPos4f;
        _ptrc_glRasterPos4fv = wrapperglRasterPos4fv;
        _ptrc_glRasterPos4i = wrapperglRasterPos4i;
        _ptrc_glRasterPos4iv = wrapperglRasterPos4iv;
        _ptrc_glRasterPos4s = wrapperglRasterPos4s;
        _ptrc_glRasterPos4sv = wrapperglRasterPos4sv;
        _ptrc_glReadBuffer = wrapperglReadBuffer;
        _ptrc_glReadPixels = wrapperglReadPixels;
        _ptrc_glRectd = wrapperglRectd;
        _ptrc_glRectdv = wrapperglRectdv;
        _ptrc_glRectf = wrapperglRectf;
        _ptrc_glRectfv = wrapperglRectfv;
        _ptrc_glRecti = wrapperglRecti;
        _ptrc_glRectiv = wrapperglRectiv;
        _ptrc_glRects = wrapperglRects;
        _ptrc_glRectsv = wrapperglRectsv;
        _ptrc_glRenderMode = wrapperglRenderMode;
        _ptrc_glRotated = wrapperglRotated;
        _ptrc_glRotatef = wrapperglRotatef;
        _ptrc_glScaled = wrapperglScaled;
        _ptrc_glScalef = wrapperglScalef;
        _ptrc_glScissor = wrapperglScissor;
        _ptrc_glSelectBuffer = wrapperglSelectBuffer;
        _ptrc_glShadeModel = wrapperglShadeModel;
        _ptrc_glStencilFunc = wrapperglStencilFunc;
        _ptrc_glStencilMask = wrapperglStencilMask;
        _ptrc_glStencilOp = wrapperglStencilOp;
        _ptrc_glTexCoord1d = wrapperglTexCoord1d;
        _ptrc_glTexCoord1dv = wrapperglTexCoord1dv;
        _ptrc_glTexCoord1f = wrapperglTexCoord1f;
        _ptrc_glTexCoord1fv = wrapperglTexCoord1fv;
        _ptrc_glTexCoord1i = wrapperglTexCoord1i;
        _ptrc_glTexCoord1iv = wrapperglTexCoord1iv;
        _ptrc_glTexCoord1s = wrapperglTexCoord1s;
        _ptrc_glTexCoord1sv = wrapperglTexCoord1sv;
        _ptrc_glTexCoord2d = wrapperglTexCoord2d;
        _ptrc_glTexCoord2dv = wrapperglTexCoord2dv;
        _ptrc_glTexCoord2f = wrapperglTexCoord2f;
        _ptrc_glTexCoord2fv = wrapperglTexCoord2fv;
        _ptrc_glTexCoord2i = wrapperglTexCoord2i;
        _ptrc_glTexCoord2iv = wrapperglTexCoord2iv;
        _ptrc_glTexCoord2s = wrapperglTexCoord2s;
        _ptrc_glTexCoord2sv = wrapperglTexCoord2sv;
        _ptrc_glTexCoord3d = wrapperglTexCoord3d;
        _ptrc_glTexCoord3dv = wrapperglTexCoord3dv;
        _ptrc_glTexCoord3f = wrapperglTexCoord3f;
        _ptrc_glTexCoord3fv = wrapperglTexCoord3fv;
        _ptrc_glTexCoord3i = wrapperglTexCoord3i;
        _ptrc_glTexCoord3iv = wrapperglTexCoord3iv;
        _ptrc_glTexCoord3s = wrapperglTexCoord3s;
        _ptrc_glTexCoord3sv = wrapperglTexCoord3sv;
        _ptrc_glTexCoord4d = wrapperglTexCoord4d;
        _ptrc_glTexCoord4dv = wrapperglTexCoord4dv;
        _ptrc_glTexCoord4f = wrapperglTexCoord4f;
        _ptrc_glTexCoord4fv = wrapperglTexCoord4fv;
        _ptrc_glTexCoord4i = wrapperglTexCoord4i;
        _ptrc_glTexCoord4iv = wrapperglTexCoord4iv;
        _ptrc_glTexCoord4s = wrapperglTexCoord4s;
        _ptrc_glTexCoord4sv = wrapperglTexCoord4sv;
        _ptrc_glTexEnvf = wrapperglTexEnvf;
        _ptrc_glTexEnvfv = wrapperglTexEnvfv;
        _ptrc_glTexEnvi = wrapperglTexEnvi;
        _ptrc_glTexEnviv = wrapperglTexEnviv;
        _ptrc_glTexGend = wrapperglTexGend;
        _ptrc_glTexGendv = wrapperglTexGendv;
        _ptrc_glTexGenf = wrapperglTexGenf;
        _ptrc_glTexGenfv = wrapperglTexGenfv;
        _ptrc_glTexGeni = wrapperglTexGeni;
        _ptrc_glTexGeniv = wrapperglTexGeniv;
        _ptrc_glTexImage1D = wrapperglTexImage1D;
        _ptrc_glTexImage2D = wrapperglTexImage2D;
        _ptrc_glTexParameterf = wrapperglTexParameterf;
        _ptrc_glTexParameterfv = wrapperglTexParameterfv;
        _ptrc_glTexParameteri = wrapperglTexParameteri;
        _ptrc_glTexParameteriv = wrapperglTexParameteriv;
        _ptrc_glTranslated = wrapperglTranslated;
        _ptrc_glTranslatef = wrapperglTranslatef;
        _ptrc_glVertex2d = wrapperglVertex2d;
        _ptrc_glVertex2dv = wrapperglVertex2dv;
        _ptrc_glVertex2f = wrapperglVertex2f;
        _ptrc_glVertex2fv = wrapperglVertex2fv;
        _ptrc_glVertex2i = wrapperglVertex2i;
        _ptrc_glVertex2iv = wrapperglVertex2iv;
        _ptrc_glVertex2s = wrapperglVertex2s;
        _ptrc_glVertex2sv = wrapperglVertex2sv;
        _ptrc_glVertex3d = wrapperglVertex3d;
        _ptrc_glVertex3dv = wrapperglVertex3dv;
        _ptrc_glVertex3f = wrapperglVertex3f;
        _ptrc_glVertex3fv = wrapperglVertex3fv;
        _ptrc_glVertex3i = wrapperglVertex3i;
        _ptrc_glVertex3iv = wrapperglVertex3iv;
        _ptrc_glVertex3s = wrapperglVertex3s;
        _ptrc_glVertex3sv = wrapperglVertex3sv;
        _ptrc_glVertex4d = wrapperglVertex4d;
        _ptrc_glVertex4dv = wrapperglVertex4dv;
        _ptrc_glVertex4f = wrapperglVertex4f;
        _ptrc_glVertex4fv = wrapperglVertex4fv;
        _ptrc_glVertex4i = wrapperglVertex4i;
        _ptrc_glVertex4iv = wrapperglVertex4iv;
        _ptrc_glVertex4s = wrapperglVertex4s;
        _ptrc_glVertex4sv = wrapperglVertex4sv;
        _ptrc_glViewport = wrapperglViewport;

        _ptrc_glAreTexturesResident = wrapperglAreTexturesResident;
        _ptrc_glArrayElement = wrapperglArrayElement;
        _ptrc_glBindTexture = wrapperglBindTexture;
        _ptrc_glColorPointer = wrapperglColorPointer;
        _ptrc_glCopyTexImage1D = wrapperglCopyTexImage1D;
        _ptrc_glCopyTexImage2D = wrapperglCopyTexImage2D;
        _ptrc_glCopyTexSubImage1D = wrapperglCopyTexSubImage1D;
        _ptrc_glCopyTexSubImage2D = wrapperglCopyTexSubImage2D;
        _ptrc_glDeleteTextures = wrapperglDeleteTextures;
        _ptrc_glDisableClientState = wrapperglDisableClientState;
        _ptrc_glDrawArrays = wrapperglDrawArrays;
        _ptrc_glDrawElements = wrapperglDrawElements;
        _ptrc_glEdgeFlagPointer = wrapperglEdgeFlagPointer;
        _ptrc_glEnableClientState = wrapperglEnableClientState;
        _ptrc_glGenTextures = wrapperglGenTextures;
        _ptrc_glIndexPointer = wrapperglIndexPointer;
        _ptrc_glIndexub = wrapperglIndexub;
        _ptrc_glIndexubv = wrapperglIndexubv;
        _ptrc_glInterleavedArrays = wrapperglInterleavedArrays;
        _ptrc_glIsTexture = wrapperglIsTexture;
        _ptrc_glNormalPointer = wrapperglNormalPointer;
        _ptrc_glPolygonOffset = wrapperglPolygonOffset;
        _ptrc_glPopClientAttrib = wrapperglPopClientAttrib;
        _ptrc_glPrioritizeTextures = wrapperglPrioritizeTextures;
        _ptrc_glPushClientAttrib = wrapperglPushClientAttrib;
        _ptrc_glTexCoordPointer = wrapperglTexCoordPointer;
        _ptrc_glTexSubImage1D = wrapperglTexSubImage1D;
        _ptrc_glTexSubImage2D = wrapperglTexSubImage2D;
        _ptrc_glVertexPointer = wrapperglVertexPointer;

        _ptrc_glBlendColor = wrapperglBlendColor;
        _ptrc_glBlendEquation = wrapperglBlendEquation;
        _ptrc_glCopyTexSubImage3D = wrapperglCopyTexSubImage3D;
        _ptrc_glDrawRangeElements = wrapperglDrawRangeElements;
        _ptrc_glTexImage3D = wrapperglTexImage3D;
        _ptrc_glTexSubImage3D = wrapperglTexSubImage3D;

        _ptrc_glActiveTexture = wrapperglActiveTexture;
        _ptrc_glClientActiveTexture = wrapperglClientActiveTexture;
        _ptrc_glCompressedTexImage1D = wrapperglCompressedTexImage1D;
        _ptrc_glCompressedTexImage2D = wrapperglCompressedTexImage2D;
        _ptrc_glCompressedTexImage3D = wrapperglCompressedTexImage3D;
        _ptrc_glCompressedTexSubImage1D = wrapperglCompressedTexSubImage1D;
        _ptrc_glCompressedTexSubImage2D = wrapperglCompressedTexSubImage2D;
        _ptrc_glCompressedTexSubImage3D = wrapperglCompressedTexSubImage3D;
        _ptrc_glGetCompressedTexImage = wrapperglGetCompressedTexImage;
        _ptrc_glLoadTransposeMatrixd = wrapperglLoadTransposeMatrixd;
        _ptrc_glLoadTransposeMatrixf = wrapperglLoadTransposeMatrixf;
        _ptrc_glMultTransposeMatrixd = wrapperglMultTransposeMatrixd;
        _ptrc_glMultTransposeMatrixf = wrapperglMultTransposeMatrixf;
        _ptrc_glMultiTexCoord1d = wrapperglMultiTexCoord1d;
        _ptrc_glMultiTexCoord1dv = wrapperglMultiTexCoord1dv;
        _ptrc_glMultiTexCoord1f = wrapperglMultiTexCoord1f;
        _ptrc_glMultiTexCoord1fv = wrapperglMultiTexCoord1fv;
        _ptrc_glMultiTexCoord1i = wrapperglMultiTexCoord1i;
        _ptrc_glMultiTexCoord1iv = wrapperglMultiTexCoord1iv;
        _ptrc_glMultiTexCoord1s = wrapperglMultiTexCoord1s;
        _ptrc_glMultiTexCoord1sv = wrapperglMultiTexCoord1sv;
        _ptrc_glMultiTexCoord2d = wrapperglMultiTexCoord2d;
        _ptrc_glMultiTexCoord2dv = wrapperglMultiTexCoord2dv;
        _ptrc_glMultiTexCoord2f = wrapperglMultiTexCoord2f;
        _ptrc_glMultiTexCoord2fv = wrapperglMultiTexCoord2fv;
        _ptrc_glMultiTexCoord2i = wrapperglMultiTexCoord2i;
        _ptrc_glMultiTexCoord2iv = wrapperglMultiTexCoord2iv;
        _ptrc_glMultiTexCoord2s = wrapperglMultiTexCoord2s;
        _ptrc_glMultiTexCoord2sv = wrapperglMultiTexCoord2sv;
        _ptrc_glMultiTexCoord3d = wrapperglMultiTexCoord3d;
        _ptrc_glMultiTexCoord3dv = wrapperglMultiTexCoord3dv;
        _ptrc_glMultiTexCoord3f = wrapperglMultiTexCoord3f;
        _ptrc_glMultiTexCoord3fv = wrapperglMultiTexCoord3fv;
        _ptrc_glMultiTexCoord3i = wrapperglMultiTexCoord3i;
        _ptrc_glMultiTexCoord3iv = wrapperglMultiTexCoord3iv;
        _ptrc_glMultiTexCoord3s = wrapperglMultiTexCoord3s;
        _ptrc_glMultiTexCoord3sv = wrapperglMultiTexCoord3sv;
        _ptrc_glMultiTexCoord4d = wrapperglMultiTexCoord4d;
        _ptrc_glMultiTexCoord4dv = wrapperglMultiTexCoord4dv;
        _ptrc_glMultiTexCoord4f = wrapperglMultiTexCoord4f;
        _ptrc_glMultiTexCoord4fv = wrapperglMultiTexCoord4fv;
        _ptrc_glMultiTexCoord4i = wrapperglMultiTexCoord4i;
        _ptrc_glMultiTexCoord4iv = wrapperglMultiTexCoord4iv;
        _ptrc_glMultiTexCoord4s = wrapperglMultiTexCoord4s;
        _ptrc_glMultiTexCoord4sv = wrapperglMultiTexCoord4sv;
        _ptrc_glSampleCoverage = wrapperglSampleCoverage;

        _ptrc_glBlendFuncSeparate = wrapperglBlendFuncSeparate;
        _ptrc_glFogCoordPointer = wrapperglFogCoordPointer;
        _ptrc_glFogCoordd = wrapperglFogCoordd;
        _ptrc_glFogCoorddv = wrapperglFogCoorddv;
        _ptrc_glFogCoordf = wrapperglFogCoordf;
        _ptrc_glFogCoordfv = wrapperglFogCoordfv;
        _ptrc_glMultiDrawArrays = wrapperglMultiDrawArrays;
        _ptrc_glMultiDrawElements = wrapperglMultiDrawElements;
        _ptrc_glPointParameterf = wrapperglPointParameterf;
        _ptrc_glPointParameterfv = wrapperglPointParameterfv;
        _ptrc_glPointParameteri = wrapperglPointParameteri;
        _ptrc_glPointParameteriv = wrapperglPointParameteriv;
        _ptrc_glSecondaryColor3b = wrapperglSecondaryColor3b;
        _ptrc_glSecondaryColor3bv = wrapperglSecondaryColor3bv;
        _ptrc_glSecondaryColor3d = wrapperglSecondaryColor3d;
        _ptrc_glSecondaryColor3dv = wrapperglSecondaryColor3dv;
        _ptrc_glSecondaryColor3f = wrapperglSecondaryColor3f;
        _ptrc_glSecondaryColor3fv = wrapperglSecondaryColor3fv;
        _ptrc_glSecondaryColor3i = wrapperglSecondaryColor3i;
        _ptrc_glSecondaryColor3iv = wrapperglSecondaryColor3iv;
        _ptrc_glSecondaryColor3s = wrapperglSecondaryColor3s;
        _ptrc_glSecondaryColor3sv = wrapperglSecondaryColor3sv;
        _ptrc_glSecondaryColor3ub = wrapperglSecondaryColor3ub;
        _ptrc_glSecondaryColor3ubv = wrapperglSecondaryColor3ubv;
        _ptrc_glSecondaryColor3ui = wrapperglSecondaryColor3ui;
        _ptrc_glSecondaryColor3uiv = wrapperglSecondaryColor3uiv;
        _ptrc_glSecondaryColor3us = wrapperglSecondaryColor3us;
        _ptrc_glSecondaryColor3usv = wrapperglSecondaryColor3usv;
        _ptrc_glSecondaryColorPointer = wrapperglSecondaryColorPointer;
        _ptrc_glWindowPos2d = wrapperglWindowPos2d;
        _ptrc_glWindowPos2dv = wrapperglWindowPos2dv;
        _ptrc_glWindowPos2f = wrapperglWindowPos2f;
        _ptrc_glWindowPos2fv = wrapperglWindowPos2fv;
        _ptrc_glWindowPos2i = wrapperglWindowPos2i;
        _ptrc_glWindowPos2iv = wrapperglWindowPos2iv;
        _ptrc_glWindowPos2s = wrapperglWindowPos2s;
        _ptrc_glWindowPos2sv = wrapperglWindowPos2sv;
        _ptrc_glWindowPos3d = wrapperglWindowPos3d;
        _ptrc_glWindowPos3dv = wrapperglWindowPos3dv;
        _ptrc_glWindowPos3f = wrapperglWindowPos3f;
        _ptrc_glWindowPos3fv = wrapperglWindowPos3fv;
        _ptrc_glWindowPos3i = wrapperglWindowPos3i;
        _ptrc_glWindowPos3iv = wrapperglWindowPos3iv;
        _ptrc_glWindowPos3s = wrapperglWindowPos3s;
        _ptrc_glWindowPos3sv = wrapperglWindowPos3sv;

        _ptrc_glBeginQuery = wrapperglBeginQuery;
        _ptrc_glBindBuffer = wrapperglBindBuffer;
        _ptrc_glBufferData = wrapperglBufferData;
        _ptrc_glBufferSubData = wrapperglBufferSubData;
        _ptrc_glDeleteBuffers = wrapperglDeleteBuffers;
        _ptrc_glDeleteQueries = wrapperglDeleteQueries;
        _ptrc_glEndQuery = wrapperglEndQuery;
        _ptrc_glGenBuffers = wrapperglGenBuffers;
        _ptrc_glGenQueries = wrapperglGenQueries;
        _ptrc_glGetBufferParameteriv = wrapperglGetBufferParameteriv;
        _ptrc_glGetBufferPointerv = wrapperglGetBufferPointerv;
        _ptrc_glGetBufferSubData = wrapperglGetBufferSubData;
        _ptrc_glGetQueryObjectiv = wrapperglGetQueryObjectiv;
        _ptrc_glGetQueryObjectuiv = wrapperglGetQueryObjectuiv;
        _ptrc_glGetQueryiv = wrapperglGetQueryiv;
        _ptrc_glIsBuffer = wrapperglIsBuffer;
        _ptrc_glIsQuery = wrapperglIsQuery;
        _ptrc_glMapBuffer = wrapperglMapBuffer;
        _ptrc_glUnmapBuffer = wrapperglUnmapBuffer;

        _ptrc_glAttachShader = wrapperglAttachShader;
        _ptrc_glBindAttribLocation = wrapperglBindAttribLocation;
        _ptrc_glBlendEquationSeparate = wrapperglBlendEquationSeparate;
        _ptrc_glCompileShader = wrapperglCompileShader;
        _ptrc_glCreateProgram = wrapperglCreateProgram;
        _ptrc_glCreateShader = wrapperglCreateShader;
        _ptrc_glDeleteProgram = wrapperglDeleteProgram;
        _ptrc_glDeleteShader = wrapperglDeleteShader;
        _ptrc_glDetachShader = wrapperglDetachShader;
        _ptrc_glDisableVertexAttribArray = wrapperglDisableVertexAttribArray;
        _ptrc_glDrawBuffers = wrapperglDrawBuffers;
        _ptrc_glEnableVertexAttribArray = wrapperglEnableVertexAttribArray;
        _ptrc_glGetActiveAttrib = wrapperglGetActiveAttrib;
        _ptrc_glGetActiveUniform = wrapperglGetActiveUniform;
        _ptrc_glGetAttachedShaders = wrapperglGetAttachedShaders;
        _ptrc_glGetAttribLocation = wrapperglGetAttribLocation;
        _ptrc_glGetProgramInfoLog = wrapperglGetProgramInfoLog;
        _ptrc_glGetProgramiv = wrapperglGetProgramiv;
        _ptrc_glGetShaderInfoLog = wrapperglGetShaderInfoLog;
        _ptrc_glGetShaderSource = wrapperglGetShaderSource;
        _ptrc_glGetShaderiv = wrapperglGetShaderiv;
        _ptrc_glGetUniformLocation = wrapperglGetUniformLocation;
        _ptrc_glGetUniformfv = wrapperglGetUniformfv;
        _ptrc_glGetUniformiv = wrapperglGetUniformiv;
        _ptrc_glGetVertexAttribPointerv = wrapperglGetVertexAttribPointerv;
        _ptrc_glGetVertexAttribdv = wrapperglGetVertexAttribdv;
        _ptrc_glGetVertexAttribfv = wrapperglGetVertexAttribfv;
        _ptrc_glGetVertexAttribiv = wrapperglGetVertexAttribiv;
        _ptrc_glIsProgram = wrapperglIsProgram;
        _ptrc_glIsShader = wrapperglIsShader;
        _ptrc_glLinkProgram = wrapperglLinkProgram;
        _ptrc_glShaderSource = wrapperglShaderSource;
        _ptrc_glStencilFuncSeparate = wrapperglStencilFuncSeparate;
        _ptrc_glStencilMaskSeparate = wrapperglStencilMaskSeparate;
        _ptrc_glStencilOpSeparate = wrapperglStencilOpSeparate;
        _ptrc_glUniform1f = wrapperglUniform1f;
        _ptrc_glUniform1fv = wrapperglUniform1fv;
        _ptrc_glUniform1i = wrapperglUniform1i;
        _ptrc_glUniform1iv = wrapperglUniform1iv;
        _ptrc_glUniform2f = wrapperglUniform2f;
        _ptrc_glUniform2fv = wrapperglUniform2fv;
        _ptrc_glUniform2i = wrapperglUniform2i;
        _ptrc_glUniform2iv = wrapperglUniform2iv;
        _ptrc_glUniform3f = wrapperglUniform3f;
        _ptrc_glUniform3fv = wrapperglUniform3fv;
        _ptrc_glUniform3i = wrapperglUniform3i;
        _ptrc_glUniform3iv = wrapperglUniform3iv;
        _ptrc_glUniform4f = wrapperglUniform4f;
        _ptrc_glUniform4fv = wrapperglUniform4fv;
        _ptrc_glUniform4i = wrapperglUniform4i;
        _ptrc_glUniform4iv = wrapperglUniform4iv;
        _ptrc_glUniformMatrix2fv = wrapperglUniformMatrix2fv;
        _ptrc_glUniformMatrix3fv = wrapperglUniformMatrix3fv;
        _ptrc_glUniformMatrix4fv = wrapperglUniformMatrix4fv;
        _ptrc_glUseProgram = wrapperglUseProgram;
        _ptrc_glValidateProgram = wrapperglValidateProgram;
        _ptrc_glVertexAttrib1d = wrapperglVertexAttrib1d;
        _ptrc_glVertexAttrib1dv = wrapperglVertexAttrib1dv;
        _ptrc_glVertexAttrib1f = wrapperglVertexAttrib1f;
        _ptrc_glVertexAttrib1fv = wrapperglVertexAttrib1fv;
        _ptrc_glVertexAttrib1s = wrapperglVertexAttrib1s;
        _ptrc_glVertexAttrib1sv = wrapperglVertexAttrib1sv;
        _ptrc_glVertexAttrib2d = wrapperglVertexAttrib2d;
        _ptrc_glVertexAttrib2dv = wrapperglVertexAttrib2dv;
        _ptrc_glVertexAttrib2f = wrapperglVertexAttrib2f;
        _ptrc_glVertexAttrib2fv = wrapperglVertexAttrib2fv;
        _ptrc_glVertexAttrib2s = wrapperglVertexAttrib2s;
        _ptrc_glVertexAttrib2sv = wrapperglVertexAttrib2sv;
        _ptrc_glVertexAttrib3d = wrapperglVertexAttrib3d;
        _ptrc_glVertexAttrib3dv = wrapperglVertexAttrib3dv;
        _ptrc_glVertexAttrib3f = wrapperglVertexAttrib3f;
        _ptrc_glVertexAttrib3fv = wrapperglVertexAttrib3fv;
        _ptrc_glVertexAttrib3s = wrapperglVertexAttrib3s;
        _ptrc_glVertexAttrib3sv = wrapperglVertexAttrib3sv;
        _ptrc_glVertexAttrib4Nbv = wrapperglVertexAttrib4Nbv;
        _ptrc_glVertexAttrib4Niv = wrapperglVertexAttrib4Niv;
        _ptrc_glVertexAttrib4Nsv = wrapperglVertexAttrib4Nsv;
        _ptrc_glVertexAttrib4Nub = wrapperglVertexAttrib4Nub;
        _ptrc_glVertexAttrib4Nubv = wrapperglVertexAttrib4Nubv;
        _ptrc_glVertexAttrib4Nuiv = wrapperglVertexAttrib4Nuiv;
        _ptrc_glVertexAttrib4Nusv = wrapperglVertexAttrib4Nusv;
        _ptrc_glVertexAttrib4bv = wrapperglVertexAttrib4bv;
        _ptrc_glVertexAttrib4d = wrapperglVertexAttrib4d;
        _ptrc_glVertexAttrib4dv = wrapperglVertexAttrib4dv;
        _ptrc_glVertexAttrib4f = wrapperglVertexAttrib4f;
        _ptrc_glVertexAttrib4fv = wrapperglVertexAttrib4fv;
        _ptrc_glVertexAttrib4iv = wrapperglVertexAttrib4iv;
        _ptrc_glVertexAttrib4s = wrapperglVertexAttrib4s;
        _ptrc_glVertexAttrib4sv = wrapperglVertexAttrib4sv;
        _ptrc_glVertexAttrib4ubv = wrapperglVertexAttrib4ubv;
        _ptrc_glVertexAttrib4uiv = wrapperglVertexAttrib4uiv;
        _ptrc_glVertexAttrib4usv = wrapperglVertexAttrib4usv;
        _ptrc_glVertexAttribPointer = wrapperglVertexAttribPointer;

        _ptrc_glUniformMatrix2x3fv = wrapperglUniformMatrix2x3fv;
        _ptrc_glUniformMatrix2x4fv = wrapperglUniformMatrix2x4fv;
        _ptrc_glUniformMatrix3x2fv = wrapperglUniformMatrix3x2fv;
        _ptrc_glUniformMatrix3x4fv = wrapperglUniformMatrix3x4fv;
        _ptrc_glUniformMatrix4x2fv = wrapperglUniformMatrix4x2fv;
        _ptrc_glUniformMatrix4x3fv = wrapperglUniformMatrix4x3fv;

        _ptrc_glBeginConditionalRender = wrapperglBeginConditionalRender;
        _ptrc_glBeginTransformFeedback = wrapperglBeginTransformFeedback;
        _ptrc_glBindBufferBase = wrapperglBindBufferBase;
        _ptrc_glBindBufferRange = wrapperglBindBufferRange;
        _ptrc_glBindFragDataLocation = wrapperglBindFragDataLocation;
        _ptrc_glBindFramebuffer = wrapperglBindFramebuffer;
        _ptrc_glBindRenderbuffer = wrapperglBindRenderbuffer;
        _ptrc_glBindVertexArray = wrapperglBindVertexArray;
        _ptrc_glBlitFramebuffer = wrapperglBlitFramebuffer;
        _ptrc_glCheckFramebufferStatus = wrapperglCheckFramebufferStatus;
        _ptrc_glClampColor = wrapperglClampColor;
        _ptrc_glClearBufferfi = wrapperglClearBufferfi;
        _ptrc_glClearBufferfv = wrapperglClearBufferfv;
        _ptrc_glClearBufferiv = wrapperglClearBufferiv;
        _ptrc_glClearBufferuiv = wrapperglClearBufferuiv;
        _ptrc_glColorMaski = wrapperglColorMaski;
        _ptrc_glDeleteFramebuffers = wrapperglDeleteFramebuffers;
        _ptrc_glDeleteRenderbuffers = wrapperglDeleteRenderbuffers;
        _ptrc_glDeleteVertexArrays = wrapperglDeleteVertexArrays;
        _ptrc_glDisablei = wrapperglDisablei;
        _ptrc_glEnablei = wrapperglEnablei;
        _ptrc_glEndConditionalRender = wrapperglEndConditionalRender;
        _ptrc_glEndTransformFeedback = wrapperglEndTransformFeedback;
        _ptrc_glFlushMappedBufferRange = wrapperglFlushMappedBufferRange;
        _ptrc_glFramebufferRenderbuffer = wrapperglFramebufferRenderbuffer;
        _ptrc_glFramebufferTexture1D = wrapperglFramebufferTexture1D;
        _ptrc_glFramebufferTexture2D = wrapperglFramebufferTexture2D;
        _ptrc_glFramebufferTexture3D = wrapperglFramebufferTexture3D;
        _ptrc_glFramebufferTextureLayer = wrapperglFramebufferTextureLayer;
        _ptrc_glGenFramebuffers = wrapperglGenFramebuffers;
        _ptrc_glGenRenderbuffers = wrapperglGenRenderbuffers;
        _ptrc_glGenVertexArrays = wrapperglGenVertexArrays;
        _ptrc_glGenerateMipmap = wrapperglGenerateMipmap;
        _ptrc_glGetBooleani_v = wrapperglGetBooleani_v;
        _ptrc_glGetFragDataLocation = wrapperglGetFragDataLocation;
        _ptrc_glGetFramebufferAttachmentParameteriv = wrapperglGetFramebufferAttachmentParameteriv;
        _ptrc_glGetIntegeri_v = wrapperglGetIntegeri_v;
        _ptrc_glGetRenderbufferParameteriv = wrapperglGetRenderbufferParameteriv;
        _ptrc_glGetStringi = wrapperglGetStringi;
        _ptrc_glGetTexParameterIiv = wrapperglGetTexParameterIiv;
        _ptrc_glGetTexParameterIuiv = wrapperglGetTexParameterIuiv;
        _ptrc_glGetTransformFeedbackVarying = wrapperglGetTransformFeedbackVarying;
        _ptrc_glGetUniformuiv = wrapperglGetUniformuiv;
        _ptrc_glGetVertexAttribIiv = wrapperglGetVertexAttribIiv;
        _ptrc_glGetVertexAttribIuiv = wrapperglGetVertexAttribIuiv;
        _ptrc_glIsEnabledi = wrapperglIsEnabledi;
        _ptrc_glIsFramebuffer = wrapperglIsFramebuffer;
        _ptrc_glIsRenderbuffer = wrapperglIsRenderbuffer;
        _ptrc_glIsVertexArray = wrapperglIsVertexArray;
        _ptrc_glMapBufferRange = wrapperglMapBufferRange;
        _ptrc_glRenderbufferStorage = wrapperglRenderbufferStorage;
        _ptrc_glRenderbufferStorageMultisample = wrapperglRenderbufferStorageMultisample;
        _ptrc_glTexParameterIiv = wrapperglTexParameterIiv;
        _ptrc_glTexParameterIuiv = wrapperglTexParameterIuiv;
        _ptrc_glTransformFeedbackVaryings = wrapperglTransformFeedbackVaryings;
        _ptrc_glUniform1ui = wrapperglUniform1ui;
        _ptrc_glUniform1uiv = wrapperglUniform1uiv;
        _ptrc_glUniform2ui = wrapperglUniform2ui;
        _ptrc_glUniform2uiv = wrapperglUniform2uiv;
        _ptrc_glUniform3ui = wrapperglUniform3ui;
        _ptrc_glUniform3uiv = wrapperglUniform3uiv;
        _ptrc_glUniform4ui = wrapperglUniform4ui;
        _ptrc_glUniform4uiv = wrapperglUniform4uiv;
        _ptrc_glVertexAttribI1i = wrapperglVertexAttribI1i;
        _ptrc_glVertexAttribI1iv = wrapperglVertexAttribI1iv;
        _ptrc_glVertexAttribI1ui = wrapperglVertexAttribI1ui;
        _ptrc_glVertexAttribI1uiv = wrapperglVertexAttribI1uiv;
        _ptrc_glVertexAttribI2i = wrapperglVertexAttribI2i;
        _ptrc_glVertexAttribI2iv = wrapperglVertexAttribI2iv;
        _ptrc_glVertexAttribI2ui = wrapperglVertexAttribI2ui;
        _ptrc_glVertexAttribI2uiv = wrapperglVertexAttribI2uiv;
        _ptrc_glVertexAttribI3i = wrapperglVertexAttribI3i;
        _ptrc_glVertexAttribI3iv = wrapperglVertexAttribI3iv;
        _ptrc_glVertexAttribI3ui = wrapperglVertexAttribI3ui;
        _ptrc_glVertexAttribI3uiv = wrapperglVertexAttribI3uiv;
        _ptrc_glVertexAttribI4bv = wrapperglVertexAttribI4bv;
        _ptrc_glVertexAttribI4i = wrapperglVertexAttribI4i;
        _ptrc_glVertexAttribI4iv = wrapperglVertexAttribI4iv;
        _ptrc_glVertexAttribI4sv = wrapperglVertexAttribI4sv;
        _ptrc_glVertexAttribI4ubv = wrapperglVertexAttribI4ubv;
        _ptrc_glVertexAttribI4ui = wrapperglVertexAttribI4ui;
        _ptrc_glVertexAttribI4uiv = wrapperglVertexAttribI4uiv;
        _ptrc_glVertexAttribI4usv = wrapperglVertexAttribI4usv;
        _ptrc_glVertexAttribIPointer = wrapperglVertexAttribIPointer;

}

static void ClearExtensionVariables()
{
	ogl_ext_EXT_geometry_shader4 = 0;
	ogl_ext_ARB_geometry_shader4 = 0;
	ogl_ext_ARB_tessellation_shader = 0;
	ogl_ext_ARB_compute_shader = 0;
	ogl_ext_ARB_debug_output = 0;
	ogl_ext_KHR_debug = 0;
	ogl_ext_EXT_texture_compression_s3tc = 0;
	ogl_ext_EXT_texture_sRGB = 0;
	ogl_ext_EXT_texture_filter_anisotropic = 0;
	ogl_ext_ARB_compressed_texture_pixel_storage = 0;
	ogl_ext_ARB_conservative_depth = 0;
	ogl_ext_ARB_ES2_compatibility = 0;
	ogl_ext_ARB_get_program_binary = 0;
	ogl_ext_ARB_explicit_uniform_location = 0;
	ogl_ext_ARB_internalformat_query = 0;
	ogl_ext_ARB_internalformat_query2 = 0;
	ogl_ext_ARB_map_buffer_alignment = 0;
	ogl_ext_ARB_program_interface_query = 0;
	ogl_ext_ARB_separate_shader_objects = 0;
	ogl_ext_ARB_shading_language_420pack = 0;
	ogl_ext_ARB_shading_language_packing = 0;
	ogl_ext_ARB_texture_buffer_range = 0;
	ogl_ext_ARB_texture_storage = 0;
	ogl_ext_ARB_texture_view = 0;
	ogl_ext_ARB_vertex_attrib_binding = 0;
	ogl_ext_ARB_viewport_array = 0;
	ogl_ext_ARB_arrays_of_arrays = 0;
	ogl_ext_ARB_clear_buffer_object = 0;
	ogl_ext_ARB_copy_image = 0;
	ogl_ext_ARB_ES3_compatibility = 0;
	ogl_ext_ARB_fragment_layer_viewport = 0;
	ogl_ext_ARB_framebuffer_no_attachments = 0;
	ogl_ext_ARB_invalidate_subdata = 0;
	ogl_ext_ARB_robust_buffer_access_behavior = 0;
	ogl_ext_ARB_stencil_texturing = 0;
	ogl_ext_ARB_texture_query_levels = 0;
	ogl_ext_ARB_texture_storage_multisample = 0;
	ogl_ext_NV_texture_barrier = 0;
	ogl_ext_NV_copy_image = 0;
}

void ProcExtsFromExtList()
{
	GLint iLoop;
	GLint iNumExtensions = 0;
	_original_glGetIntegerv(GL_NUM_EXTENSIONS, &iNumExtensions);

	for(iLoop = 0; iLoop < iNumExtensions; iLoop++)
	{
		const char *strExtensionName = (const char *)_original_glGetStringi(GL_EXTENSIONS, iLoop);
		LoadExtByName(strExtensionName);
	}
}

static int g_major_version = 0;
static int g_minor_version = 0;

static void GetGLVersion()
{
	glGetIntegerv(GL_MAJOR_VERSION, &g_major_version);
	glGetIntegerv(GL_MINOR_VERSION, &g_minor_version);
}

int ogl_GetMajorVersion()
{
	if(g_major_version == 0)
		GetGLVersion();
	return g_major_version;
}

int ogl_GetMinorVersion()
{
	if(g_major_version == 0) //Yes, check the major version to get the minor one.
		GetGLVersion();
	return g_minor_version;
}

int ogl_IsVersionGEQ(int majorVersion, int minorVersion)
{
	if(g_major_version == 0)
		GetGLVersion();
		
	if(majorVersion > g_major_version) return 1;
	if(majorVersion < g_major_version) return 0;
	if(minorVersion >= g_minor_version) return 1;
	return 0;
}
typedef int (*PFN_LOADFUNCPOINTERS)();
typedef struct ogl_StrToExtMap_s
{
	char *extensionName;
	int *extensionVariable;
	PFN_LOADFUNCPOINTERS LoadExtension;
} ogl_StrToExtMap;

static ogl_StrToExtMap ExtensionMap[39] = {
	{"GL_EXT_geometry_shader4", &ogl_ext_EXT_geometry_shader4, Load_EXT_geometry_shader4},
	{"GL_ARB_geometry_shader4", &ogl_ext_ARB_geometry_shader4, Load_ARB_geometry_shader4},
	{"GL_ARB_tessellation_shader", &ogl_ext_ARB_tessellation_shader, Load_ARB_tessellation_shader},
	{"GL_ARB_compute_shader", &ogl_ext_ARB_compute_shader, Load_ARB_compute_shader},
	{"GL_ARB_debug_output", &ogl_ext_ARB_debug_output, Load_ARB_debug_output},
	{"GL_KHR_debug", &ogl_ext_KHR_debug, Load_KHR_debug},
	{"GL_EXT_texture_compression_s3tc", &ogl_ext_EXT_texture_compression_s3tc, NULL},
	{"GL_EXT_texture_sRGB", &ogl_ext_EXT_texture_sRGB, NULL},
	{"GL_EXT_texture_filter_anisotropic", &ogl_ext_EXT_texture_filter_anisotropic, NULL},
	{"GL_ARB_compressed_texture_pixel_storage", &ogl_ext_ARB_compressed_texture_pixel_storage, NULL},
	{"GL_ARB_conservative_depth", &ogl_ext_ARB_conservative_depth, NULL},
	{"GL_ARB_ES2_compatibility", &ogl_ext_ARB_ES2_compatibility, Load_ARB_ES2_compatibility},
	{"GL_ARB_get_program_binary", &ogl_ext_ARB_get_program_binary, Load_ARB_get_program_binary},
	{"GL_ARB_explicit_uniform_location", &ogl_ext_ARB_explicit_uniform_location, NULL},
	{"GL_ARB_internalformat_query", &ogl_ext_ARB_internalformat_query, Load_ARB_internalformat_query},
	{"GL_ARB_internalformat_query2", &ogl_ext_ARB_internalformat_query2, Load_ARB_internalformat_query2},
	{"GL_ARB_map_buffer_alignment", &ogl_ext_ARB_map_buffer_alignment, NULL},
	{"GL_ARB_program_interface_query", &ogl_ext_ARB_program_interface_query, Load_ARB_program_interface_query},
	{"GL_ARB_separate_shader_objects", &ogl_ext_ARB_separate_shader_objects, Load_ARB_separate_shader_objects},
	{"GL_ARB_shading_language_420pack", &ogl_ext_ARB_shading_language_420pack, NULL},
	{"GL_ARB_shading_language_packing", &ogl_ext_ARB_shading_language_packing, NULL},
	{"GL_ARB_texture_buffer_range", &ogl_ext_ARB_texture_buffer_range, Load_ARB_texture_buffer_range},
	{"GL_ARB_texture_storage", &ogl_ext_ARB_texture_storage, Load_ARB_texture_storage},
	{"GL_ARB_texture_view", &ogl_ext_ARB_texture_view, Load_ARB_texture_view},
	{"GL_ARB_vertex_attrib_binding", &ogl_ext_ARB_vertex_attrib_binding, Load_ARB_vertex_attrib_binding},
	{"GL_ARB_viewport_array", &ogl_ext_ARB_viewport_array, Load_ARB_viewport_array},
	{"GL_ARB_arrays_of_arrays", &ogl_ext_ARB_arrays_of_arrays, NULL},
	{"GL_ARB_clear_buffer_object", &ogl_ext_ARB_clear_buffer_object, Load_ARB_clear_buffer_object},
	{"GL_ARB_copy_image", &ogl_ext_ARB_copy_image, Load_ARB_copy_image},
	{"GL_ARB_ES3_compatibility", &ogl_ext_ARB_ES3_compatibility, NULL},
	{"GL_ARB_fragment_layer_viewport", &ogl_ext_ARB_fragment_layer_viewport, NULL},
	{"GL_ARB_framebuffer_no_attachments", &ogl_ext_ARB_framebuffer_no_attachments, Load_ARB_framebuffer_no_attachments},
	{"GL_ARB_invalidate_subdata", &ogl_ext_ARB_invalidate_subdata, Load_ARB_invalidate_subdata},
	{"GL_ARB_robust_buffer_access_behavior", &ogl_ext_ARB_robust_buffer_access_behavior, NULL},
	{"GL_ARB_stencil_texturing", &ogl_ext_ARB_stencil_texturing, NULL},
	{"GL_ARB_texture_query_levels", &ogl_ext_ARB_texture_query_levels, NULL},
	{"GL_ARB_texture_storage_multisample", &ogl_ext_ARB_texture_storage_multisample, Load_ARB_texture_storage_multisample},
	{"GL_NV_texture_barrier", &ogl_ext_NV_texture_barrier, Load_NV_texture_barrier},
	{"GL_NV_copy_image", &ogl_ext_NV_copy_image, Load_NV_copy_image},
};

static int g_extensionMapSize = 39;

static ogl_StrToExtMap *FindExtEntry(const char *extensionName)
{
	int loop;
	ogl_StrToExtMap *currLoc = ExtensionMap;
	for(loop = 0; loop < g_extensionMapSize; ++loop, ++currLoc)
	{
		if(strcmp(extensionName, currLoc->extensionName) == 0)
			return currLoc;
	}
	
	return NULL;
}

static void LoadExtByName(const char *extensionName)
{
	ogl_StrToExtMap *entry = NULL;
	entry = FindExtEntry(extensionName);
	if(entry)
	{
		if(entry->LoadExtension)
		{
			int numFailed = entry->LoadExtension();
			if(numFailed == 0)
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
			}
			else
			{
				*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED + numFailed;
			}
		}
		else
		{
			*(entry->extensionVariable) = ogl_LOAD_SUCCEEDED;
		}
	}
}



// if enforceKHRdebug is true, the KHR_debug extension will be available
// if enforceDebugContext is true, the context will behave like a debug context
//
// This means:
// * if both bools are false, do nothing and see what we get.
// * if the OpenGL context is already a debug context (with KHR_debug) the bools will get ignored (no need to enforce something that is already there)
// * if the context is no debug context but enforceDebugContext is true, KHR_debug will also get emulated
//   -> this will mean a performance loss.
// * if enforceDebugContext is false, enforceKHRdebug is true and KHR_debug is not supported, it will get emulated but no error messages will get 
//   generated -> no performance loss.
//
// Note: If KHR_debug gets simulated it will work like being part of the core spec -> it will not nessesarily get reported as an extension 
//       but the functions are save to be used.
int ogl_LoadFunctionsForDebug( GLboolean enforceKHRdebug, GLboolean enforceDebugContext )
{
	int numFailed = 0;
	ClearExtensionVariables();
	
	_original_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_original_glGetIntegerv) return ogl_LOAD_FAILED;
	_original_glGetStringi = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glGetStringi");
	if(!_original_glGetStringi) return ogl_LOAD_FAILED;
	
	ProcExtsFromExtList();
	numFailed = Load_Version_3_0();
	
	// map externally visible OGL functions to real GL functions:
	mapOriginalGLFunctions();
	
	int isDebugContext = 0;
	int KHR_debugIsAvailable = 0;
	int major = ogl_GetMajorVersion();
	int minor = ogl_GetMinorVersion();
	// KHR_debug detection:
	if ((major == 4 && minor >= 3) || (major >= 5) || (ogl_ext_KHR_debug == 1)) {
		//printf("KHR_debug is supported\n");
		KHR_debugIsAvailable = 1;
	
		GLint v;
		glGetIntegerv( GL_CONTEXT_FLAGS, &v );
		if ((v & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
			isDebugContext = 1;
		}
	}
	
	// if there is a debug context already, ignore our simulation
	// as a debug context implies a running KHR_debug the way we tested for that
	if (isDebugContext != 1) {
		if (enforceDebugContext == 1) {
			// we have to simulate everything
			simulateDebugContext();
		} else {
			// if KHR_debug works already, ignore the flag
			if (KHR_debugIsAvailable != 1) {
				if (enforceKHRdebug) {
					// only simulate KHR_debug in a non-debug context
					simulateKHR_debug();
				}
			}
		}
	}
	
	if(numFailed == 0) {
		return ogl_LOAD_SUCCEEDED;
	} else {
		return ogl_LOAD_SUCCEEDED + numFailed;
	}
}

// parameter less loader to be compatible with the C loader from glLoadGen:
// KHR_debug extension function will always be available
int ogl_LoadFunctions() {
	return ogl_LoadFunctionsForDebug( GL_TRUE, GL_FALSE );
}


