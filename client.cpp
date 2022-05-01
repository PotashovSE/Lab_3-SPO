#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>
using namespace std;

#define BUFFER_SIZE 64

//client
int main()
{   //connecting to the named pipe by client
    string name_pipe;
    cout<<"Enter name pipe:";
    getline(cin >> ws, name_pipe);
    auto path = "\\\\.\\pipe\\" + name_pipe;

    auto client_pipe = CreateFile(path.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

    if (client_pipe == INVALID_HANDLE_VALUE)
    {
        cout << "Error:"<< GetLastError()<<endl;
        return 0;
    }

    string command (BUFFER_SIZE,'\0');


    //sending a request to the server
    while (true)
    {
        cout << ">> ";
        getline(cin >> ws, command);
        WriteFile(client_pipe, command.c_str(), command.size(), NULL, NULL);
        if (command == "quit")
        {
          CloseHandle(client_pipe);
          break;
        }
        else if (command.substr(0, 3) == "set" || command.substr(0, 3) == "get" ||
                 command == "list" || command.substr(0, 6) == "delete")
        {
            string response(64,'\0');
            ReadFile(client_pipe, &response[0], response.size(), NULL, NULL);
            cout<<response<<endl;
        }
        else
        {
            cout << "Non-existent command! Try again: "<<'\n';
            continue;
        }
    }
}
