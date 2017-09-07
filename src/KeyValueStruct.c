#include <stdbool.h>
#include <stdlib.h>
#include "KeyValueStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern bool delete_struct(struct KeyValueStruct * self) {
  free(self->key);
  free((void *)self->value);
  free(self);
  return true;
}

extern bool createStruct(struct KeyValueStruct * ptr, const char * key,
  enum avroType type, void * data) {
    ptr->key = (char *)malloc(sizeof(key));
    if (ptr->key == NULL) {
      return false;
    }
    strcpy(ptr->key, key);
    ptr->type = type;
    ptr->value = data;
    return true;
}

#ifdef __cplusplus
}
#endif
