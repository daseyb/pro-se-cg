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
void (CODEGEN_FUNCPTR *_original_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearDepth)(GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glCullFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum) = NULL;
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
void (CODEGEN_FUNCPTR *_original_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFinish)() = NULL;
void (CODEGEN_FUNCPTR *_original_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFlush)() = NULL;
void (CODEGEN_FUNCPTR *_original_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum, GLboolean *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetDoublev)(GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetDoublev)(GLenum, GLdouble *) = NULL;
GLenum (CODEGEN_FUNCPTR *_original_glGetError)() = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glGetError)() = NULL;
void (CODEGEN_FUNCPTR *_original_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetFloatv)(GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetIntegerv)(GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetIntegerv)(GLenum, GLint *) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_original_glGetString)(GLenum) = NULL;
const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum) = NULL;
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
GLboolean (CODEGEN_FUNCPTR *_original_glIsEnabled)(GLenum) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStoref)(GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPixelStorei)(GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPointSize)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonMode)(GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadBuffer)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilFunc)(GLenum, GLint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilMask)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glStencilOp)(GLenum, GLenum, GLenum) = NULL;
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
void (CODEGEN_FUNCPTR *_original_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei) = NULL;

// Extension: 1.1
void (CODEGEN_FUNCPTR *_original_glBindTexture)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum, GLuint) = NULL;
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
void (CODEGEN_FUNCPTR *_original_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArrays)(GLenum, GLint, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElements)(GLenum, GLsizei, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenTextures)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei, GLuint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsTexture)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = NULL;

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
void (CODEGEN_FUNCPTR *_original_glSampleCoverage)(GLfloat, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleCoverage)(GLfloat, GLboolean) = NULL;

// Extension: 1.4
void (CODEGEN_FUNCPTR *_original_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = NULL;
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

// Extension: 3.1
void (CODEGEN_FUNCPTR *_original_glCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawArraysInstanced)(GLenum, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysInstanced)(GLenum, GLint, GLsizei, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElementsInstanced)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstanced)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveUniformBlockName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveUniformBlockiv)(GLuint, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockiv)(GLuint, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveUniformName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveUniformsiv)(GLuint, GLsizei, const GLuint *, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformsiv)(GLuint, GLsizei, const GLuint *, GLenum, GLint *) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGetUniformBlockIndex)(GLuint, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetUniformBlockIndex)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformIndices)(GLuint, GLsizei, const GLchar *const*, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformIndices)(GLuint, GLsizei, const GLchar *const*, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glPrimitiveRestartIndex)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPrimitiveRestartIndex)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexBuffer)(GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexBuffer)(GLenum, GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformBlockBinding)(GLuint, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformBlockBinding)(GLuint, GLuint, GLuint) = NULL;

// Extension: 3.2
GLenum (CODEGEN_FUNCPTR *_original_glClientWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;
GLenum (CODEGEN_FUNCPTR *_ptrc_glClientWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteSync)(GLsync) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteSync)(GLsync) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElementsBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElementsInstancedBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstancedBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawRangeElementsBaseVertex)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElementsBaseVertex)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint) = NULL;
GLsync (CODEGEN_FUNCPTR *_original_glFenceSync)(GLenum, GLbitfield) = NULL;
GLsync (CODEGEN_FUNCPTR *_ptrc_glFenceSync)(GLenum, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glFramebufferTexture)(GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture)(GLenum, GLenum, GLuint, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetBufferParameteri64v)(GLenum, GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteri64v)(GLenum, GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetInteger64i_v)(GLenum, GLuint, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64i_v)(GLenum, GLuint, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetInteger64v)(GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64v)(GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetMultisamplefv)(GLenum, GLuint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetMultisamplefv)(GLenum, GLuint, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetSynciv)(GLsync, GLenum, GLsizei, GLsizei *, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSynciv)(GLsync, GLenum, GLsizei, GLsizei *, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsSync)(GLsync) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSync)(GLsync) = NULL;
void (CODEGEN_FUNCPTR *_original_glMultiDrawElementsBaseVertex)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElementsBaseVertex)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glProvokingVertex)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glProvokingVertex)(GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glSampleMaski)(GLuint, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSampleMaski)(GLuint, GLbitfield) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexImage2DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage2DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glTexImage3DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glTexImage3DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean) = NULL;
void (CODEGEN_FUNCPTR *_original_glWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glWaitSync)(GLsync, GLbitfield, GLuint64) = NULL;

// Extension: 3.3
void (CODEGEN_FUNCPTR *_original_glBindFragDataLocationIndexed)(GLuint, GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocationIndexed)(GLuint, GLuint, GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindSampler)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindSampler)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteSamplers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteSamplers)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenSamplers)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenSamplers)(GLsizei, GLuint *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetFragDataIndex)(GLuint, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataIndex)(GLuint, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryObjecti64v)(GLuint, GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjecti64v)(GLuint, GLenum, GLint64 *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryObjectui64v)(GLuint, GLenum, GLuint64 *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectui64v)(GLuint, GLenum, GLuint64 *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetSamplerParameterIiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIiv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetSamplerParameterIuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIuiv)(GLuint, GLenum, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetSamplerParameterfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterfv)(GLuint, GLenum, GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetSamplerParameteriv)(GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameteriv)(GLuint, GLenum, GLint *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsSampler)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSampler)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glQueryCounter)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glQueryCounter)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameterIiv)(GLuint, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIiv)(GLuint, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameterIuiv)(GLuint, GLenum, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIuiv)(GLuint, GLenum, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameterf)(GLuint, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterf)(GLuint, GLenum, GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameterfv)(GLuint, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterfv)(GLuint, GLenum, const GLfloat *) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameteri)(GLuint, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteri)(GLuint, GLenum, GLint) = NULL;
void (CODEGEN_FUNCPTR *_original_glSamplerParameteriv)(GLuint, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteriv)(GLuint, GLenum, const GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribDivisor)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribDivisor)(GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP1ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP1uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP2ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP2uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP3ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP3uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP4ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4ui)(GLuint, GLenum, GLboolean, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribP4uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4uiv)(GLuint, GLenum, GLboolean, const GLuint *) = NULL;

// Extension: 4.0
void (CODEGEN_FUNCPTR *_original_glBeginQueryIndexed)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBeginQueryIndexed)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBindTransformFeedback)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBindTransformFeedback)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendEquationSeparatei)(GLuint, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparatei)(GLuint, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendEquationi)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationi)(GLuint, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendFuncSeparatei)(GLuint, GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparatei)(GLuint, GLenum, GLenum, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glBlendFunci)(GLuint, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glBlendFunci)(GLuint, GLenum, GLenum) = NULL;
void (CODEGEN_FUNCPTR *_original_glDeleteTransformFeedbacks)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDeleteTransformFeedbacks)(GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawArraysIndirect)(GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysIndirect)(GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawElementsIndirect)(GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsIndirect)(GLenum, GLenum, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawTransformFeedback)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedback)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glDrawTransformFeedbackStream)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedbackStream)(GLenum, GLuint, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glEndQueryIndexed)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glEndQueryIndexed)(GLenum, GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glGenTransformFeedbacks)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGenTransformFeedbacks)(GLsizei, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveSubroutineName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveSubroutineUniformName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetActiveSubroutineUniformiv)(GLuint, GLenum, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformiv)(GLuint, GLenum, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetProgramStageiv)(GLuint, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetProgramStageiv)(GLuint, GLenum, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetQueryIndexediv)(GLenum, GLuint, GLenum, GLint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetQueryIndexediv)(GLenum, GLuint, GLenum, GLint *) = NULL;
GLuint (CODEGEN_FUNCPTR *_original_glGetSubroutineIndex)(GLuint, GLenum, const GLchar *) = NULL;
GLuint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineIndex)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_original_glGetSubroutineUniformLocation)(GLuint, GLenum, const GLchar *) = NULL;
GLint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineUniformLocation)(GLuint, GLenum, const GLchar *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformSubroutineuiv)(GLenum, GLint, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformSubroutineuiv)(GLenum, GLint, GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_original_glGetUniformdv)(GLuint, GLint, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetUniformdv)(GLuint, GLint, GLdouble *) = NULL;
GLboolean (CODEGEN_FUNCPTR *_original_glIsTransformFeedback)(GLuint) = NULL;
GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTransformFeedback)(GLuint) = NULL;
void (CODEGEN_FUNCPTR *_original_glMinSampleShading)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glMinSampleShading)(GLfloat) = NULL;
void (CODEGEN_FUNCPTR *_original_glPauseTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glPauseTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_original_glResumeTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glResumeTransformFeedback)() = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1d)(GLint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1d)(GLint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform1dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform1dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2d)(GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2d)(GLint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform2dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform2dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3d)(GLint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3d)(GLint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform3dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform3dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4d)(GLint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4d)(GLint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniform4dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniform4dv)(GLint, GLsizei, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2x3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix2x4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3x2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix3x4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4x2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformMatrix4x3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3dv)(GLint, GLsizei, GLboolean, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glUniformSubroutinesuiv)(GLenum, GLsizei, const GLuint *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glUniformSubroutinesuiv)(GLenum, GLsizei, const GLuint *) = NULL;

// Extension: 4.1
void (CODEGEN_FUNCPTR *_original_glGetVertexAttribLdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribLdv)(GLuint, GLenum, GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1d)(GLuint, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2d)(GLuint, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3d)(GLuint, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribL4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4dv)(GLuint, const GLdouble *) = NULL;
void (CODEGEN_FUNCPTR *_original_glVertexAttribLPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *) = NULL;
void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *) = NULL;

