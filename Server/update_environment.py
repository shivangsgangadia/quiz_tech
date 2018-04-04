import sqlite3

dbobj = sqlite3.connect("questions_database.db")
cur = dbobj.cursor()

query = "SELECT attempts FROM questions;"
cur.execute(query)
vals = cur.fetchall()
attempts_hash = ""
for item in vals:
	attempts_hash += str(item[0])

query = 'UPDATE environment SET valu = ' + str('"'+attempts_hash+'"') + ' WHERE envVariables = "attempts_hash";'
cur.execute(query)
dbobj.commit()
