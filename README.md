# Smart-Healthcare
IoT-Based Project: Wearable Smart Healthcare Device Using ESP32 + ThingSpeak.

1. Introduction
The increasing prevalence of chronic diseases and the need for continuous health monitoring have led to the development of wearable health devices. These devices can provide real-time data on various health parameters, enabling timely medical interventions. The "Wearable HealthCare Device" project aims to create a compact, user-friendly device that monitors vital signs such as heart rate, blood oxygen levels, body temperature, and motion. By utilizing multiple sensors and cloud-based data transmission, this device offers a comprehensive health monitoring solution.
2. Objectives
  ●	Design and Development: To design and develop a wearable device capable of monitoring heart rate, blood oxygen levels, body temperature, and motion.
  ●	Data Transmission: To implement data transmission to the ThingSpeak platform for remote monitoring and analysis.
  ●	User -Friendliness: To ensure the device is portable, lightweight, and easy to use for everyday health monitoring.
3. Components and Instruments
3.1 MAX30102
The MAX30102 is a pulse oximeter and heart rate sensor module that employs photoplethysmography (PPG) to measure heart rate and blood oxygen saturation levels. It consists of infrared and red LEDs and a photodetector.
Key Features:
  ●	Integrated LED Drivers: Allows for efficient operation and reduced power consumption.
  ●	Low Power Consumption: Ideal for battery-operated wearable devices.
  ●	I2C Interface: Facilitates easy communication with microcontrollers.
3.2 MPU6050
The MPU6050 is a 6DOF sensor that combines a 3-axis gyroscope and a 3-axis accelerometer. It is used to monitor motion and orientation, which can be useful for detecting falls or other movements.
Key Features:
●	Digital Motion Processing: Provides real-time motion data.
●	I2C Interface: Simplifies communication with microcontrollers.
●	Built-in Temperature Sensor: Offers additional data for health monitoring.
3.3 LM35
The LM35 is a precision temperature sensor that outputs an analog voltage proportional to temperature in degrees Celsius.
Key Features:
  •	Linear + 10mV/°C Scale Factor: Easy conversion from voltage to temperature
  •	Calibrated Directly in Celsius: No complex conversions needed
  •	0.5°C Accuracy (at 25°C): Reliable temperature measurements
  •	Range: -55°C to 150°C: Suitable for body temperature monitoring
  •	Low Self-Heating: Minimal impact on temperature readings
4. System Architecture
The system architecture of the Wearable HealthCare Device is designed to facilitate the seamless integration of various sensors, data processing, and cloud-based data transmission. This section provides a detailed overview of the architecture, including the components involved, their interactions, and the overall workflow of the system.
4.1 Overview of System Components
The system architecture consists of the following key components:
  1.	Microcontroller: The central processing unit that manages the operation of the device, interfaces with the sensors, processes the data, and handles communication with the cloud.
  2.	Sensors: Various sensors are used to monitor different health parameters:
    ●	MAX30102: Measures heart rate and blood oxygen saturation.
    ●	MPU6050: Tracks motion and orientation.
    ●	LM35: Measures body temperature.
  3.	Power Supply: Provides the necessary power to the microcontroller and sensors. This can be a rechargeable battery or a USB power source.
  4.	Communication Module: Enables data transmission to the ThingSpeak platform. This can be achieved using Wi-Fi (ESP8266/ESP32) or GSM modules.
  5.	Cloud Platform (ThingSpeak): A cloud-based service that stores, analyzes, and visualizes the data collected from the device.
