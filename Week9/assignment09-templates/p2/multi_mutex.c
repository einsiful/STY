#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>

#include "multi_mutex.h"

int multi_mutex_unlock(pthread_mutex_t **mutexv, int mutexc) {
    if (mutexv == NULL || mutexc <= 0) {
        return -1;
    }

    int ret = 0;
    for (int i = 0; i < mutexc; ++i) {
        if (pthread_mutex_unlock(mutexv[i]) != 0) {
            ret = -1;
        }
    }
    return ret;
}

int multi_mutex_trylock(pthread_mutex_t **mutexv, int mutexc) {
    if (mutexv == NULL || mutexc <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < mutexc; ++i) {
        if (pthread_mutex_trylock(mutexv[i]) != 0) {

            while (--i >= 0) {
                pthread_mutex_unlock(mutexv[i]);
            }
            return -1;
        }
    }
    return 0;
}