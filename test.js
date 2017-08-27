const avro = require('./build/Release/native-avro.node');
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"string\"},\
     {\"name\" : \"d\", \"type\": \"string\"}]}";

let avroBuf = avro.write(testScehma, {'c': 'ff', 'd': 'aabcd'});
console.log(avroBuf.toString());
