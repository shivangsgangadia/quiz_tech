'''def process_request(request):
	req_tuple = request.split(';')
	if req_tuple[0]=='g':
		if req_tuple[1]=='c':
			tid = req_tuple[2]
'''
import sqlite3
from http.server import BaseHTTPRequestHandler, HTTPServer

dbobj = sqlite3.connect("questions_database.db")
cur = dbobj.cursor()

#DATA FROM THE TEAMS DATABASE

def get_from_teams(col, tid):
	query = 'SELECT '+str(col)+' FROM teams WHERE team_id="'+str(tid)+'";'
	cur.execute(query)
	ans = (cur.fetchone())[0]
	return ans

def get_attempts_left(tid,n):
	query = 'SELECT attempts_left FROM teams WHERE team_id="'+str(tid)+'";'
	cur.execute(query)
	q = (cur.fetchone())[0]
	ans = str(q[n-1])
	return ans
	
def set_attempts_left(tid,n,a):									#Add a to current attempts of given question n
	query = 'SELECT attempts_left FROM teams WHERE team_id="'+str(tid)+'";'
	cur.execute(query)
	q = (cur.fetchone())[0]
	mod_str = q[0:(n-1)]
	current_attempts = str(q[n-1])
	if(current_attempts != 'i' or current_attempts != '0'):
		curr_att_int = int(current_attempts) + a
		mod_str += str(curr_att_int)
		mod_str += q[n:]
		query = "UPDATE teams SET attempts_left="+str('"'+mod_str+'"')+"WHERE team_id="+str('"'+tid+'"')+";"
		cur.execute(query)
	elif(current_attempts == 'i'):
		mod_str += '0'
		mod_str += q[n:]
		query = "UPDATE teams SET attempts_left="+str('"'+mod_str+'"')+"WHERE team_id="+str('"'+tid+'"')+";"
		cur.execute(query)
	dbobj.commit()
	
def set_current_ques_no(tid,n):
	query = "UPDATE teams SET current_ques_no="+str(n)+" WHERE team_id="+str('"'+tid+'"')+";"
	cur.execute(query)
	dbobj.commit()
	
def update_points(tid, points, i):
	curr_points = get_from_teams("points",tid)
	#curr_points = get_points(tid)
	new_points = 0
	if i==1:										#Add points
		new_points = curr_points + points
	elif i==0:										#Reduce points
		new_points = curr_points - points
	query = "UPDATE teams SET points="+str(new_points)+" WHERE team_id="+str('"'+tid+'"')+";"
	cur.execute(query)
	dbobj.commit()

#DATA FROM THE QUESTIONS DATABASE

def get_from_questions(col, sr):
	query = 'SELECT '+str(col)+' FROM questions WHERE sr="'+str(sr)+'";'
	cur.execute(query)
	ans = (cur.fetchone())[0]
	return ans

def get_question_text(qno):
	query = "SELECT path FROM questions WHERE sr="+str(qno)+";"
	cur.execute(query)
	path = str((cur.fetchone())[0])
	fp = open(path,"r")
	q_text = fp.read()
	fp.close()
	return q_text
	

#Login and Register
def try_login(tid):
	query = "SELECT sr FROM teams WHERE team_id="+str('"'+tid+'"')+";"
	cur.execute(query)
	no = cur.fetchone()
	if(no):
		return("s")			#Team is registered
	else:
		return("f")			#Team is not registered
	dbobj.commit()

def try_register(tid):
	query = "SELECT sr FROM teams WHERE team_id="+str('"'+tid+'"')+";"
	cur.execute(query)
	no = cur.fetchone()
	if(no):
		return("f")		#Team already exists
	else:
		q = 'SELECT valu FROM environment WHERE envVariables="attempts_hash";'
		cur.execute(q)
		attempts_hash = (cur.fetchone())[0]
		in_tuple = ((tid,attempts_hash),)
		q = "INSERT INTO teams (team_id,attempts_left) VALUES(?,?);"
		cur.executemany(q,in_tuple)
		return("s")		#Successfully registered
	dbobj.commit()