static int Load_Version_4_1()
{
	int numFailed = 0;
	_original_glBlendFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glBlendFunc");
	if(!_original_glBlendFunc) numFailed++;
	_original_glClear = (void (CODEGEN_FUNCPTR *)(GLbitfield))IntGetProcAddress("glClear");
	if(!_original_glClear) numFailed++;
	_original_glClearColor = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glClearColor");
	if(!_original_glClearColor) numFailed++;
	_original_glClearDepth = (void (CODEGEN_FUNCPTR *)(GLdouble))IntGetProcAddress("glClearDepth");
	if(!_original_glClearDepth) numFailed++;
	_original_glClearStencil = (void (CODEGEN_FUNCPTR *)(GLint))IntGetProcAddress("glClearStencil");
	if(!_original_glClearStencil) numFailed++;
	_original_glColorMask = (void (CODEGEN_FUNCPTR *)(GLboolean, GLboolean, GLboolean, GLboolean))IntGetProcAddress("glColorMask");
	if(!_original_glColorMask) numFailed++;
	_original_glCullFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glCullFace");
	if(!_original_glCullFace) numFailed++;
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
	_original_glEnable = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glEnable");
	if(!_original_glEnable) numFailed++;
	_original_glFinish = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFinish");
	if(!_original_glFinish) numFailed++;
	_original_glFlush = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glFlush");
	if(!_original_glFlush) numFailed++;
	_original_glFrontFace = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glFrontFace");
	if(!_original_glFrontFace) numFailed++;
	_original_glGetBooleanv = (void (CODEGEN_FUNCPTR *)(GLenum, GLboolean *))IntGetProcAddress("glGetBooleanv");
	if(!_original_glGetBooleanv) numFailed++;
	_original_glGetDoublev = (void (CODEGEN_FUNCPTR *)(GLenum, GLdouble *))IntGetProcAddress("glGetDoublev");
	if(!_original_glGetDoublev) numFailed++;
	_original_glGetError = (GLenum (CODEGEN_FUNCPTR *)())IntGetProcAddress("glGetError");
	if(!_original_glGetError) numFailed++;
	_original_glGetFloatv = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat *))IntGetProcAddress("glGetFloatv");
	if(!_original_glGetFloatv) numFailed++;
	_original_glGetIntegerv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint *))IntGetProcAddress("glGetIntegerv");
	if(!_original_glGetIntegerv) numFailed++;
	_original_glGetString = (const GLubyte * (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glGetString");
	if(!_original_glGetString) numFailed++;
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
	_original_glIsEnabled = (GLboolean (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glIsEnabled");
	if(!_original_glIsEnabled) numFailed++;
	_original_glLineWidth = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glLineWidth");
	if(!_original_glLineWidth) numFailed++;
	_original_glLogicOp = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glLogicOp");
	if(!_original_glLogicOp) numFailed++;
	_original_glPixelStoref = (void (CODEGEN_FUNCPTR *)(GLenum, GLfloat))IntGetProcAddress("glPixelStoref");
	if(!_original_glPixelStoref) numFailed++;
	_original_glPixelStorei = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPixelStorei");
	if(!_original_glPixelStorei) numFailed++;
	_original_glPointSize = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glPointSize");
	if(!_original_glPointSize) numFailed++;
	_original_glPolygonMode = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum))IntGetProcAddress("glPolygonMode");
	if(!_original_glPolygonMode) numFailed++;
	_original_glReadBuffer = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glReadBuffer");
	if(!_original_glReadBuffer) numFailed++;
	_original_glReadPixels = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *))IntGetProcAddress("glReadPixels");
	if(!_original_glReadPixels) numFailed++;
	_original_glScissor = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissor");
	if(!_original_glScissor) numFailed++;
	_original_glStencilFunc = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint))IntGetProcAddress("glStencilFunc");
	if(!_original_glStencilFunc) numFailed++;
	_original_glStencilMask = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glStencilMask");
	if(!_original_glStencilMask) numFailed++;
	_original_glStencilOp = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum))IntGetProcAddress("glStencilOp");
	if(!_original_glStencilOp) numFailed++;
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
	_original_glViewport = (void (CODEGEN_FUNCPTR *)(GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glViewport");
	if(!_original_glViewport) numFailed++;
	_original_glBindTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTexture");
	if(!_original_glBindTexture) numFailed++;
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
	_original_glDrawArrays = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei))IntGetProcAddress("glDrawArrays");
	if(!_original_glDrawArrays) numFailed++;
	_original_glDrawElements = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *))IntGetProcAddress("glDrawElements");
	if(!_original_glDrawElements) numFailed++;
	_original_glGenTextures = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTextures");
	if(!_original_glGenTextures) numFailed++;
	_original_glIsTexture = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTexture");
	if(!_original_glIsTexture) numFailed++;
	_original_glPolygonOffset = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glPolygonOffset");
	if(!_original_glPolygonOffset) numFailed++;
	_original_glTexSubImage1D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage1D");
	if(!_original_glTexSubImage1D) numFailed++;
	_original_glTexSubImage2D = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *))IntGetProcAddress("glTexSubImage2D");
	if(!_original_glTexSubImage2D) numFailed++;
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
	_original_glSampleCoverage = (void (CODEGEN_FUNCPTR *)(GLfloat, GLboolean))IntGetProcAddress("glSampleCoverage");
	if(!_original_glSampleCoverage) numFailed++;
	_original_glBlendFuncSeparate = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparate");
	if(!_original_glBlendFuncSeparate) numFailed++;
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
	_original_glCopyBufferSubData = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr))IntGetProcAddress("glCopyBufferSubData");
	if(!_original_glCopyBufferSubData) numFailed++;
	_original_glDrawArraysInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLsizei, GLsizei))IntGetProcAddress("glDrawArraysInstanced");
	if(!_original_glDrawArraysInstanced) numFailed++;
	_original_glDrawElementsInstanced = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glDrawElementsInstanced");
	if(!_original_glDrawElementsInstanced) numFailed++;
	_original_glGetActiveUniformBlockName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformBlockName");
	if(!_original_glGetActiveUniformBlockName) numFailed++;
	_original_glGetActiveUniformBlockiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformBlockiv");
	if(!_original_glGetActiveUniformBlockiv) numFailed++;
	_original_glGetActiveUniformName = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveUniformName");
	if(!_original_glGetActiveUniformName) numFailed++;
	_original_glGetActiveUniformsiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLuint *, GLenum, GLint *))IntGetProcAddress("glGetActiveUniformsiv");
	if(!_original_glGetActiveUniformsiv) numFailed++;
	_original_glGetUniformBlockIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetUniformBlockIndex");
	if(!_original_glGetUniformBlockIndex) numFailed++;
	_original_glGetUniformIndices = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLchar *const*, GLuint *))IntGetProcAddress("glGetUniformIndices");
	if(!_original_glGetUniformIndices) numFailed++;
	_original_glPrimitiveRestartIndex = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glPrimitiveRestartIndex");
	if(!_original_glPrimitiveRestartIndex) numFailed++;
	_original_glTexBuffer = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint))IntGetProcAddress("glTexBuffer");
	if(!_original_glTexBuffer) numFailed++;
	_original_glUniformBlockBinding = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint))IntGetProcAddress("glUniformBlockBinding");
	if(!_original_glUniformBlockBinding) numFailed++;
	_original_glClientWaitSync = (GLenum (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glClientWaitSync");
	if(!_original_glClientWaitSync) numFailed++;
	_original_glDeleteSync = (void (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glDeleteSync");
	if(!_original_glDeleteSync) numFailed++;
	_original_glDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLint))IntGetProcAddress("glDrawElementsBaseVertex");
	if(!_original_glDrawElementsBaseVertex) numFailed++;
	_original_glDrawElementsInstancedBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint))IntGetProcAddress("glDrawElementsInstancedBaseVertex");
	if(!_original_glDrawElementsInstancedBaseVertex) numFailed++;
	_original_glDrawRangeElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint))IntGetProcAddress("glDrawRangeElementsBaseVertex");
	if(!_original_glDrawRangeElementsBaseVertex) numFailed++;
	_original_glFenceSync = (GLsync (CODEGEN_FUNCPTR *)(GLenum, GLbitfield))IntGetProcAddress("glFenceSync");
	if(!_original_glFenceSync) numFailed++;
	_original_glFramebufferTexture = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLuint, GLint))IntGetProcAddress("glFramebufferTexture");
	if(!_original_glFramebufferTexture) numFailed++;
	_original_glGetBufferParameteri64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint64 *))IntGetProcAddress("glGetBufferParameteri64v");
	if(!_original_glGetBufferParameteri64v) numFailed++;
	_original_glGetInteger64i_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLint64 *))IntGetProcAddress("glGetInteger64i_v");
	if(!_original_glGetInteger64i_v) numFailed++;
	_original_glGetInteger64v = (void (CODEGEN_FUNCPTR *)(GLenum, GLint64 *))IntGetProcAddress("glGetInteger64v");
	if(!_original_glGetInteger64v) numFailed++;
	_original_glGetMultisamplefv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetMultisamplefv");
	if(!_original_glGetMultisamplefv) numFailed++;
	_original_glGetSynciv = (void (CODEGEN_FUNCPTR *)(GLsync, GLenum, GLsizei, GLsizei *, GLint *))IntGetProcAddress("glGetSynciv");
	if(!_original_glGetSynciv) numFailed++;
	_original_glIsSync = (GLboolean (CODEGEN_FUNCPTR *)(GLsync))IntGetProcAddress("glIsSync");
	if(!_original_glIsSync) numFailed++;
	_original_glMultiDrawElementsBaseVertex = (void (CODEGEN_FUNCPTR *)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *))IntGetProcAddress("glMultiDrawElementsBaseVertex");
	if(!_original_glMultiDrawElementsBaseVertex) numFailed++;
	_original_glProvokingVertex = (void (CODEGEN_FUNCPTR *)(GLenum))IntGetProcAddress("glProvokingVertex");
	if(!_original_glProvokingVertex) numFailed++;
	_original_glSampleMaski = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield))IntGetProcAddress("glSampleMaski");
	if(!_original_glSampleMaski) numFailed++;
	_original_glTexImage2DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage2DMultisample");
	if(!_original_glTexImage2DMultisample) numFailed++;
	_original_glTexImage3DMultisample = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean))IntGetProcAddress("glTexImage3DMultisample");
	if(!_original_glTexImage3DMultisample) numFailed++;
	_original_glWaitSync = (void (CODEGEN_FUNCPTR *)(GLsync, GLbitfield, GLuint64))IntGetProcAddress("glWaitSync");
	if(!_original_glWaitSync) numFailed++;
	_original_glBindFragDataLocationIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint, GLuint, const GLchar *))IntGetProcAddress("glBindFragDataLocationIndexed");
	if(!_original_glBindFragDataLocationIndexed) numFailed++;
	_original_glBindSampler = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glBindSampler");
	if(!_original_glBindSampler) numFailed++;
	_original_glDeleteSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteSamplers");
	if(!_original_glDeleteSamplers) numFailed++;
	_original_glGenSamplers = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenSamplers");
	if(!_original_glGenSamplers) numFailed++;
	_original_glGetFragDataIndex = (GLint (CODEGEN_FUNCPTR *)(GLuint, const GLchar *))IntGetProcAddress("glGetFragDataIndex");
	if(!_original_glGetFragDataIndex) numFailed++;
	_original_glGetQueryObjecti64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint64 *))IntGetProcAddress("glGetQueryObjecti64v");
	if(!_original_glGetQueryObjecti64v) numFailed++;
	_original_glGetQueryObjectui64v = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint64 *))IntGetProcAddress("glGetQueryObjectui64v");
	if(!_original_glGetQueryObjectui64v) numFailed++;
	_original_glGetSamplerParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetSamplerParameterIiv");
	if(!_original_glGetSamplerParameterIiv) numFailed++;
	_original_glGetSamplerParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint *))IntGetProcAddress("glGetSamplerParameterIuiv");
	if(!_original_glGetSamplerParameterIuiv) numFailed++;
	_original_glGetSamplerParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat *))IntGetProcAddress("glGetSamplerParameterfv");
	if(!_original_glGetSamplerParameterfv) numFailed++;
	_original_glGetSamplerParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetSamplerParameteriv");
	if(!_original_glGetSamplerParameteriv) numFailed++;
	_original_glIsSampler = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsSampler");
	if(!_original_glIsSampler) numFailed++;
	_original_glQueryCounter = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glQueryCounter");
	if(!_original_glQueryCounter) numFailed++;
	_original_glSamplerParameterIiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glSamplerParameterIiv");
	if(!_original_glSamplerParameterIiv) numFailed++;
	_original_glSamplerParameterIuiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLuint *))IntGetProcAddress("glSamplerParameterIuiv");
	if(!_original_glSamplerParameterIuiv) numFailed++;
	_original_glSamplerParameterf = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLfloat))IntGetProcAddress("glSamplerParameterf");
	if(!_original_glSamplerParameterf) numFailed++;
	_original_glSamplerParameterfv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLfloat *))IntGetProcAddress("glSamplerParameterfv");
	if(!_original_glSamplerParameterfv) numFailed++;
	_original_glSamplerParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glSamplerParameteri");
	if(!_original_glSamplerParameteri) numFailed++;
	_original_glSamplerParameteriv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLint *))IntGetProcAddress("glSamplerParameteriv");
	if(!_original_glSamplerParameteriv) numFailed++;
	_original_glVertexAttribDivisor = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glVertexAttribDivisor");
	if(!_original_glVertexAttribDivisor) numFailed++;
	_original_glVertexAttribP1ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP1ui");
	if(!_original_glVertexAttribP1ui) numFailed++;
	_original_glVertexAttribP1uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP1uiv");
	if(!_original_glVertexAttribP1uiv) numFailed++;
	_original_glVertexAttribP2ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP2ui");
	if(!_original_glVertexAttribP2ui) numFailed++;
	_original_glVertexAttribP2uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP2uiv");
	if(!_original_glVertexAttribP2uiv) numFailed++;
	_original_glVertexAttribP3ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP3ui");
	if(!_original_glVertexAttribP3ui) numFailed++;
	_original_glVertexAttribP3uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP3uiv");
	if(!_original_glVertexAttribP3uiv) numFailed++;
	_original_glVertexAttribP4ui = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, GLuint))IntGetProcAddress("glVertexAttribP4ui");
	if(!_original_glVertexAttribP4ui) numFailed++;
	_original_glVertexAttribP4uiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLboolean, const GLuint *))IntGetProcAddress("glVertexAttribP4uiv");
	if(!_original_glVertexAttribP4uiv) numFailed++;
	_original_glBeginQueryIndexed = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glBeginQueryIndexed");
	if(!_original_glBeginQueryIndexed) numFailed++;
	_original_glBindTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glBindTransformFeedback");
	if(!_original_glBindTransformFeedback) numFailed++;
	_original_glBlendEquationSeparatei = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum))IntGetProcAddress("glBlendEquationSeparatei");
	if(!_original_glBlendEquationSeparatei) numFailed++;
	_original_glBlendEquationi = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum))IntGetProcAddress("glBlendEquationi");
	if(!_original_glBlendEquationi) numFailed++;
	_original_glBlendFuncSeparatei = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLenum, GLenum))IntGetProcAddress("glBlendFuncSeparatei");
	if(!_original_glBlendFuncSeparatei) numFailed++;
	_original_glBlendFunci = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum))IntGetProcAddress("glBlendFunci");
	if(!_original_glBlendFunci) numFailed++;
	_original_glDeleteTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteTransformFeedbacks");
	if(!_original_glDeleteTransformFeedbacks) numFailed++;
	_original_glDrawArraysIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, const GLvoid *))IntGetProcAddress("glDrawArraysIndirect");
	if(!_original_glDrawArraysIndirect) numFailed++;
	_original_glDrawElementsIndirect = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, const GLvoid *))IntGetProcAddress("glDrawElementsIndirect");
	if(!_original_glDrawElementsIndirect) numFailed++;
	_original_glDrawTransformFeedback = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glDrawTransformFeedback");
	if(!_original_glDrawTransformFeedback) numFailed++;
	_original_glDrawTransformFeedbackStream = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLuint))IntGetProcAddress("glDrawTransformFeedbackStream");
	if(!_original_glDrawTransformFeedbackStream) numFailed++;
	_original_glEndQueryIndexed = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint))IntGetProcAddress("glEndQueryIndexed");
	if(!_original_glEndQueryIndexed) numFailed++;
	_original_glGenTransformFeedbacks = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenTransformFeedbacks");
	if(!_original_glGenTransformFeedbacks) numFailed++;
	_original_glGetActiveSubroutineName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveSubroutineName");
	if(!_original_glGetActiveSubroutineName) numFailed++;
	_original_glGetActiveSubroutineUniformName = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetActiveSubroutineUniformName");
	if(!_original_glGetActiveSubroutineUniformName) numFailed++;
	_original_glGetActiveSubroutineUniformiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLuint, GLenum, GLint *))IntGetProcAddress("glGetActiveSubroutineUniformiv");
	if(!_original_glGetActiveSubroutineUniformiv) numFailed++;
	_original_glGetProgramStageiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLenum, GLint *))IntGetProcAddress("glGetProgramStageiv");
	if(!_original_glGetProgramStageiv) numFailed++;
	_original_glGetQueryIndexediv = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLenum, GLint *))IntGetProcAddress("glGetQueryIndexediv");
	if(!_original_glGetQueryIndexediv) numFailed++;
	_original_glGetSubroutineIndex = (GLuint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetSubroutineIndex");
	if(!_original_glGetSubroutineIndex) numFailed++;
	_original_glGetSubroutineUniformLocation = (GLint (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLchar *))IntGetProcAddress("glGetSubroutineUniformLocation");
	if(!_original_glGetSubroutineUniformLocation) numFailed++;
	_original_glGetUniformSubroutineuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLint, GLuint *))IntGetProcAddress("glGetUniformSubroutineuiv");
	if(!_original_glGetUniformSubroutineuiv) numFailed++;
	_original_glGetUniformdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLdouble *))IntGetProcAddress("glGetUniformdv");
	if(!_original_glGetUniformdv) numFailed++;
	_original_glIsTransformFeedback = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsTransformFeedback");
	if(!_original_glIsTransformFeedback) numFailed++;
	_original_glMinSampleShading = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glMinSampleShading");
	if(!_original_glMinSampleShading) numFailed++;
	_original_glPatchParameterfv = (void (CODEGEN_FUNCPTR *)(GLenum, const GLfloat *))IntGetProcAddress("glPatchParameterfv");
	if(!_original_glPatchParameterfv) numFailed++;
	_original_glPatchParameteri = (void (CODEGEN_FUNCPTR *)(GLenum, GLint))IntGetProcAddress("glPatchParameteri");
	if(!_original_glPatchParameteri) numFailed++;
	_original_glPauseTransformFeedback = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glPauseTransformFeedback");
	if(!_original_glPauseTransformFeedback) numFailed++;
	_original_glResumeTransformFeedback = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glResumeTransformFeedback");
	if(!_original_glResumeTransformFeedback) numFailed++;
	_original_glUniform1d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble))IntGetProcAddress("glUniform1d");
	if(!_original_glUniform1d) numFailed++;
	_original_glUniform1dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform1dv");
	if(!_original_glUniform1dv) numFailed++;
	_original_glUniform2d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble))IntGetProcAddress("glUniform2d");
	if(!_original_glUniform2d) numFailed++;
	_original_glUniform2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform2dv");
	if(!_original_glUniform2dv) numFailed++;
	_original_glUniform3d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glUniform3d");
	if(!_original_glUniform3d) numFailed++;
	_original_glUniform3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform3dv");
	if(!_original_glUniform3dv) numFailed++;
	_original_glUniform4d = (void (CODEGEN_FUNCPTR *)(GLint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glUniform4d");
	if(!_original_glUniform4d) numFailed++;
	_original_glUniform4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, const GLdouble *))IntGetProcAddress("glUniform4dv");
	if(!_original_glUniform4dv) numFailed++;
	_original_glUniformMatrix2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2dv");
	if(!_original_glUniformMatrix2dv) numFailed++;
	_original_glUniformMatrix2x3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2x3dv");
	if(!_original_glUniformMatrix2x3dv) numFailed++;
	_original_glUniformMatrix2x4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix2x4dv");
	if(!_original_glUniformMatrix2x4dv) numFailed++;
	_original_glUniformMatrix3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3dv");
	if(!_original_glUniformMatrix3dv) numFailed++;
	_original_glUniformMatrix3x2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3x2dv");
	if(!_original_glUniformMatrix3x2dv) numFailed++;
	_original_glUniformMatrix3x4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix3x4dv");
	if(!_original_glUniformMatrix3x4dv) numFailed++;
	_original_glUniformMatrix4dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4dv");
	if(!_original_glUniformMatrix4dv) numFailed++;
	_original_glUniformMatrix4x2dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4x2dv");
	if(!_original_glUniformMatrix4x2dv) numFailed++;
	_original_glUniformMatrix4x3dv = (void (CODEGEN_FUNCPTR *)(GLint, GLsizei, GLboolean, const GLdouble *))IntGetProcAddress("glUniformMatrix4x3dv");
	if(!_original_glUniformMatrix4x3dv) numFailed++;
	_original_glUniformSubroutinesuiv = (void (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLuint *))IntGetProcAddress("glUniformSubroutinesuiv");
	if(!_original_glUniformSubroutinesuiv) numFailed++;
	_original_glActiveShaderProgram = (void (CODEGEN_FUNCPTR *)(GLuint, GLuint))IntGetProcAddress("glActiveShaderProgram");
	if(!_original_glActiveShaderProgram) numFailed++;
	_original_glBindProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glBindProgramPipeline");
	if(!_original_glBindProgramPipeline) numFailed++;
	_original_glClearDepthf = (void (CODEGEN_FUNCPTR *)(GLfloat))IntGetProcAddress("glClearDepthf");
	if(!_original_glClearDepthf) numFailed++;
	_original_glCreateShaderProgramv = (GLuint (CODEGEN_FUNCPTR *)(GLenum, GLsizei, const GLchar *const*))IntGetProcAddress("glCreateShaderProgramv");
	if(!_original_glCreateShaderProgramv) numFailed++;
	_original_glDeleteProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *))IntGetProcAddress("glDeleteProgramPipelines");
	if(!_original_glDeleteProgramPipelines) numFailed++;
	_original_glDepthRangeArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLdouble *))IntGetProcAddress("glDepthRangeArrayv");
	if(!_original_glDepthRangeArrayv) numFailed++;
	_original_glDepthRangeIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glDepthRangeIndexed");
	if(!_original_glDepthRangeIndexed) numFailed++;
	_original_glDepthRangef = (void (CODEGEN_FUNCPTR *)(GLfloat, GLfloat))IntGetProcAddress("glDepthRangef");
	if(!_original_glDepthRangef) numFailed++;
	_original_glGenProgramPipelines = (void (CODEGEN_FUNCPTR *)(GLsizei, GLuint *))IntGetProcAddress("glGenProgramPipelines");
	if(!_original_glGenProgramPipelines) numFailed++;
	_original_glGetDoublei_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLdouble *))IntGetProcAddress("glGetDoublei_v");
	if(!_original_glGetDoublei_v) numFailed++;
	_original_glGetFloati_v = (void (CODEGEN_FUNCPTR *)(GLenum, GLuint, GLfloat *))IntGetProcAddress("glGetFloati_v");
	if(!_original_glGetFloati_v) numFailed++;
	_original_glGetProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLenum *, GLvoid *))IntGetProcAddress("glGetProgramBinary");
	if(!_original_glGetProgramBinary) numFailed++;
	_original_glGetProgramPipelineInfoLog = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, GLsizei *, GLchar *))IntGetProcAddress("glGetProgramPipelineInfoLog");
	if(!_original_glGetProgramPipelineInfoLog) numFailed++;
	_original_glGetProgramPipelineiv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint *))IntGetProcAddress("glGetProgramPipelineiv");
	if(!_original_glGetProgramPipelineiv) numFailed++;
	_original_glGetShaderPrecisionFormat = (void (CODEGEN_FUNCPTR *)(GLenum, GLenum, GLint *, GLint *))IntGetProcAddress("glGetShaderPrecisionFormat");
	if(!_original_glGetShaderPrecisionFormat) numFailed++;
	_original_glGetVertexAttribLdv = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLdouble *))IntGetProcAddress("glGetVertexAttribLdv");
	if(!_original_glGetVertexAttribLdv) numFailed++;
	_original_glIsProgramPipeline = (GLboolean (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glIsProgramPipeline");
	if(!_original_glIsProgramPipeline) numFailed++;
	_original_glProgramBinary = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glProgramBinary");
	if(!_original_glProgramBinary) numFailed++;
	_original_glProgramParameteri = (void (CODEGEN_FUNCPTR *)(GLuint, GLenum, GLint))IntGetProcAddress("glProgramParameteri");
	if(!_original_glProgramParameteri) numFailed++;
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
	_original_glReleaseShaderCompiler = (void (CODEGEN_FUNCPTR *)())IntGetProcAddress("glReleaseShaderCompiler");
	if(!_original_glReleaseShaderCompiler) numFailed++;
	_original_glScissorArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLint *))IntGetProcAddress("glScissorArrayv");
	if(!_original_glScissorArrayv) numFailed++;
	_original_glScissorIndexed = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLint, GLsizei, GLsizei))IntGetProcAddress("glScissorIndexed");
	if(!_original_glScissorIndexed) numFailed++;
	_original_glScissorIndexedv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLint *))IntGetProcAddress("glScissorIndexedv");
	if(!_original_glScissorIndexedv) numFailed++;
	_original_glShaderBinary = (void (CODEGEN_FUNCPTR *)(GLsizei, const GLuint *, GLenum, const GLvoid *, GLsizei))IntGetProcAddress("glShaderBinary");
	if(!_original_glShaderBinary) numFailed++;
	_original_glUseProgramStages = (void (CODEGEN_FUNCPTR *)(GLuint, GLbitfield, GLuint))IntGetProcAddress("glUseProgramStages");
	if(!_original_glUseProgramStages) numFailed++;
	_original_glValidateProgramPipeline = (void (CODEGEN_FUNCPTR *)(GLuint))IntGetProcAddress("glValidateProgramPipeline");
	if(!_original_glValidateProgramPipeline) numFailed++;
	_original_glVertexAttribL1d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble))IntGetProcAddress("glVertexAttribL1d");
	if(!_original_glVertexAttribL1d) numFailed++;
	_original_glVertexAttribL1dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL1dv");
	if(!_original_glVertexAttribL1dv) numFailed++;
	_original_glVertexAttribL2d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL2d");
	if(!_original_glVertexAttribL2d) numFailed++;
	_original_glVertexAttribL2dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL2dv");
	if(!_original_glVertexAttribL2dv) numFailed++;
	_original_glVertexAttribL3d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL3d");
	if(!_original_glVertexAttribL3d) numFailed++;
	_original_glVertexAttribL3dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL3dv");
	if(!_original_glVertexAttribL3dv) numFailed++;
	_original_glVertexAttribL4d = (void (CODEGEN_FUNCPTR *)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))IntGetProcAddress("glVertexAttribL4d");
	if(!_original_glVertexAttribL4d) numFailed++;
	_original_glVertexAttribL4dv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLdouble *))IntGetProcAddress("glVertexAttribL4dv");
	if(!_original_glVertexAttribL4dv) numFailed++;
	_original_glVertexAttribLPointer = (void (CODEGEN_FUNCPTR *)(GLuint, GLint, GLenum, GLsizei, const GLvoid *))IntGetProcAddress("glVertexAttribLPointer");
	if(!_original_glVertexAttribLPointer) numFailed++;
	_original_glViewportArrayv = (void (CODEGEN_FUNCPTR *)(GLuint, GLsizei, const GLfloat *))IntGetProcAddress("glViewportArrayv");
	if(!_original_glViewportArrayv) numFailed++;
	_original_glViewportIndexedf = (void (CODEGEN_FUNCPTR *)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))IntGetProcAddress("glViewportIndexedf");
	if(!_original_glViewportIndexedf) numFailed++;
	_original_glViewportIndexedfv = (void (CODEGEN_FUNCPTR *)(GLuint, const GLfloat *))IntGetProcAddress("glViewportIndexedfv");
	if(!_original_glViewportIndexedfv) numFailed++;
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

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendFunc)(GLenum, GLenum)
void APIENTRY wrapperglBlendFunc(GLenum sfactor, GLenum dfactor){
   _original_glBlendFunc(sfactor, dfactor); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClear)(GLbitfield)
