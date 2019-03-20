/* nji.c - Wrappers for JNI functions. At present the set of functions that are
 * wrapped is not complete. If you require the use of any other JNI functions
 * simply add the  wrapped implemenation here. */

#include <stdarg.h>
#include "debug.h"
#include "jenv.h"
#include "nji.h"
#include "throwable.h"


/* Deal with any exceptions that might have been raised. This MUST be done
 * anytime a method is called on the jenv */
static nji_error handle_exception(JNIEnv *jenv) {
    jthrowable exception = (*jenv)->ExceptionOccurred(jenv);
    if (exception) {
        (*jenv)->ExceptionClear(jenv);

#ifdef __DEBUG__
        if (JCLASS(Throwable)) {
            jstring message;
            if (Throwable.toString(&message, exception) == NJI_OK) {
                const char *utf;
                if (GetStringUTFChars(&utf, message, NULL) == NJI_OK) {
                    DEBUG("%s\n", utf);
                    ReleaseStringUTFChars(message, utf);
                }
                DeleteLocalRef(message);
            }
        }
#endif

        (*jenv)->DeleteLocalRef(jenv, exception);
        return NJI_EXCEPTION;
    }

    return NJI_OK;
}


nji_error NewGlobalRef(jobject *ref, jobject obj) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *ref = (*jenv)->NewGlobalRef(jenv, obj);
    return handle_exception(jenv);
}


nji_error NewLocalRef(jobject *ref, jobject obj) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *ref = (*jenv)->NewLocalRef(jenv, obj);
    return handle_exception(jenv);
}


nji_error DeleteGlobalRef(jobject ref) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->DeleteGlobalRef(jenv, ref);
    return handle_exception(jenv);
}


nji_error DeleteLocalRef(jobject localRef) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->DeleteLocalRef(jenv, localRef);
    return handle_exception(jenv);
}

    
nji_error PushLocalFrame(jint capacity) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->PushLocalFrame(jenv, capacity);
    return handle_exception(jenv);
}


nji_error PopLocalFrame(jobject *ref, jobject result) {
    jobject r;
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    r = (*jenv)->PopLocalFrame(jenv, result);
    if (ref)
        *ref = r;
    return handle_exception(jenv);
}


nji_error FindClass(jclass *clazz, const char *name) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *clazz = (*jenv)->FindClass(jenv, name);
    return handle_exception(jenv);
}


nji_error GetFieldID(jfieldID *result, jclass clazz, const char *name, const char *sig) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetFieldID(jenv, clazz, name, sig);
    return handle_exception(jenv);
}


nji_error GetStaticFieldID(jfieldID *result, jclass clazz, const char *name, const char *sig) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetStaticFieldID(jenv, clazz, name, sig);
    return handle_exception(jenv);
}


nji_error GetMethodID(jmethodID *result, jclass clazz, const char *name, const char *sig) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetMethodID(jenv, clazz, name, sig);
    return handle_exception(jenv);
}


nji_error GetStaticMethodID(jmethodID *result, jclass clazz, const char *name, const char *sig) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetStaticMethodID(jenv, clazz, name, sig);
    return handle_exception(jenv);
}


#define _FIELD_BODY(s, T)                                               \
    JNIEnv *jenv = get_jenv();                                          \
    if (!jenv)                                                          \
        return NJI_BAD_ENVIRONMENT;                                     \
    if (mode == FIELD_SET) {                                            \
        (*jenv)->Set ## s ## T ## Field(jenv, obj, field, *value);      \
    } else {                                                            \
        *value = (*jenv)->Get ## s ## T ## Field(jenv, obj, field);     \
    }                                                                   \
    return handle_exception(jenv);
#define FIELD(T, t)                                         \
    _FIELD_STATIC_PROTO(T, t) { _FIELD_BODY(Static, T) }    \
    _FIELD_PROTO(T, t) { _FIELD_BODY(, T) }
FIELD(Object, jobject)
FIELD(Boolean, jboolean)
FIELD(Byte, jbyte)
FIELD(Char, jchar)
FIELD(Short, jshort)
FIELD(Int, jint)
FIELD(Long, jlong)
FIELD(Float, jfloat)
FIELD(Double, jdouble)


nji_error NewObject(jobject *result, jclass clazz, jmethodID meth, ...) {
    va_list args;
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    va_start(args, meth);
    *result = (*jenv)->NewObjectV(jenv, clazz, meth, args);
    va_end(args);
    return handle_exception(jenv);
}


