import matplotlib.pyplot as plt
from datetime import datetime
import numpy as np
import pandas as pd

data = np.genfromtxt('./datapoints-2021-06-19 00_33_23.533579.csv', delimiter=',', skip_header=1, names=['Datetime', 'Watermark', 
        'Battery', 'Temperature', 'Humidity'], dtype="S32,i8,f8,f8,f8")

def getTimestamp(datapoint: bytes) -> datetime:
    return datetime.strptime(bytes.decode(datapoint), '%Y-%m-%d %H:%M:%S.%f')

dates = [getTimestamp(i) for i in data['Datetime']]

df = pd.DataFrame({'datetime': dates, 'watermark': data['Watermark']})
df2 = pd.DataFrame({'datetime': dates, 'battery': data['Battery']})
df3 = pd.DataFrame({'datetime': dates, 'temperature': data['Temperature']})
df4 = pd.DataFrame({'datetime': dates, 'humidity': data['Humidity']})

plt.plot(df.datetime, df.watermark, label='watermark', color='blue')
plt.plot(df2.datetime, df2.battery, label='battery', color='red')
plt.plot(df3.datetime, df3.temperature, label='temperature', color='green')
plt.plot(df4.datetime, df4.humidity, label='humidity', color='cyan')

plt.title("Garden Edge Data")
plt.xlabel('Date')
plt.ylabel('Values')
plt.legend()

plt.show()