#Main processor Logic
def process_request(req):
	response = ""
	req = req.strip('(')
	req = req.strip(')')
	req_array = req.split(';')
	
	if req_array[0]=='l':					#Trying login
		a = try_login(str(req_array[1]))
		if(a=='s'):
			response = "S;" + str(get_from_teams("points", str(req_array[1])))
		elif(a=='f'):
			response = "N"
		else:
			print("Some other error....please check request type..")
			response = "F"
	
	elif req_array[0]=='r':					#Trying registeration
		a = try_register(str(req_array[1]))
		if a=='s':
			response = "S"
		elif(a=='f'):
			response = "E"
		else:
			print("Some other error....please check request type..")
			response = "F"
	
	elif req_array[0]=='q':
		n=0
		if req_array[1]=='c':
			n = int(get_from_teams("current_ques_no", str(req_array[2])))		#Get the current question number
		else:
			n = int(req_array[1])
			set_current_ques_no(str(req_array[2]), n)
		ques_title = get_from_questions("title",n)		
		ques_text = get_question_text(n)
		ques_diff = get_from_questions("difficulty", n)
		att_left = get_attempts_left(str(req_array[2]), n)
		response = ques_title + ";" + ques_text + ";" +ques_diff+ ";" +str(att_left)
	
	elif req_array[0]=='a':
		tid = str(req_array[1])
		q_no = int(get_from_teams("current_ques_no", tid))
		sub_ans = str(req_array[2])
		exp_ans = str(get_from_questions("answer", q_no))
		
		if(get_attempts_left(tid, q_no)=='0'):				#If attempts are over or question is already answered
			response = "x"
			
		elif((sub_ans==exp_ans) and get_attempts_left(tid, q_no)!='0'):								#Answer is correct
			get_points = int(get_from_questions("points", q_no))
			update_points(tid, get_points, 1)
			curr_attempts = int(get_attempts_left(tid, q_no))
			set_attempts_left(tid, q_no, -curr_attempts)
			response = "c;"+str(get_from_teams("points",tid))
			
		elif(sub_ans!=exp_ans):							#Answer is not correct
			set_attempts_left(tid, q_no, -1)
			response = "w;"+str(get_attempts_left(tid,q_no))
		else:
			response = "Error"
	else:
		response = "Error"
	dbobj.commit()		
	return response


#The server code
from http.server import BaseHTTPRequestHandler, HTTPServer

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        req = (self.path).strip("/")
        self._set_response()
        self.wfile.write(process_request(req).encode('utf-8'))

def run(server_class=HTTPServer, handler_class=S, port=8080):
    server_address = ('127.0.0.1', port)
    httpd = server_class(server_address, handler_class)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()

		


#Testing data from Teams
def test_1():
	print("\nChecking for team id team1: ")
	print("\nCurrent question number: "+str(get_from_teams("current_ques_no","team1")))
	print("\nUnlocked Upto: "+str(get_from_teams("unlocked_upto","team1")))
	print("\nPoints: "+str(get_from_teams("points","team1")))
	print("\nAttempts left for current question: "+str(get_attempts_left("team1")))
	update_points("team1",100,1)
	print("\nIncrementing points by 100 and printing points again: "+str(get_from_teams("points","team1")))
	update_points("team1",100,0)
	print("\nDecrementing points by 100 and printing points again: "+str(get_from_teams("points","team1")))

#Testing data from questions
def test_2():
	print("\nChecking for questions no 1: ")
	print("\nDifficulty of question: "+get_from_questions("difficulty",1))
	print("\nStatement of question: "+get_question_text(1))
	print("\nAnswer of question: "+get_from_questions("answer",1))
	print("\nPoints of question: "+str(get_from_questions("points",1)))

#Testing login systems
#LOGIN
def test_3():
	x = try_login("team1")
	if(x=='s'):
		print("Login Successful.")
	elif(x=='f'):
		print("User Not Found.");

#REGISTER
def test_4():
	x = try_register("team2")
	if(x=='f'):
		print("Team registeration failed! team already exists!")
	elif(x=='s'):
		print("Registration Successful!")
		
#Testing the main processor logic
def test_5():
	print(process_request("(l;team1)"))
	print(process_request("(r;team3)"))
	print(process_request("(q;c;team1)"))
	print(process_request("(q;1;team1)"))
	print(process_request("(a;team1;23123123)"))
	

#test_5()
#dbobj.commit()
run()
dbobj.close()

