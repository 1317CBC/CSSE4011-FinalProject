# CSSE4011-FinalProject Vulcan_Scarlet
**Final Projct for CSSE4011 in UQ 2024 Semester 1**<br>
This project encompasses the development of a motion-sensing universal remote controller based on the M5 Core2 board, augmented with an infrared (IR) receiver/transmitter module. This versatile controller is designed to command any type of appliance that receives standard IR signals.

Key to this system is the M5Core2 controller, which serves as the user interface and command centre. The M5Core2, running the Zephyr Real-Time Operating System (RTOS), captures the user's hand gestures to determine intent. These gestures are precisely interpreted through the onboard accelerometer and gyroscope sensors. To complement gesture control, a touch-enabled UI is generated on the M5Core2's display. This interactive interface allows users to seamlessly select the device they intend to control.

For the transmission of commands from the M5Core2 to the NRF_52840DK, Bluetooth Low Energy (BLE) is employed, utilizing iBeacon technology for broadcasting 'advertisements'. This method ensures an efficient transfer of control signals without the need for establishing a continuous connection, thereby reducing power consumption and increasing the responsiveness of the system.

Upon receiving the data, the NRF_52840DK is responsible for encoding these signals into the appropriate IR format. This encoding process allows for the remote controller to interface with a diverse array of appliances, from televisions and air conditioners to various multimedia systems. The system is designed to demonstrate control of at least two different appliances, showcasing its capability as a truly universal remote control solution.

# Key Performance Indicator
To meet the highest standard for Key Performance Indicators as per the given criteria, detailed KPIs for the project based on the NRF_52840DK and M5core2 development boards to develop a motion-sensing universal remote control using IR to control various infrared devices would be:

## 1. Gesture Input Recognition via Accelerometer:
The M5core2's onboard accelerometer is to be calibrated and programmed to detect specific motion gestures. These gestures will serve as inputs to control various functions of infrared devices. Detailed metrics will be established to measure the accuracy and responsiveness of gesture recognition.

## 2. Bluetooth Communication Efficiency:
Establish a robust Bluetooth communication protocol between the M5core2 and NRF_52840DK boards. The performance will be measured based on connection stability, data transmission speed, and error rate. A seamless transmission of commands with minimal latency is critical for a positive user experience.

## 3. User Interface (UI) Development on M5core2:
Design an intuitive UI to be displayed on the M5core2 screen, which will show real-time gesture recognition feedback and command statuses. Effectiveness will be judged based on the UI's ease of use, clarity, and the user's ability to navigate and execute control functions without confusion or delay.

## 4. IR Signal Encoding Accuracy:
Program the NRF52840DK to accurately encode Bluetooth-received commands into IR signals compatible with a wide range of devices. KPIs will measure the encoding success rate, signal strength, range, and compatibility with various consumer electronics.

## 5. Versatility and Control of Multiple Devices:
The universal remote should be tested across multiple devices to confirm its ability to control at least two distinct devices from different manufacturers. The criteria for this KPI include the number of devices successfully controlled, the range of commands executed, and the user feedback on the efficacy and utility of the remote in real-world scenarios.

# Flow Chart

## 1. M5 Core2 Flowchart
![M5 Core2 Flowchart](flowchart/M5core2.png "M5 Core2 Flowchart")

## 2. nrf52840dk_nrf52840 flowchart
![View nrf52840dk_nrf52840 Flowchart](/flowchart/nrf52840dk.png "nrf52840dk Flowchart")


# DIKW Pyramid Analysis of m5stack core2 and NRF 52840DK Universal Remote Control System

This README provides a comprehensive analysis of the m5stack core2 and NRF 52840DK universal remote control system using the DIKW (Data, Information, Knowledge, Wisdom) Pyramid model.

## 1. Data
The MPU6886 sensor collects raw data from the accelerometer and gyroscope, such as acceleration values and angular velocity values on the XYZ axes. This forms the foundational data layer of the system.

## 2. Information
The m5stack core2 processes and fuses the collected raw data to generate accurate device orientation information. For example, it parses the raw data to determine the current spatial orientation of the remote control. Through data fusion, the raw sensor data is transformed into meaningful orientation information.

## 3. Knowledge
The embedded algorithms and logic in the system interpret user intentions based on the orientation information, thereby knowing what kind of infrared signal to send. For instance, if the user tilts the remote control to the right, the system recognizes this action as corresponding to the "increase volume" operation. These mappings between gestures and commands constitute the knowledge layer of the system, enabling it to understand gestures and translate them into control commands.

## 4. Wisdom
In designing this universal remote control system, we ingeniously integrated their understanding and experience in human-computer interaction, sensor technology, data processing, and infrared control. This demonstrates a higher level of thinking beyond specific knowledge, a systemic wisdom. It is manifested in the following aspects:

