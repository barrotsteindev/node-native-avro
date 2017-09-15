#include "AsyncAvroWriter.h"
#include "AvroUtils.h"

AsyncAvroWriter::AsyncAvroWriter(Nan::Callback * callback,
  struct KeyValueStruct * * avroStructArray, const char * strSchema,
  int numOfKeys) : m_avroArray(avroStructArray), m_numOfKeys(numOfKeys),
  m_perm_schema(strSchema), Nan::AsyncWorker(callback) {
    init_schema(m_perm_schema.c_str(), & m_avroSchema);
  }
// Executes in worker thread
void AsyncAvroWriter::Execute() {
  avro_value_iface_t * iface = avro_generic_class_from_schema(m_avroSchema);
  avro_value_t * avroRecord = SeralizeToAvro(m_avroArray, m_numOfKeys,
                                             & m_avroSchema, iface);
  if (avroRecord == NULL) {
    printf("could not SeralizeToAvro\n");
    return;
  }
  FILE * memStream = open_memstream(& m_data, & m_data_size);
  if (!WriteAvroToStream(memStream, avroRecord, iface, & m_avroSchema)) {
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
