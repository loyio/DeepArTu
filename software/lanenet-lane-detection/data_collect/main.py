import cv2
import uuid
import os


video="rtsp://192.168.123.84:8080/h264_ulaw.sdp"  #ip摄像头的地址
cap = cv2.VideoCapture(video)
n = 10000
num = 1

while(1):
    ret, frame = cap.read()
    if n > 0 and n % 5 == 0:
        print("capture image: "+str(num))
        imgname = os.path.join("images", '{}.jpg'.format(str(uuid.uuid1())))
        cv2.imwrite(imgname, frame)
        n-=1
        num+=1
    else:
        n-=1
    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()

