import argparse
import os
import sqlite3

p = argparse.ArgumentParser()
p.add_argument("ques_title",help = "Title of the question", type=str)
p.add_argument("ques_file_path",help = "Path of txt file containing question", type=str)
p.add_argument("ques_difficulty", help = "Difficulty level of question as e - easy, m - medium, h - hard, b - bonus ", type=str)
p.add_argument("ques_points", help = "Points worth of the question", type=int)
p.add_argument("ques_answer", help = "Answer of the question", type=str)
p.add_argument("attempts", help = "Attempts allowed for the question", type=str)
s = p.parse_args()

ques_title = s.ques_title
file_path = s.ques_file_path 
difficulty = s.ques_difficulty
points = str(s.ques_points)
answer = str(s.ques_answer)
attempts = s.attempts

#Read the question Text from given filepath
f = open(file_path, 'r')
ques_text_ini = f.read()
f.close()

ques_text = ""

#Validate question text by removing ';'
for item in ques_text_ini:
	if item==';':
		ques_text += "."
	else:
		ques_text += item

#Generate filename
x=1
filename = "./questions/"+str(x) + ".txt"
while(os.path.isfile(filename)):
	x+=1
	filename = "./questions/"+str(x) + ".txt"

#Copy question text to new filename
f2 = open(filename,'w')
f2.write(ques_text)
f2.close()

#Generate and Execute Query
q_fill = ((difficulty, filename, answer, points, attempts,ques_title),)
query = "INSERT INTO questions VALUES(NULL,?,?,?,?,?,?);"

dbobj = sqlite3.connect("questions_database.db")
cur = dbobj.cursor()
cur.executemany(query,q_fill)
dbobj.commit()
dbobj.close()
