'''
2022-11-24
MADE BY ADEL<SOLO>   IN TEAM_JUICE(VENI VEDI VICI)

PLEASE WRITE A "conf.json" BEFORE USING

LICENSE FOLLOW  THE LICENSE CLARIFIED ON  GITHUB

'''



import csv
import os
import sys
import json #json 파일 읽기

if len(sys.argv) != 7: 
    print("Lack of Arguments")
    sys.exit()

with open('./func/conf.json') as f:
    dir = json.load(f)['save_dir'] 



if not os.path.exists(dir):
    print(dir)
    print("Error of dir, Please check conf.json")
    sys,exit()




#argv[0]은 스크립트 코드, 즉 1부터 시작해야 함

grade = sys.argv[1]
clas = sys.argv[2]
num = sys.argv[3]
name = sys.argv[4]
time = sys.argv[5]
month = sys.argv[6]

if os.path.exists(dir+"\\"+month+".csv"):
    with open(dir+"\\"+month+".csv", 'a', newline='') as fi:
        w = csv.writer(fi)
        w.writerow([grade, clas, num, name, time])

else:
    with open(dir+"\\"+month+".csv", 'w', newline='') as fi:
        w = csv.writer(fi)
        w.writerow(['학년', '반', '번호', '이름', '먹은 시간'])
        w.writerow([grade, clas, num, name, time])
