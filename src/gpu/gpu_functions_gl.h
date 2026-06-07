#ifndef ENGINE_H_FUNCTIONS_GL
#define ENGINE_H_FUNCTIONS_GL

// Utility function types.
typedef void (*GLDebugProc)(u32_enum source, u32_enum type, u32 id, u32_enum severity, s32 length, const char* message, const void* userParam);

// =====================================================================================
// @: OpenGL Functions.
// =====================================================================================

// X Macro declaration for defining the OpenGL function prototypes.
#define FOR_OPEN_GL_FUNCTIONS(_DO)                                                 \
  _DO(glDebugMessageCallback, void, (GLDebugProc callback, const void* userParam)) \
  _DO(glCreateShader, u32, (u32_enum source))                                      \

// =====================================================================================

// Use X macro to generate the function pointer declarations.
#define DO_FUNCTION_DECLARATIONS(_NAME, _RETURN, _PARAMS) extern _RETURN (*_NAME) _PARAMS; 

FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_DECLARATIONS)

#undef DO_FUNCTION_DECLARATIONS 

#endif // ENGINE_H_FUNCTIONS_GL 

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPL_FUNCTIONS_GL

// Use X macro to generate the function pointers definitions.
#define DO_FUNCTION_POINTER_DEFINITIONS(_NAME, _RETURN, _PARAMS) _RETURN (*_NAME) _PARAMS = NULL;                               \

FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_POINTER_DEFINITIONS)

#undef DO_FUNCTION_POINTER_DEFINITIONS 

#endif // ENGINE_IMPL_FUNCTIONS_GL
