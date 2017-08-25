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

  static void V8ObjToStructArray(KeyValueStruct * structArray,
    const int & numberOfKeys, const v8::Local<v8::Array> keyList,
    const v8::Local<v8::Object> obj) {
      for (int i = 0; i < numberOfKeys; i++) {
        v8::Local<v8::String> keyName = keyList->Get(i)->ToString();
        v8::Local<v8::Value> keyValue = obj->Get(keyName);
        structArray[i] = ParseToKeyValStruct(keyName, keyValue);
      }
  }

  static KeyValueStruct ParseToKeyValStruct(v8::Local<v8::String> & keyName,
    v8::Local<v8::Value> & keyValue) {
      KeyValueStruct currStruct;
      currStruct.key = (const char *)v8StringtoC_str(keyName).c_str();
      if (keyValue->IsString()) {
        currStruct.type = AVRO_STRING_TYPE;
        currStruct.value = (const void *)v8StringtoC_str(keyValue->ToString())
                                                              .c_str();
      } else {
        printf("unsportted type in key: %s, skipping\n", currStruct.key);
      }
      return currStruct;
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
    KeyValueStruct * avroStructs = (KeyValueStruct *)
                                     malloc(keyLength * sizeof(KeyValueStruct));
    V8ObjToStructArray(avroStructs, keyLength, keyNames, json);
    avro_schema_t avroSchema;
    if (!init_schema(schema.c_str(), & avroSchema)) {
       return Nan::ThrowError("could not create avro schema");
    }
    avro_value_iface_t * iface = avro_generic_class_from_schema(avroSchema);
    avro_value_t avroRecord;
    avro_generic_value_new(iface, & avroRecord);
    for (int i = 0; i < keyLength; i++) {
      KeyValueStruct currAvroStruct = avroStructs[i];
      enum avroType currType = currAvroStruct.type;
      avro_value_t curVal;
      size_t avroSize;
      avro_value_get_by_name(& avroRecord, currAvroStruct.key,
                             & curVal, & avroSize);
      if (currType == AVRO_STRING_TYPE) {
        if (avro_value_set_string(& curKey, (char *)currAvroStruct.value)) {
          return Nan::ThrowError("could not serialize key");
        }
      }
      avro_value_sizeof(& curKey, & avroSize);
      char * buf = (char *)malloc(avroSize);
      printf("%d\n", avroSize);
      avro_writer_t avroMemoryWriter = avro_writer_memory(buf, avroSize);
      avro_value_write(avroMemoryWriter, & val);
      printf("%s\n", buf);

      info.GetReturnValue().Set(Nan::NewBuffer(buf,
                                   sizeof(buf)).ToLocalChecked());
    }
  }
};

NODE_MODULE(NATIVEAVRO, AvroWrap::Init);
