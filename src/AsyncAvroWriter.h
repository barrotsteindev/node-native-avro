#ifndef ASYNCAVROWRITER_H
#define ASYNCAVROWRITER_H

#include <nan.h>
#include <avro.h>
#include <string>
#include <mutex>
#include "KeyValueStruct.h"

class AsyncAvroWriter : public Nan::AsyncWorker {
 public:
    AsyncAvroWriter(Nan::Callback * callback,
      struct KeyValueStruct * * avroStructArray,
      const char * strSchema, int numOfKeys);
    void Execute();
    void HandleOKCallback();
 private:
    ~AsyncAvroWriter();
    std::string m_perm_schema;
    struct KeyValueStruct * * m_avroArray;
    avro_schema_t m_avroSchema;
    int m_numOfKeys;
    char * m_data;
    size_t m_data_size;
    avro_value_t * m_avroRecord;
    avro_value_iface * m_iface;
};

#endif  // ASYNCAVROWRITER_H
