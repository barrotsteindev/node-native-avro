/*
 * node-native-avro - Node.js wrapper for Avro-C library

 */

'use strict';

 const nativeAvro = require('../build/Release/native-avro.node');

 function writeFromJson(json) {
   avro_schema = {"type": "record"};
   for (key in Object.keys(json)) {
     avro_schema[key]
   }
 }

 module.exports = { write: nativeAvro.write };
