const avro = require('./build/Release/native-avro.node');
const fs = require('fs');
const testScehma = "{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"c\", \"type\": \"int\"},\
     {\"name\" : \"d\", \"type\": \"string\"},\
     {\"name\" : \"e\", \"type\": \"int\"}]}";

for (let i = 0; i < 100000; i++) {
//    avro.writeSync(testScehma, {'c': 1121215, 'd': 'abcd', 'e': -10});
    avro.write(testScehma, {'c': 1121215, 'd': 'abcd', 'e': -10}, (err, buf) => {
               if (err) {
                   return console.log('error: ' + err);
               }
    });
}

let avroBuf = avro.writeSync(testScehma, {'c': 1121215, 'd': 'abcd', 'e': -10});

if(fs.existsSync('out.avro')) {
   fs.unlinkSync('out.avro');
 }
let writeStream = fs.createWriteStream('out.avro');
writeStream.write(avroBuf);
writeStream.close();
