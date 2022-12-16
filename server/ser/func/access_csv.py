'''
2022-11-24
MADE BY ADEL<SOLO>   IN T.JUICE(VENI VEDI VICI)

PLEASE WRITE A "conf.json" BEFORE USING

Copyright adel hart <t2amju1ce.official@gmail.com>

'''



import csv #csv 파일 모듈
import os #시스템 모듈
import sys # 시스템 모듈
import json #json 파일 읽기

if len(sys.argv) != 7: #만약 받는 인자(정보)가 7개가 아니면
    print("Lack of Arguments")
    sys.exit() #오류 출력하고 종료

with open('./func/conf.json') as f: #conf.json을 읽어서 경로 가져오기
    dir = json.load(f)['save_dir'] 



if not os.path.exists(dir): #conf.json에 써있는 경로가 존재하지 않으면
    print(dir)
    print("Error of dir, Please check conf.json")
    sys,exit() #오류 출력하고 종료




#argv[0]은 스크립트 코드, 즉 1부터 시작해야 함

grade = sys.argv[1]
clas = sys.argv[2]
num = sys.argv[3]
name = sys.argv[4]
time = sys.argv[5]
month = sys.argv[6] #받은 인자(정보)들을 각 변수에 저장

if os.path.exists(dir+"\\"+month+".csv"): #만약 달.csv가 존재 한다면(이미 파일이 있으면 현재 그 달과 같은 달이므로 계속 이어서 써야함)
    with open(dir+"\\"+month+".csv", 'a', newline='') as fi:
        w = csv.writer(fi)
        w.writerow([grade, clas, num, name, time]) #정보저장

else:
    with open(dir+"\\"+month+".csv", 'w', newline='') as fi: #만약 달.csv가 존재하지 않는다면(달.csv가 없다면, 즉 현재달과 같은 달의 정보가 없음 새로운 달이라 정보를 새로 만들기 이 경우는 11월 1일, 10월 1일 등 달의 시작일때 일 것임.)
        w = csv.writer(fi)
        w.writerow(['학년', '반', '번호', '이름', '먹은 시간'])
        w.writerow([grade, clas, num, name, time]) #정보 설정을 하고 저장
