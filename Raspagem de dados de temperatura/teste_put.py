import pandas as pd
import requests
from firebase import firebase
from bs4 import BeautifulSoup
from datetime import datetime
import random
import time

firebase = firebase.FirebaseApplication("https://iotsensor-cc9bd.firebaseio.com/")

while 1:
    temp1 = random.randint(0,30)
    temp2 = random.randint(0,30)
    hum1 = random.randint(0,100)
    hum2 = random.randint(0,100)
    firebase.post('temperature/',temp1)
    firebase.post('temperature2/',temp2)
    firebase.post('humidity/',hum1)
    firebase.post('humidity2/',hum2)
    print("dados enviados!")

    time.sleep(3)

