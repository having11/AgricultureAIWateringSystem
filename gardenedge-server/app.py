from flask import Flask, request
from datetime import datetime
import traceback
import csv

app = Flask(__name__)

filename = 'datapoints-{}.csv'.format(str(datetime.now()))
print(filename)

@app.route('/gardenEdge', methods=['POST'])
def log_datapoint():
    try:
        print('Got request: ', request.json)
        req = request.json

        with open(filename, mode='a') as data_file:
            writer = csv.writer(data_file, delimiter=',', lineterminator='\n')
            writer.writerow([str(datetime.now()), req['watermark'],
                req['battery'], req['temperature'], req['humidity']])

        return 'ok'
    except Exception:
        traceback.print_exc()

if __name__ == '__main__':
    with open(filename, mode='w') as data_file:
        writer = csv.writer(data_file, delimiter=',', lineterminator='\n')
        writer.writerow(['takenAt', 'watermark', 'battery', 'temperature', 'humidity'])

    app.run(host='0.0.0.0', port=8080)
    