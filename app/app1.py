import threading

import pymysql
from flask import *
import cv2
from deepface import DeepFace

emotion = "Normal"
faceCascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

app = Flask(__name__)

con = pymysql.connect(host='localhost', port=3306, user='root', password='root', db='patient_monitoring')
cmd = con.cursor()


@app.route('/test', methods=['POST'])
def test():
    global emotion
    print("emotion:   ", emotion)
    val = str(request.get_data())
    split = str.split(val, "'")
    v = split[1]
    split = str.split(v, ",")
    a = split[0]
    b = split[1]
    print(a)
    print(b)
    cmd.execute("INSERT INTO readings VALUES (NULL,'" + b + "','" + a + "','" + emotion + "',curdate(),curtime())")
    con.commit()
    return "ok"


@app.route('/view', methods=['get', 'post'])
def view():
    cmd.execute("select * from readings order by id desc")
    row_headers = [x[0] for x in cmd.description]
    s = cmd.fetchall()
    json_data = []
    for result in s:
        json_data.append(dict(zip(row_headers, result)))
    con.commit()
    return jsonify(json_data)


def flsk():
    app.run(host='0.0.0.0', port=5000)


def main():
    global emotion
    threading.Thread(target=flsk).start()
    vid = cv2.VideoCapture(0)
    if not vid.isOpened():
        raise IOError("Cannot open Webcam")

    while True:
        ret, frame = vid.read()

        result = DeepFace.analyze(frame, actions=['emotion'], enforce_detection=False)[0]

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(gray, 1.1, 4)

        for (x, y, w, h) in faces:
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 1)

            font = cv2.FONT_HERSHEY_SIMPLEX
            emotion = result['dominant_emotion']

            cv2.putText(frame, emotion, (int(x), int(y)), font, 1, (0, 255, 0), 2, cv2.LINE_4)

        cv2.imshow('Video', frame)

        if cv2.waitKey(2) & 0xFF == ord('q'):
            break

    vid.release()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
