#ifndef SRC_AVROUTILS_H_
#define SRC_AVROUTILS_H_
#include <avro.h>

#include "KeyValueStruct.h"

bool init_schema(const char * schema, avro_schema_t * avro_schema) {
  const char * b = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"ID\", \"type\": \"long\"},\
     {\"name\": \"First\", \"type\": \"string\"},\
     {\"name\": \"Last\", \"type\": \"string\"},\
     {\"name\": \"Phone\", \"type\": \"string\"},\
     {\"name\": \"Age\", \"type\": \"int\"}]}";
  // char * a = (char *)malloc(400 * sizeof(char));
  char a[243];
  strcpy(a, b);
  printf("a\n");
  printf("%s\n", a);
  // return true;
  return avro_schema_from_json_literal(a, avro_schema);
}

bool InsertToAvroRecord(KeyValueStruct * avroStruct, avro_datum_t & avro_record) {
  if (avroStruct->type == avroType::AVRO_STRING_TYPE) {
    char * data = (char *)avroStruct->value;
    avro_datum_t avroString = avro_string(data);
    return avro_record_set(avro_record, avroStruct->key, avroString);
  }
}

#endif // SRC_AVROUTILS_H_
