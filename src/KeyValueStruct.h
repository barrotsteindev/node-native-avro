#ifndef SRC_KEYVALUESTRUCT_H_
#define SRC_KEYVALUESTRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum avroType { AVRO_INT_TYPE, AVRO_DOUBLE_TYPE, AVRO_STRING_TYPE };

typedef struct KeyValueStruct {
  const void * value;
  enum avroType type;
  char * key;
};

bool delete_struct(struct KeyValueStruct * self);
#ifdef __cplusplus
}
#endif
#endif // SRC_KEYVALUESTRUCT_H_
