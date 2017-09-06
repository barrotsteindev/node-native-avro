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

// bool OpenMemoryStream(FILE * stream, size_t * stream_size) {
//   stream = open_memstream(stream, stream_size);
//   return stream == NULL;
// }

bool WriteBufToStream(FILE * stream, char * buf, size_t avroSize) {
  stream = fopen("test.avro", "w+");
  fwrite(buf, 1, avroSize, stream);
  fclose(stream);
  return true;
}

#endif // SRC_AVROUTILS_H_
