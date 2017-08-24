const avro = require('./build/Release/native-avro.node');
// const testScehma = "{\"type\":\"record\",\
//   \"name\":\"Test\",\
//   \"fields\":[\
//      {\"name\": \"a\", \"type\": \"string\"}]}";
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"ID\", \"type\": \"long\"},\
     {\"name\": \"First\", \"type\": \"string\"},\
     {\"name\": \"Last\", \"type\": \"string\"},\
     {\"name\": \"Phone\", \"type\": \"string\"},\
     {\"name\": \"Age\", \"type\": \"int\"}]}";

console.log(testScehma);

avro.write(testScehma, {'a': 'b'});
