# DOMUS-Data-Aquisition
This Arduino code forms the basis for the data aquisition on the DOMUS project.

# Description
This code is written to read data from 3 *Wind Sensor Rev C*'s from moderndevices.com. a DHT22 sensoe, a Carbon Dioxide sensor supplied from a separate power source and 6 K-type thermocouples. All of this data is logged into and as-card via the Datalogging Arduino shield that also has a real time clock.

# Initial approach
The initial approach was using MAX31850 amplifiers for the thermocouples and a level shifter. This had some issues running.
The *DOMUS_1Wire_ByAddress* example is ment to run this version. The 1Wire connection and Library was tested with a 3.3v chip, the *Blend Micro* and that worked fine.

# Running the code
* Copy the libraries if you don't have them to your Arduino IDE library location.
* Open the *DOMUS_MAIN* Sketch, compile and upload to the board, it should now be saving files to the sdCard provided everything is connected and set up correctly.

# Progress
At this point everything works except the temperature sensors. Chaning these for the MAX31855 5v compatible SPi ones might fix the problem. 
