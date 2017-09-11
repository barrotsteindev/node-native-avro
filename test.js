const avro = require('./build/Release/native-avro.node');
const fs = require('fs');
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"int\"},\
     {\"name\" : \"d\", \"type\": \"string\"},\
     {\"name\" : \"e\", \"type\": \"int\"}]}";

for (let i = 0; i < 10000000; i++) {
  avro.write(testScehma, {'c': 15, 'd': 'abc', 'e': -10});
}

let avroBuf = avro.write(testScehma, {'c': 1121215, 'd': 'abcdasfas', 'e': -10});

if(fs.existsSync('out.avro')) {
  fs.unlinkSync('out.avro');
}
let writeStream = fs.createWriteStream('out.avro');
writeStream.write(avroBuf);
writeStream.close();
