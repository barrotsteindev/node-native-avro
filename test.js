const avro = require('./build/Release/native-avro.node');
// const testScehma = "{\"type\":\"record\",\
//   \"name\":\"Person\",\
//   \"fields\":[\
//      {\"name\": \"c\", \"type\": \"string\"},\
//      {\"name\" : \"d\", \"type\": \"string\"},\
//      {\"name\" : \"e\", \"type\": \"int\"}]}";
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"int\"}]}";

let avroBuf = avro.write(testScehma, {'c': 15});
setTimeout(() => {console.log(avroBuf.toString())}, 1000);
