# Automatic Watering System with Edge Impulse and Arduino

## A brief overview

This project is a showcase of how [Edge Impulse](https://edgeimpulse.com/) can be used along with the [Arduino Edge Control board](https://store.arduino.cc/usa/edge-control) to monitor the current environmental conditions and make inferences about when water is needed. The device is also fully off-grid as it uses a battery and solar panel for power.

## Components needed

- [Arduino Edge Control](https://store.arduino.cc/usa/edge-control)
- [Arduino MKR WiFi 1010](https://store.arduino.cc/usa/mkr-wifi-1010)
- DHT22 temperature/humidity sensor
- 12V sealed lead-acid battery
- 18V solar panel
- Hydrostatic watermark sensor
- Outdoor junction box

## What's in each folder?

- The [edge-control](edge-control/) folder contains the program that runs on the Arduino Edge Control. It requires the Arduino_EdgeControl library that can be found in the Library Manager. The hydrostatic watermark sensor gets connected to `CHAN01`.
- The [wifi-board](wifi-board/) folder contains the program that runs on the Arduino MKR WiFi 1010. It uses the onboard WiFi module to communicate with the server found [here](gardenedge-server/app.py). The board also gets connected to a DHT22 sensor and it sits in the MKR1 slot on the Edge Control. Make sure to replace your WiFi credentials in [arduino_secrets.h](wifi-board/arduino_secrets.h) and the server's IP address on lines `25` and `69` within [the ino file](wifi-board/wifi-board.ino).
- [gardenedge-server](gardenedge-server) contains the [app](gardenedge-server/app.py) which is run with `$ python3 app.py`. It listens on port `8080`. Each time the program is run, a new CSV file gets created and added to as more data comes in from the device.
- Finally, [data-visualizer](data-visualizer) is a program that allows you to take your datapoints and see them on a graph plotted over time. Run it with `$ python3 visualizer.py DATAPOINTS_CSV_FILE` where `DATAPOINTS_CSV_FILE` is the path to your datapoints file that the server produced.
