#include "AsyncAvroWriter.h"
#include "AvroUtils.h"
#include "open_memstream.h"

AsyncAvroWriter::AsyncAvroWriter(Nan::Callback * callback,
  struct KeyValueStruct * * avroStructArray, const char * strSchema,
  int numOfKeys) : m_avroArray(avroStructArray), m_numOfKeys(numOfKeys),
  m_perm_schema(strSchema), Nan::AsyncWorker(callback) {
    init_schema(m_perm_schema.c_str(), & m_avroSchema);
  }

AsyncAvroWriter::~AsyncAvroWriter() {
    free(m_avroArray);
    avro_schema_decref(m_avroSchema);
    avro_value_iface_decref(m_iface);
    avro_value_decref(m_avroRecord);
    free(m_avroRecord);
}

// Executes in worker thread
void AsyncAvroWriter::Execute() {
  m_iface = avro_generic_class_from_schema(m_avroSchema);
  m_avroRecord = SeralizeToAvro(m_avroArray, m_numOfKeys, & m_avroSchema, m_iface);
  if (m_avroRecord == NULL) {
    printf("could not SeralizeToAvro\n");
    return;
  }
  FILE * memStream = Create_MemStream(& m_data, & m_data_size);
  if (!WriteAvroToStream(memStream, m_avroRecord, m_iface, & m_avroSchema)) {
    printf("could not write avro record to buffer\n");
  }
}
// Executes in event loop
void AsyncAvroWriter::HandleOKCallback() {
  if (m_data == NULL) {
    v8::Local<v8::Value> argv[] = { Nan::New("Serialization Error")
                                    .ToLocalChecked(), Nan::Null() };
    callback->Call(2, argv);
  } else {
    v8::Local<v8::Value> argv[] = { Nan::Null(), Nan::NewBuffer(m_data,
      m_data_size).ToLocalChecked() };
    callback->Call(2, argv);
  }
}
