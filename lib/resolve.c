/* resolve.c - Runtime resolution of Java classes including any constructors,
 * fields and methods. A couple of methods for finding classes are implemented
 * here, they are not exhaustive however. If you are having trouble finding
 * classes and discover and new technique for finding them you can add support
 * for it here.  */

#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include "nji.h"
#include "nji/resolve.h"
#include "classloader.h"
#include "thread.h"


/* convert jni class name to a java class name */
static jstring binaryName(const char *name) {
    char *b, *binary;
    jstring result;

    binary = strdup(name);

    /* convert '/' to '.' */
    for (b = binary; *b; b++) {
        if (*b == '/') {
            *b = '.';
        }
    }

    NewStringUTF(&result, binary);
    free(binary);

    return result;
}


/* find class using thread classloaders */
static jclass findClassFromThreads(const char *name) {
    int i;
    nji_error error;
    jint count;
    jobjectArray threads;
    jstring binName;
    jclass result = NULL;

    NJI_ENTER;

    /* get array of java threads */
    if ((error = Thread.activeCount(&count)) != NJI_OK) {
        DEBUG("Failed to get thread count: %d\n", error);
        goto err;
    }

    if ((error = NewObjectArray(&threads, count * 2, JCLASS(Thread), NULL)) != NJI_OK || !threads) {
        DEBUG("Failed to create thread array: %d\n", error);
        goto err;
    }

    if ((error = Thread.enumerate(&count, threads)) != NJI_OK) {
        DEBUG("Failed to enumerate threads: %d\n", error);
        goto err;
    }

    /* convert class name to java format */
    binName = binaryName(name);

    /* iterate threads looing for class */
    for (i = 0; i < count; i++) {
        jobject thread;

        /* get thread */
        if (GetObjectArrayElement(&thread, threads, i) == NJI_OK && thread) {
            jobject classloader;

            /* get thread classloader */
            if (Thread.getContextClassLoader(&classloader, thread) == NJI_OK && classloader) {

                /* attempt top find class */
                if (ClassLoader.findClass(&result, classloader, binName) == NJI_OK && result) {
                    break;
                }
                DeleteLocalRef(classloader);
            }
            DeleteLocalRef(thread);
        }
    }

err:
    NJI_LEAVE_WITH(result);

    return result;
}


/* Try to find class by any means */
static jclass findClass(const char *name) {
    jclass result = NULL;

    /* find with JNI */
    if ((FindClass(&result, name) == NJI_OK) && result) {
        goto done;
    }

    /* find with thread classloaders */
    result = findClassFromThreads(name);

done:
    return result;
}


/* Resolve class symbols into class definition */
nji_error resolveClassDef(ClassDef *def, ClassSym *sym) {
    nji_error error = NJI_ERROR;
    jclass class;

    DEBUG("Resolving: %s\n", sym->name);

    class = findClass(sym->name);
    if (!class) {
        DEBUG("Failed to find class any means: %s\n", sym->name);
        goto err;
    }

    if (def->Fields) {
        jfieldID *d = def->Fields;
        FieldSym *s = sym->Fields;
        for (; s->signature; d++, s++) {
            error = (s->flags & FLAG_STATIC) ?
                GetStaticFieldID(d, class, s->name, s->signature):
                GetFieldID(d, class, s->name, s->signature);
            if (error != NJI_OK && !(s->flags & FLAG_OPTIONAL)) {
                DEBUG("Failed to resolve field '%s' '%s' in '%s': %d\n", s->name, s->signature, sym->name, error);
                goto cleanup;
            }
        }
    }

    if (def->Constructors) {
        jmethodID *d = def->Constructors;
        ConstructorSym *s = sym->Constructors;
        for (; s->signature; d++, s++) {
            error = GetMethodID(d, class, "<init>", s->signature);
            if (error != NJI_OK && !(s->flags & FLAG_OPTIONAL)) {
                DEBUG("Failed to resolve constructor '%s' in '%s: %d'\n", s->signature, sym->name, error);
                goto cleanup;
            }
        }
    }

    if (def->Methods) {
        jmethodID *d = def->Methods;
        MethodSym *s = sym->Methods;
        for (; s->signature; d++, s++) {
            error = (s->flags & FLAG_STATIC) ?
                GetStaticMethodID(d, class, s->name, s->signature):
                GetMethodID(d, class, s->name, s->signature);
            if (error != NJI_OK && !(s->flags & FLAG_OPTIONAL)) {
                DEBUG("Failed to resolve method '%s' '%s' in '%s: %d'\n", s->name, s->signature, sym->name, error);
                goto cleanup;
            }
        }
    }

    if ((error = NewGlobalRef(&def->Class, class)) != NJI_OK) {
        DEBUG("Failed to create global reference for class '%s': %d\n", sym->name, error);
        goto cleanup;
    }

    DEBUG("Successfully resolved class '%s'\n", sym->name);

    error = NJI_OK;

cleanup:
    DeleteLocalRef(class);

err:
    return error;
}
