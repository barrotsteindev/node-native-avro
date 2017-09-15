#include <stdio.h>
#include <nan.h>
#include <avro.h>
#include "AsyncAvroWriter.h"
#include "KeyValueStruct.h"
#include "AvroUtils.h"

class AvroWrap {
 public:
  static NAN_MODULE_INIT(Init) {
    target->Set(Nan::New("writeSync").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(WriteSync)->GetFunction());
      target->Set(Nan::New("write").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(Write)->GetFunction());
  }

  static char * v8StringtoC_str(const v8::Local<v8::String> & v8Str) {
    return * Nan::Utf8String(v8Str);
  }

  static bool V8ObjToStructArray(KeyValueStruct * * structArray,
    const int & numberOfKeys, const v8::Local<v8::Array> keyList,
    const v8::Local<v8::Object> obj) {
      for (int i = 0; i < numberOfKeys; i++) {
        v8::Local<v8::String> keyName = keyList->Get(i)->ToString();
        v8::Local<v8::Value> keyValue = obj->Get(keyName);
        structArray[i] = ParseToKeyValStruct(keyName, keyValue);
        if (structArray[i] == NULL) {
          printf("err");
          return false;
        }
      }
      return true;
  }

  static KeyValueStruct * ParseToKeyValStruct(v8::Local<v8::String> & keyName,
    v8::Local<v8::Value> & keyValue) {
      KeyValueStruct * currStruct = (KeyValueStruct *)
                                     malloc(sizeof(KeyValueStruct));
      if (currStruct == NULL) {
        return NULL;
      }
      const char * key = v8StringtoC_str(keyName);
      void * value_ptr;
      enum avroType currType;
      if (keyValue->IsString()) {
        const char * value = v8StringtoC_str(keyValue->ToString());
        value_ptr = (char *)malloc(sizeof(value));
        strcpy((char *)value_ptr, value);
        currType = AVRO_STRING_TYPE;
      } else if (keyValue->IsNumber()) {
        currType = AVRO_INT_TYPE;
        if (keyValue->IsInt32()) {
          value_ptr = malloc(sizeof(int *));
          int32_t intValue = keyValue->Int32Value();
          memcpy(value_ptr, (void *)& intValue, sizeof(int *));
        } else if (keyValue->IsUint32()) {
          uint * value_ptr = (uint *)malloc(sizeof(uint));
          value_ptr = (uint *)keyValue->Uint32Value();
        }
      } else {
        printf("unsportted type in key: %s, skipping\n", currStruct->key);
        return NULL;
      }
      if (!createStruct(currStruct, key, currType, value_ptr)) {
        return NULL;
      }
      return currStruct;
  }

  static NAN_METHOD(Write) {
    if (info.Length() != 3) {
      return Nan::ThrowError("Wrong Number of Arguments");
    }
    if (!info[0]->IsString()) {
      return Nan::ThrowTypeError("First param must be a string");
    }
    if (!info[1]->IsObject()) {
      return Nan::ThrowTypeError("Second param must be a json object");
    }
    if(!info[2]->IsFunction()) {
      return Nan::ThrowTypeError("Third param must be a callback function");
    }
    // todo: check that 3rd param is function
    Nan::Callback * callback = new Nan::Callback(info[2].As<v8::Function>());

    v8::Local<v8::Object> json = info[1]->ToObject();
    v8::Local<v8::Array> keyNames = json->GetOwnPropertyNames();
    int keyLength = keyNames->Length();
    KeyValueStruct * * avroStructs = (KeyValueStruct * *)
                                     malloc(keyLength * sizeof(KeyValueStruct));
    if (avroStructs == NULL) {
      return Nan::ThrowError("could not allocate memory");
    }
    if (!V8ObjToStructArray(avroStructs, keyLength, keyNames, json)) {
      return Nan::ThrowError("could not parse json array");
    }
    const char * strSchema = * Nan::Utf8String(info[0]->ToString());

    Nan::AsyncQueueWorker(new AsyncAvroWriter(callback, avroStructs,
                                              strSchema, keyLength));


  }

  static NAN_METHOD(WriteSync) {
    if (info.Length() != 2) {
      return Nan::ThrowError("Wrong Number of Arguments");
    }
    if (!info[0]->IsString()) {
      return Nan::ThrowTypeError("First param must be a string");
    }
    if (!info[1]->IsObject()) {
      return Nan::ThrowTypeError("Second param must be a json object");
    }
    v8::Local<v8::Object> json = info[1]->ToObject();
    v8::Local<v8::Array> keyNames = json->GetOwnPropertyNames();
    int keyLength = keyNames->Length();
    KeyValueStruct * * avroStructs = (KeyValueStruct * *)
                                     malloc(keyLength * sizeof(KeyValueStruct));
    if (avroStructs == NULL) {
      return Nan::ThrowError("could not allocate memory");
    }
    if (!V8ObjToStructArray(avroStructs, keyLength, keyNames, json)) {
      return Nan::ThrowError("could not parse json array");
    }
    const char * strSchema = * Nan::Utf8String(info[0]->ToString());
    avro_schema_t avroSchema;
    if (!init_schema(strSchema, & avroSchema)) {
       return Nan::ThrowError("cloud not parse schema");
    }
    avro_value_iface_t * iface = avro_generic_class_from_schema(avroSchema);
    avro_value_t * avroRecord = SeralizeToAvro(avroStructs, keyLength,
                                               & avroSchema, iface);
    if (avroRecord == NULL) {
      return Nan::ThrowError("could not create avro record");
    }
    char * memStreamContent;
    size_t memStreamSize;
    FILE * memStream = open_memstream(& memStreamContent, & memStreamSize);
    if (!WriteAvroToStream(memStream, avroRecord, iface, & avroSchema)) {
      return Nan::ThrowError("could not write avro record to buffer");
    }
    info.GetReturnValue().Set(Nan::NewBuffer(memStreamContent,
      memStreamSize).ToLocalChecked());

    avro_value_decref(avroRecord);
    avro_value_iface_decref(iface);
    avro_schema_decref(avroSchema);
  }
};

NODE_MODULE(NATIVEAVRO, AvroWrap::Init);
