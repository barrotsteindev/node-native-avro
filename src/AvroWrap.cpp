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
        if (structArray[i] == NULL) {
          printf("err");
          return;
        }
      }
  }

  static KeyValueStruct * ParseToKeyValStruct(v8::Local<v8::String> & keyName,
    v8::Local<v8::Value> & keyValue) {
      KeyValueStruct * currStruct = (KeyValueStruct *)
                                     malloc(sizeof(KeyValueStruct));
      if (currStruct == NULL) {
        return NULL;
      }
      const char * key = v8StringtoC_str(keyName).c_str();
      currStruct->key = (char *)malloc(sizeof(key));
      strcpy(currStruct->key, key);
      if (keyValue->IsString()) {
        printf("string\n");
        currStruct->type = AVRO_STRING_TYPE;
        const char * value = v8StringtoC_str(keyValue->ToString()).c_str();
        char * value_ptr = (char *)malloc(sizeof(value));
        strcpy(value_ptr, value);
        currStruct->value = (const void *)value_ptr;
      } else if (keyValue->IsNumber()) {
        currStruct->type = AVRO_INT_TYPE;
        if (keyValue->IsInt32()) {
          void * value_ptr = malloc(sizeof(int *));
          int32_t intValue = keyValue->Int32Value();
          memcpy(value_ptr, (void *)&intValue, sizeof(int *));
          currStruct->value = (const void *)value_ptr;
        } else if (keyValue->IsUint32()) {
          uint * value_ptr = (uint *)malloc(sizeof(uint));
          value_ptr = (uint *)keyValue->Uint32Value();
          currStruct->value = (const void *)value_ptr;
        }
      } else {
        printf("unsportted type in key: %s, skipping\n", currStruct->key);
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
    KeyValueStruct * * avroStructs = (KeyValueStruct * *)
                                     malloc(keyLength * sizeof(KeyValueStruct));
    if (avroStructs == NULL) {
      return Nan::ThrowError("could not allocate memory");
    }
    V8ObjToStructArray(avroStructs, keyLength, keyNames, json);
    avro_schema_t avroSchema = NULL;
    size_t sizeOfRecord = 0;
    const char * strSchema = schema.c_str();
    if (!init_schema(strSchema, & avroSchema)) {
       return Nan::ThrowError("could not create avro schema");
    }
    avro_value_iface_t * iface = avro_generic_class_from_schema(avroSchema);
    avro_value_t avroRecord;
    avro_generic_value_new(iface, & avroRecord);
    for (int i = 0; i < keyLength; i++) {
      KeyValueStruct * currAvroStruct = avroStructs[i];
      enum avroType currType = currAvroStruct->type;
      avro_value_t curVal;
      size_t avroSize;
      avro_value_get_by_name(& avroRecord, currAvroStruct->key,
                             & curVal, & avroSize);
      if (currType == AVRO_STRING_TYPE) {
        if (avro_value_set_string(& curVal, (char *)currAvroStruct->value)) {
          return Nan::ThrowError("could not serialize key");
        }
      } else if (currType == AVRO_INT_TYPE) {
        if (avro_value_set_int(& curVal, *(int32_t *)currAvroStruct->value)) {
          return Nan::ThrowError("could not serialize key");
        }
      }
      delete_struct(currAvroStruct);
    }
    free(avroStructs);
    avro_value_sizeof(& avroRecord, & sizeOfRecord);
    char * buf = (char *)malloc(sizeOfRecord + 1);
    printf("%d\n", sizeOfRecord);
    avro_writer_t avroMemoryWriter = avro_writer_memory(buf, sizeOfRecord);
    avro_file_writer_t file_writer;
    FILE memStream;
    FILE * fStream = fopen("test.avro", "w+");
    if (avro_file_writer_create_fp(fStream, "", 1, avroSchema, & file_writer)) {
      printf("err: %s\n", avro_strerror());
    }

    // avro_writer_file(& memStream);
    // size_t memSize;
    printf("gonna write\n");
    avro_file_writer_append_value(file_writer, & avroRecord);
    printf("written\n");
    avro_file_writer_close(file_writer);
    avro_value_write(avroMemoryWriter, & avroRecord);
    // if (!WriteBufToStream(& memStream, buf, avro_writer_tell(avroMemoryWriter))) {
    //   return Nan::ThrowError("could not create memory stream");
    // }
    info.GetReturnValue().Set(Nan::NewBuffer(buf,
                                 sizeOfRecord).ToLocalChecked());
    avro_value_iface_decref(iface);
    avro_schema_decref(avroSchema);
  }
};

NODE_MODULE(NATIVEAVRO, AvroWrap::Init);
