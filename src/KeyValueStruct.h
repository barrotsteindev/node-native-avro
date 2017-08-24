#ifndef SRC_KEYVALUESTRUCT_H_
#define SRC_KEYVALUESTRUCT_H_

enum avroType { AVRO_INT_TYPE, AVRO_DOUBLE_TYPE, AVRO_STRING_TYPE };

typedef struct KeyValueStruct {
  const void * value;
  enum avroType type;
  const char * key;
};

#endif // SRC_KEYVALUESTRUCT_H_