void APIENTRY wrapperglClear(GLbitfield mask){
   _original_glClear(mask); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glClearStencil)(GLint)
void APIENTRY wrapperglClearStencil(GLint s){
   _original_glClearStencil(s); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean)
void APIENTRY wrapperglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha){
   _original_glColorMask(red, green, blue, alpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCullFace)(GLenum)
void APIENTRY wrapperglCullFace(GLenum mode){
   _original_glCullFace(mode); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEnable)(GLenum)
void APIENTRY wrapperglEnable(GLenum cap){
   _original_glEnable(cap); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFrontFace)(GLenum)
void APIENTRY wrapperglFrontFace(GLenum mode){
   _original_glFrontFace(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBooleanv)(GLenum, GLboolean *)
void APIENTRY wrapperglGetBooleanv(GLenum pname, GLboolean * params){
   _original_glGetBooleanv(pname, params); 
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


 // WRAPPER FOR const GLubyte * (CODEGEN_FUNCPTR *_ptrc_glGetString)(GLenum)
const GLubyte * APIENTRY wrapperglGetString(GLenum name){
   const GLubyte * returnValue = _original_glGetString(name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
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


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsEnabled)(GLenum)
GLboolean APIENTRY wrapperglIsEnabled(GLenum cap){
   GLboolean returnValue = _original_glIsEnabled(cap);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLineWidth)(GLfloat)
void APIENTRY wrapperglLineWidth(GLfloat width){
   _original_glLineWidth(width); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glLogicOp)(GLenum)
void APIENTRY wrapperglLogicOp(GLenum opcode){
   _original_glLogicOp(opcode); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glScissor)(GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglScissor(GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glScissor(x, y, width, height); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glViewport)(GLint, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglViewport(GLint x, GLint y, GLsizei width, GLsizei height){
   _original_glViewport(x, y, width, height); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 1.1

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindTexture)(GLenum, GLuint)
void APIENTRY wrapperglBindTexture(GLenum target, GLuint texture){
   _original_glBindTexture(target, texture); 
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


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenTextures)(GLsizei, GLuint *)
void APIENTRY wrapperglGenTextures(GLsizei n, GLuint * textures){
   _original_glGenTextures(n, textures); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTexture)(GLuint)
GLboolean APIENTRY wrapperglIsTexture(GLuint texture){
   GLboolean returnValue = _original_glIsTexture(texture);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPolygonOffset)(GLfloat, GLfloat)
void APIENTRY wrapperglPolygonOffset(GLfloat factor, GLfloat units){
   _original_glPolygonOffset(factor, units); 
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


// Extension: 3.1

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)
void APIENTRY wrapperglCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size){
   _original_glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysInstanced)(GLenum, GLint, GLsizei, GLsizei)
void APIENTRY wrapperglDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount){
   _original_glDrawArraysInstanced(mode, first, count, instancecount); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstanced)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei)
void APIENTRY wrapperglDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount){
   _original_glDrawElementsInstanced(mode, count, type, indices, instancecount); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName){
   _original_glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformBlockiv)(GLuint, GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params){
   _original_glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformName){
   _original_glGetActiveUniformName(program, uniformIndex, bufSize, length, uniformName); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveUniformsiv)(GLuint, GLsizei, const GLuint *, GLenum, GLint *)
void APIENTRY wrapperglGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params){
   _original_glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGetUniformBlockIndex)(GLuint, const GLchar *)
GLuint APIENTRY wrapperglGetUniformBlockIndex(GLuint program, const GLchar * uniformBlockName){
   GLuint returnValue = _original_glGetUniformBlockIndex(program, uniformBlockName);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformIndices)(GLuint, GLsizei, const GLchar *const*, GLuint *)
void APIENTRY wrapperglGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices){
   _original_glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPrimitiveRestartIndex)(GLuint)
