/*  Name : Pratibha Singh (pratibhasingh@kgpian.iitkgp.ac.in)
    Roll No : 20CS60R12

    File name : server.cpp
    Date : Jan 29 , 2021
    Description : The task is to implement file transfer protocol on top of TCP sockets.
                  The server will communicate with multiple client. Each client process 
                  will open a new connection with the server and server will use 'select' 
                  to handle multiple client requests.

    Compilation Instructions:
    To complie : g++ <source_filename> -o l<Any name>
                eg: g++ client.cpp -o client
    To Run : ./<Any name> <localhost> <Port number>
            eg: ./client localhost 3000
*/

//header files
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sstream>

using namespace std;


/*  Function Name : data_connection
    Input :         socket Id of control connection
    Output:         return data connection socket Id
    Description:    It establishes a data connection between client and server for sending/recieving data information
*/
int data_connection(int &psock_fd)
{
    int sock_fd, num_of_char;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[1024];
    bzero(buffer, 1024);

    num_of_char = read(psock_fd, buffer, 1023);         //reading the port number send by server for data connection
    if(num_of_char < 0)
    {
        perror("Error while reading from socket");
        exit(EXIT_FAILURE);
    }
    if(num_of_char == 0)
    {
        cout << "Server got exit....\n";
        exit(1);
    }
    
    string new_port_num;
    for(int i=0;i<strlen(buffer);i++)
    {
        new_port_num = new_port_num + buffer[i];
    }

    stringstream str(new_port_num);         //to convert string port to int
    int port_no;
    str >> port_no;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
    {
        perror("Erro!!!, socket call failed");
        exit(EXIT_FAILURE);
    }

    server = gethostbyname("localhost");
    if(server == NULL)
    {
        fprintf(stderr, "Error!!!, no such host exit\n");
        exit(0);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_no);

    //data connection estabishing step
    if(connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error while connecting");
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "*****\tData Connection Established\t*****\n";
    }
    return sock_fd;
}


/*  Function Name : ftp_RETR
    Input :         Client input and socket Id
    Description:    This command causes the client to accept the requested file over the data connection
*/
void ftp_RETR(string file_name, int &sock_fd)
{
    char buffer[1024];
    int num_of_char;

    //establishing a data connection with the server    
    int data_socket_fd = data_connection(sock_fd);

    bzero(buffer, 1024);
    num_of_char = read(data_socket_fd, buffer, 1023);                
    if(num_of_char < 0)
    {
        perror("Error!!! Reading Failed");
        exit(EXIT_FAILURE);
    }
    
    if(buffer[0] == '$')                    //this symbol indicates that file exist in server directory and it is sending the file
    {
        const char *f_name = file_name.c_str();
        FILE *f_ptr = fopen(f_name, "wb");
        while(1)
        {
            bzero(buffer, 1024);
            num_of_char = read(data_socket_fd, buffer, 1023);
            if(num_of_char == 0)
            {
                break;
            }             
            if(num_of_char < 0)
            {
                perror("Error!!! Reading Failed");
                exit(EXIT_FAILURE);
            }
            fwrite(&buffer, 1, num_of_char, f_ptr);
        }
        cout <<"Requested file " << file_name << " received successfully" <<endl;
        fclose(f_ptr);      //closing file
    }
    else
    {
        printf( "Server Message : %s\n", buffer );
    }
    close(data_socket_fd);
    cout << "*****\tData Connection get closed\t*****\n";
}


/*  Function Name : ftp_STOR
    Input :         Client input and socket Id
    Description:    This command causes to store a file to the server directory
*/
void ftp_STOR(string file_name, int &sock_fd)
{
    char buffer[1024];
    int num_of_char;

    //establishing a data connection with the server 
    int data_socket_fd = data_connection(sock_fd);

    bzero(buffer, 1024);
    num_of_char = read(data_socket_fd, buffer, 1023);                
    if(num_of_char < 0)
    {
        perror("Error!!! Reading Failed");
        exit(EXIT_FAILURE);
    }

    if(buffer[0] == '$')
    {
        FILE *f_ptr;
        const char *f_name = file_name.c_str();
        f_ptr = fopen(f_name, "rb");

        //bzero(buffer, 1024);
        while(fgets(buffer, 1024, f_ptr) != NULL)
        {
            if(write(data_socket_fd, buffer, strlen(buffer)) < 0)
            {
                perror("Error while writing");
                exit(EXIT_FAILURE);
            }
            bzero(buffer, 1024);
        }
        fclose(f_ptr);              //closing file
        cout<<"File Stored successfully on server side\n";   
    }
    else
    {
        printf( "Server Message : %s\n", buffer );
    }
    close(data_socket_fd);
    cout << "*****\tData Connection get closed\t*****\n";
}


