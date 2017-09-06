#include <stdbool.h>
#include "KeyValueStruct.h"

extern bool delete_struct(struct KeyValueStruct * self) {
  free(self->key);
  free((void *)self->value);
  free(self);
  return true;
}
