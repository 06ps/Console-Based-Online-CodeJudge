/*  Name : Pratibha Singh (pratibhasingh@kgpian.iitkgp.ac.in)
    Roll No : 20CS60R12

    File name : server.cpp
    Date : Jan 29 , 2021
    Description : The task is to implement file transfer protocol on top of TCP sockets.
                  The server will communicate with multiple client. Each client process 
                  will open a new connection with the server and server will use 'select' 
                  to handle multiple client requests.

    Compilation Instructions:
    To complie : g++ <source_filename> -o <Any name>
                eg: g++ server.cpp -o server
    To Run : ./<Any name> <Port number>
            eg: ./server 3000
*/

//header files
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<fstream>
#include<math.h>
#include<arpa/inet.h>
#include<time.h>
#include<sstream>
#include<dirent.h>
#include<ctime>

using namespace std;

struct node              // to store active port numbers in the linked list
{
    int port;
    struct node *next;
};
struct node *head = NULL;

void insert(int x)                  //insert active port number in linked list
{
    struct node *temp = (struct node *)malloc(sizeof(struct node *));

    temp->port = x;
    if(head == NULL)
    {
        head = temp;
        temp->next = NULL;
    }
    else
    {
        temp->next = head;
        head = temp;
    }
}

int check(int x)                    //to check if the port number already in use or not
{
    struct node *temp = head;
    int op=0;

    while(temp)
    {
        if(temp->port == x)
            return 1;
        
        temp = temp->next;
    }
    return 0;
}

void remove(int x)             //to remove used port number from linked list
{
    struct node *curr = head;
    struct node *prev;

    if(curr->port == x)
    {
        head = head->next;
        curr = NULL;
        free(curr);
    }
    else
    {
        while(curr && curr->port == x)
        {
            prev = curr;
            curr = curr->next;
        }

        prev->next = curr->next;
        curr = NULL;
        free(curr);
    }
}


/*  Function Name : generate_port()
    Description:    It generates random port number between 1024-65353 and return it.
                    It will also check whether the generated random port number is being used already 
                    if yes, then will keep generating port no. untill find an unused port number 
*/
int generate_port()
{
    srand(time(0));                 //to generate differnt port number while running each time
    int port_num = 1024 + (rand() % (65353 - 1024 +1));
    if(check(port_num))
    {
        return generate_port();
    }
    else
    {
        insert(port_num);
        return port_num;
    }
    
}


/*  Function Name : data_connection
    Input :         socket Id of control connection, arr which stores the active client IP + Port address
    Output:         return data connection socket Id
    Description:    It establishes a data connection between client and server for sending/recieving data information
*/
int data_connection(int &x, string arr[])
{
    int port_num;
    port_num = generate_port();
    int socket_fd, newsocket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    char buffer[1024];
    int num_of_char;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        perror("Error!!! Socket Failed\n");
        exit(EXIT_FAILURE);
    }

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error!!!, Binding failed");
        exit(EXIT_FAILURE);
    }

    if(listen(socket_fd, 5) < 0)
    {
        perror("Error!!!, Listening Failed");
        exit(EXIT_FAILURE);
    }

    client_addr_size = sizeof(client_addr);

    //sending new port number to client so that both server and client can get connected for data connection
    bzero(buffer, 1024);
    string p_n = to_string(port_num);
    strcpy(buffer, p_n.c_str());
    num_of_char = write(x, buffer, strlen(buffer));
    if(num_of_char < 0)
    {
        perror("Error while writing to socket");
        exit(EXIT_FAILURE);
    }

    //Data connection accepting step
    newsocket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_addr_size);
    if(newsocket_fd < 0)
    {
        perror("Error!!! ACcept Failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "*****\tData Connection Established with client " << arr[x] << "\t*****" << endl;
    }
    return newsocket_fd;
}


