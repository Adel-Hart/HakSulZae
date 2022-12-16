/*
made by ADEL

Copyright adel hart <t2amju1ce.official@gmail.com>

*/



const express = require('express'); //express 모듈 사용 선언
const maria = require('./database/connect/maria'); //마리아 모듈 사용 선언
const PythonShell = require('python-shell'); //기타 모듈 설정
const today = new Date(); //현 시간을 받기위한 설정


const app = express(); //서버 설정
const port = 4885; //서버가 열릴 포트

const schedule = require('node-schedule'); //스케듈링 설정
const e = require('express');

app.use(express.json()); 
app.use(express.urlencoded({ extended: false })); //json 값 가져오기 설정

maria.connect(); //데이터베이스 설정

const doit = schedule.scheduleJob('0 0 23 * * *', ()=>{ //(저 숫자는 '초 분 시간 일 달 년' 순서입니다) 23시에,
    maria.query("UPDATE data SET IS_EAT=0"); //데이터베이스의 IS_EAT항목을 모두 0으로 하세요
    console.log("IS_EAT Initialize Compelete.") //완료 메세지
})  //메일 11시에 IS_EAT 초기화

function csv(grade, clas, num, name, time, month){ //CSV 저장 함수 호출(python으로)
    var options = { mode: 'text',   pythonPath: '', pythonOptions: ['-u'], scriptPath: '',  args: [grade, clas, num, name, time, month]} //옵션 설정(args : [학년, 반, 번, 이름, 시간, 달]을 실행하면서 전송합니다)

    PythonShell.PythonShell.run('./func/access_csv.py', options, (err) => { //위 코드를 실행 및 데이터 전송(위의 args)
        if(err) throw err; //에러나면 에러 메세지 출력
    })
}



app.post('/check', (req, res) => {   //메인, tmi.일주일 동안 404 not found떠서 패킷 뜯어가면서 해맷는데 사실 이걸 get으로 써서 그랬다 ㅋㅋ
    var uid = req.body.uid; //받은 데이터의 uid 항목을 가져옵니다. (!아두이노 코드중 패킷을 보내는 항목에서 데이터 전송부분에 있습니다!)
    maria.query('SELECT * FROM data WHERE UID = ' + uid+' limit 1', (err, resu, fields) =>{ //uid에 맞는 항목을 데이터베이스에서 찾습니다
        try{ //오류 감지
            if(resu.length  > 0) { //찾았을때 결과 값이 존재하면(길이가 0보다 크면)
                
                if(resu[0].IS_EAT == 1){ //IS_EAT 항목이 1이면 즉, 이미 먹었는데 또 찍으면
                    res.send('%2^'); //아두이노에게 %2^를 전송
                }
                else{
                    res.send('%1^'); //이미 먹었는데 또 찍은게 아니면(즉, 먹는게 맞으면) %1^전송
                    maria.query('UPDATE data SET IS_EAT=1 WHERE UID = '+uid, (err) => { //데이터베이스 IS_EAT 항목을 1로 (중복 사용 방지)
                        if(err) {console.log("somethis error in writing in_eat"); //에러나면 에러 출력
                    } else {
                        console.log(`check successfuly ${uid} and successfully update is_eat`);

                        let year = today.getFullYear(); // 년도 
                        let month = today.getMonth() + 1;  // 월
                        let date = today.getDate();  // 날짜

                        let hours = today.getHours(); // 시
                        let minutes = today.getMinutes();  // 분
                        let seconds = today.getSeconds();  // 초를 가져옴

                        let res_time = year+"/"+month+"/"+date+" "+hours+":"+minutes+":"+seconds; //이를 예쁘게 다듬기

                        csv(resu[0].GRADE, resu[0].CLASS, resu[0].NUM, resu[0].NAME, res_time, month); //위에서 만든 csv함수로 호출(csv 저장함수)
                        
                    }
                    });
                }
                
            } else { //만약 결과 값이 존재하지 않는다면(유저 정보가 데이터베이스에 없다면)
                console.log(`not join ${uid}`); //콘솔 출력
                res.send('%0^'); //%0^전송
            }
        
        } catch (e){
            if(e.name == "TypeError")   res.send('Please send int'); //만약 형식 오류가 나면(외부에서 접속시 매우 드뭄) 오류나면 오류 출력
        }
    });

});

app.post('/test', (req, res) => {
    console.log(req.body.uid);
});

app.listen(port, ()=>{ //서버 작동
    console.log("Server is listening on gho.kro.kr:4885");
});


//%1^이런 식으로 전송하는 이유 >> 아두이노 측에서 %와 ^를 기준으로 그 가운데 즉 1,2,0 값을 얻기 위해서임.
