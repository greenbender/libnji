#include "object.h"
#include "class.h"
#include "thread.h"


void test_lookups(void) {
    printf("Thread jclass = %p\n", (void *)JCLASS(Thread));
    printf("Thread.MIN_PRIORITY jfieldID = %p\n", (void *)JFIELDID(Thread, MIN_PRIORITY));
    printf("Thread.activeCount jmethodID = %p\n", (void *)JMETHODID(Thread, MIN_PRIORITY));
}


void test_fields(void) {
    jint value;

    NJI_ENTER;

    if (Thread.MIN_PRIORITY(&value, FIELD_GET) == NJI_OK)
        printf("Thread.MIN_PRIORITY = %d\n", value);
    if (Thread.MAX_PRIORITY(&value, FIELD_GET) == NJI_OK)
        printf("Thread.MAX_PRIORITY = %d\n", value);
    if (Thread.NORM_PRIORITY(&value, FIELD_GET) == NJI_OK)
        printf("Thread.NORM_PRIORITY = %d\n", value);
    value++; 
    if (Thread.NORM_PRIORITY(&value, FIELD_SET) == NJI_OK)
        if (Thread.NORM_PRIORITY(&value, FIELD_GET) == NJI_OK)
            printf("Thread.NORM_PRIORITY = %d\n", value);

    NJI_LEAVE;
}


void test_methods(void) {
    jsize count;
    jobjectArray threads;
    jint i;

    NJI_ENTER;

    if (Thread.activeCount(&count) != NJI_OK)
        goto cleanup;

    if (Thread.getThreads(&threads) != NJI_OK)
        goto cleanup;

    if (GetArrayLength(&count, threads) != NJI_OK)
        goto cleanup;

    printf("Length = %d\n", count);

    for (i = 0; i < count; i++) {
        jobject thread;
        jobject classloader;
        jlong id;
        jstring name;
        const char *name_;
        jclass clazz;

        NJI_ENTER;

        if (GetObjectArrayElement(&thread, threads, i) != NJI_OK)
            continue;
    
        if (Thread.getId(&id, thread) == NJI_OK)
            printf("Thread ID = %ld\n", id);

        if (Thread.getName(&name, thread) == NJI_OK) {
            if (GetStringUTFChars(&name_, name, NULL) == NJI_OK) {
                printf("Thread name = %s\n", name_);
                ReleaseStringUTFChars(name, name_);
            }
        }

        if (Thread.getContextClassLoader(&classloader, thread) != NJI_OK)
            continue;

        if (!classloader)
            continue;

        if (Object.getClass(&clazz, classloader) != NJI_OK)
            continue;

        if (Class.getName(&name, clazz) == NJI_OK) {
            if (GetStringUTFChars(&name_, name, NULL) == NJI_OK) {
                printf("Classloader class = %s\n", name_);
                ReleaseStringUTFChars(name, name_);
            }
        }

        NJI_LEAVE;
    }

cleanup:
    NJI_LEAVE;
}


int main(void) {
    test_lookups();
    test_fields();
    test_methods();
    return 0;
}
