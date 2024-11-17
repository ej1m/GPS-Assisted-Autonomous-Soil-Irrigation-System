Hardware Configuration

The system uses capacitive soil moisture V1.2, HC-SR04 ultrasonic sensors, NEO-6M GPS module, and RFID RC522 reader for sensing, powered by a 12V DC battery with LM2596 buck and MT3608 boost converters for voltage regulation.

Actuator Setup

The system employs DC motors for movement, SPDT relay-controlled water pump for irrigation, servomotors for sensor positioning and an OLED display for status monitoring.
The autonomous irrigation robot navigates using GPS coordinates while employing ultrasonic sensors for obstacle avoidance and RFID technology for precise docking alignment at the refill station.
Soil monitoring is performed using a capacitive moisture sensor positioned by servomotors, with data collected and analyzed to determine irrigation needs.
The robot features a self-sustaining water management system, utilizing an ultrasonic sensor to monitor its water tank level and automatically navigating to a refill station when needed.
The refill station incorporates its own ESP32, water pump, and ultrasonic sensor for water level detection, communicating with the robot via MQTT protocol.
Navigation combines GPS for general positioning, ultrasonic sensors for obstacle avoidance and RFID tags for precise docking alignment at the refill station.
The system employs an ESP32 microcontroller as its brain, managing sensor data, motor control and wireless communication through both MQTT and remote control apps (Arduino IoT).
Multiple RFID tags are strategically placed to create different zones (approach, docking, final position) for precise navigation and docking at the refill station.
The entire system operates autonomously, with remote monitoring and control capabilities through the Arduino IoT remote app, allowing for manual override when necessary.
