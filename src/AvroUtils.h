#ifndef SRC_AVROUTILS_H_
#define SRC_AVROUTILS_H_
#include <avro.h>

#include "KeyValueStruct.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool init_schema(const char * schema, avro_schema_t * avro_schema);

bool InsertToAvroRecord(struct KeyValueStruct * avroStruct,
  avro_value_t * avroRecord);

#ifdef __cplusplus
}
#endif
#endif // SRC_AVROUTILS_H_