void APIENTRY wrapperglPrimitiveRestartIndex(GLuint index){
   _original_glPrimitiveRestartIndex(index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexBuffer)(GLenum, GLenum, GLuint)
void APIENTRY wrapperglTexBuffer(GLenum target, GLenum internalformat, GLuint buffer){
   _original_glTexBuffer(target, internalformat, buffer); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformBlockBinding)(GLuint, GLuint, GLuint)
void APIENTRY wrapperglUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding){
   _original_glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 3.2

 // WRAPPER FOR GLenum (CODEGEN_FUNCPTR *_ptrc_glClientWaitSync)(GLsync, GLbitfield, GLuint64)
GLenum APIENTRY wrapperglClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout){
   GLenum returnValue = _original_glClientWaitSync(sync, flags, timeout);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteSync)(GLsync)
void APIENTRY wrapperglDeleteSync(GLsync sync){
   _original_glDeleteSync(sync); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLint)
void APIENTRY wrapperglDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex){
   _original_glDrawElementsBaseVertex(mode, count, type, indices, basevertex); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsInstancedBaseVertex)(GLenum, GLsizei, GLenum, const GLvoid *, GLsizei, GLint)
void APIENTRY wrapperglDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount, GLint basevertex){
   _original_glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawRangeElementsBaseVertex)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *, GLint)
void APIENTRY wrapperglDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex){
   _original_glDrawRangeElementsBaseVertex(mode, start, end, count, type, indices, basevertex); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLsync (CODEGEN_FUNCPTR *_ptrc_glFenceSync)(GLenum, GLbitfield)
GLsync APIENTRY wrapperglFenceSync(GLenum condition, GLbitfield flags){
   GLsync returnValue = _original_glFenceSync(condition, flags);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glFramebufferTexture)(GLenum, GLenum, GLuint, GLint)
void APIENTRY wrapperglFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level){
   _original_glFramebufferTexture(target, attachment, texture, level); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetBufferParameteri64v)(GLenum, GLenum, GLint64 *)
void APIENTRY wrapperglGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 * params){
   _original_glGetBufferParameteri64v(target, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64i_v)(GLenum, GLuint, GLint64 *)
void APIENTRY wrapperglGetInteger64i_v(GLenum target, GLuint index, GLint64 * data){
   _original_glGetInteger64i_v(target, index, data); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetInteger64v)(GLenum, GLint64 *)
void APIENTRY wrapperglGetInteger64v(GLenum pname, GLint64 * params){
   _original_glGetInteger64v(pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetMultisamplefv)(GLenum, GLuint, GLfloat *)
void APIENTRY wrapperglGetMultisamplefv(GLenum pname, GLuint index, GLfloat * val){
   _original_glGetMultisamplefv(pname, index, val); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetSynciv)(GLsync, GLenum, GLsizei, GLsizei *, GLint *)
void APIENTRY wrapperglGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values){
   _original_glGetSynciv(sync, pname, bufSize, length, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSync)(GLsync)
GLboolean APIENTRY wrapperglIsSync(GLsync sync){
   GLboolean returnValue = _original_glIsSync(sync);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMultiDrawElementsBaseVertex)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei, const GLint *)
void APIENTRY wrapperglMultiDrawElementsBaseVertex(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount, const GLint * basevertex){
   _original_glMultiDrawElementsBaseVertex(mode, count, type, indices, drawcount, basevertex); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glProvokingVertex)(GLenum)
void APIENTRY wrapperglProvokingVertex(GLenum mode){
   _original_glProvokingVertex(mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSampleMaski)(GLuint, GLbitfield)
void APIENTRY wrapperglSampleMaski(GLuint index, GLbitfield mask){
   _original_glSampleMaski(index, mask); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexImage2DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLboolean)
void APIENTRY wrapperglTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations){
   _original_glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glTexImage3DMultisample)(GLenum, GLsizei, GLint, GLsizei, GLsizei, GLsizei, GLboolean)
void APIENTRY wrapperglTexImage3DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations){
   _original_glTexImage3DMultisample(target, samples, internalformat, width, height, depth, fixedsamplelocations); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glWaitSync)(GLsync, GLbitfield, GLuint64)