#define _METHOD_BODY(s, T)                                                  \
    va_list args;                                                           \
    JNIEnv *jenv = get_jenv();                                              \
    if (!jenv)                                                              \
        return NJI_BAD_ENVIRONMENT;                                         \
    va_start(args, meth);                                                   \
    *result = (*jenv)->Call ## s ## T ## MethodV(jenv, obj, meth, args);    \
    va_end(args);                                                           \
    return handle_exception(jenv);
#define METHOD(T, t)                                        \
    _METHOD_STATIC_PROTO(T, t) { _METHOD_BODY(Static, T) }  \
    _METHOD_PROTO(T, t) { _METHOD_BODY(, T) }
METHOD(Object, jobject)
METHOD(Boolean, jboolean)
METHOD(Byte, jbyte)
METHOD(Char, jchar)
METHOD(Short, jshort)
METHOD(Int, jint)
METHOD(Long, jlong)
METHOD(Float, jfloat)
METHOD(Double, jdouble)


nji_error CallStaticVoidMethod(jclass obj, jmethodID meth, ...) {
    va_list args;
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    va_start(args, meth);
    (*jenv)->CallStaticVoidMethodV(jenv, obj, meth, args);
    va_end(args);
    return handle_exception(jenv);
}


nji_error CallVoidMethod(jobject obj, jmethodID meth, ...) {
    va_list args;
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    va_start(args, meth);
    (*jenv)->CallVoidMethodV(jenv, obj, meth, args);
    va_end(args);
    return handle_exception(jenv);
}


nji_error GetStringUTFChars(const char **utf, jstring string, jboolean *isCopy) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *utf = (*jenv)->GetStringUTFChars(jenv, string, isCopy);
    return handle_exception(jenv);
}


nji_error GetStringUTFLength(jsize *length, jstring string) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *length = (*jenv)->GetStringUTFLength(jenv, string);
    return handle_exception(jenv);
}


nji_error ReleaseStringUTFChars(jstring string, const char *utf) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->ReleaseStringUTFChars(jenv, string, utf);
    return handle_exception(jenv);
}


nji_error NewStringUTF(jstring *string, const char *utf) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *string = (*jenv)->NewStringUTF(jenv, utf);
    return handle_exception(jenv);
}


nji_error NewObjectArray(jobjectArray *result, jsize length, jclass elementClass, jobject initialElement) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->NewObjectArray(jenv, length, elementClass, initialElement);
    return handle_exception(jenv);
}


nji_error GetArrayLength(jsize *length, jarray array) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *length = (*jenv)->GetArrayLength(jenv, array);
    return handle_exception(jenv);
}


nji_error GetObjectArrayElement(jobject *result, jobjectArray array, jsize index) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetObjectArrayElement(jenv, array, index);
    return handle_exception(jenv);
}


nji_error NewByteArray(jbyteArray *result, jsize len) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->NewByteArray(jenv, len);
    return handle_exception(jenv);
}


nji_error SetByteArrayRegion(jbyteArray array, jsize start, jsize len, const jbyte *buf) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->SetByteArrayRegion(jenv, array, start, len, buf);
    return handle_exception(jenv);
}


nji_error DefineClass(jclass *result, const char *name, jobject loader, const jbyte *buf, jsize bufLen) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->DefineClass(jenv, name, loader, buf, bufLen);
    return handle_exception(jenv);
}


nji_error GetObjectClass(jclass *result, jobject instance) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->GetObjectClass(jenv, instance);
    return handle_exception(jenv);
}


nji_error IsInstanceOf(jboolean *result, jobject instance, jclass clazz) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->IsInstanceOf(jenv, instance, clazz);
    return handle_exception(jenv);
}


nji_error IsSameObject(jboolean *result, jobject one, jobject two) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    *result = (*jenv)->IsSameObject(jenv, one, two);
    return handle_exception(jenv);
}


nji_error MonitorEnter(jobject obj) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->MonitorEnter(jenv, obj);
    return handle_exception(jenv);
}


nji_error MonitorExit(jobject obj) {
    JNIEnv *jenv = get_jenv();
    if (!jenv)
        return NJI_BAD_ENVIRONMENT;
    (*jenv)->MonitorExit(jenv, obj);
    return handle_exception(jenv);
}


nji_error AllocObject(jobject *obj, jclass clazz) {
    JNIEnv *jenv = get_jenv();
    if(!jenv)
        return NJI_BAD_ENVIRONMENT;
    *obj = (*jenv)->AllocObject(jenv, clazz);
    return handle_exception(jenv);
}
