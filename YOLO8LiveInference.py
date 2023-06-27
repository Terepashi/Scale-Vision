import cv2
import time
import os
import pyrebase
import torch
import numpy  
import firebase_admin
from ultralytics import YOLO
from firebase_admin import db
from firebase_admin import credentials


# Firebase
config = {
    "apiKey": "AIzaSyBP8FDAg7z7kjNS44jWiln7U18UOHE49C8",
    "authDomain": "projeksemester3-7951b.firebaseapp.com",
    "databaseURL": "https://projeksemester3-7951b-default-rtdb.firebaseio.com",
    "projectId": "projeksemester3-7951b",
    "storageBucket": "projeksemester3-7951b.appspot.com",
    "messagingSenderId": "841712847324",
    "appId": "1:841712847324:web:2cb7917ae9aed528c12719"
  };
firebase = pyrebase.initialize_app(config)
storage = firebase.storage()
database = firebase.database()

# # Initialize the Firebase app
# cred = credentials.Certificate('C:\Try_Harder\Try_5\projeksemester3-7951b-firebase-adminsdk-v0yaq-1d67075a72.json')
# firebase_admin.initialize_app(cred, {
#     'databaseURL': 'https://projeksemester3-7951b-default-rtdb.firebaseio.com'
# })

# Create a new directory to store the screenshots
if not os.path.exists("screenshots"):
    os.makedirs("screenshots")

# Initialize the camera
cap = cv2.VideoCapture(0)



# Initialize the YOLO model
model = YOLO("best.pt")
# model.predict(source="0", show=True, conf=0.5)




# Initialize the filename and filepath for the screenshot
filename = "screenshot.jpg"
filepath = os.path.join("screenshots", filename)

# Loop to capture images every 3 seconds
while True:
    ret, frame = cap.read()
    if not ret:
        break
    # model.predict(source="0", show=True, conf=0.5)
    # Run object detection on the image
    #print("tes1")
    detections = model.predict(frame, conf=0.5, show = True)
    c=[]
    for i in detections:
        c = i.boxes.cls

    cc = c.numpy()
    classes = detections[0].names

    class_name = ""

    for ii in range(len(cc)):
        class_name = classes[cc[ii]]
        print(class_name)
    

    
    # database.child("ProjekAkhir/class_name").set(class_name)
    database.child("ProjekAkhir/").child("class_name").set(class_name, {'.priority': 'class_name'})
    # database.child("ProjekAkhir/weight/ProjekAkhir").child("class_name").set(class_name)
    # database.child("ProjekAkhir").update({"class_name": class_name})

    # for ii in range(len(cc)):
    #     print(classes[cc[ii]]) 

    # Save the image to the new directory
    cv2.imwrite(filepath, frame)

    # Upload the screenshot to Firebase Storage and overwrite the previous one
    storage.child("screenshots/" + filename).put(filepath)

    # Wait for 3 seconds
    time.sleep(1)

# Release the camera and close all windows
cap.release()
cv2.destroyAllWindows()