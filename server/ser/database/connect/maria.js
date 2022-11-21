const maria = require('mysql');

const cone = maria.createConnection({
    host:'localhost',
    port:3306,
    user:'root',
    password:'sm2022!@',
    database:'GupSik'
});

module.exports = cone;