/*  Function Name : ftp_LIST
    Input :         Client input and socket Id
    Description:    This command list out all the files present in server current direcotry
*/
void ftp_LIST(int &sock_fd)
{
    char buffer[1024];
    int num_of_char;

    //establishing a data connection with the server    
    int data_socket_fd = data_connection(sock_fd);

    cout << "\n\tList of Files present in the Server Directory\n\n";

    bzero(buffer, 1024);
    while(1)
    {  
        num_of_char = read(data_socket_fd, buffer, 1023); 
        if(num_of_char == 0)
            break;               
        if(num_of_char < 0)
        {
            perror("Error!!! Reading Failed");
            exit(EXIT_FAILURE);
        }
        printf( "%s",buffer);
        //cout << buffer << endl;
        bzero(buffer, 1024);
    }
    close(data_socket_fd);
    cout << "\n*****\tData Connection get closed\t*****\n";
}


/*  Function Name : ftp_DELE
    Input :         Socket Id
    Description:    This command delete the file from server
*/
void ftp_DELE(int &sock_fd)
{
    char buffer[1024];
    int num_of_char;
    //establishing a data connection with the server 
    int data_socket_fd = data_connection(sock_fd);

    bzero(buffer, 1024);
    num_of_char = read(data_socket_fd, buffer, 1023);
    if(num_of_char < 0)
    {
        perror("Error!!! Reading Failed");
        exit(EXIT_FAILURE);
    }
    printf( "Server Message : %s\n", buffer );

    close(data_socket_fd);
    cout << "*****\tData Connection get closed\t*****\n";
}


/*  Function Name : ftp_CODEJUD
    Input :         Socket Id
    Description:    This command will receive message from server regearding progess/status of ftp_CODEJUD
*/
void ftp_CODEJUD(int &sock_fd)
{
    char buffer[1024];
    int num_of_char;

    //establishing a data connection with the server
    int data_socket_fd = data_connection(sock_fd);
    cout <<endl;

    while(1)
    {
        bzero(buffer, 1024);
        num_of_char = read(data_socket_fd, buffer, 1023);
        if(num_of_char < 0)
        {
            perror("Error!!! Reading Failed");
            exit(EXIT_FAILURE);
        }
        if(num_of_char == 0)
            break;
            
        printf( "Server Message : %s", buffer );
    }
    close(data_socket_fd);
    cout <<endl<< "*****\tData Connection get closed\t*****\n";
}


