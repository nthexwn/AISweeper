PROJECT: AISweeper

AUTHOR: Alexander Johnson (nthexwn@gmail.com)

DESCRIPTION: A modular Minesweeper platform that allows for experimentation with different AIs.

STATUS: This project is currently being transitioned from a C++ implementation using Qt to a C implementation due to limitations encountered within the Qt framework (only one GUI thread may exist at a time).  I am using this as an opportunity to re-learn low-level programming using C on Linux.  Work is currently underway to split the program into separate client and server console programs which communicate using sockets.  Longterm plans are to split the server program between a shared gateway and separate game instances in order to support multiple simultaneous users.  At this point the client program will likely be ported to a Javascript frontend which could then be hosted in an AWS/Azure/DigitalOcean cloud alongside instances of the gateway and game programs.  Once I'm comfortable with the reliability and scalability of the different program components then I will revisit the introduction of AI to the game, possibly by providing an API for third-party users to plug in their own AIs.

