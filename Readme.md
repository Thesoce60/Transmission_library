# Transmission
This Arduino library is for communicating command between boards with a serial communication bus (inherit of a Stream).
You can also send commands without classic stream with decode and encode functions.
## Commands ?
 Yes, pinMode, analogWrite, analogRead, digitalRead, digitalWrite. This commands and many others can be sent over the bus to change another Arduino state who listening on the bus.
