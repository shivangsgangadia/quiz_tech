Requirements:
-sqlite3
-python3


How to use?
It is currently configured to run at IP: 127.0.0.1
Port: 8080
Just run :
$ python3 serve_and_process_request.py

Uses of other scripts:

1: add_question.py
Used to add questions to the database.
$ python3 add_question.py QuestionTitle /path/of/question/file difficulty points answer attempts

-here difficulty: e -> easy
		  m -> medium
		  h -> hard
		  b -> bonus

      attempts:   numeral for finite attempts
		  'i' for infinite attempts
2: update_environment.py
Used to update the "environment" table containing the hash table for attempts
$ python3 update_environment.py



Schema/Defination for database tables: 
1: questions
$ CREATE TABLE questions(sr INTEGER PRIMARY KEY, difficulty CHAR, path TEXT, answer TEXT, points INT, attempts CHAR);

2: teams
$ CREATE TABLE teams(sr INTEGER PRIMARY KEY, team_id TEXT, current_ques_no INT, unlocked_upto INT, points INT, attempts_left TEXT);