/* Driver Function in which control connection establish between server and client.
The client send FTP command to server and server perform certain action according to the command passed
*/
int main(int argc, char *argv[])
{
    int sock_fd, num_of_char;
    int port_no;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[1024];

    if(argc < 3)
    {
        fprintf(stderr, "%s hostname or port missing", argv[0]);
        cout<<endl;
        exit(0);
    }

    port_no = atoi(argv[2]);
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
    {
        perror("Error!!!, Socket call failed");
        exit(EXIT_FAILURE);
    }
    cout <<"\n*****\tSocket Creation successfull\t*****\n";

    server = gethostbyname("localhost");
    if(server == NULL)
    {
        fprintf(stderr, "Error!!!, no such host exist\n");
        exit(0);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_no);

    if(connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error while connecting");
        exit(EXIT_FAILURE);
    }else
    {
        cout << "\n*****\tControl connection established with the Server\t*****\n" <<endl;
    }
    
    while(1)
    {   
        string input;
        cout << "\nEnter FTP command : ";
        getline(cin, input);
        
        string command, file_name;
        stringstream str(input);
        str>>command;

        if(command == "RETR")
        {
            str>>file_name;
            string choice;

            if(file_name != "\0")
            {
                const char *f_name = file_name.c_str();
                FILE *f_ptr = fopen(f_name, "r");
                if(f_ptr)
                {
                    cout << "Another file with same name exist.Do you want to overwrite? \nPress 'Y' for yes or 'N' for No: ";
                    getline(cin, choice);

                    if(choice == "Y")
                    {
                        bzero(buffer, 1024);
                        strcpy(buffer, input.c_str());
                        num_of_char = write(sock_fd, buffer, strlen(buffer));
                        if(num_of_char < 0)
                        {
                            perror("Error while writing to socket");
                            exit(EXIT_FAILURE);
                        }
                        ftp_RETR(file_name, sock_fd);
                    }
                    else
                    {
                        cout << "User does't want to overwrite the existing file\n\n";
                    }
                    
                }
                else
                {
                    bzero(buffer, 1024);
                    strcpy(buffer, input.c_str());
                    num_of_char = write(sock_fd, buffer, strlen(buffer));
                    if(num_of_char < 0)
                    {
                        perror("Error while writing to socket");
                        exit(EXIT_FAILURE);
                    }
                    ftp_RETR(file_name, sock_fd);
                }                
            }
            else
            {
                bzero(buffer, 1024);
                strcpy(buffer, input.c_str());
                num_of_char = write(sock_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
                cout << "Error!!! File name not provided"<<endl;
            }   
        }
        else if( command == "STOR")
        {
            str>>file_name;
            if(file_name != "\0")
            {
                const char *f_name = file_name.c_str();
                FILE *f_ptr = fopen(f_name, "r");
                if(f_ptr)
                {
                    bzero(buffer, 1024);
                    strcpy(buffer, input.c_str());
                    num_of_char = write(sock_fd, buffer, strlen(buffer));
                    if(num_of_char < 0)
                    {
                        perror("Error while writing to socket");
                        exit(EXIT_FAILURE);
                    }
                    ftp_STOR(file_name, sock_fd);
                }
                else
                {
                    cout << "Invalid file name.... No file exist with name " <<file_name <<endl;
                }
            }
            else
            {
                bzero(buffer, 1024);
                strcpy(buffer, input.c_str());
                num_of_char = write(sock_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
                cout << "Error!!! File name not provided...."<<endl;
            }
        }
        else if( command == "LIST")
        {
            bzero(buffer, 1024);
            strcpy(buffer, input.c_str());
            num_of_char = write(sock_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
            ftp_LIST(sock_fd);
        }
        else if( command ==  "QUIT")
        {
            bzero(buffer, 1024);
            strcpy(buffer, input.c_str());
            num_of_char = write(sock_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
            cout << "Client closing....."<<endl;
            break;
        }
        else if( command == "DELE")
        {
            bzero(buffer, 1024);
            strcpy(buffer, input.c_str());
            num_of_char = write(sock_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }

            str>>file_name;
            if(file_name != "\0")
            {
               ftp_DELE(sock_fd);
            }
            else
            {
                cout << "Error!!! File name not provided...."<<endl;
            }
        }
        else if( command == "CODEJUD")
        {
            bzero(buffer, 1024);
            strcpy(buffer, input.c_str());
            num_of_char = write(sock_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }

            string extension;
            str>>file_name;
            str>>extension;

            if(file_name != "\0" && extension != "\0")
            {
                ftp_CODEJUD(sock_fd);
            }
            else
            {
                cout << "Error!!! File name or extension not provided...."<<endl;
            }
        }
        else
        {
            bzero(buffer, 1024);
            strcpy(buffer, input.c_str());
            num_of_char = write(sock_fd, buffer, strlen(buffer));
            if(num_of_char <= 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }

            bzero(buffer, 1024);
            num_of_char = read(sock_fd, buffer, 1023);
            if(num_of_char < 0)
            {
                perror("Error while reading from socket");
                exit(EXIT_FAILURE);
            }
            if(num_of_char == 0)
            {
                cout <<"server got exit....\n";
                exit(1);
            }
            printf("Server Message : %s\n", buffer);
        }

    }

    cout << "\n****\tControl connection get Closed\t*****\n";
    close(sock_fd);
    return 0;
}