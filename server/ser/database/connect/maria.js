const maria = require('mysql');



const cone = maria.createConnection({ //이 아래에 있는 4개의 코드를 각 환경에 맞게 수정하세요.
    host:'localhost', //수정 X, 자신의 컴퓨터를 의미
    port:3306, //데이터베이스 설치할때 설정한 포트
    user:'root', //데이터베이스 사용자의 이름
    password:'sm2022!@', //그 사용자의 비밀번호
    database:'GupSik' //데이터베이스의 이름
});

module.exports = cone;