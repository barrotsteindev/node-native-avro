const avro = require('./build/Release/native-avro.node');
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"int\"},\
     {\"name\" : \"d\", \"type\": \"string\"},\
     {\"name\" : \"e\", \"type\": \"int\"}]}";

for (let i = 0; i < 100000; i++) {
  avro.write(testScehma, {'c': 15, 'd': 'abc', 'e': -10});
}

// let avroBuf = avro.write(testScehma, {'c': 15, 'd': 'abc', 'e': -10});
// setTimeout(() => {console.log(avroBuf.toString())}, 1000);
