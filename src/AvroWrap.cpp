#include <nan.h>
#include <avro.h>
#include "KeyValueStruct.h"
#include "AvroUtils.h"

class AvroWrap {
 public:
  static NAN_MODULE_INIT(Init) {
    target->Set(Nan::New("write").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(Write)->GetFunction());
  }

  static std::string v8StringtoC_str(const v8::Local<v8::String> & v8Str) {
    std::string regStr(* Nan::Utf8String(v8Str));
    return regStr;
  }

  static void V8ObjToStructArray(KeyValueStruct * * structArray,
    const int & numberOfKeys, const v8::Local<v8::Array> keyList,
    const v8::Local<v8::Object> obj) {
      for (int i = 0; i < numberOfKeys; i++) {
        v8::Local<v8::String> keyName = keyList->Get(i)->ToString();
        v8::Local<v8::Value> keyValue = obj->Get(keyName);
        structArray[i] = ParseToKeyValStruct(keyName, keyValue);
      }
  }

  static KeyValueStruct * ParseToKeyValStruct(v8::Local<v8::String> & keyName,
    v8::Local<v8::Value> & keyValue) {
      KeyValueStruct currStruct;
      currStruct.key = (const char *)v8StringtoC_str(keyName).c_str();
      if (keyValue->IsString()) {
        currStruct.type = avroType::AVRO_STRING_TYPE;
        currStruct.value = (const void *)v8StringtoC_str(keyValue->ToString())
                                                              .c_str();
      } else {
        printf("unsportted type in key: %s, skipping\n", currStruct.key);
      }
  }

  static NAN_METHOD(Write) {
    if (info.Length() != 2) {
      return Nan::ThrowError("Wrong Number of Arguments");
    }
    if (!info[0]->IsString()) {
      return Nan::ThrowTypeError("first param must be a string");
    }
    if (!info[1]->IsObject()) {
      return Nan::ThrowTypeError("second param must be a json object");
    }
    v8::Local<v8::String> v8Schema = info[0]->ToString();
    v8::Local<v8::Object> json = info[1]->ToObject();
    std::string schema(* Nan::Utf8String(v8Schema));
    v8::Local<v8::Array> keyNames = json->GetOwnPropertyNames();
    int keyLength = keyNames->Length();
    KeyValueStruct * * avroStructs = (KeyValueStruct * *)
                                     malloc(keyLength * sizeof(KeyValueStruct));
    V8ObjToStructArray(avroStructs, keyLength, keyNames, json);
    for (int i = 0; i < keyLength; i++) {
      avro_schema_t avroSchema;
      const char * a = schema.c_str();
      // const char a[] = "{\"type\":\"record\",\
      // \"name\":\"Person\",\
      // \"fields\":[\
      //    {\"name\": \"ID\", \"type\": \"long\"},\
      //    {\"name\": \"First\", \"type\": \"string\"},\
      //    {\"name\": \"Last\", \"type\": \"string\"},\
      //    {\"name\": \"Phone\", \"type\": \"string\"},\
      //    {\"name\": \"Age\", \"type\": \"int\"}]}";
      if (init_schema(a, & avroSchema)) {
         printf("%s\n", a);
      }
      printf("after\n");
      avro_datum_t currRec = avro_record(avroSchema);
      // avro_datum_t currValue;
      // KeyValueStruct * currAvroStruct = avroStructs[i];
      // switch (currAvroStruct->type) {
      //   case avroType::AVRO_STRING_TYPE :
      //     currValue = avro_string((char *)currAvroStruct->value);
      //   default:
      //     printf("type %s is not supported\n", currAvroStruct->type);
      // }
      // avro_record_set(currRec, currAvroStruct->key, currValue);
      // long int * bufferSize;
      // char * bufferBytes;
      // avro_bytes_get(currRec, & bufferBytes, bufferSize);
      // info.GetReturnValue().Set(Nan::NewBuffer(bufferBytes,
      //                              sizeof(bufferBytes)).ToLocalChecked());
    }
  }
};

NODE_MODULE(NATIVEAVRO, AvroWrap::Init);
