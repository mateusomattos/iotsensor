import pandas as pd
import requests
from firebase import firebase
from bs4 import BeautifulSoup
from datetime import datetime

firebase = firebase.FirebaseApplication("https://iotsensor-cc9bd.firebaseio.com/")
req = requests.get('https://www.holiday-weather.com/porto/averages/')
today = datetime.today()
month = int(today.month)
print(month)
if req.status_code == 200:
    print('Requisicao bem sucedida!')
    content = req.content

    soup = BeautifulSoup(content, 'html.parser')
    table = soup.find_all(name='table')
    table_str = str(table)
    df = pd.read_html(table_str)[2]
    dfMaxTemp = df.iloc[0,1:]
    dfMinTemp = df.iloc[2,1:]
    maxTemp = dfMaxTemp.values.tolist()
    minTemp = dfMinTemp.values.tolist()
    maxTempMonth = maxTemp[month-1]
    minTempMonth = minTemp[month-1]
    print (maxTemp[month-1])
    print(minTemp[month-1])

    dataTemp = {
        'maxtemp':int(maxTempMonth),
        'mintemp':int(minTempMonth)
    }

    firebase.put('maxTemp/','maxTempMonth',int(maxTempMonth))
    firebase.put('minTemp/','minTempMonth',int(minTempMonth))

