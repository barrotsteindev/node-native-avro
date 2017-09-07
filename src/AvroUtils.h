#ifndef SRC_AVROUTILS_H_
#define SRC_AVROUTILS_H_
#include <avro.h>

#include "KeyValueStruct.h"

bool init_schema(const char * schema, avro_schema_t * avro_schema) {
  avro_schema_error_t e;
  if (avro_schema_from_json(schema, (strlen(schema) - 1), avro_schema, &e)) {
    return false;
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

bool InsertToAvroRecord(KeyValueStruct * avroStruct, avro_value_t & avroRecord) {
  avro_value_t curVal;
  size_t avroSize;
  avro_value_get_by_name(& avroRecord, avroStruct->key,
    & curVal, & avroSize);
  if (avroStruct->type == avroType::AVRO_STRING_TYPE) {
    return !avro_value_set_string(& curVal, (char *)avroStruct->value);
  } else if (avroStruct->type == avroType::AVRO_INT_TYPE) {
    return !avro_value_set_int(& curVal, *(int32_t *)avroStruct->value);
  }
  printf("unkown type\n");
  return false;
}

#endif // SRC_AVROUTILS_H_