/*  Function Name : ftp_RETR
    Input :         socket Id, filename, arr which stores the active client IP + Port address
    Description:    This command causes the server to initiate a data connection and to
                    send the requested file over the data connection to the client
*/
void ftp_RETR(int &x, string file_name, string arr[])
{
    //Initiating a data connection
    int data_socket_fd = data_connection(x, arr);

    char buffer[1024];
    int num_of_char;
    FILE *f_ptr;
    const char *f_name = file_name.c_str();

    //opening file in binary mode for reading
    f_ptr = fopen(f_name, "rb");           
    if(f_ptr)                       //if the file present in the current directory
    {
        string op = "$";               //to give a indication to client that file transfer is going to take place
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
        
        while (fgets(buffer, 1024, f_ptr) != NULL)
        {
            if(write(data_socket_fd, buffer, strlen(buffer)) < 0)
            {
                perror("Error while writitng");
                exit(EXIT_FAILURE);
            }
            bzero(buffer, 1024);
        }
        
        cout << "File transfer successful to client " << arr[x] <<endl;
        fclose(f_ptr);          //closing file
    }
    else                        //if file not present in currrent directory
    {
        cout << "Filename " << file_name <<" doesn't exist in the current directory "<<endl;
        string op = "Filename " + file_name + " doesn't exist on the server side";
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
    } 

    close(data_socket_fd);
    cout << "*****\tData connection get Closed with client "<<arr[x]<< "\t*****" << endl;
}


/*  Function Name : ftp_STOR
    Input :         socket Id, filename, arr which stores the active client IP + Port address
    Description:    This command causes to store a file into the current directory of the server
*/
void ftp_STOR(int &x, string file_name, string arr[])
{
    //Initiating a data connection
    int data_socket_fd = data_connection(x, arr);
    char buffer[1024];
    int num_of_char;
    //checking if file exist already
    const char *f_check = file_name.c_str();
    FILE *f_ch = fopen(f_check, "r");
    if(f_ch)
    {
        cout << "Another file with same name exists\n";
        string op = "Another file with same name exists in server side";   
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
        fclose(f_ch);
    }
    else
    {
        string op = "$";               //to give a indication to client that file transfer is going to take place
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }

        const char *f_name = file_name.c_str();
        FILE *f_ptr = fopen(f_name, "wb");

        //storing the file in current directory
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

        fclose(f_ptr);              //closing the file
        cout <<"File Stored successfully....\n";
    }  
    close(data_socket_fd);
    cout << "*****\tData connection get Closed with client "<<arr[x]<< "\t*****" <<endl;
}