- Using gestures as a means of human-computer interaction, making it intuitive and user-friendly, lowering the barrier to use.
- Choosing accelerometer and gyroscope sensors to obtain orientation information, avoiding complex spatial positioning.
- Improving the accuracy of orientation judgment through data fusion, enhancing system reliability.
- Assigning complex data processing to M5Stack and focusing NRF52840 on infrared signal encoding and transmission, leveraging their respective strengths and simplifying system design.

In summary, this universal remote control system well illustrates the various levels of the DIKW Pyramid. The lower layers of data collection and information processing are realized by the m5stack core2, the middle layer of gesture recognition and command mapping embodies the system's embedded knowledge, and the top layer of system design and integration crystallizes the developer's wisdom. These layers are organically combined to achieve a powerful and easy-to-use remote control solution.

# Full Sensor Integration Details for M5Stack Core2 and NRF52840DK Universal Infrared Remote Control System

### M5Stack Core2
The M5Stack Core2 integrates the following sensors and features for the universal infrared remote control system:

## 1. MPU6886 Inertial Measurement Unit (IMU):
- The MPU6886 is a 6-axis motion tracking device that combines a 3-axis gyroscope and a 3-axis accelerometer.
- It provides raw acceleration and angular velocity data for the X, Y, and Z axes.
- The M5Stack Core2 reads the raw data from the MPU6886 sensor using the I2C communication protocol.

## 2. Data Fusion:
- The raw data from the MPU6886 is processed using a data fusion algorithm, such as the kalman filter, to estimate the precise orientation of the M5Stack Core2.
- The data fusion algorithm combines the accelerometer and gyroscope data to compensate for drift and provide stable orientation information.
- The resulting orientation data is represented as quaternions or Euler angles (roll, pitch, yaw).

## 3. Gesture Recognition:
- The M5Stack Core2 analyzes the fused orientation data to recognize different gestures performed by the user.
- Each gesture is mapped to a specific command or action to be executed by the NRF52840DK.
- For example, tilting the M5Stack Core2 to the right may correspond to the "volume up" command, while tilting it to the left may represent the "volume down" command.

## 4. Touch Screen Interface:
- The M5Stack Core2 features a capacitive touch screen display.
- The touch screen is used to provide a user-friendly interface for controlling the universal infrared remote control system.
- Users can interact with the touch screen to select devices, navigate menus, and trigger specific commands.
- The touch screen can display information such as the current device being controlled, available commands, and system status.

## 5. Command Transmission:
- Once a gesture is recognized or a command is selected through the touch screen interface, the M5Stack Core2 sends the corresponding command to the NRF52840DK by.
- The command is transmitted wirelessly using a suitable communication protocol, such as Ibeacon.
- The M5Stack Core2 encodes the command into a specific format that can be interpreted by the NRF52840DK.

### NRF52840DK
The NRF52840DK receives commands from the M5Stack Core2 and performs the following tasks:

## 1. Command Reception:
- The NRF52840DK listens for incoming commands from the M5Stack Core2 using the selected wireless communication protocol (Ibeacon).
- It decodes the received command to determine the specific action to be performed.

## 2. Infrared Encoding:
- Based on the received command, the NRF52840DK encodes the corresponding infrared signal.
- It uses infrared protocols specific to the device being controlled, such as TV or Fans
- The infrared encoding process involves generating the appropriate sequence of pulses and pauses that represent the command.

## 3. Infrared Transmission:
- The NRF52840DK is equipped with an infrared LED or an external infrared transmitter module.
- It transmits the encoded infrared signal through the infrared LED or transmitter module.
- The infrared signal is emitted in the direction of the target device to be controlled.

### System Flow
The overall flow of the universal infrared remote control system can be summarized as follows:
- The user interacts with the M5Stack Core2 through the touch screen interface or by performing gestures.
- The M5Stack Core2 reads the raw data from the MPU6886 IMU sensor.
- The raw data is processed using a data fusion algorithm to estimate the precise orientation of the M5Stack Core2.
- Gesture recognition is performed based on the fused orientation data, or commands are selected through the touch screen interface.
- The M5Stack Core2 sends the corresponding command to the NRF52840DK wirelessly.
- The NRF52840DK receives the command and encodes the appropriate infrared signal based on the command.
- The encoded infrared signal is transmitted through the infrared LED or transmitter module.
- The target device receives the infrared signal and executes the corresponding action.

This sensor integration architecture allows for a seamless and intuitive user experience, enabling control of various devices through gestures and touch interactions using the M5Stack Core2 and NRF52840DK universal infrared remote control system.
