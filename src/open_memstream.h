//
//  open_memstream.h
//  binding
//
//  Created by Bar Rotstein on 9/23/17.
//
//

#ifndef open_memstream_h
#define open_memstream_h

#if __POSIX_VISIBLE < 200809
#include "MemStream.h"
#define __create_memstream__
#endif

#ifdef __cplusplus
extern "C"
{
#endif

FILE * Create_MemStream(char * * buf, size_t * size) {
#ifdef __create_memstream__
    return CreateMemStream(buf, size);
#else
    return open_memstream(buf, size);
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* open_memstream_h */
