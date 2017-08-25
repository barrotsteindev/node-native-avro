const avro = require('./build/Release/native-avro.node');
// const testScehma = "{\"type\":\"record\",\
//   \"name\":\"Test\",\
//   \"fields\":[\
//      {\"name\": \"a\", \"type\": \"string\"}]}";
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"string\"}]}";

console.log(testScehma);

let avroBuf = avro.write(testScehma, {'c': 'ff'});
console.log(avroBuf.toString());
