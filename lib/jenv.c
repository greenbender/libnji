/* jenv.c - Provides accessor method for acquiring and Java Environment for the
 * current thread. The implementation here is the most simple case where access
 * to a JavaVM is as straight forward as creating one via the java library. If
 * you are using a platform that doesn't suport this re-implementing get_jenv()
 * for your platform should be all you need to do to get libnji working */

#include <pthread.h>
#include "debug.h"
#include "jenv.h"


static JavaVM *get_jvm(void) {
    static JavaVM *jvm = NULL;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    if (pthread_mutex_lock(&mutex) == 0) {
        if (!jvm) {
            jsize count;
            if ((JNI_GetCreatedJavaVMs(&jvm, 1, &count) == JNI_OK) && count) {
                DEBUG("Got already created JVM\n");
            } else {
                JNIEnv *jenv;
#ifdef __DEBUG__
                JavaVMOption options[] = {
                    { .optionString = "-verbose:jni" },
                };
                JavaVMInitArgs args = {
                    .version = JNI_VERSION_1_4,
                    .options = options,
                    .nOptions = sizeof(options)/sizeof(*options),
                    .ignoreUnrecognized = JNI_TRUE,
                };
#else
                JavaVMInitArgs args = {
                    .version = JNI_VERSION_1_4,
                    .ignoreUnrecognized = JNI_TRUE,
                };
#endif
                if (JNI_CreateJavaVM(&jvm, (void **)&jenv, &args) == JNI_OK) {
                    DEBUG("Created a new JVM\n");
                } else {
                    DEBUG("Failed to get JVM\n");
                    jvm = NULL;
                }
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    return jvm;
}


static pthread_key_t jenv_key;
static pthread_once_t got_jenv_key = PTHREAD_ONCE_INIT;
static void make_jenv_key(void) {
    (void)pthread_key_create(&jenv_key, NULL);
}


JNIEnv *get_jenv(void) {
    JNIEnv *jenv;
    (void)pthread_once(&got_jenv_key, make_jenv_key);
    jenv = (JNIEnv *)pthread_getspecific(jenv_key);
    if (!jenv) {
        JavaVM *jvm = get_jvm();
        if (jvm) {
            jint result = (*jvm)->GetEnv(jvm, (void **)&jenv, JNI_VERSION_1_4);

            /* thread already attached to JVM */
            if (result == JNI_OK) {
                DEBUG("Got already attached JENV\n");

            /* attach thread to JVM */
            } else if (result == JNI_EDETACHED) {
                JavaVMAttachArgs args = { .version = JNI_VERSION_1_4 };
                if ((*jvm)->AttachCurrentThreadAsDaemon(jvm, (void **)&jenv, &args) == JNI_OK) {
                    DEBUG("Attached thread to JVM\n");
                }
            }
        } else {
            DEBUG("JavaVM is NULL\n");
        }
        pthread_setspecific(jenv_key, jenv);
    }
    return jenv;
}


void close_jenv(void) {
    JNIEnv *jenv;
    (void)pthread_once(&got_jenv_key, make_jenv_key);
    jenv = (JNIEnv *)pthread_getspecific(jenv_key);
    if (jenv) {
        JavaVM *jvm = get_jvm();
        if (jvm) {
            (*jvm)->DetachCurrentThread(jvm);
        } else {
            DEBUG("JavaVM is NULL\n");
        }
        pthread_setspecific(jenv_key, NULL);
    }
}
