const express = require('express');
const maria = require('./database/connect/maria');
const PythonShell = require('python-shell'); //기타
const today = new Date();




const app = express(); //서버 설정
const port = 4885;

const schedule = require('node-schedule'); //스케쥴랑 솔종

app.use(express.json()); 
app.use(express.urlencoded({ extended: false })); //json 파싱 설정

maria.connect(); //DB 설정

const doit = schedule.scheduleJob('0 0 23 * * *', ()=>{
    maria.query("UPDATE data SET IS_EAT=0");
})  //메일 11시에 IS_EAT 초기화

function csv(grade, clas, num, name, time, month){ //CSV 저장 함수 호출(python으로)
    var options = { mode: 'text',   pythonPath: '', pythonOptions: ['-u'], scriptPath: '',  args: [grade, clas, num, name, time, month]}

    PythonShell.PythonShell.run('./func/access_csv.py', options, (err) => {
        if(err) throw err;
    })
}



app.post('/check', (req, res) => {   //메인, tmi.일주일 동안 404 not found떠서 패킷 뜯어가면서 해맷는데 사실 이걸 get으로 써서 그랬다 ㅋㅋ
    var uid = req.body.uid;
    maria.query('SELECT * FROM data WHERE UID = ' + uid+' limit 1', (err, resu, fields) =>{ //람다식~

        if(resu.length  > 0) {
            
            if(resu[0].IS_EAT == 1){ // 이미 먹었는데 또 찍으면
                res.send('%2^');
            }
            else{
                res.send('%1^'); //아두이노 측에서 파싱하기 쉽게 구분 문자 삽입!
                maria.query('UPDATE data SET IS_EAT=1 WHERE UID = '+uid, (err) => {
                    if(err) {console.log("somethis error in writing in_eat");
                } else {
                    console.log(`check successfuly ${uid} and successfully update is_eat`);

                    let year = today.getFullYear(); // 년도
                    let month = today.getMonth() + 1;  // 월
                    let date = today.getDate();  // 날짜

                    let hours = today.getHours(); // 시
                    let minutes = today.getMinutes();  // 분
                    let seconds = today.getSeconds();  // 초

                    let res_time = year+"/"+month+"/"+date+" "+hours+":"+minutes+":"+seconds;

                    csv(resu[0].GRADE, resu[0].CLASS, resu[0].NUM, resu[0].NAME, res_time, month);
                    
                }
                });
            }
            
        } else {
            console.log(`not join ${uid}`);
            res.send('%0^');
        }
        

    });

});

app.post('/test', (req, res) => {
    console.log(req.body.uid);
});

app.listen(port);