void APIENTRY wrapperglWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout){
   _original_glWaitSync(sync, flags, timeout); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 3.3

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindFragDataLocationIndexed)(GLuint, GLuint, GLuint, const GLchar *)
void APIENTRY wrapperglBindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar * name){
   _original_glBindFragDataLocationIndexed(program, colorNumber, index, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindSampler)(GLuint, GLuint)
void APIENTRY wrapperglBindSampler(GLuint unit, GLuint sampler){
   _original_glBindSampler(unit, sampler); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteSamplers)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteSamplers(GLsizei count, const GLuint * samplers){
   _original_glDeleteSamplers(count, samplers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenSamplers)(GLsizei, GLuint *)
void APIENTRY wrapperglGenSamplers(GLsizei count, GLuint * samplers){
   _original_glGenSamplers(count, samplers); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetFragDataIndex)(GLuint, const GLchar *)
GLint APIENTRY wrapperglGetFragDataIndex(GLuint program, const GLchar * name){
   GLint returnValue = _original_glGetFragDataIndex(program, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjecti64v)(GLuint, GLenum, GLint64 *)
void APIENTRY wrapperglGetQueryObjecti64v(GLuint id, GLenum pname, GLint64 * params){
   _original_glGetQueryObjecti64v(id, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryObjectui64v)(GLuint, GLenum, GLuint64 *)
void APIENTRY wrapperglGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64 * params){
   _original_glGetQueryObjectui64v(id, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIiv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint * params){
   _original_glGetSamplerParameterIiv(sampler, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterIuiv)(GLuint, GLenum, GLuint *)
void APIENTRY wrapperglGetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint * params){
   _original_glGetSamplerParameterIuiv(sampler, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameterfv)(GLuint, GLenum, GLfloat *)
void APIENTRY wrapperglGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat * params){
   _original_glGetSamplerParameterfv(sampler, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetSamplerParameteriv)(GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint * params){
   _original_glGetSamplerParameteriv(sampler, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsSampler)(GLuint)
GLboolean APIENTRY wrapperglIsSampler(GLuint sampler){
   GLboolean returnValue = _original_glIsSampler(sampler);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glQueryCounter)(GLuint, GLenum)
void APIENTRY wrapperglQueryCounter(GLuint id, GLenum target){
   _original_glQueryCounter(id, target); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIiv)(GLuint, GLenum, const GLint *)
void APIENTRY wrapperglSamplerParameterIiv(GLuint sampler, GLenum pname, const GLint * param){
   _original_glSamplerParameterIiv(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterIuiv)(GLuint, GLenum, const GLuint *)
void APIENTRY wrapperglSamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint * param){
   _original_glSamplerParameterIuiv(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterf)(GLuint, GLenum, GLfloat)
void APIENTRY wrapperglSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param){
   _original_glSamplerParameterf(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameterfv)(GLuint, GLenum, const GLfloat *)
void APIENTRY wrapperglSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat * param){
   _original_glSamplerParameterfv(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteri)(GLuint, GLenum, GLint)
void APIENTRY wrapperglSamplerParameteri(GLuint sampler, GLenum pname, GLint param){
   _original_glSamplerParameteri(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glSamplerParameteriv)(GLuint, GLenum, const GLint *)
void APIENTRY wrapperglSamplerParameteriv(GLuint sampler, GLenum pname, const GLint * param){
   _original_glSamplerParameteriv(sampler, pname, param); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribDivisor)(GLuint, GLuint)
void APIENTRY wrapperglVertexAttribDivisor(GLuint index, GLuint divisor){
   _original_glVertexAttribDivisor(index, divisor); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1ui)(GLuint, GLenum, GLboolean, GLuint)
void APIENTRY wrapperglVertexAttribP1ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){
   _original_glVertexAttribP1ui(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP1uiv)(GLuint, GLenum, GLboolean, const GLuint *)
void APIENTRY wrapperglVertexAttribP1uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value){
   _original_glVertexAttribP1uiv(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2ui)(GLuint, GLenum, GLboolean, GLuint)
void APIENTRY wrapperglVertexAttribP2ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){
   _original_glVertexAttribP2ui(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP2uiv)(GLuint, GLenum, GLboolean, const GLuint *)
void APIENTRY wrapperglVertexAttribP2uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value){
   _original_glVertexAttribP2uiv(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3ui)(GLuint, GLenum, GLboolean, GLuint)
void APIENTRY wrapperglVertexAttribP3ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){
   _original_glVertexAttribP3ui(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP3uiv)(GLuint, GLenum, GLboolean, const GLuint *)
void APIENTRY wrapperglVertexAttribP3uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value){
   _original_glVertexAttribP3uiv(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4ui)(GLuint, GLenum, GLboolean, GLuint)
void APIENTRY wrapperglVertexAttribP4ui(GLuint index, GLenum type, GLboolean normalized, GLuint value){
   _original_glVertexAttribP4ui(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribP4uiv)(GLuint, GLenum, GLboolean, const GLuint *)
void APIENTRY wrapperglVertexAttribP4uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value){
   _original_glVertexAttribP4uiv(index, type, normalized, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 4.0

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBeginQueryIndexed)(GLenum, GLuint, GLuint)
void APIENTRY wrapperglBeginQueryIndexed(GLenum target, GLuint index, GLuint id){
   _original_glBeginQueryIndexed(target, index, id); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBindTransformFeedback)(GLenum, GLuint)
void APIENTRY wrapperglBindTransformFeedback(GLenum target, GLuint id){
   _original_glBindTransformFeedback(target, id); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationSeparatei)(GLuint, GLenum, GLenum)
void APIENTRY wrapperglBlendEquationSeparatei(GLuint buf, GLenum modeRGB, GLenum modeAlpha){
   _original_glBlendEquationSeparatei(buf, modeRGB, modeAlpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendEquationi)(GLuint, GLenum)
void APIENTRY wrapperglBlendEquationi(GLuint buf, GLenum mode){
   _original_glBlendEquationi(buf, mode); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendFuncSeparatei)(GLuint, GLenum, GLenum, GLenum, GLenum)
void APIENTRY wrapperglBlendFuncSeparatei(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha){
   _original_glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glBlendFunci)(GLuint, GLenum, GLenum)
void APIENTRY wrapperglBlendFunci(GLuint buf, GLenum src, GLenum dst){
   _original_glBlendFunci(buf, src, dst); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDeleteTransformFeedbacks)(GLsizei, const GLuint *)
void APIENTRY wrapperglDeleteTransformFeedbacks(GLsizei n, const GLuint * ids){
   _original_glDeleteTransformFeedbacks(n, ids); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawArraysIndirect)(GLenum, const GLvoid *)
void APIENTRY wrapperglDrawArraysIndirect(GLenum mode, const GLvoid * indirect){
   _original_glDrawArraysIndirect(mode, indirect); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawElementsIndirect)(GLenum, GLenum, const GLvoid *)
void APIENTRY wrapperglDrawElementsIndirect(GLenum mode, GLenum type, const GLvoid * indirect){
   _original_glDrawElementsIndirect(mode, type, indirect); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedback)(GLenum, GLuint)
void APIENTRY wrapperglDrawTransformFeedback(GLenum mode, GLuint id){
   _original_glDrawTransformFeedback(mode, id); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glDrawTransformFeedbackStream)(GLenum, GLuint, GLuint)
void APIENTRY wrapperglDrawTransformFeedbackStream(GLenum mode, GLuint id, GLuint stream){
   _original_glDrawTransformFeedbackStream(mode, id, stream); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glEndQueryIndexed)(GLenum, GLuint)
void APIENTRY wrapperglEndQueryIndexed(GLenum target, GLuint index){
   _original_glEndQueryIndexed(target, index); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGenTransformFeedbacks)(GLsizei, GLuint *)
void APIENTRY wrapperglGenTransformFeedbacks(GLsizei n, GLuint * ids){
   _original_glGenTransformFeedbacks(n, ids); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetActiveSubroutineName(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name){
   _original_glGetActiveSubroutineName(program, shadertype, index, bufsize, length, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)
void APIENTRY wrapperglGetActiveSubroutineUniformName(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name){
   _original_glGetActiveSubroutineUniformName(program, shadertype, index, bufsize, length, name); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetActiveSubroutineUniformiv)(GLuint, GLenum, GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetActiveSubroutineUniformiv(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint * values){
   _original_glGetActiveSubroutineUniformiv(program, shadertype, index, pname, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetProgramStageiv)(GLuint, GLenum, GLenum, GLint *)
void APIENTRY wrapperglGetProgramStageiv(GLuint program, GLenum shadertype, GLenum pname, GLint * values){
   _original_glGetProgramStageiv(program, shadertype, pname, values); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetQueryIndexediv)(GLenum, GLuint, GLenum, GLint *)
void APIENTRY wrapperglGetQueryIndexediv(GLenum target, GLuint index, GLenum pname, GLint * params){
   _original_glGetQueryIndexediv(target, index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLuint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineIndex)(GLuint, GLenum, const GLchar *)
GLuint APIENTRY wrapperglGetSubroutineIndex(GLuint program, GLenum shadertype, const GLchar * name){
   GLuint returnValue = _original_glGetSubroutineIndex(program, shadertype, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR GLint (CODEGEN_FUNCPTR *_ptrc_glGetSubroutineUniformLocation)(GLuint, GLenum, const GLchar *)
GLint APIENTRY wrapperglGetSubroutineUniformLocation(GLuint program, GLenum shadertype, const GLchar * name){
   GLint returnValue = _original_glGetSubroutineUniformLocation(program, shadertype, name);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformSubroutineuiv)(GLenum, GLint, GLuint *)
void APIENTRY wrapperglGetUniformSubroutineuiv(GLenum shadertype, GLint location, GLuint * params){
   _original_glGetUniformSubroutineuiv(shadertype, location, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetUniformdv)(GLuint, GLint, GLdouble *)
void APIENTRY wrapperglGetUniformdv(GLuint program, GLint location, GLdouble * params){
   _original_glGetUniformdv(program, location, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR GLboolean (CODEGEN_FUNCPTR *_ptrc_glIsTransformFeedback)(GLuint)
GLboolean APIENTRY wrapperglIsTransformFeedback(GLuint id){
   GLboolean returnValue = _original_glIsTransformFeedback(id);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glMinSampleShading)(GLfloat)
void APIENTRY wrapperglMinSampleShading(GLfloat value){
   _original_glMinSampleShading(value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glPauseTransformFeedback)()
void APIENTRY wrapperglPauseTransformFeedback(){
   _original_glPauseTransformFeedback(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glResumeTransformFeedback)()
void APIENTRY wrapperglResumeTransformFeedback(){
   _original_glResumeTransformFeedback(); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1d)(GLint, GLdouble)
void APIENTRY wrapperglUniform1d(GLint location, GLdouble x){
   _original_glUniform1d(location, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform1dv)(GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglUniform1dv(GLint location, GLsizei count, const GLdouble * value){
   _original_glUniform1dv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2d)(GLint, GLdouble, GLdouble)
void APIENTRY wrapperglUniform2d(GLint location, GLdouble x, GLdouble y){
   _original_glUniform2d(location, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform2dv)(GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglUniform2dv(GLint location, GLsizei count, const GLdouble * value){
   _original_glUniform2dv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3d)(GLint, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglUniform3d(GLint location, GLdouble x, GLdouble y, GLdouble z){
   _original_glUniform3d(location, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform3dv)(GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglUniform3dv(GLint location, GLsizei count, const GLdouble * value){
   _original_glUniform3dv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4d)(GLint, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglUniform4d(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w){
   _original_glUniform4d(location, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniform4dv)(GLint, GLsizei, const GLdouble *)
void APIENTRY wrapperglUniform4dv(GLint location, GLsizei count, const GLdouble * value){
   _original_glUniform4dv(location, count, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix2dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x3dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix2x3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix2x3dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix2x4dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix2x4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix2x4dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix3dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x2dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix3x2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix3x2dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix3x4dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix3x4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix3x4dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix4dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix4dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x2dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix4x2dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix4x2dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformMatrix4x3dv)(GLint, GLsizei, GLboolean, const GLdouble *)
void APIENTRY wrapperglUniformMatrix4x3dv(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value){
   _original_glUniformMatrix4x3dv(location, count, transpose, value); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glUniformSubroutinesuiv)(GLenum, GLsizei, const GLuint *)
void APIENTRY wrapperglUniformSubroutinesuiv(GLenum shadertype, GLsizei count, const GLuint * indices){
   _original_glUniformSubroutinesuiv(shadertype, count, indices); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


// Extension: 4.1

 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glGetVertexAttribLdv)(GLuint, GLenum, GLdouble *)
void APIENTRY wrapperglGetVertexAttribLdv(GLuint index, GLenum pname, GLdouble * params){
   _original_glGetVertexAttribLdv(index, pname, params); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1d)(GLuint, GLdouble)
void APIENTRY wrapperglVertexAttribL1d(GLuint index, GLdouble x){
   _original_glVertexAttribL1d(index, x); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL1dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttribL1dv(GLuint index, const GLdouble * v){
   _original_glVertexAttribL1dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2d)(GLuint, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttribL2d(GLuint index, GLdouble x, GLdouble y){
   _original_glVertexAttribL2d(index, x, y); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL2dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttribL2dv(GLuint index, const GLdouble * v){
   _original_glVertexAttribL2dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3d)(GLuint, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttribL3d(GLuint index, GLdouble x, GLdouble y, GLdouble z){
   _original_glVertexAttribL3d(index, x, y, z); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL3dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttribL3dv(GLuint index, const GLdouble * v){
   _original_glVertexAttribL3dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4d)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)
void APIENTRY wrapperglVertexAttribL4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){
   _original_glVertexAttribL4d(index, x, y, z, w); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribL4dv)(GLuint, const GLdouble *)
void APIENTRY wrapperglVertexAttribL4dv(GLuint index, const GLdouble * v){
   _original_glVertexAttribL4dv(index, v); 
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}


 // WRAPPER FOR void (CODEGEN_FUNCPTR *_ptrc_glVertexAttribLPointer)(GLuint, GLint, GLenum, GLsizei, const GLvoid *)
void APIENTRY wrapperglVertexAttribLPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer){
   _original_glVertexAttribLPointer(index, size, type, stride, pointer); 
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

        _ptrc_glBlendFunc = _original_glBlendFunc;
        _ptrc_glClear = _original_glClear;
        _ptrc_glClearColor = _original_glClearColor;
        _ptrc_glClearDepth = _original_glClearDepth;
        _ptrc_glClearStencil = _original_glClearStencil;
        _ptrc_glColorMask = _original_glColorMask;
        _ptrc_glCullFace = _original_glCullFace;
        _ptrc_glDepthFunc = _original_glDepthFunc;
        _ptrc_glDepthMask = _original_glDepthMask;
        _ptrc_glDepthRange = _original_glDepthRange;
        _ptrc_glDisable = _original_glDisable;
        _ptrc_glDrawBuffer = _original_glDrawBuffer;
        _ptrc_glEnable = _original_glEnable;
        _ptrc_glFinish = _original_glFinish;
        _ptrc_glFlush = _original_glFlush;
        _ptrc_glFrontFace = _original_glFrontFace;
        _ptrc_glGetBooleanv = _original_glGetBooleanv;
        _ptrc_glGetDoublev = _original_glGetDoublev;
        _ptrc_glGetError = _original_glGetError;
        _ptrc_glGetFloatv = _original_glGetFloatv;
        _ptrc_glGetIntegerv = _original_glGetIntegerv;
        _ptrc_glGetString = _original_glGetString;
        _ptrc_glGetTexImage = _original_glGetTexImage;
        _ptrc_glGetTexLevelParameterfv = _original_glGetTexLevelParameterfv;
        _ptrc_glGetTexLevelParameteriv = _original_glGetTexLevelParameteriv;
        _ptrc_glGetTexParameterfv = _original_glGetTexParameterfv;
        _ptrc_glGetTexParameteriv = _original_glGetTexParameteriv;
        _ptrc_glHint = _original_glHint;
        _ptrc_glIsEnabled = _original_glIsEnabled;
        _ptrc_glLineWidth = _original_glLineWidth;
        _ptrc_glLogicOp = _original_glLogicOp;
        _ptrc_glPixelStoref = _original_glPixelStoref;
        _ptrc_glPixelStorei = _original_glPixelStorei;
        _ptrc_glPointSize = _original_glPointSize;
        _ptrc_glPolygonMode = _original_glPolygonMode;
        _ptrc_glReadBuffer = _original_glReadBuffer;
        _ptrc_glReadPixels = _original_glReadPixels;
        _ptrc_glScissor = _original_glScissor;
        _ptrc_glStencilFunc = _original_glStencilFunc;
        _ptrc_glStencilMask = _original_glStencilMask;
        _ptrc_glStencilOp = _original_glStencilOp;
        _ptrc_glTexImage1D = _original_glTexImage1D;
        _ptrc_glTexImage2D = _original_glTexImage2D;
        _ptrc_glTexParameterf = _original_glTexParameterf;
        _ptrc_glTexParameterfv = _original_glTexParameterfv;
        _ptrc_glTexParameteri = _original_glTexParameteri;
        _ptrc_glTexParameteriv = _original_glTexParameteriv;
        _ptrc_glViewport = _original_glViewport;

        _ptrc_glBindTexture = _original_glBindTexture;
        _ptrc_glCopyTexImage1D = _original_glCopyTexImage1D;
        _ptrc_glCopyTexImage2D = _original_glCopyTexImage2D;
        _ptrc_glCopyTexSubImage1D = _original_glCopyTexSubImage1D;
        _ptrc_glCopyTexSubImage2D = _original_glCopyTexSubImage2D;
        _ptrc_glDeleteTextures = _original_glDeleteTextures;
        _ptrc_glDrawArrays = _original_glDrawArrays;
        _ptrc_glDrawElements = _original_glDrawElements;
        _ptrc_glGenTextures = _original_glGenTextures;
        _ptrc_glIsTexture = _original_glIsTexture;
        _ptrc_glPolygonOffset = _original_glPolygonOffset;
        _ptrc_glTexSubImage1D = _original_glTexSubImage1D;
        _ptrc_glTexSubImage2D = _original_glTexSubImage2D;

        _ptrc_glBlendColor = _original_glBlendColor;
        _ptrc_glBlendEquation = _original_glBlendEquation;
        _ptrc_glCopyTexSubImage3D = _original_glCopyTexSubImage3D;
        _ptrc_glDrawRangeElements = _original_glDrawRangeElements;
        _ptrc_glTexImage3D = _original_glTexImage3D;
        _ptrc_glTexSubImage3D = _original_glTexSubImage3D;

        _ptrc_glActiveTexture = _original_glActiveTexture;
        _ptrc_glCompressedTexImage1D = _original_glCompressedTexImage1D;
        _ptrc_glCompressedTexImage2D = _original_glCompressedTexImage2D;
        _ptrc_glCompressedTexImage3D = _original_glCompressedTexImage3D;
        _ptrc_glCompressedTexSubImage1D = _original_glCompressedTexSubImage1D;
        _ptrc_glCompressedTexSubImage2D = _original_glCompressedTexSubImage2D;
        _ptrc_glCompressedTexSubImage3D = _original_glCompressedTexSubImage3D;
        _ptrc_glGetCompressedTexImage = _original_glGetCompressedTexImage;
        _ptrc_glSampleCoverage = _original_glSampleCoverage;

        _ptrc_glBlendFuncSeparate = _original_glBlendFuncSeparate;
        _ptrc_glMultiDrawArrays = _original_glMultiDrawArrays;
        _ptrc_glMultiDrawElements = _original_glMultiDrawElements;
        _ptrc_glPointParameterf = _original_glPointParameterf;
        _ptrc_glPointParameterfv = _original_glPointParameterfv;
        _ptrc_glPointParameteri = _original_glPointParameteri;
        _ptrc_glPointParameteriv = _original_glPointParameteriv;

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

        _ptrc_glCopyBufferSubData = _original_glCopyBufferSubData;
        _ptrc_glDrawArraysInstanced = _original_glDrawArraysInstanced;
        _ptrc_glDrawElementsInstanced = _original_glDrawElementsInstanced;
        _ptrc_glGetActiveUniformBlockName = _original_glGetActiveUniformBlockName;
        _ptrc_glGetActiveUniformBlockiv = _original_glGetActiveUniformBlockiv;
        _ptrc_glGetActiveUniformName = _original_glGetActiveUniformName;
        _ptrc_glGetActiveUniformsiv = _original_glGetActiveUniformsiv;
        _ptrc_glGetUniformBlockIndex = _original_glGetUniformBlockIndex;
        _ptrc_glGetUniformIndices = _original_glGetUniformIndices;
        _ptrc_glPrimitiveRestartIndex = _original_glPrimitiveRestartIndex;
        _ptrc_glTexBuffer = _original_glTexBuffer;
        _ptrc_glUniformBlockBinding = _original_glUniformBlockBinding;

        _ptrc_glClientWaitSync = _original_glClientWaitSync;
        _ptrc_glDeleteSync = _original_glDeleteSync;
        _ptrc_glDrawElementsBaseVertex = _original_glDrawElementsBaseVertex;
        _ptrc_glDrawElementsInstancedBaseVertex = _original_glDrawElementsInstancedBaseVertex;
        _ptrc_glDrawRangeElementsBaseVertex = _original_glDrawRangeElementsBaseVertex;
        _ptrc_glFenceSync = _original_glFenceSync;
        _ptrc_glFramebufferTexture = _original_glFramebufferTexture;
        _ptrc_glGetBufferParameteri64v = _original_glGetBufferParameteri64v;
        _ptrc_glGetInteger64i_v = _original_glGetInteger64i_v;
        _ptrc_glGetInteger64v = _original_glGetInteger64v;
        _ptrc_glGetMultisamplefv = _original_glGetMultisamplefv;
        _ptrc_glGetSynciv = _original_glGetSynciv;
        _ptrc_glIsSync = _original_glIsSync;
        _ptrc_glMultiDrawElementsBaseVertex = _original_glMultiDrawElementsBaseVertex;
        _ptrc_glProvokingVertex = _original_glProvokingVertex;
        _ptrc_glSampleMaski = _original_glSampleMaski;
        _ptrc_glTexImage2DMultisample = _original_glTexImage2DMultisample;
        _ptrc_glTexImage3DMultisample = _original_glTexImage3DMultisample;
        _ptrc_glWaitSync = _original_glWaitSync;

        _ptrc_glBindFragDataLocationIndexed = _original_glBindFragDataLocationIndexed;
        _ptrc_glBindSampler = _original_glBindSampler;
        _ptrc_glDeleteSamplers = _original_glDeleteSamplers;
        _ptrc_glGenSamplers = _original_glGenSamplers;
        _ptrc_glGetFragDataIndex = _original_glGetFragDataIndex;
        _ptrc_glGetQueryObjecti64v = _original_glGetQueryObjecti64v;
        _ptrc_glGetQueryObjectui64v = _original_glGetQueryObjectui64v;
        _ptrc_glGetSamplerParameterIiv = _original_glGetSamplerParameterIiv;
        _ptrc_glGetSamplerParameterIuiv = _original_glGetSamplerParameterIuiv;
        _ptrc_glGetSamplerParameterfv = _original_glGetSamplerParameterfv;
        _ptrc_glGetSamplerParameteriv = _original_glGetSamplerParameteriv;
        _ptrc_glIsSampler = _original_glIsSampler;
        _ptrc_glQueryCounter = _original_glQueryCounter;
        _ptrc_glSamplerParameterIiv = _original_glSamplerParameterIiv;
        _ptrc_glSamplerParameterIuiv = _original_glSamplerParameterIuiv;
        _ptrc_glSamplerParameterf = _original_glSamplerParameterf;
        _ptrc_glSamplerParameterfv = _original_glSamplerParameterfv;
        _ptrc_glSamplerParameteri = _original_glSamplerParameteri;
        _ptrc_glSamplerParameteriv = _original_glSamplerParameteriv;
        _ptrc_glVertexAttribDivisor = _original_glVertexAttribDivisor;
        _ptrc_glVertexAttribP1ui = _original_glVertexAttribP1ui;
        _ptrc_glVertexAttribP1uiv = _original_glVertexAttribP1uiv;
        _ptrc_glVertexAttribP2ui = _original_glVertexAttribP2ui;
        _ptrc_glVertexAttribP2uiv = _original_glVertexAttribP2uiv;
        _ptrc_glVertexAttribP3ui = _original_glVertexAttribP3ui;
        _ptrc_glVertexAttribP3uiv = _original_glVertexAttribP3uiv;
        _ptrc_glVertexAttribP4ui = _original_glVertexAttribP4ui;
        _ptrc_glVertexAttribP4uiv = _original_glVertexAttribP4uiv;

        _ptrc_glBeginQueryIndexed = _original_glBeginQueryIndexed;
        _ptrc_glBindTransformFeedback = _original_glBindTransformFeedback;
        _ptrc_glBlendEquationSeparatei = _original_glBlendEquationSeparatei;
        _ptrc_glBlendEquationi = _original_glBlendEquationi;
        _ptrc_glBlendFuncSeparatei = _original_glBlendFuncSeparatei;
        _ptrc_glBlendFunci = _original_glBlendFunci;
        _ptrc_glDeleteTransformFeedbacks = _original_glDeleteTransformFeedbacks;
        _ptrc_glDrawArraysIndirect = _original_glDrawArraysIndirect;
        _ptrc_glDrawElementsIndirect = _original_glDrawElementsIndirect;
        _ptrc_glDrawTransformFeedback = _original_glDrawTransformFeedback;
        _ptrc_glDrawTransformFeedbackStream = _original_glDrawTransformFeedbackStream;
        _ptrc_glEndQueryIndexed = _original_glEndQueryIndexed;
        _ptrc_glGenTransformFeedbacks = _original_glGenTransformFeedbacks;
        _ptrc_glGetActiveSubroutineName = _original_glGetActiveSubroutineName;
        _ptrc_glGetActiveSubroutineUniformName = _original_glGetActiveSubroutineUniformName;
        _ptrc_glGetActiveSubroutineUniformiv = _original_glGetActiveSubroutineUniformiv;
        _ptrc_glGetProgramStageiv = _original_glGetProgramStageiv;
        _ptrc_glGetQueryIndexediv = _original_glGetQueryIndexediv;
        _ptrc_glGetSubroutineIndex = _original_glGetSubroutineIndex;
        _ptrc_glGetSubroutineUniformLocation = _original_glGetSubroutineUniformLocation;
        _ptrc_glGetUniformSubroutineuiv = _original_glGetUniformSubroutineuiv;
        _ptrc_glGetUniformdv = _original_glGetUniformdv;
        _ptrc_glIsTransformFeedback = _original_glIsTransformFeedback;
        _ptrc_glMinSampleShading = _original_glMinSampleShading;
        _ptrc_glPauseTransformFeedback = _original_glPauseTransformFeedback;
        _ptrc_glResumeTransformFeedback = _original_glResumeTransformFeedback;
        _ptrc_glUniform1d = _original_glUniform1d;
        _ptrc_glUniform1dv = _original_glUniform1dv;
        _ptrc_glUniform2d = _original_glUniform2d;
        _ptrc_glUniform2dv = _original_glUniform2dv;
        _ptrc_glUniform3d = _original_glUniform3d;
        _ptrc_glUniform3dv = _original_glUniform3dv;
        _ptrc_glUniform4d = _original_glUniform4d;
        _ptrc_glUniform4dv = _original_glUniform4dv;
        _ptrc_glUniformMatrix2dv = _original_glUniformMatrix2dv;
        _ptrc_glUniformMatrix2x3dv = _original_glUniformMatrix2x3dv;
        _ptrc_glUniformMatrix2x4dv = _original_glUniformMatrix2x4dv;
        _ptrc_glUniformMatrix3dv = _original_glUniformMatrix3dv;
        _ptrc_glUniformMatrix3x2dv = _original_glUniformMatrix3x2dv;
        _ptrc_glUniformMatrix3x4dv = _original_glUniformMatrix3x4dv;
        _ptrc_glUniformMatrix4dv = _original_glUniformMatrix4dv;
        _ptrc_glUniformMatrix4x2dv = _original_glUniformMatrix4x2dv;
        _ptrc_glUniformMatrix4x3dv = _original_glUniformMatrix4x3dv;
        _ptrc_glUniformSubroutinesuiv = _original_glUniformSubroutinesuiv;

        _ptrc_glGetVertexAttribLdv = _original_glGetVertexAttribLdv;
        _ptrc_glVertexAttribL1d = _original_glVertexAttribL1d;
        _ptrc_glVertexAttribL1dv = _original_glVertexAttribL1dv;
        _ptrc_glVertexAttribL2d = _original_glVertexAttribL2d;
        _ptrc_glVertexAttribL2dv = _original_glVertexAttribL2dv;
        _ptrc_glVertexAttribL3d = _original_glVertexAttribL3d;
        _ptrc_glVertexAttribL3dv = _original_glVertexAttribL3dv;
        _ptrc_glVertexAttribL4d = _original_glVertexAttribL4d;
        _ptrc_glVertexAttribL4dv = _original_glVertexAttribL4dv;
        _ptrc_glVertexAttribLPointer = _original_glVertexAttribLPointer;

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

        _ptrc_glBlendFunc = wrapperglBlendFunc;
        _ptrc_glClear = wrapperglClear;
        _ptrc_glClearColor = wrapperglClearColor;
        _ptrc_glClearDepth = wrapperglClearDepth;
        _ptrc_glClearStencil = wrapperglClearStencil;
        _ptrc_glColorMask = wrapperglColorMask;
        _ptrc_glCullFace = wrapperglCullFace;
        _ptrc_glDepthFunc = wrapperglDepthFunc;
        _ptrc_glDepthMask = wrapperglDepthMask;
        _ptrc_glDepthRange = wrapperglDepthRange;
        _ptrc_glDrawBuffer = wrapperglDrawBuffer;
        _ptrc_glFinish = wrapperglFinish;
        _ptrc_glFlush = wrapperglFlush;
        _ptrc_glFrontFace = wrapperglFrontFace;
        _ptrc_glGetBooleanv = wrapperglGetBooleanv;
        _ptrc_glGetDoublev = wrapperglGetDoublev;
        _ptrc_glGetFloatv = wrapperglGetFloatv;
        _ptrc_glGetString = wrapperglGetString;
        _ptrc_glGetTexImage = wrapperglGetTexImage;
        _ptrc_glGetTexLevelParameterfv = wrapperglGetTexLevelParameterfv;
        _ptrc_glGetTexLevelParameteriv = wrapperglGetTexLevelParameteriv;
        _ptrc_glGetTexParameterfv = wrapperglGetTexParameterfv;
        _ptrc_glGetTexParameteriv = wrapperglGetTexParameteriv;
        _ptrc_glHint = wrapperglHint;
        _ptrc_glLineWidth = wrapperglLineWidth;
        _ptrc_glLogicOp = wrapperglLogicOp;
        _ptrc_glPixelStoref = wrapperglPixelStoref;
        _ptrc_glPixelStorei = wrapperglPixelStorei;
        _ptrc_glPointSize = wrapperglPointSize;
        _ptrc_glPolygonMode = wrapperglPolygonMode;
        _ptrc_glReadBuffer = wrapperglReadBuffer;
        _ptrc_glReadPixels = wrapperglReadPixels;
        _ptrc_glScissor = wrapperglScissor;
        _ptrc_glStencilFunc = wrapperglStencilFunc;
        _ptrc_glStencilMask = wrapperglStencilMask;
        _ptrc_glStencilOp = wrapperglStencilOp;
        _ptrc_glTexImage1D = wrapperglTexImage1D;
        _ptrc_glTexImage2D = wrapperglTexImage2D;
        _ptrc_glTexParameterf = wrapperglTexParameterf;
        _ptrc_glTexParameterfv = wrapperglTexParameterfv;
        _ptrc_glTexParameteri = wrapperglTexParameteri;
        _ptrc_glTexParameteriv = wrapperglTexParameteriv;
        _ptrc_glViewport = wrapperglViewport;

        _ptrc_glBindTexture = wrapperglBindTexture;
        _ptrc_glCopyTexImage1D = wrapperglCopyTexImage1D;
        _ptrc_glCopyTexImage2D = wrapperglCopyTexImage2D;
        _ptrc_glCopyTexSubImage1D = wrapperglCopyTexSubImage1D;
        _ptrc_glCopyTexSubImage2D = wrapperglCopyTexSubImage2D;
        _ptrc_glDeleteTextures = wrapperglDeleteTextures;
        _ptrc_glDrawArrays = wrapperglDrawArrays;
        _ptrc_glDrawElements = wrapperglDrawElements;
        _ptrc_glGenTextures = wrapperglGenTextures;
        _ptrc_glIsTexture = wrapperglIsTexture;
        _ptrc_glPolygonOffset = wrapperglPolygonOffset;
        _ptrc_glTexSubImage1D = wrapperglTexSubImage1D;
        _ptrc_glTexSubImage2D = wrapperglTexSubImage2D;

        _ptrc_glBlendColor = wrapperglBlendColor;
        _ptrc_glBlendEquation = wrapperglBlendEquation;
        _ptrc_glCopyTexSubImage3D = wrapperglCopyTexSubImage3D;
        _ptrc_glDrawRangeElements = wrapperglDrawRangeElements;
        _ptrc_glTexImage3D = wrapperglTexImage3D;
        _ptrc_glTexSubImage3D = wrapperglTexSubImage3D;

        _ptrc_glActiveTexture = wrapperglActiveTexture;
        _ptrc_glCompressedTexImage1D = wrapperglCompressedTexImage1D;
        _ptrc_glCompressedTexImage2D = wrapperglCompressedTexImage2D;
        _ptrc_glCompressedTexImage3D = wrapperglCompressedTexImage3D;
        _ptrc_glCompressedTexSubImage1D = wrapperglCompressedTexSubImage1D;
        _ptrc_glCompressedTexSubImage2D = wrapperglCompressedTexSubImage2D;
        _ptrc_glCompressedTexSubImage3D = wrapperglCompressedTexSubImage3D;
        _ptrc_glGetCompressedTexImage = wrapperglGetCompressedTexImage;
        _ptrc_glSampleCoverage = wrapperglSampleCoverage;

        _ptrc_glBlendFuncSeparate = wrapperglBlendFuncSeparate;
        _ptrc_glMultiDrawArrays = wrapperglMultiDrawArrays;
        _ptrc_glMultiDrawElements = wrapperglMultiDrawElements;
        _ptrc_glPointParameterf = wrapperglPointParameterf;
        _ptrc_glPointParameterfv = wrapperglPointParameterfv;
        _ptrc_glPointParameteri = wrapperglPointParameteri;
        _ptrc_glPointParameteriv = wrapperglPointParameteriv;

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

        _ptrc_glCopyBufferSubData = wrapperglCopyBufferSubData;
        _ptrc_glDrawArraysInstanced = wrapperglDrawArraysInstanced;
        _ptrc_glDrawElementsInstanced = wrapperglDrawElementsInstanced;
        _ptrc_glGetActiveUniformBlockName = wrapperglGetActiveUniformBlockName;
        _ptrc_glGetActiveUniformBlockiv = wrapperglGetActiveUniformBlockiv;
        _ptrc_glGetActiveUniformName = wrapperglGetActiveUniformName;
        _ptrc_glGetActiveUniformsiv = wrapperglGetActiveUniformsiv;
        _ptrc_glGetUniformBlockIndex = wrapperglGetUniformBlockIndex;
        _ptrc_glGetUniformIndices = wrapperglGetUniformIndices;
        _ptrc_glPrimitiveRestartIndex = wrapperglPrimitiveRestartIndex;
        _ptrc_glTexBuffer = wrapperglTexBuffer;
        _ptrc_glUniformBlockBinding = wrapperglUniformBlockBinding;

        _ptrc_glClientWaitSync = wrapperglClientWaitSync;
        _ptrc_glDeleteSync = wrapperglDeleteSync;
        _ptrc_glDrawElementsBaseVertex = wrapperglDrawElementsBaseVertex;
        _ptrc_glDrawElementsInstancedBaseVertex = wrapperglDrawElementsInstancedBaseVertex;
        _ptrc_glDrawRangeElementsBaseVertex = wrapperglDrawRangeElementsBaseVertex;
        _ptrc_glFenceSync = wrapperglFenceSync;
        _ptrc_glFramebufferTexture = wrapperglFramebufferTexture;
        _ptrc_glGetBufferParameteri64v = wrapperglGetBufferParameteri64v;
        _ptrc_glGetInteger64i_v = wrapperglGetInteger64i_v;
        _ptrc_glGetInteger64v = wrapperglGetInteger64v;
        _ptrc_glGetMultisamplefv = wrapperglGetMultisamplefv;
        _ptrc_glGetSynciv = wrapperglGetSynciv;
        _ptrc_glIsSync = wrapperglIsSync;
        _ptrc_glMultiDrawElementsBaseVertex = wrapperglMultiDrawElementsBaseVertex;
        _ptrc_glProvokingVertex = wrapperglProvokingVertex;
        _ptrc_glSampleMaski = wrapperglSampleMaski;
        _ptrc_glTexImage2DMultisample = wrapperglTexImage2DMultisample;
        _ptrc_glTexImage3DMultisample = wrapperglTexImage3DMultisample;
        _ptrc_glWaitSync = wrapperglWaitSync;

        _ptrc_glBindFragDataLocationIndexed = wrapperglBindFragDataLocationIndexed;
        _ptrc_glBindSampler = wrapperglBindSampler;
        _ptrc_glDeleteSamplers = wrapperglDeleteSamplers;
        _ptrc_glGenSamplers = wrapperglGenSamplers;
        _ptrc_glGetFragDataIndex = wrapperglGetFragDataIndex;
        _ptrc_glGetQueryObjecti64v = wrapperglGetQueryObjecti64v;
        _ptrc_glGetQueryObjectui64v = wrapperglGetQueryObjectui64v;
        _ptrc_glGetSamplerParameterIiv = wrapperglGetSamplerParameterIiv;
        _ptrc_glGetSamplerParameterIuiv = wrapperglGetSamplerParameterIuiv;
        _ptrc_glGetSamplerParameterfv = wrapperglGetSamplerParameterfv;
        _ptrc_glGetSamplerParameteriv = wrapperglGetSamplerParameteriv;
        _ptrc_glIsSampler = wrapperglIsSampler;
        _ptrc_glQueryCounter = wrapperglQueryCounter;
        _ptrc_glSamplerParameterIiv = wrapperglSamplerParameterIiv;
        _ptrc_glSamplerParameterIuiv = wrapperglSamplerParameterIuiv;
        _ptrc_glSamplerParameterf = wrapperglSamplerParameterf;
        _ptrc_glSamplerParameterfv = wrapperglSamplerParameterfv;
        _ptrc_glSamplerParameteri = wrapperglSamplerParameteri;
        _ptrc_glSamplerParameteriv = wrapperglSamplerParameteriv;
        _ptrc_glVertexAttribDivisor = wrapperglVertexAttribDivisor;
        _ptrc_glVertexAttribP1ui = wrapperglVertexAttribP1ui;
        _ptrc_glVertexAttribP1uiv = wrapperglVertexAttribP1uiv;
        _ptrc_glVertexAttribP2ui = wrapperglVertexAttribP2ui;
        _ptrc_glVertexAttribP2uiv = wrapperglVertexAttribP2uiv;
        _ptrc_glVertexAttribP3ui = wrapperglVertexAttribP3ui;
        _ptrc_glVertexAttribP3uiv = wrapperglVertexAttribP3uiv;
        _ptrc_glVertexAttribP4ui = wrapperglVertexAttribP4ui;
        _ptrc_glVertexAttribP4uiv = wrapperglVertexAttribP4uiv;

        _ptrc_glBeginQueryIndexed = wrapperglBeginQueryIndexed;
        _ptrc_glBindTransformFeedback = wrapperglBindTransformFeedback;
        _ptrc_glBlendEquationSeparatei = wrapperglBlendEquationSeparatei;
        _ptrc_glBlendEquationi = wrapperglBlendEquationi;
        _ptrc_glBlendFuncSeparatei = wrapperglBlendFuncSeparatei;
        _ptrc_glBlendFunci = wrapperglBlendFunci;
        _ptrc_glDeleteTransformFeedbacks = wrapperglDeleteTransformFeedbacks;
        _ptrc_glDrawArraysIndirect = wrapperglDrawArraysIndirect;
        _ptrc_glDrawElementsIndirect = wrapperglDrawElementsIndirect;
        _ptrc_glDrawTransformFeedback = wrapperglDrawTransformFeedback;
        _ptrc_glDrawTransformFeedbackStream = wrapperglDrawTransformFeedbackStream;
        _ptrc_glEndQueryIndexed = wrapperglEndQueryIndexed;
        _ptrc_glGenTransformFeedbacks = wrapperglGenTransformFeedbacks;
        _ptrc_glGetActiveSubroutineName = wrapperglGetActiveSubroutineName;
        _ptrc_glGetActiveSubroutineUniformName = wrapperglGetActiveSubroutineUniformName;
        _ptrc_glGetActiveSubroutineUniformiv = wrapperglGetActiveSubroutineUniformiv;
        _ptrc_glGetProgramStageiv = wrapperglGetProgramStageiv;
        _ptrc_glGetQueryIndexediv = wrapperglGetQueryIndexediv;
        _ptrc_glGetSubroutineIndex = wrapperglGetSubroutineIndex;
        _ptrc_glGetSubroutineUniformLocation = wrapperglGetSubroutineUniformLocation;
        _ptrc_glGetUniformSubroutineuiv = wrapperglGetUniformSubroutineuiv;
        _ptrc_glGetUniformdv = wrapperglGetUniformdv;
        _ptrc_glIsTransformFeedback = wrapperglIsTransformFeedback;
        _ptrc_glMinSampleShading = wrapperglMinSampleShading;
        _ptrc_glPauseTransformFeedback = wrapperglPauseTransformFeedback;
        _ptrc_glResumeTransformFeedback = wrapperglResumeTransformFeedback;
        _ptrc_glUniform1d = wrapperglUniform1d;
        _ptrc_glUniform1dv = wrapperglUniform1dv;
        _ptrc_glUniform2d = wrapperglUniform2d;
        _ptrc_glUniform2dv = wrapperglUniform2dv;
        _ptrc_glUniform3d = wrapperglUniform3d;
        _ptrc_glUniform3dv = wrapperglUniform3dv;
        _ptrc_glUniform4d = wrapperglUniform4d;
        _ptrc_glUniform4dv = wrapperglUniform4dv;
        _ptrc_glUniformMatrix2dv = wrapperglUniformMatrix2dv;
        _ptrc_glUniformMatrix2x3dv = wrapperglUniformMatrix2x3dv;
        _ptrc_glUniformMatrix2x4dv = wrapperglUniformMatrix2x4dv;
        _ptrc_glUniformMatrix3dv = wrapperglUniformMatrix3dv;
        _ptrc_glUniformMatrix3x2dv = wrapperglUniformMatrix3x2dv;
        _ptrc_glUniformMatrix3x4dv = wrapperglUniformMatrix3x4dv;
        _ptrc_glUniformMatrix4dv = wrapperglUniformMatrix4dv;
        _ptrc_glUniformMatrix4x2dv = wrapperglUniformMatrix4x2dv;
        _ptrc_glUniformMatrix4x3dv = wrapperglUniformMatrix4x3dv;
        _ptrc_glUniformSubroutinesuiv = wrapperglUniformSubroutinesuiv;

        _ptrc_glGetVertexAttribLdv = wrapperglGetVertexAttribLdv;
        _ptrc_glVertexAttribL1d = wrapperglVertexAttribL1d;
        _ptrc_glVertexAttribL1dv = wrapperglVertexAttribL1dv;
        _ptrc_glVertexAttribL2d = wrapperglVertexAttribL2d;
        _ptrc_glVertexAttribL2dv = wrapperglVertexAttribL2dv;
        _ptrc_glVertexAttribL3d = wrapperglVertexAttribL3d;
        _ptrc_glVertexAttribL3dv = wrapperglVertexAttribL3dv;
        _ptrc_glVertexAttribL4d = wrapperglVertexAttribL4d;
        _ptrc_glVertexAttribL4dv = wrapperglVertexAttribL4dv;
        _ptrc_glVertexAttribLPointer = wrapperglVertexAttribLPointer;

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
	numFailed = Load_Version_4_1();
	
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


