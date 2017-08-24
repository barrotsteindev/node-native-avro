#ifndef SRC_AVROUTILS_H_
#define SRC_AVROUTILS_H_
#include <avro.h>

#include "KeyValueStruct.h"

bool init_schema(const char * schema, avro_schema_t * avro_schema) {
  avro_schema_error_t e;
  if (!avro_schema_from_json(schema, sizeof(schema), avro_schema, &e)) {
    return true;
  }
  return true;
}

bool InsertToAvroRecord(KeyValueStruct * avroStruct, avro_datum_t & avro_record) {
  if (avroStruct->type == avroType::AVRO_STRING_TYPE) {
    char * data = (char *)avroStruct->value;
    avro_datum_t avroString = avro_string(data);
    return avro_record_set(avro_record, avroStruct->key, avroString);
  }
}

#endif // SRC_AVROUTILS_H_
