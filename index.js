'use strict'

const {databaseWrapper} = require('bindings')('tkrzw-node')
module.exports = databaseWrapper;

var fs = require('fs');
let tkrzw_config = fs.readFileSync('tkrzw_config.json', 'utf8');


const db1 = new databaseWrapper(tkrzw_config, "YaHeidar.tkh");
db1.set("Heidar(a)", "Amir-al-momenin");
console.log( db1.getSimple("Heidar(a)", "Key not_set") );
console.log( db1.getSimple("non existant key", "Key not_set") );