5. Data Transmission to ThingSpeak
5.1 Overview of ThingSpeak
ThingSpeak is an open-source Internet of Things (IoT) analytics platform that allows users to collect, store, and analyze data from their devices. It provides a cloud-based service where users can visualize their data in real-time through graphs and charts. ThingSpeak supports various data types and can be accessed via RESTful APIs, making it suitable for IoT applications.
5.2 Setting Up ThingSpeak
As part of the development of the Wearable Healthcare Device, I used ThingSpeak to store and visualize the data collected from the sensors. Below is the step-by-step process I followed to set up ThingSpeak for this project:
Create a ThingSpeak Account
  1.	I visited the official ThingSpeak website and signed up for a free account.
  2.	After registration, I logged in to access the ThingSpeak dashboard.
  Create a New Channel
  1.	On the dashboard, I navigated to the "Channels" tab and clicked on "New Channel" to create a dedicated channel for the project.
  2.	In the channel setup, I defined the fields to match the data collected by the device:
    ○	Heart Rate
    ○	Blood Oxygen Level
    ○	ECG Value
    ○	Body Temperature
    ○	Motion Data (captured from the MPU6050 gyroscope and accelerometer module)
  3.	After defining the fields, I saved the channel, which generated a unique Channel ID and a Write API Key.
    ○	These credentials are essential for sending data to the channel via the IoT device.
    Integration with Device Code
  ●	I included the Channel ID and Write API Key in the code to enable seamless data transmission from the sensors to ThingSpeak.
  This setup allowed real-time storage and visualization of health parameters, enabling further analysis and insights through ThinkSpeak’s tools.
5.3 Data Transmission Process
The data transmission process involves sending the collected sensor data to ThingSpeak at regular intervals. The following steps outline this process:
        1.	Connect to Wi-Fi: Use the Wi-Fi module (ESP32) to connect to the internet.
        2.	Prepare Data: Format the data collected from the sensors into a suitable format for  transmission.
        3.	Send Data: Use the ThingSpeak API to send the data to your channel.
6. Implementation
The implementation of the Wearable HealthCare Device involves both hardware and software components. This section provides a detailed overview of the hardware setup, software development, and integration of the various components.
6.1 Hardware Setup
The hardware setup is crucial for the successful operation of the wearable health device. The following steps outline the process of assembling the components:
6.1.1 Components Required
    ●	Microcontroller: Arduino Uno or ESP8266/ESP32 (for Wi-Fi capability)
    ●	Sensors:
    ●	MAX30102 (Pulse Oximeter and Heart Rate Sensor)
    ●	MPU6050 (6DOF Gyroscope and Accelerometer)
    ●	LM35 (Contact-based Temperature Sensor (Thermometer))
    ●	Power Supply: USB power supply
    ●	Breadboard and Jumper Wires: For prototyping and connections
6.1.2 Circuit Connections
The following connections are made to integrate the sensors with the microcontroller:
  1.	MAX30102 Connections:
    •	VCC to 3.3V 
    •	GND to Ground 
    •	SDA to GPIO21 (default I2C SDA) 
    •	SCL to GPIO22 (default I2C SCL)
  2.	MPU6050 Connections:
    •	VCC to 3.3V 
    •	GND to Ground 
    •	SDA to GPIO21 (shared I2C bus with MAX30102) 
    •	SCL to GPIO22 (shared I2C bus with MAX30102)
  3.	LM35 Connections:
    ●	LM35’S VCC to 3.3V 
    ●	GND to Ground
    ●	OUT to GPIO34 
6.1.3 Power Supply
The device can be powered using a rechargeable battery or USB power supply. If using a battery, ensure that the voltage levels are compatible with the sensors and microcontroller. A battery management system may be included for charging and power regulation.
6.2 Software Development
The software development process involves writing code to initialize the sensors, read data, process the data, and transmit it to the ThingSpeak platform. The following sections outline the key components of the code.
6.2.1 Development Environment
  ●	IDE: Arduino IDE or PlatformIO
  ●	Libraries: Include necessary libraries for each sensor and ThingSpeak. For example:
  ●	MAX30105.h for MAX30102 (Heart Rate + Oxygen Level Measurement)
  ●	MPU6050.h for MPU6050 (Accelerometer to measure steps)
  ●	ThingSpeak.h for ThingSpeak API integration
