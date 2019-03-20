#ifndef __NJI_H__
#define __NJI_H__


#include <jni.h>
#include "nji/error.h"


#ifdef __cplusplus
extern "C" {
#endif


#define NJI_HEAD                        \
    jclass (*__class__)(void);          \
    jfieldID (*__field__)(int);         \
    jmethodID (*__constructor__)(int);  \
    jmethodID (*__method__)(int)


/* Helper to get hold of the jclass for an NJI class. It is safe to assume that
 * the class is avail method will return non-null ONLY after the NJI class is initialised (an
 * NJI class is initialised AFTER it's first successful use */
#define JCLASS(c) (c.__class__())


/* Helpers to get hold of IDs for methods and fields for an NJI class. It is
 * safe to assume that method or field has been successfully resolved if a
 * non-null value is returned */
#define JID(c, n) JID_ ## c ## _ ## n
#define JFIELDID(c, n) (c.__field__(JID(c, n)))
#define JCONSTRUCTORID(c, n) (c.__constructor__(JID(c, n)))
#define JMETHODID(c, n) (c.__method__(JID(c, n)))


/* The number of local refernces that can be attached to a frame */
#ifndef NJI_FRAMESIZE
#define NJI_FRAMESIZE 16
#endif


/* Enter a frame. Local references to jobjects created or retrieved within the
 * frame are attached to the frame */
#define NJI_ENTER PushLocalFrame(NJI_FRAMESIZE)


/* Leave a frame. All local references to jobects that are attached to the
 * frame are released */
#define NJI_LEAVE PopLocalFrame(NULL, NULL)


/* Same as NJI_LEAVE but a new local reference for the supplied jobject is
 * returned. The new local reference is attached to the 'outer' frame */
#define NJI_LEAVE_WITH(o) PopLocalFrame(&(o), o)


/* Wrappers for JNI global reference methods */
nji_error NewGlobalRef(jobject *ref, jobject obj);
nji_error DeleteGlobalRef(jobject ref);


/* Wrappers for JNI local reference methods */
nji_error NewLocalRef(jobject *ref, jobject obj);
nji_error DeleteLocalRef(jobject localRef);
nji_error PushLocalFrame(jint capacity);
nji_error PopLocalFrame(jobject *ref, jobject result);


/* Wrappers for JNI lookup method */
nji_error FindClass(jclass *clazz, const char *name);
nji_error GetFieldID(jfieldID *result, jclass clazz, const char *name, const char *sig);
nji_error GetStaticFieldID(jfieldID *result, jclass clazz, const char *name, const char *sig);
nji_error GetMethodID(jmethodID *result, jclass clazz, const char *name, const char *sig);
nji_error GetStaticMethodID(jmethodID *result, jclass clazz, const char *name, const char *sig);


/* Whether to set or get field */
typedef enum {
    FIELD_GET,
    FIELD_SET,
} field_mode;


/* Wrappers for JNI field methods */
#define _FIELD_STATIC_PROTO(T, t)   \
    nji_error GetOrSetStatic ## T ## Field(t *value, jclass obj, jfieldID field, field_mode mode)
#define _FIELD_PROTO(T, t)  \
    nji_error GetOrSet ## T ## Field(t *value, jobject obj, jfieldID field, field_mode mode)
#define FIELD_PROTO(T, t)       \
    _FIELD_STATIC_PROTO(T, t);  \
    _FIELD_PROTO(T, t);
FIELD_PROTO(Object, jobject)
FIELD_PROTO(Boolean, jboolean)
FIELD_PROTO(Byte, jbyte)
FIELD_PROTO(Char, jchar)
FIELD_PROTO(Short, jshort)
FIELD_PROTO(Int, jint)
FIELD_PROTO(Long, jlong)
FIELD_PROTO(Float, jfloat)
FIELD_PROTO(Double, jdouble)


/* Wrapper for JNI constructor */
nji_error NewObject(jobject *result, jclass clazz, jmethodID meth, ...);


/* Wrappers for JNI method methods */
#define _METHOD_STATIC_PROTO(T, t)  \
    nji_error CallStatic ## T ## Method(t *result, jclass obj, jmethodID meth, ...)
#define _METHOD_PROTO(T, t)  \
    nji_error Call ## T ## Method(t *result, jobject obj, jmethodID meth, ...)
#define METHOD_PROTO(T, t)      \
    _METHOD_STATIC_PROTO(T, t); \
    _METHOD_PROTO(T, t);
METHOD_PROTO(Object, jobject)
METHOD_PROTO(Boolean, jboolean)
METHOD_PROTO(Byte, jbyte)
METHOD_PROTO(Char, jchar)
METHOD_PROTO(Short, jshort)
METHOD_PROTO(Int, jint)
METHOD_PROTO(Long, jlong)
METHOD_PROTO(Float, jfloat)
METHOD_PROTO(Double, jdouble)
nji_error CallStaticVoidMethod(jclass obj, jmethodID meth, ...);
nji_error CallVoidMethod(jobject obj, jmethodID meth, ...);


/* Wrappers for JNI string methods */
nji_error GetStringUTFChars(const char **utf, jstring string, jboolean *isCopy);
nji_error GetStringUTFLength(jsize *length, jstring string);
nji_error ReleaseStringUTFChars(jstring string, const char *utf);
nji_error NewStringUTF(jstring *string, const char *utf);


/* Wrappers for JNI array methods */
nji_error NewObjectArray(jobjectArray *result, jsize length, jclass elementClass, jobject initialElement);
nji_error GetArrayLength(jsize *length, jarray array);
nji_error GetObjectArrayElement(jobject *result, jobjectArray array, jsize index);


/* Wrappers for JNI byte array methods */
nji_error NewByteArray(jbyteArray *result, jsize count);
nji_error SetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf);


/* Wrapper for JNI define class */
nji_error DefineClass(jclass *result, const char *name, jobject loader, const jbyte *buf, jsize bufLen);


/* Wrapper for JNI GetObjectClass */
nji_error GetObjectClass(jclass *result, jobject instance);


/* Wrapper for JNI IsInstanceOf */
nji_error IsInstanceOf(jboolean *result, jobject instance, jclass clazz);


/* Wrapper for JNI IsSameObject */
nji_error IsSameObject(jboolean *result, jobject one, jobject two);


/* Wrappers for JNI monitor methods (AKA synchronized) */
nji_error MonitorEnter(jobject obj);
nji_error MonitorExit(jobject obj);


/* Wrapper for NJI AllocObject */
nji_error AllocObject(jobject *obj, jclass clazz);


#ifdef __cplusplus
}
#endif


#endif
