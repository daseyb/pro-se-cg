
// include your OpenGL header here:
#include <ACGL/OpenGL/GL.hh>

// our emulator only works when used with our own GL function loader:
#if defined(ACGL_EXTENSION_LOADER_GLLOADGEN)

/*
 * Simulates KHR_debug
 *
 * Use-cases:
 * ----------
 *
 * Use this as a fallback on systems that don't implement KHR_debug for debugging only,
 * don't use this code in shipping release builds - it will slow down the application!
 * Using a debug callback instead of lots of glGetError() calls should work fine,
 * MessageControl, DebugGroups and DebugLabels are only implemented as fallbacks, in case
 * you have to rely on those features, you want to reimplement them in a more efficient way.
 *
 *
 * Known restrictions:
 * -------------------
 *
 *    Wrong behavior:
 *    ---------------
 *
 *    * Does not support multiple OpenGL contexts, all errors from all contexts are mixed.
 *      All settings (including the debug callback) are set for all contexts.
 *
 *    * glObjectLabel and glObjectPtrLabel do not check if the object to label exists and thus
 *      will not generate a GL_INVALID_VALUE.
 *
 *    * glObjectLabel can label GL_DISPLAY_LIST even in Core profiles.
 *
 *    Inefficiency:
 *    -------------
 *
 *    * Using this, the number of GL calls doubles as each call will get followed by a glGetError.
 *    * This will also force OpenGL to run synchronous which will reduce the performance!
 *    * ObjectLabels are implemented inefficiently and are not used internally. The functionality is
 *      only present to be compatible with KHR_debug.
 *    * DebugGroups and glDebugMessageControl are not efficiently implemented.
 *    * Some memory will not get released.
 *
 *    Implementation dependent limits:
 *    --------------------------------
 *
 *    * GL_MAX_DEBUG_MESSAGE_LENGTH and Gl_MAX_LABEL_LENGTH are arbitrary and can be changed.
 *    * GL_MAX_DEBUG_GROUP_STACK_DEPTH is set to the lowest allowed value of 64 but can be changed
 *    * GL_DEBUG_LOGGED_MESSAGES is set to 1 - increasing this will be more work.
 *
 *    * This implementation always behaves synchronous, even if GL_DEBUG_OUTPUT_SYNCHRONOUS is
 *      disabled (the default btw.). This is legal by the spec.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// global variables and implementation dependent limits:
///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// GL state:
GLDEBUGPROC KHR_DEBUG_EMULATOR_callback      = NULL;
void *      KHR_DEBUG_EMULATOR_userParam     = NULL;
int         KHR_DEBUG_EMULATOR_isEnabled     = 1;
int         KHR_DEBUG_EMULATOR_isSynchronous = 0;
GLenum      KHR_DEBUG_EMULATOR_lastGLError   = GL_NO_ERROR;

// limits:
const int   KHR_DEBUG_EMULATOR_MAX_DEBUG_MESSAGE_LENGTH    = 256;
const int   KHR_DEBUG_EMULATOR_MAX_DEBUG_LOGGED_MESSAGES   = 1;
#define     KHR_DEBUG_EMULATOR_MAX_DEBUG_GROUP_STACK_DEPTH   64
const int   KHR_DEBUG_EMULATOR_MAX_LABEL_LENGTH            = 256;


// extern declarations of internal original OpenGL calls (unwrapped)
extern GLenum    (CODEGEN_FUNCPTR *_original_glGetError)();
extern GLvoid    (CODEGEN_FUNCPTR *_original_glEnable)( GLenum );
extern GLvoid    (CODEGEN_FUNCPTR *_original_glDisable)( GLenum );
extern GLboolean (CODEGEN_FUNCPTR *_original_glIsEnabled)( GLenum );
extern GLvoid    (CODEGEN_FUNCPTR *_original_glGetIntegerv)( GLenum, GLint * );
extern GLvoid    (CODEGEN_FUNCPTR *_original_glGetPointerv)( GLenum, GLvoid ** );
// not used in here, just used to check if glClear redirects to the original function ;-)
extern GLvoid(CODEGEN_FUNCPTR *_original_glClear)(GLbitfield);

// shortcut to add OpenGL errors detected by this emulation:
#define INSERT_API_ERROR( e, m ) do { KHR_DEBUG_EMULATOR_lastGLError = e; KHR_DEBUG_EMULATOR_DebugMessageInsert_internal(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, e, GL_DEBUG_SEVERITY_HIGH, -1, m ); } while(0)

//
// support to query debug messages (only one):
struct DebugMessage {
    GLenum       source;
    GLenum       type;
    GLuint       id;
    GLenum       severity;
    GLsizei      length;
    const GLchar *buf;
};
DebugMessage g_LastDebugMessage;
GLboolean    g_LastDebugMessageEmpty = true;

//
// support for debug message control
// NOTE: mixing rules for specific IDs and all IDs was probably a bad idea
typedef struct DebugMessageControlRule
{
    GLenum source;
    GLenum type;
    GLenum severity;
    GLuint id;
    GLboolean allIDs; // if set, ignore id
    GLboolean enabled;

    GLuint debugGroup;

    DebugMessageControlRule *previousRule;
    DebugMessageControlRule *nextRule;
} DebugMessageControlRule;

// yes, I'm aware that this will never get cleaned up completely :-(
DebugMessageControlRule *g_FirstDebugMessageControlRule = NULL;
DebugMessageControlRule *g_LastDebugMessageControlRule  = NULL;
GLuint g_DebugGroupNumber = 0; // 0 == default group

//
// support for ObjectLabels
// All known labels are stored in a double-linked list. Not pretty but should work as a
// compatable fallback.
typedef struct ObjectLabel {
    GLenum  identifier;
    GLuint  name;
    GLsizei length;
    GLchar  *label;

    ObjectLabel *nextLabel;
    ObjectLabel *previousLabel;
} ObjectLabel;
ObjectLabel *g_ObjectLabels = NULL;

//
// support for ObjectPtrLabels
// The same idea as for ObjectLabels.
typedef struct ObjectPtrLabel {
    const void * ptr;
    GLsizei length;
    GLchar * label;

    ObjectPtrLabel *nextLabel;
    ObjectPtrLabel *previousLabel;
} ObjectPtrLabel;
ObjectPtrLabel *g_ObjectPtrLabels = NULL;

//
// Support for DebugGroups
// There is a maximum number of groups, so just use a big array. Element 0, the default
// group is never used.
struct DebugGroupDescription {
    GLenum  source;
    GLuint  id;
    GLsizei length;
    GLchar  *message;
};
DebugGroupDescription g_DebugGroupDescriptions[ KHR_DEBUG_EMULATOR_MAX_DEBUG_GROUP_STACK_DEPTH ];

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Replacements of the extension functions:
///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////
///   glDebugMessageInsert
///////////////////////////
//
// most work is done in KHR_DEBUG_EMULATOR_DebugMessageInsert_internal, but called from the application some more strict
// checks have to be performed.
//
GLboolean isValidSeverity( GLenum e );
GLboolean isValidType( GLenum e );
void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageInsert_internal(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf);
// returns true if the message should not get ignored by the rules defined by glDebugMessageControl:
GLboolean shouldMessageGetProcessed( GLenum source, GLenum type, GLuint id, GLenum severity );

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf)
{
    if (KHR_DEBUG_EMULATOR_isEnabled == 0) return;

    // calls from the application are a bit more restricted in the types of errors they are allowed to generate:
    if ((source != GL_DEBUG_SOURCE_APPLICATION) && (source != GL_DEBUG_SOURCE_THIRD_PARTY)) {
        INSERT_API_ERROR( GL_INVALID_ENUM, "invalid enum in glDebugMessageInsert: source has to be GL_DEBUG_SOURCE_APPLICATION or GL_DEBUG_SOURCE_THIRD_PARTY" );
        return;
    }
    KHR_DEBUG_EMULATOR_DebugMessageInsert_internal( source, type, id, severity, length, buf );
}

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageInsert_internal(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf)
{
    if (KHR_DEBUG_EMULATOR_isEnabled == 0) return;

    if ( !isValidSeverity( severity ) ) {
        INSERT_API_ERROR( GL_INVALID_ENUM , "invalid enum in glDebugMessageInsert: severity is invalid" );
        return;
    }
    if ( !isValidType( type ) ) {
        INSERT_API_ERROR( GL_INVALID_ENUM , "invalid enum in glDebugMessageInsert: type is invalid" );
        return;
    }

    // length can be -1 which means that buf is 0 terminated.
    // however, the messages created should always set length to the number of chars in the message (excluding the trailing 0)
    if (length < 0) {
        length = strlen( buf );
    }

    if (length > KHR_DEBUG_EMULATOR_MAX_DEBUG_MESSAGE_LENGTH) {
        INSERT_API_ERROR( GL_INVALID_VALUE , "invalid value in glDebugMessageInsert: message is too long" );
        return;
    }

    // there might be rules inserted by glDebugMessageControl to mute this message:
    if ( !shouldMessageGetProcessed(source, type, id, severity) ) {
        return;
    }

    if (KHR_DEBUG_EMULATOR_callback) {
        KHR_DEBUG_EMULATOR_callback( source, type, id, severity, length, buf, KHR_DEBUG_EMULATOR_userParam );
    } else {
        g_LastDebugMessageEmpty = false;
        g_LastDebugMessage.source = source;
        g_LastDebugMessage.type   = type;
        g_LastDebugMessage.id     = id;
        g_LastDebugMessage.severity = severity;
        g_LastDebugMessage.length = length;
        g_LastDebugMessage.buf    = buf;
    }
}


/////////////////////////////
///   glDebugMessageCallback
/////////////////////////////
GLboolean debugContextIsSimulated();

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageCallback(GLDEBUGPROC callback, void * userParam)
{
    KHR_DEBUG_EMULATOR_callback  = callback;
    KHR_DEBUG_EMULATOR_userParam = userParam;

    if (debugContextIsSimulated()) {
        // we will never get here if the debug context is real in the first place!
        KHR_DEBUG_EMULATOR_DebugMessageInsert_internal( GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Note: Application is not running in a real debug context, debug context behaviour is only simulated in a slow way!" );
    }
}


/////////////////////////////
///   glDebugMessageControl
/////////////////////////////

// true if a given rule matches the message:
GLboolean ruleApplies( DebugMessageControlRule *rule, GLenum source, GLenum type, GLuint id, GLenum severity );
// check if the current rules allow this message:
GLboolean shouldMessageGetProcessed( GLenum source, GLenum type, GLuint id, GLenum severity );
// adds a new rule:
void addRule( DebugMessageControlRule *newRule );

void APIENTRY KHR_DEBUG_EMULATOR_DebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled)
{
    DebugMessageControlRule *rule = (DebugMessageControlRule*) malloc( sizeof(DebugMessageControlRule) );
    rule->source   = source;
    rule->type     = type;
    rule->severity = severity;
    rule->enabled  = enabled;
    rule->debugGroup = g_DebugGroupNumber;

    if (count == 0) {
        // ID-agnostic rule
        rule->allIDs = true;
        rule->id       = 0;

        addRule( rule );
    } else {
        // rules for specific IDs
        rule->allIDs = false;

        if (source == GL_DONT_CARE || type == GL_DONT_CARE || severity != GL_DONT_CARE) {
            // see KHR_debug 5.5.4
            INSERT_API_ERROR( GL_INVALID_OPERATION , "invalid operation in glDebugMessageControl: if an ID is specified, source and type have to be specified as well but severity has to be GL_DONT_CARE" );
        }

        for (int i = 0; i < count; ++i) {
            rule->id = ids[i];
            addRule( rule );
        }
    }
}

/////////////////////////////
///   glGetDebugMessageLog
/////////////////////////////
GLuint APIENTRY KHR_DEBUG_EMULATOR_GetDebugMessageLog(GLuint count, GLsizei bufsize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog)
{
    if (bufsize < 0 && messageLog != NULL) {
        INSERT_API_ERROR( GL_INVALID_VALUE , "invalid value in glGetDebugMessageLog: bufsize < 0 and messageLog != NULL" );
        return 0;
    }

    if (g_LastDebugMessageEmpty || count == 0) return 0;

    if (types)      types[0]      = g_LastDebugMessage.type;
    if (sources)    sources[0]    = g_LastDebugMessage.source;
    if (ids)        ids[0]        = g_LastDebugMessage.id;
    if (severities) severities[0] = g_LastDebugMessage.severity;
    if (lengths)    lengths[0]    = g_LastDebugMessage.length;

    // length is without the 0-termination
    if (bufsize <= g_LastDebugMessage.length) {
        // won't fit, don't return the error :-(
        // 6.1.15 of KHR_debug
        return 0;
    }

    strncpy( messageLog, g_LastDebugMessage.buf, bufsize );
    messageLog[bufsize-1] = 0;

    g_LastDebugMessageEmpty = true;
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Handling of ObjectLabels:
/// ( not very pretty, just for compatibility )
///
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
///   glObjectLabel
/////////////////////////////
///
GLboolean isValidObjectLabelNamespace( GLenum e );
ObjectLabel *findObjectLabel(GLenum identifier, GLuint name);
void insertObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label);

//
// TODO: check if name is a valid object, emit an INVALID_VALUE otherwise
//
void APIENTRY KHR_DEBUG_EMULATOR_ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label)
{
    if ( !isValidObjectLabelNamespace(identifier) ) {
        INSERT_API_ERROR( GL_INVALID_ENUM, "invalid enum in glObjectLabel" );
        return;
    }

    if (length < 0) {
        length = strlen( label );
    }

    if (length >= KHR_DEBUG_EMULATOR_MAX_LABEL_LENGTH) {
        INSERT_API_ERROR( GL_INVALID_VALUE, "invalid value in glObjectLabel: label too long" );
        return;
    }

    ObjectLabel *thelabel = findObjectLabel( identifier, name );

    if (thelabel == NULL) {
        insertObjectLabel( identifier, name, length, label );
    } else {
        // update
        if (length == 0) {
            // delete
            ObjectLabel *pre = thelabel->previousLabel;
            ObjectLabel *next = thelabel->nextLabel;

            if (pre) pre->nextLabel = next;
            if (next) next->previousLabel = pre;
            if (!pre) g_ObjectLabels = next; // this was the first label

            free( (void*) thelabel->label );
            free( (void*) thelabel );
        } else {
            // update name & length:
            if (length < 0) {
                length = strlen( label );
            }
            free( (void*) thelabel->label );
            thelabel->label      = (GLchar *) malloc( length+1 );
            strncpy( thelabel->label , label, length );
            thelabel->label[ length ] = 0;
            thelabel->length = length;
        }
    }
}

/////////////////////////////
///   glGetObjectLabel
/////////////////////////////
void APIENTRY KHR_DEBUG_EMULATOR_GetObjectLabel(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label)
{
    if ( !isValidObjectLabelNamespace(identifier) ) {
        INSERT_API_ERROR( GL_INVALID_ENUM, "invalid enum in glObjectGetLabel" );
        return;
    }

    ObjectLabel *thelabel = findObjectLabel( identifier, name );
    if (thelabel == NULL) {
        // label not found:
        label[0] = 0;
        *length = 0;
        return;
    }

    size_t size = thelabel->length + 1;
    if ((size_t)bufSize <= size) size = bufSize-1;

    if (label != NULL) {
        strncpy( label, thelabel->label, size );
        label[size] = 0;
    }
    *length = size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Handling of ObjectPtrLabels:
/// ( not very pretty, just for compatibility )
///
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
///   glObjectPtrLabel
/////////////////////////////
ObjectPtrLabel *findObjectPtrLabel(const void * ptr);
void insertObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label);

void APIENTRY KHR_DEBUG_EMULATOR_ObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label)
{
    if (length < 0) {
        length = strlen( label );
    }

    if (length >= KHR_DEBUG_EMULATOR_MAX_LABEL_LENGTH) {
        INSERT_API_ERROR( GL_INVALID_VALUE, "invalid value in glObjectPtrLabel: label too long" );
        return;
    }

    ObjectPtrLabel *thelabel = findObjectPtrLabel( ptr );

    if (thelabel == NULL) {
        insertObjectPtrLabel( ptr, length, label );
    } else {
        // update
        if (length == 0) {
            // delete
            ObjectPtrLabel *pre  = thelabel->previousLabel;
            ObjectPtrLabel *next = thelabel->nextLabel;

            if (pre) pre->nextLabel = next;
            if (next) next->previousLabel = pre;
            if (!pre) g_ObjectPtrLabels = next; // this was the first label

            free( (void*) thelabel->label );
            free( (void*) thelabel );
        } else {
            // update name & length:
            if (length < 0) {
                length = strlen( label );
            }
            free( (void*) thelabel->label );
            thelabel->label      = (GLchar *) malloc( length+1 );
            strncpy( thelabel->label , label, length );
            thelabel->label[ length ] = 0;
            thelabel->length = length;
        }
    }
}

/////////////////////////////
///   glGetObjectPtrLabel
/////////////////////////////
void APIENTRY KHR_DEBUG_EMULATOR_GetObjectPtrLabel(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label)
{
    ObjectPtrLabel *thelabel = findObjectPtrLabel( ptr );
    if (thelabel == NULL) {
        // label not found:
        label[0] = 0;
        *length = 0;
        return;
    }

    size_t size = thelabel->length + 1;
    if ((size_t)bufSize <= size) size = bufSize-1;

    if (label != NULL) {
        strncpy( label, thelabel->label, size );
        label[size] = 0;
    }
    *length = size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Handling of DebugGroups
///
///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
///   glPopDebugGroup
/////////////////////////////
void APIENTRY KHR_DEBUG_EMULATOR_PopDebugGroup()
{
    if (g_DebugGroupNumber == 0) {
        INSERT_API_ERROR( GL_STACK_UNDERFLOW, "stack underflow in glPushDebugGroup: can't pop default group" );
        return;
    }

    //
    // delete all rules from the poped group:
    while (g_LastDebugMessageControlRule && g_LastDebugMessageControlRule->debugGroup >= g_DebugGroupNumber) {

        DebugMessageControlRule *ruleToDelete = g_LastDebugMessageControlRule;

        // set new last rule:
        g_LastDebugMessageControlRule = g_LastDebugMessageControlRule->previousRule;
        if (g_LastDebugMessageControlRule != NULL) {
            g_LastDebugMessageControlRule->nextRule = NULL;
        } else {
            g_FirstDebugMessageControlRule = NULL;
        }

        // now delete the rule:
        free( (void*) ruleToDelete );
    }

    GLenum source    = g_DebugGroupDescriptions[g_DebugGroupNumber].source;
    GLuint id        = g_DebugGroupDescriptions[g_DebugGroupNumber].id;
    GLsizei length   = g_DebugGroupDescriptions[g_DebugGroupNumber].length;
    GLchar * message = g_DebugGroupDescriptions[g_DebugGroupNumber].message;

    // goodby message:
    KHR_DEBUG_EMULATOR_DebugMessageInsert_internal( source, GL_DEBUG_TYPE_POP_GROUP, id, GL_DEBUG_SEVERITY_NOTIFICATION, length, message );
    // clean up:
    free( (void*) g_DebugGroupDescriptions[g_DebugGroupNumber].message );
    g_DebugGroupDescriptions[g_DebugGroupNumber].message = NULL;

    g_DebugGroupNumber--;
}

/////////////////////////////
///   glPushDebugGroup
/////////////////////////////
void APIENTRY KHR_DEBUG_EMULATOR_PushDebugGroup(GLenum source, GLuint id, GLsizei length, const GLchar * message)
{
    if ((source != GL_DEBUG_SOURCE_APPLICATION) && (source != GL_DEBUG_SOURCE_THIRD_PARTY)) {
        INSERT_API_ERROR( GL_INVALID_ENUM, "invalid enum in glPushDebugGroup: source has to be GL_DEBUG_SOURCE_APPLICATION or GL_DEBUG_SOURCE_THIRD_PARTY" );
        return;
    }
    if (g_DebugGroupNumber >= KHR_DEBUG_EMULATOR_MAX_DEBUG_GROUP_STACK_DEPTH-1) {
        INSERT_API_ERROR( GL_STACK_OVERFLOW, "stack overflow in glPushDebugGroup: already too many groups pushed" );
        return;
    }

    KHR_DEBUG_EMULATOR_DebugMessageInsert_internal( source, GL_DEBUG_TYPE_PUSH_GROUP, id, GL_DEBUG_SEVERITY_NOTIFICATION, length, message );

    g_DebugGroupNumber++;

    if (length < 0) {
        length = strlen( message );
    }

    g_DebugGroupDescriptions[g_DebugGroupNumber].id      = id;
    g_DebugGroupDescriptions[g_DebugGroupNumber].source  = source;
    g_DebugGroupDescriptions[g_DebugGroupNumber].length  = length;
    g_DebugGroupDescriptions[g_DebugGroupNumber].message = (GLchar *) malloc( length+1 );

    strncpy( g_DebugGroupDescriptions[g_DebugGroupNumber].message, message, length );
    g_DebugGroupDescriptions[g_DebugGroupNumber].message[ length ] = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Replacements for normal GL functions that have to behave a bit differently when KHR_debug
/// is present:
///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void APIENTRY KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();

//
// glGetError
//   needs to return the last error even if this KHR_debug emulator already got the error code
//
GLenum APIENTRY KHR_DEBUG_EMULATOR_GetError() {
    // if there was an error, report it. if not report the last global error
    // which might got set by the automatic error checks
    GLenum currentError = _original_glGetError();
    if ( currentError == GL_NO_ERROR ) {
            currentError = KHR_DEBUG_EMULATOR_lastGLError;
    }
    KHR_DEBUG_EMULATOR_lastGLError = GL_NO_ERROR;
    return currentError;
}

//
// glDisable | glEnable | glIsEnabled
//   need to recognize GL_DEBUG_OUTPUT & GL_DEBUG_OUTPUT_SYNCHRONOUS
//
void APIENTRY KHR_DEBUG_EMULATOR_Disable(GLenum cap){
  if (cap == GL_DEBUG_OUTPUT) {
    KHR_DEBUG_EMULATOR_isEnabled = 0;
    return;
  }
  if (cap == GL_DEBUG_OUTPUT_SYNCHRONOUS) {
    KHR_DEBUG_EMULATOR_isSynchronous = 0;
    return;
  }
   _original_glDisable(cap);
   KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}

void APIENTRY KHR_DEBUG_EMULATOR_Enable(GLenum cap){
  if (cap == GL_DEBUG_OUTPUT) {
    KHR_DEBUG_EMULATOR_isEnabled = 1;
    return;
  }
  if (cap == GL_DEBUG_OUTPUT_SYNCHRONOUS) {
    KHR_DEBUG_EMULATOR_isSynchronous = 1;
    return;
  }

   _original_glEnable(cap);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}

GLboolean APIENTRY KHR_DEBUG_EMULATOR_IsEnabled(GLenum cap){
  if (cap == GL_DEBUG_OUTPUT) {
    return (KHR_DEBUG_EMULATOR_isEnabled == 1);
  }
  if (cap == GL_DEBUG_OUTPUT_SYNCHRONOUS) {
    return (KHR_DEBUG_EMULATOR_isSynchronous == 1);
  }

   GLboolean returnValue = _original_glIsEnabled(cap);
  KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  return returnValue;
}

//
// glGetIntegerv
//   needs to recognize a few new tokens
//
void APIENTRY KHR_DEBUG_EMULATOR_GetIntegerv(GLenum pname, GLint * params){
    if (pname == GL_CONTEXT_FLAGS) {
        _original_glGetIntegerv(pname, params);

        if (debugContextIsSimulated()) {
            // debug context simulation
            *params |= GL_CONTEXT_FLAG_DEBUG_BIT; // we make this a debug context ;-)
        }
    } else if (pname == GL_MAX_DEBUG_MESSAGE_LENGTH) {
        *params = KHR_DEBUG_EMULATOR_MAX_DEBUG_MESSAGE_LENGTH;
    } else if (pname == GL_MAX_DEBUG_LOGGED_MESSAGES) {
        *params = KHR_DEBUG_EMULATOR_MAX_DEBUG_LOGGED_MESSAGES;
    } else if (pname == GL_MAX_DEBUG_GROUP_STACK_DEPTH) {
        *params = KHR_DEBUG_EMULATOR_MAX_DEBUG_GROUP_STACK_DEPTH;
    } else if (pname == GL_MAX_LABEL_LENGTH) {
        *params = KHR_DEBUG_EMULATOR_MAX_LABEL_LENGTH;
    } else if (pname == GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH) {
        if (g_LastDebugMessageEmpty) {
            *params = 0;
        } else {
            *params = g_LastDebugMessage.length;
        }
    } else {
        _original_glGetIntegerv(pname, params);
    }
    KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
}

//
// glGetPointerv
//   needs to recognize GL_DEBUG_CALLBACK_FUNCTION & GL_DEBUG_CALLBACK_USER_PARAM
//
void APIENTRY KHR_DEBUG_EMULATOR_GetPointerv( GLenum pname, GLvoid ** params ){

  if (pname == GL_DEBUG_CALLBACK_FUNCTION) {
    *params = (GLvoid*) KHR_DEBUG_EMULATOR_callback;
  } else if (pname == GL_DEBUG_CALLBACK_USER_PARAM) {
    *params = (GLvoid*) KHR_DEBUG_EMULATOR_userParam;
  } else {
    _original_glGetPointerv( pname, params );
    KHR_DEBUG_EMULATOR_CHECK_GL_ERROR();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Generating errors:
///
///////////////////////////////////////////////////////////////////////////////////////////////////

const char* APIENTRY KHR_DEBUG_EMULATOR_GET_ERROR_STRING( GLenum _errorCode )
{
    if      (_errorCode == GL_INVALID_ENUM)                  { return (char*) "OpenGL error: GL_INVALID_ENUM"; }
    else if (_errorCode == GL_INVALID_VALUE)                 { return (char*) "OpenGL error: GL_INVALID_VALUE"; }
    else if (_errorCode == GL_INVALID_OPERATION)             { return (char*) "OpenGL error: GL_INVALID_OPERATION"; }
    else if (_errorCode == GL_INVALID_FRAMEBUFFER_OPERATION) { return (char*) "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION"; }
    else if (_errorCode == GL_OUT_OF_MEMORY)                 { return (char*) "OpenGL error: GL_OUT_OF_MEMORY"; }
    else if (_errorCode == GL_NO_ERROR)                      { return (char*) "OpenGL error: GL_NO_ERROR"; }
    else if (_errorCode == GL_STACK_UNDERFLOW)               { return (char*) "OpenGL error: GL_STACK_UNDERFLOW"; }
    else if (_errorCode == GL_STACK_OVERFLOW)                { return (char*) "OpenGL error: GL_STACK_OVERFLOW"; }
    else {
        return (char*) "Unknown OpenGL error";
    }
}

// internal error check that gets triggered after every GL call
// * check for errors, if there was one, trigger a debug message but store the error code to fake the original glGetError behavior
void APIENTRY KHR_DEBUG_EMULATOR_CHECK_GL_ERROR() {
    //printf("check error\n");
    GLenum currentError = _original_glGetError();
    if ( currentError != GL_NO_ERROR ) {
        KHR_DEBUG_EMULATOR_lastGLError = currentError;
        KHR_DEBUG_EMULATOR_DebugMessageInsert_internal( GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, currentError, GL_DEBUG_SEVERITY_HIGH, -1, KHR_DEBUG_EMULATOR_GET_ERROR_STRING( currentError ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Other helping functions:
///
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GLboolean debugContextIsSimulated()
{
    // Test if we simulate a debug context:
    // if the externally visible glClear is also the original function, we don't
    // simulate a debug context. Note that if they are the same, it doesn't mean there
    // is a real debug context, just that we don't mess with the original context!
    return (_original_glClear != glClear);
}

GLboolean isValidSeverity( GLenum e )
{
    if ((e == GL_DEBUG_SEVERITY_HIGH) || (e == GL_DEBUG_SEVERITY_MEDIUM) || (e == GL_DEBUG_SEVERITY_LOW) || (e == GL_DEBUG_SEVERITY_NOTIFICATION)) return true;
    return false;
}

GLboolean isValidType( GLenum e )
{
    if (  (e == GL_DEBUG_TYPE_ERROR)       || (e == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) || (e == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
       || (e == GL_DEBUG_TYPE_PERFORMANCE) || (e == GL_DEBUG_TYPE_PORTABILITY)         || (e == GL_DEBUG_TYPE_OTHER)
       || (e == GL_DEBUG_TYPE_MARKER)      || (e == GL_DEBUG_TYPE_PUSH_GROUP)          || (e == GL_DEBUG_TYPE_POP_GROUP)) return true;
    return false;
}

GLboolean isValidSource( GLenum e )
{
    if (  (e == GL_DEBUG_SOURCE_API)         || (e == GL_DEBUG_SOURCE_SHADER_COMPILER) || (e == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
       || (e == GL_DEBUG_SOURCE_THIRD_PARTY) || (e == GL_DEBUG_SOURCE_APPLICATION)     || (e == GL_DEBUG_SOURCE_OTHER)) return true;
    return false;
}

GLboolean ruleApplies( DebugMessageControlRule *rule, GLenum source, GLenum type, GLuint id, GLenum severity )
{
    if ( ( rule->allIDs   != true )         && ( rule->id       != id       ) ) return false; // ID mismatch
    if ( ( rule->source   != GL_DONT_CARE ) && ( rule->source   != source   ) ) return false; // source mismatch
    if ( ( rule->type     != GL_DONT_CARE ) && ( rule->type     != type     ) ) return false; // type mismatch
    if ( ( rule->severity != GL_DONT_CARE ) && ( rule->severity != severity ) ) return false; // severity mismatch

    return true;
}

GLboolean shouldMessageGetProcessed( GLenum source, GLenum type, GLuint id, GLenum severity )
{
    // check from the newest to the oldest rule,
    // first one to be applyable to this message defines if it gets processed:

    DebugMessageControlRule *ruleToCheck = g_LastDebugMessageControlRule;

    while (ruleToCheck != NULL) {
        if ( ruleApplies( ruleToCheck, source, type, id, severity) ) {
            return ruleToCheck->enabled;
        }
        ruleToCheck = ruleToCheck->previousRule;
    }

    // no matching rule found, apply default behavior:
    if (severity == GL_DEBUG_SEVERITY_LOW) {
        return false;
    }
    return true;
}

void addRule( DebugMessageControlRule *newRule )
{
    newRule->nextRule     = NULL;
    newRule->previousRule = NULL;

    if (g_FirstDebugMessageControlRule == NULL) {
        // first rule to insert:
        g_FirstDebugMessageControlRule = newRule;
        g_LastDebugMessageControlRule  = newRule;
        return;
    }

    g_LastDebugMessageControlRule->nextRule = newRule;
    newRule->previousRule = g_LastDebugMessageControlRule;
    g_LastDebugMessageControlRule = newRule;
}


#ifndef GL_TRANSFORM_FEEDBACK
#define GL_TRANSFORM_FEEDBACK 0x8E22
#endif

#ifndef GL_DISPLAY_LIST
#define GL_DISPLAY_LIST 0x82E7
#endif

GLboolean isValidObjectLabelNamespace( GLenum e )
{
    if (e == GL_BUFFER || e == GL_SHADER || e == GL_PROGRAM || e == GL_VERTEX_ARRAY
            || e == GL_QUERY || e == GL_PROGRAM_PIPELINE || e == GL_TRANSFORM_FEEDBACK
            || e == GL_SAMPLER || e == GL_TEXTURE || e == GL_RENDERBUFFER || e == GL_FRAMEBUFFER
            || e == GL_DISPLAY_LIST ) {
        return true;
    }

    return false;
}

ObjectLabel *findObjectLabel(GLenum identifier, GLuint name)
{
    ObjectLabel *l = g_ObjectLabels;

    while ( l != NULL ) {

        if (l->identifier == identifier && l->name == name) return l;

        l = l->nextLabel;
    }

    // none found:
    return NULL;
}

void insertObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar * label)
{
    ObjectLabel *l = (ObjectLabel *) malloc( sizeof(ObjectLabel) );

    l->identifier = identifier;
    l->name       = name;
    l->length     = length;
    l->label      = (GLchar *) malloc( length+1 );
    l->nextLabel  = NULL;
    l->previousLabel = NULL;

    strncpy( l->label , label, length );
    l->label[ length ] = 0;

    // find last label
    ObjectLabel *last = g_ObjectLabels;

    if (last == NULL) {
        // first label:
        g_ObjectLabels = l;
        return;
    }

    while ( last->nextLabel != NULL ) { last = last->nextLabel; }

    last->nextLabel = l;
    l->previousLabel = last;
}

ObjectPtrLabel *findObjectPtrLabel(const void * ptr)
{
    ObjectPtrLabel *l = g_ObjectPtrLabels;

    while ( l != NULL ) {

        if (l->ptr == ptr) return l;

        l = l->nextLabel;
    }

    // none found:
    return NULL;
}

void insertObjectPtrLabel(const void * ptr, GLsizei length, const GLchar * label)
{
    ObjectPtrLabel *l = (ObjectPtrLabel *) malloc( sizeof(ObjectPtrLabel) );

    l->ptr        = ptr;
    l->length     = length;
    l->label      = (GLchar *) malloc( length+1 );
    l->nextLabel  = NULL;
    l->previousLabel = NULL;

    strncpy( l->label , label, length );
    l->label[ length ] = 0;

    // find last label
    ObjectPtrLabel *last = g_ObjectPtrLabels;

    if (last == NULL) {
        // first label:
        g_ObjectPtrLabels = l;
        return;
    }

    while ( last->nextLabel != NULL ) { last = last->nextLabel; }

    last->nextLabel = l;
    l->previousLabel = last;
}

#endif

