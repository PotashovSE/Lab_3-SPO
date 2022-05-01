#include <iostream>
#include <windows.h>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string.h>

#define BUFFER_SIZE 64

using namespace std;

int set_f (HANDLE pipe,map<string, string> &data, istringstream *parser,string &key, string &value);
int get_f (HANDLE pipe, map<string, string> data, istringstream *parser);
int list_f(HANDLE pipe, map<string, string> data);
int delete_f(HANDLE pipe, map<string, string> &data, istringstream *parser);
int quit(HANDLE pipe);

int main()
{
    map<string, string> data{};

    cout<< "Enter pipe name: ";
    string name_pipe;
    getline(cin,name_pipe);
    //the named pipe name
    auto path = "\\\\.\\pipe\\" + name_pipe;

    //create named pipe
    auto pipe = CreateNamedPipeA(path.c_str(),
                                 PIPE_ACCESS_DUPLEX,
                                 PIPE_TYPE_MESSAGE,
                                 PIPE_UNLIMITED_INSTANCES,
                                 BUFFER_SIZE,
                                 BUFFER_SIZE,
                                 0,
                                 NULL);

    if (pipe == INVALID_HANDLE_VALUE)
      {
          cout << "Error:"<< GetLastError()<<'\n';
          return 0;
      }

    bool flag = true;
    while (flag)
    {
        //Waiting for a connection
        std::cout << "The client connecting, wait please"<<'\n';
        if (ConnectNamedPipe(pipe, NULL))
        {
           std::cout << "connection successful"<<'\n';
        }
        else
        {
            std::cout << "Error:"<< GetLastError()<<'\n';
            CloseHandle(pipe);
        }

        //ReadFile and commands
        string command (BUFFER_SIZE, '\0');
        string cmd,key,value, response{};
        while(true)
        {
            std::cout << "Waiting for a command..."<<'\n';
            if (!ReadFile(pipe,&command[0], command.size(), NULL, NULL))
            {
                std::cout << "Error:" << GetLastError()<<'\n';
            }
            command.resize(command.find('\0'));

            std::istringstream parser{command};
            parser >> cmd;
            cout << "Command: " << command<< '\n';
            if(cmd == "set")
                set_f(pipe, data, &parser, key, value);
            else if (cmd == "get")
                get_f(pipe, data, &parser);

            else if (cmd == "list")
                list_f(pipe, data);

            else if (cmd == "delete")
                delete_f(pipe, data, &parser);

            else if (cmd == "quit")
            {
                DisconnectNamedPipe(pipe);
                break;
            }
            else
            {
             cout << "Non-existent command! Try again: "<<'\n';
            }
            command.replace(0, command.size(), command.size(), '\0');
            command.resize(BUFFER_SIZE, '\0');
            //response.clear();
            name_pipe.clear();
            value.clear();
        }
        //Continue working or end session
        char proceed;
        std::cout << "Do you want to continue?[y or n]: "<<'\n';
        while (true)
        {
           std::cin >> proceed;
           if (proceed == 'y')
           {
               break;
           }
           else if (proceed == 'n')
           {
               CloseHandle(pipe);
               flag = false;
               break;
           }
           else
           {
               std::cout << "Non-existent command! Try again: "<<'\n';
               continue;
           }
        }
    }

    printf("Work finished!\n");
    return 0;
}


int set_f (HANDLE pipe, map<string, string> &data, istringstream *parser, string &key, string &value)
{
    *parser >> key >> value;
    data[key] = value;
    string response = "acknowledged";
    WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
    return 1;
}


int get_f (HANDLE pipe, map<string, string> data, istringstream *parser)
{
    string key;
    *parser >> key;
    if (data.find(key) != data.end())
    {
        std::string response = "found " + data[key];
        WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
    }
    else
    {
        std::string response = "missing";
        WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
    }
}


int list_f(HANDLE pipe, map<string, string> data)
{
    string response;
    for (auto& i : data)
        response += i.first + " ";
    WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
}




int delete_f(HANDLE pipe, map<string, string> &data, istringstream *parser)
{
    string key;
    string response;
    *parser >> key;
    auto del = data.find(key);
    if (del != data.end())
    {
        data.erase(del);
        response = "deleted";
        WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
    }
    else
    {
        response = "missing";
        WriteFile(pipe, response.c_str(), response.size(), NULL, NULL);
    }
}




