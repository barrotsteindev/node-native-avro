#include <stdbool.h>
#include "AvroUtils.h"

extern bool init_schema(const char * schema, avro_schema_t * avro_schema) {
  avro_schema_error_t e;
  if (avro_schema_from_json(schema, (strlen(schema) - 1), avro_schema, & e)) {
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
        return !avro_value_set_int(& curVal, *(int32_t *)avroStruct->value);
        break;
      }
      case AVRO_STRING_TYPE: {
        return !avro_value_set_string(& curVal, (char *)avroStruct->value);
        break;
      }
      default:
      printf("unsportted type %s key\n", avroStruct->key);
      return false;
      break;
    }
}

extern avro_value_t * SeralizeToAvro(struct KeyValueStruct * * avroStructArray,
  int keyLength, avro_schema_t * avroSchema, avro_value_iface_t * iface) {
    avro_value_t * avroRecord = (avro_value_t *)malloc(sizeof(avro_value_t *));
    if (avroRecord == NULL) {
      return NULL;
    }
    avro_generic_value_new(iface, avroRecord);
    for (int i = 0; i < keyLength; i++) {
      struct KeyValueStruct * currAvroStruct = avroStructArray[i];
      if (!InsertToAvroRecord(currAvroStruct, avroRecord)) {
        return NULL;
      }
      delete_struct(currAvroStruct);
    }
    return avroRecord;
}

extern bool WriteAvroToStream(FILE * stream, avro_value_t * avroRecord,
  avro_value_iface_t * iface, avro_schema_t * avroSchemaPtr) {
    avro_schema_t avroSchema = (*avroSchemaPtr);
    avro_file_writer_t file_writer;
    if (avro_file_writer_create_fp(stream, "", 1, avroSchema, & file_writer)) {
      return false;
    }
    avro_file_writer_append_value(file_writer, avroRecord);
    avro_file_writer_close(file_writer);
    return true;
  }
