//
//  MemStream.h
//  binding
//
//  Created by Bar Rotstein on 9/14/17.
//
//

#ifndef MemStream_h
#define MemStream_h

#ifdef __cplusplus
extern "C"
{
#endif

FILE * CreateMemStream(char * * buf, size_t * size);

#ifdef __cplusplus
}
#endif

#endif /* MemStream_h */
