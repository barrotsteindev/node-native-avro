#include <stdbool.h>
#include "AvroUtils.h"

extern bool init_schema(const char * schema, avro_schema_t * avro_schema) {
  avro_schema_error_t e;
  if (avro_schema_from_json(schema, (strlen(schema) - 1), avro_schema, &e)) {
    return false;
  }
  return true;
}

extern bool InsertToAvroRecord(struct KeyValueStruct * avroStruct,
  avro_value_t * avroRecord) {
    avro_value_t curVal;
    size_t avroSize;
    avro_value_get_by_name(avroRecord, avroStruct->key,
      & curVal, & avroSize);
    switch (avroStruct->type) {
      case AVRO_INT_TYPE: {
        bool setInt = !avro_value_set_int(& curVal,
          *(int32_t *)avroStruct->value);
        return setInt;
        break;
      }
      case AVRO_STRING_TYPE: {
        bool setString = !avro_value_set_string(& curVal,
          (char *)avroStruct->value);
        return setString;
        break;
      }
      default:
      printf("unsportted type %s key\n", avroStruct->key);
      return false;
      break;
    }
}
