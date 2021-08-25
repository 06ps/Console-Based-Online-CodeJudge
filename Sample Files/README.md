**SampleCode:** Contains some sample .cpp and .c files which will be present at server side. Client can request server to run those files, server will take the file, complie it, check for the compilation errors, execute the code by providing some secret inputs and collect the code's output and then will check for runtime errors and finally match the code output with its hidden test outputs. If in between these phases(Compliation, Execution and Matching), code file throws some error then server will report those errors to the client. If code file passes all the phases then server will display following message to the client.

![Image1](https://user-images.githubusercontent.com/32514527/130774171-72ee89bf-3bb0-4766-a33b-8dbe9b864db0.png)


**SampleInput:** Secret inputs

**SampleOutput:** Hidden test outputs
