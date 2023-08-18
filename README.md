# Swine house system PGR212 Exam (2023)
A pig farmer heard about your Greenhouse prototype (Obligatory Submission 1) and needs a similar system.
He has taken over the neighbouring farms and now has three times as many pigs as before. His problem is
that the three different farmhouses all have different systems and the needs to use 3 different App’s to check
the conditions for his pigs.

The client wants an App (web or mobile) that can show the state of all 3 pig sty’s and alert him of problems.
The requirements are:
- You must register and store Temperature, Humidity and Light from your ESP32 Edge device
- The client wants the solution to run on their own servers, so you must make a Node API to store the sensor
data
- They want a Dashboard written in React that shows a graph of the last 24 hours of sensor data
- Deliver a video showing the final solution, all required code and written documentation. The reviewer should
be able to replicate your solution.

You can build the Dashboard as either a web App or a mobile App written in Expo. If you are able to come up
reasons to use other sensors in the Edge device, please do so. If you think the Dashboard needs more than
just a graph, you are free to add features to the solution. Draw on your experience from the obligatory
submissions and lectures to get ideas for good additions.

## Solution
This project covers the edge service of the solution. The program is written in C++ using the PlatformIO IDE with Arduino framework. The microcontroller uses different sensors to read the environment of the stys, such as temperature, humidity and light. It also uses a hall sensor with a magnet to detect if the door opens, and accelerometer to know that the sensor is standing still and has not fallen out of place. This solution connects to your local network and sends the data in real time to a MQTT broker. It also connects to an API, with support for both SSL and TCP, which means it can use the backend server deployed on the cloud or a local running instance. The program is easily expandable and allows for multiple sensors, only by changing the sensor id which is found in the Credentials header file.

### Components
- Hardware: adafruit_feather_esp32s3_nopsram
- SSL and TCP support for network connection
- Color-coded LED status indicators
- SHT31 temperature and humidity sensors
- LIS3DH accelerometer sensor
- LTR329ALS lux sensor
- MT869AT Hall effect sensor using magnet
- Libraries used for implementation

### Usage
- Follow the steps for configurations.
- Set up the edge credentials.
- Monitor LED colors for system status.
- Observe the serial monitor.
- Read data in Web App.

### Edge configuration
- In the credentials.h file, change ssid and pass to connect to the same wifi your computer is currently connected to.
- If you choose to run the backend server locally, make sure to change hostDomainLocal in the Credentials header file to your own.
- I've noticed that the hall sensor works best when the microcontroller is standing vertically.
- Follow the MQTT configuration steps. https://github.com/hivemq/mqtt-cli
- Optional: If you want to implement multiple sensors, all you have to do before uploading the software to the microcontroller is to change the sensorId to a name of your choosing. Tip! Stick to a theme.
- Optional: Create your own HiveMQ cluster. https://console.hivemq.cloud/
- Optional: Change brightness of LED.

### How to use
- pio run --target upload (once per microcontroller)
- pio device monitor

### Color codes for LED
- Green:    Ok, everything is working as it should
- Blue:     Wifi connected
- Yellow:   Wifi not connected
- Orange:   Something wrong with sensor
- Red:      Door is open

## Libraries
- adafruit/Adafruit LIS3DH@^1.2.4
- adafruit/Adafruit LTR329 and LTR303@^2.0.0
- adafruit/Adafruit SHT31 Library@^2.2.0
- adafruit/Adafruit DotStar@^1.2.1
- bblanchon/ArduinoJson@^6.20.0
- knolleary/PubSubClient@^2.8.0
- arduino-libraries/NTPClient@^3.2.1