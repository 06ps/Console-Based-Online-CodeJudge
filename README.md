## Console-Based-Online-CodeJudger
##### Computing Lab-II (CS69012) | Spring 2021 | Guide: Prof. Animesh Mukherjee

Developed a Console based online Code Judge with FTP(file transfer protocol) on top of TCP(Transmission Control Protocol) sockets. This Project implements an online code judge functionality which take the codes, compile it, check for compilation errors, execute code by providing some secret inputs and collect code's output, then futher check for runtime errors and finally match the output with its hidden test outputs. Along with CodeJuger it also provides some FTP commands functionality like RETR, STOR, LIST etc. It is a console based design where there will be a server and multiple clients communicating with the server. Each client will open a new connection with the server. Multiple client request at server side is handles via *"select()"* system call which allows to monitor multiple file descriptors, waiting untill one of the file descriptors become active. Select works like an interupt handler, which get activated as soon as any file descriptor sends any data.

_**Functionallity Provided by this Project:**_

####  CODEJUD

This command takes a c/c++ filename from the client and server will reply to the client whether the given code is successful or giving error at any point of time during execution of code. If the execution is successful then the server will also check and reply to the client abount the acceptance of the c/c++ file.
Working of server and client is as follows:-

After succesful connection:

**Client side:** Will give input through console to server using command "CODEJUD <filename> <ext(c/c++)>". The file that client wants server to run should be present at server side.
  
**Server side:** 
  
  **1. Compilation Phase -** This phase will take client provided input as string which contains the source code file name and its coding language i.e. extension(c/c++). This phase will compile the file and send compilation error message to clients if any, else it will create an object file and send a compilation success message to the client.
  
  **2. Execution Phase -** This phase will take object file and will execute the code by providing some secreat inputs if required and will decide if the input file has any error(TLE or Runtime error). For TLE(time limit exceed) error time limit is 1 sec. It the code execute successfully then it will create a new output.txt file and send an execution successful message to the client.
  
  **3. Matching Phase -** This phase will compare correct testcase.txt file with the actual output.txt file, it it is matched then the successful message will be passed to the client, else the wrong output message will be passed to client.
  
#### FTP Commands
  
  __1. RETR -__ This command causes the remote host to initiate a data connection and to send the requested file over the data connection. Syntax: RETR<filename>
  
  **2. STOR -** This command causes to store a file into the current directory of the remote host. Syntax: STOR<filename>
  
  **3. LIST -** This command sends a request to display the list of all files present in the directory. Syntax: LIST
  
  **4. QUIT -** This command terminates a user and if file transfer is not in progress, then server closes the control connection. Syntax: QUIT
  
  **5. DELE -** This command deletes a file in the current directory of server. Syntax:DELE<filename>

#### Commands to run the code:
  _**Server side**_
  
  Compilation command:
  
  ![command1](https://user-images.githubusercontent.com/32514527/130773321-ef9fba2c-f2f9-4858-9397-6019cf2e82c9.png)

  
  Run command:
  
  ![command2](https://user-images.githubusercontent.com/32514527/130773368-69095ac8-0b76-4bc7-9f48-1f06fd5b4d6a.png)

  
  _**Client side**_
  
  Compilation command:
  
  ![command3](https://user-images.githubusercontent.com/32514527/130773413-55a17d47-e04f-43e7-84bf-f5f8c8d21fe7.png)

  
  Run command:
  
  ![command4](https://user-images.githubusercontent.com/32514527/130773464-ed97377b-c844-47af-8960-9cd39171c323.png)
