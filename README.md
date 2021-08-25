## Console-Based-Online-CodeJudger
##### Computing Lab-II (CS69012) | Spring 2021 | Guide: Prof. Animesh Mukherjee
element {
  font-size: 75;
}
Developed a Console based online Code Judge with FTP(file transfer protocol) on top of TCP(Transmission Control Protocol) sockets. This Project implements an online code judge functionality which take the codes, compile it, check for compilation errors, execute code by providing some secret inputs and collect code's output, then futher check for runtime errors and finally match the output with its hidden test outputs. Along with CodeJuger it also provides some FTP commands functionality like RETR, STOR, LIST etc. It is a console based design where there will be a server and multiple clients communicating with the server. Each client will open a new connection with the server. Multiple client request at server side is handles via *"select()"* system call which allows to monitor multiple file descriptors, waiting untill one of the file descriptors become active. Select works like an interupt handler, which get activated as soon as any file descriptor sends any data.

_**Functionallity Provided by this Project:**_

**1. CODEJUD**

This command takes a c/c++ filename from the client and server will reply to the client whether the given code is successful or giving error at any point of thime during execution of code. If the execution is successful then the server will also check and reply to the client abount the acceptance of the c/c++ file.  
