import cv2
from deepface import DeepFace

faceCascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

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

        cv2.putText(frame,emotion, (int(x), int(y)), font, 1, (0, 255, 0), 2, cv2.LINE_4)

    cv2.imshow('Video', frame)

    if cv2.waitKey(2) & 0xFF == ord('q'):
        break

vid.release()
cv2.destroyAllWindows()
