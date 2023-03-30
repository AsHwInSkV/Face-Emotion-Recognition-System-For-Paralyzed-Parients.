import pymysql
from flask import *
app=Flask(__name__)
con=pymysql.connect(host='localhost',port=3306,user='root',password='root',db='demoreg')
cmd=con.cursor()

@app.route('/test', methods=['POST'])
def test():
    val = str(request.get_data())
    split = str.split(val , "'")
    v = split[1]
    split=str.split(v, ",")
    a = split[0]
    b = split[1]
    print(a)
    print(b)
    cmd.execute("INSERT INTO studinfo VALUES (NULL,'" + a + "','" + b + "',curdate(),curtime())")
    con.commit()
    return "ok"

if __name__ == '__main__':
    app.run(host='0.0.0.0',port=5000)
