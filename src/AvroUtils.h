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

avro_value_t * SeralizeToAvro(struct KeyValueStruct * * avroStructArray,
  int keyLength, avro_schema_t * avroSchema, avro_value_iface_t * iface);

bool WriteAvroToStream(FILE * stream, avro_value_t * avroRecord,
  avro_value_iface_t * iface, avro_schema_t * avroSchemaPtr);

#ifdef __cplusplus
}
#endif
#endif // SRC_AVROUTILS_H_
