'use strict'

const {databaseWrapper} = require('bindings')('tkrzw-node')
module.exports = databaseWrapper;

var fs = require('fs');
let tkrzw_config = fs.readFileSync('tkrzw_config.json', 'utf8');


const db1 = new databaseWrapper(tkrzw_config, "YaHeidar.tkh");
Promise.allSettled([
db1.set("Heidar(a)", "Amir-al-momenin").then(()=>console.log("SET1 DONE!")),
db1.set("Heidar(b)", "Amir-al-momenin").then(()=>console.log("SET2 DONE!")),
db1.getSimple("Heidar(b)", "Key not_set").then((val)=>console.log("GET3: ",val)),
db1.getSimple("Heidar(a)", "Key not_set").then((val)=>console.log("GET1: ",val)),
db1.getSimple("non existant key", "Key not_set").then((val)=>console.log("GET2: ",val)).catch((val)=>console.log("GET2: ",val))
]).then(()=>db1.close());
