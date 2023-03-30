from flask import *
import pymysql

app = Flask(__name__)
con = pymysql.connect(host='localhost', port=3306, user='root', password='root', db='demoreg')
cmd = con.cursor()


app.secret_key = "my_secret_key"


@app.route('/')
def home():
    return render_template('index.html')

@app.route('/view', methods=['GET', 'POST'])
def view():
    cmd.execute("SELECT * FROM studinfo ")
    row_headers = [x[0] for x in cmd.description]
    print(row_headers)
    view = cmd.fetchall()
    json_data = []
    for result in view:
        json_data.append(dict(zip(row_headers, result)))
    con.commit()
    return jsonify(json_data)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)