const avro = require('avsc');

const type = avro.Type.forSchema({
  type: 'record',
  fields: [
    {name: 'c', type: 'int'},
    {name: 'd', type: 'string'},
    {name: 'e', type: 'int'}
  ]
});

for (let i = 0; i < 100000; i++) {
  type.toBuffer({'c': 1121215, 'd': 'abcd', 'e': -10});
}
