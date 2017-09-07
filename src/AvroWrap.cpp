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

      if (keyValue->IsString()) {
        const char * value = v8StringtoC_str(keyValue->ToString());
        char * value_ptr = (char *)malloc(sizeof(value));
        strcpy(value_ptr, value);
        if (!createStruct(currStruct, key, AVRO_STRING_TYPE, value_ptr)) {
          return NULL;
        }
      } else if (keyValue->IsNumber()) {
        currStruct->type = AVRO_INT_TYPE;
        if (keyValue->IsInt32()) {
          void * value_ptr = malloc(sizeof(int *));
          int32_t intValue = keyValue->Int32Value();
          memcpy(value_ptr, (void *)& intValue, sizeof(int *));
          if (!createStruct(currStruct, key, AVRO_INT_TYPE, value_ptr)) {
            return NULL;
          }
        } else if (keyValue->IsUint32()) {
          uint * value_ptr = (uint *)malloc(sizeof(uint));
          value_ptr = (uint *)keyValue->Uint32Value();
          currStruct->value = (const void *)value_ptr;
        }
      } else {
        printf("unsportted type in key: %s, skipping\n", currStruct->key);
        return NULL;
      }
      return currStruct;
  }

  static NAN_METHOD(Write) {
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
    avro_schema_t avroSchema;
    const char * strSchema = * Nan::Utf8String(info[0]->ToString());
    if (!init_schema(strSchema, & avroSchema)) {
       return Nan::ThrowError("could not create avro schema");
    }
    avro_value_iface_t * iface = avro_generic_class_from_schema(avroSchema);
    avro_value_t avroRecord;
    avro_generic_value_new(iface, & avroRecord);
    for (int i = 0; i < keyLength; i++) {
      KeyValueStruct * currAvroStruct = avroStructs[i];
      if (!InsertToAvroRecord(currAvroStruct, & avroRecord)) {
        return Nan::ThrowError("could not serialize to avro");
      }
      delete_struct(currAvroStruct);
    }
    free(avroStructs);
    avro_file_writer_t file_writer;
    FILE * fStream = fopen("out.avro", "w+");
    if (avro_file_writer_create_fp(fStream, "", 1, avroSchema, & file_writer)) {
      return Nan::ThrowError("Could not create memory stream");
    }
    avro_file_writer_append_value(file_writer, & avroRecord);
    avro_file_writer_close(file_writer);
    // info.GetReturnValue().Set(Nan::NewBuffer(buf,
    //                              sizeOfRecord).ToLocalChecked());
    avro_value_decref(& avroRecord);
    avro_value_iface_decref(iface);
    avro_schema_decref(avroSchema);
  }
};

NODE_MODULE(NATIVEAVRO, AvroWrap::Init);