/*  Function Name : ftp_LIST
    Input :         socket Id, arr which stores the active client IP + Port address
    Description:    This command display the list of all files present in the directory of server and send the information to client
*/
void ftp_LIST(int &x, string arr[])
{
    //Initializing data connection
    int data_socket_fd = data_connection(x, arr);
    char buffer[1024];
    int num_of_char;
    struct dirent *dir_ptr;

    DIR *dir = opendir(".");             //opening current directory

    if(dir == NULL)
    {
        cout << "Error!!! Could not open current directory\n";
    }
    else
    {
        while((dir_ptr = readdir(dir)) != NULL)
        {   
            string file_name;
            for(int i=0; i < strlen(dir_ptr->d_name); i++)
            {
                file_name = file_name + dir_ptr->d_name[i];
            }
            if(file_name == "." || file_name == "..")
            {
                continue;
            }
            file_name = file_name + '\n';
            bzero(buffer, 1024);
            strcpy(buffer, file_name.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
        }
    } 
    cout << "List of current diretory files sent successfully to the client " << arr[x] <<endl;
    close(data_socket_fd);
    cout << "*****\tData connection get Closed with client "<<arr[x] << "\t*****" <<endl;
}


/*  Function Name : ftp_DELE
    Input :         socket Id, filename, arr which stores the active client IP + Port address
    Description:    This command deletes a file in the current directory of server
*/
void ftp_DELE(int &x, string file_name, string arr[])
{
    char buffer[1024];
    int num_of_char;
    const char *f_name = file_name.c_str();
    int data_socket_fd = data_connection(x, arr);

    FILE *f_ptr = fopen(f_name, "rb");          //opeining file in binary mode
    if(f_ptr)
    {
        if( remove(f_name) == 0)
        {
            cout << "File " << file_name <<" deleted successfully "<<endl;
            string op = "File " + file_name + " deleted successfully on server side";
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
        }
        fclose(f_ptr);          //closing file
    }
    else
    {
        cout << "Filename " << file_name <<" doesn't exist in the current directory "<<endl;
        string op = "Filename " + file_name + " doesn't exist on the server side";
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
    }

    close(data_socket_fd);  
    cout << "*****\tData connection get Closed with client "<<arr[x]<< "\t*****"<<endl;
}


/*  Function Name : matching_phase
    Input :         socket Id, object_file
    Description:    This function will match the output.txt file with hidden testcases.txt files and provide the output
*/
void matching_phase(int data_socket_fd, string obj_file)
{
    char buffer[1024];
    int num_of_char;

    string op_file = "output_" + obj_file + ".txt";
    string tc_file = "testcase_" + obj_file + ".txt";
    ifstream file_op(op_file);
    ifstream file_tc;
    file_tc.open(tc_file);
    string op_line, tc_line;

    int flag = 0, t_n = 1;

    if(file_tc)
    {
        while(getline(file_op, op_line) && getline(file_tc, tc_line))
        {
            string op,tc;
            for(int i=0; i<op_line.length(); i++)
            {
                if(op_line[i] == '\n' || op_line[i] == '\r')
                    continue;
                
                op = op + op_line[i];
            }

            for(int i=0; i<tc_line.length(); i++)
            {
                if(tc_line[i] == '\n' || tc_line[i] == '\r')
                    continue;
                
                tc = tc + tc_line[i];
            }

            int result = strcmp(op.c_str(),tc.c_str());

            if(result == 0)
            {
                string op = "Testcase " + to_string(t_n) + ": Passed\n";
                cout << op;
                bzero(buffer, 1024);
                strcpy(buffer, op.c_str());
                num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                flag = 1;
                string op = "Testcase " + to_string(t_n) + ": Failed\n";
                cout << op;
                bzero(buffer, 1024);
                strcpy(buffer, op.c_str());
                num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
            }

            t_n++;
        }
        
        
        while(getline(file_tc, tc_line))
        {
            flag = 1;
            string op = "Testcase " + to_string(t_n) + ": Failed\n";
            cout << op;
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
            t_n++;
        }

        if(flag == 0)
        {
            string op = "Matching Phase Successfull.... All testcases passed\n";
            cout << op;
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            string op = "Matching Phase Failed....\n";
            cout << op;
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        string op = "Testcase file missing.....\n";
        cout << op;
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
    }
}


/*  Function Name : execuiton_phase
    Input :         socket Id, object_file
    Description:    This function will execute the object file and report the result/runtime-erro/TLE
*/
void execution_phase(int &data_socket_fd, string obj_file)
{
    char buffer[1024];
    int num_of_char;
    time_t begin, end;
    string command, file_name, op_filename;

    file_name = "input_" + obj_file +".txt";
    op_filename = "output_" + obj_file +".txt";
    ifstream input_file;
    input_file.open(file_name);

    ofstream output_file;
    output_file.open(op_filename);

    string line;
    int flag;

    if(input_file)
    {
        while(getline(input_file, line))
        {
            ofstream out;
            out.open("dummy.txt");
            out << line;
            out.close();

            command = "timeout 2 ./" + obj_file + " 0< dummy.txt 1>>" + op_filename + " 2>error";
            const char *param = command.c_str();

            time(&begin);
            if(system(param))
            {
                time(&end);
                double time = end-begin;

                if(time > 1)
                {
                    cout << "Execution Phase Failed....... TLE(time limit exceed) "<<endl;
                    string op = "Execution Phase Failed..... TLE(time limit exceed)\n";
                    bzero(buffer, 1024);
                    strcpy(buffer, op.c_str());
                    num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                    if(num_of_char < 0)
                    {
                        perror("Error while writing to socket");
                        exit(EXIT_FAILURE);
                    }
                    return;
                }
                else
                {
                    cout << "Execution Phase Failed.... Runtime Error"<<endl;
                    string op = "Execution Phase Failed.... Runtime Error\n";
                    bzero(buffer, 1024);
                    strcpy(buffer, op.c_str());
                    num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                    if(num_of_char < 0)
                    {
                        perror("Error while writing to socket");
                        exit(EXIT_FAILURE);
                    }
                    return;
                }
            }
            else
            {
                time(&end);
                double time = end-begin;

                if(time > 1)
                {
                    cout << "Execution Phase Failed....... TLE(time limit exceed) "<<endl;
                    string op = "Execution Phase Failed..... TLE(time limit exceed)\n";
                    bzero(buffer, 1024);
                    strcpy(buffer, op.c_str());
                    num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                    if(num_of_char < 0)
                    {
                        perror("Error while writing to socket");
                        exit(EXIT_FAILURE);
                    }
                    return;
                }
                else
                {
                    flag = 1;
                }
            }
        }
        remove("dummy.txt");
    }
    else
    {
        command = "timeout 2 ./" + obj_file + " 1>>" + op_filename + " 2<error";
        const char *param = command.c_str();

        time(&begin);
        if(system(param))
        {
            time(&end);
            double time = end-begin;

            if(time > 1)
            {
                cout << "Execution Phase Failed....... TLE(time limit exceed) "<<endl;
                string op = "Execution Phase Failed..... TLE(time limit exceed)\n";
                bzero(buffer, 1024);
                strcpy(buffer, op.c_str());
                num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
                return;
            }
            else
            {
                cout << "Execution Phase Failed.... Runtime Error"<<endl;
                string op = "Execution Phase Failed.... Runtime Error\n";
                bzero(buffer, 1024);
                strcpy(buffer, op.c_str());
                num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
                return;
            }   
        }
        else
        {
            time(&end);
            double time = end-begin;

            if(time > 1)
            {
                cout << "Execution Phase Failed....... TLE(time limit exceed) "<<endl;
                string op = "Execution Phase Failed..... TLE(time limit exceed) \n";
                bzero(buffer, 1024);
                strcpy(buffer, op.c_str());
                num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                if(num_of_char < 0)
                {
                    perror("Error while writing to socket");
                    exit(EXIT_FAILURE);
                }
                return;
            }
            else
            {
                flag = 1;
                
            }    
        }
    }
    
    input_file.close();
    output_file.close();

    if(flag == 1)
    {
        cout << "Execution phase Success"<<endl;
        string op = "Execution Phase Success\n";
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
        matching_phase(data_socket_fd , obj_file);
    }    
}


/*  Function Name : compilation_phase
    Input :         socket Id, filename, extension name, string array
    Description:    This function will compile the file and provides appropirate message on compile success or failure
*/
void compilation_phase(int &data_socket_fd, string file_name, string ext, string arr[])
{
    char buffer[1024];
    int num_of_char;
    string command, obj_filename;

    if(ext == "c")
    {
        int len = file_name.length();
        if(file_name[len-1] == 'c' && file_name[len-2] == '.')
        {
            for(int i=0; i<len-2; i++)
            {
                obj_filename = obj_filename + file_name[i];
            }
            command = "gcc " + file_name + " -o " + obj_filename + " 2>error";
        }
        else
        {
            string op = "Extension doesn't match with file\n";
            cout << op;
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
            return;
        }
    }
    else if(ext == "cpp" || ext == "c++")
    {

        string check_ext;
        for(int i = file_name.length()-3; i<file_name.length(); i++)
        {
            check_ext = check_ext + file_name[i];
        }

        if(check_ext == "cpp" || check_ext == "c++")
        {
            for(int i=0; i<file_name.length()-4; i++)
            {
                obj_filename = obj_filename + file_name[i];
            }
            command = "g++ " + file_name + " -o " + obj_filename + " 2>error";
        }
        else
        {
            string op = "Extension doesn't match with file\n";
            cout << op;
            bzero(buffer, 1024);
            strcpy(buffer, op.c_str());
            num_of_char = write(data_socket_fd, buffer, strlen(buffer));
            if(num_of_char < 0)
            {
                perror("Error while writing to socket");
                exit(EXIT_FAILURE);
            }
            return;
        }
        
    }
    else
    {
        string op = "Invalid Extension\n";
        cout << op;
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
        return;
    }

    int compilation_result;
    const char *param = command.c_str();

    compilation_result = system(param);
    if(compilation_result == 0)
    {
        string op = "Compilation Phase Success\n";
        cout << op;
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
        execution_phase(data_socket_fd , obj_filename);
    }
    else
    {
        string op = "Compilation Phase Failed\n";
        cout << op;
        bzero(buffer, 1024);
        strcpy(buffer, op.c_str());
        num_of_char = write(data_socket_fd, buffer, strlen(buffer));
        if(num_of_char < 0)
        {
            perror("Error while writing to socket");
            exit(EXIT_FAILURE);
        }
    }
}


/* Driver Function in which control connection establish between server and client.
The server takes multi client request of FTP through control connection and perform respective operation.
*/
int main(int argc, char *argv[])
{
    int socket_fd;
    int port_num;
    struct sockaddr_in server_addr;

    if(argc < 2)
    {
        cout << "Error!!! Port number not provided\n";
        exit(1);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        perror("Error!!!, Socket call failed");
        exit(EXIT_FAILURE);
    }
    cout <<"\n*****\tSocket Creation successfull\t*****\n";

    port_num = atoi(argv[1]);

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error!!!, Binding failed");
        exit(EXIT_FAILURE);
    }

    if(listen(socket_fd, 5) < 0)
    {
        perror("Error!!!, Listening Failed");
        exit(EXIT_FAILURE);
    }

    fd_set master, temp_fds;
    FD_ZERO(&master);
    FD_ZERO(&temp_fds);
    FD_SET(socket_fd, &master);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size;

    int size = 1000;
    string arr[size];                           //to keep IP_addr + port_num of multi clients
    for(int i=0; i<size; i++)
    {
        arr[i] = "$";
    }

    while (1)
    {
        temp_fds = master;

        if(select(FD_SETSIZE, &temp_fds, NULL, NULL, NULL) < 0)             //using select to handle multiple clients
        {
            perror("Error!!! in Select");
            exit(EXIT_FAILURE);
        }

        for(int x=0; x<FD_SETSIZE; x++)
        {
            if(FD_ISSET(x, &temp_fds))
            {
                if(x == socket_fd)
                {
                    client_addr_size = sizeof(client_addr);

                    int newsocket_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);

                    if(newsocket_fd < 0)
                    {
                        cout << "Unable to accept\n";
                        exit(EXIT_FAILURE);
                    }
                    printf("\n*****\tControl Connection  Estabished with client: %s:%d\t*****\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    int p_n = ntohs(client_addr.sin_port);
                    string ip = inet_ntoa(client_addr.sin_addr);
                    if(arr[newsocket_fd] == "$")
                    {
                        arr[newsocket_fd] = ip + ":" + to_string(p_n);
                    }

                    FD_SET(newsocket_fd, &master);
                }
                else
                {
                    int num_of_char;
                    char buffer[1024];
                    bzero(buffer, 1024);
                    num_of_char = read(x, buffer, 1023);
                    pid_t child_id;             //for fork()
                    
                    if(num_of_char < 0)
                    {
                        perror("Error!!! Reading Failed");
                        exit(EXIT_FAILURE);
                    }
                    if(num_of_char == 0)
                        continue;

                    cout <<"\nClient "<<arr[x];
                    printf( " Message: %s\n",buffer );

                    string client_input;
                    for(int i=0; i<strlen(buffer); i++)
                    {
                        client_input = client_input + buffer[i];
                    }

                    string input, file_name;
                    stringstream ss(client_input);
                    ss>>input;

                    if(input == "RETR")
                    {
                        ss>>file_name;
                        if(file_name == "\0")
                        {
                            cout << "Error!!! File name not provided by client: "<<arr[x] <<endl;
                        }
                        else
                        {
                            child_id = fork();          //using fork for data connection
                            if(child_id == 0)
                            {
                                ftp_RETR(x, file_name, arr);
                            }  
                        }  
                    }
                    else if(input == "STOR")
                    {
                        ss>>file_name;
                        if(file_name == "\0")
                        {
                            cout << "Error!!! File name not provided by client: "<<arr[x] <<endl;
                        }
                        else
                        {
                            child_id = fork();          //using fork for data connection
                            if(child_id == 0)
                            {
                                ftp_STOR(x, file_name, arr);
                            }
                        }
 
                    }
                    else if(input == "LIST")
                    {
                        child_id = fork();          //using fork for data connection
                        if(child_id == 0)
                        {
                            ftp_LIST(x, arr);
                        }    
                    }
                    else if(input == "QUIT")
                    {
                        cout << "*****\tControl connection get Closed with client "<<arr[x]<< "\t*****"<<endl;
                        FD_CLR(x, &master);
                    }
                    else if(input == "DELE")
                    {
                        ss>>file_name;
                        if(file_name == "\0")
                        {
                            cout << "Error!!! File name not provided by client: "<<arr[x] <<endl;
                        }
                        else
                        {
                            child_id = fork();          
                            if(child_id == 0)
                            {
                                ftp_DELE(x, file_name, arr);
                            }
                        }   
                    }
                    else if(input == "CODEJUD")
                    {
                        string extension;

                        ss>>file_name;
                        ss>>extension;

                        if(file_name != "\0"  && extension != "\0")
                        {
                            child_id = fork();
                            if(child_id == 0)
                            {
                                //Initializing data connection
                                int data_socket_fd = data_connection(x, arr);
                                cout <<endl;

                                ifstream file;
                                file.open(file_name);

                                if(file)
                                {
                                    compilation_phase(data_socket_fd, file_name, extension, arr);
                                }
                                else
                                {
                                    string op = "No file with name " + file_name + " exists on server side\n";
                                    bzero(buffer, 1024);
                                    strcpy(buffer, op.c_str());
                                    num_of_char = write(data_socket_fd, buffer, strlen(buffer));
                                    cout << op;
                                }                              

                                close(data_socket_fd);  
                                cout <<endl<< "*****\tData connection get Closed with client "<<arr[x]<< "\t*****"<<endl;
                            }
                        }
                        else
                        {
                            cout << "Error!!! File name or extension not provided by client: "<<arr[x] <<endl;
                        }
                        
                    }
                    else
                    {
                        cout << "Invalid Command Provided by client: "<<arr[x] <<endl; 
                        string op = "Invalid!!! Enter a valid command";
                        bzero(buffer, 1024);
                        strcpy(buffer, op.c_str());
                
                        num_of_char = write(x, buffer, strlen(buffer));
                        if(num_of_char < 0)
                        {
                            perror("Error while writing to socket");
                            exit(EXIT_FAILURE);
                        }
                    }    
                }
            }
        }
    }

    close(socket_fd);
    return 0;
}