#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sstream>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <signal.h>
using namespace std;

extern char** environ;
int pipeSize = 0;


void handler(int sigNum) {
	signal(SIGINT, handler);
}

int
spawn_proc (int in, int out, char* cmd)
{
  pid_t pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }

      return execlp(cmd,cmd,NULL);
    }

  return pid;
}

int main(int argc, char *argv[]) {
	
	system("clear");
	vector<string> cmd;
	char host[HOST_NAME_MAX+1];	
	char user[100];
	char *cwd;
	char input[100];
	pid_t pid;		
	bool condition;

	gethostname(host,HOST_NAME_MAX+1);
	getlogin_r(user,100);
	
	setenv("SHELL",getcwd(NULL,0),1);
	signal(SIGINT, handler);		
	while(1) {
		cwd = getcwd(NULL,0);
		printf("%s@%s:~%s > ",host,user,cwd);
		fgets(input,100,stdin);
		
		if(input[0] == '\n')
			continue;
		
		int ln = strlen(input) - 1;
		if(input[ln] == '\n')
			input[ln] = '\0';

		condition = false;
		pipeSize = 0;
		
		for(int i = 0; i<ln; i++) {
			if(input[i] == '|') {
				condition = true;
				pipeSize++;	
			}
		}
		
    		stringstream check(input); 
      
    		string intermediate; 
    
    		while(getline(check, intermediate, ' ')) 
    		{ 
    	    		cmd.push_back(intermediate); 
    		} 
		
		if(condition == false) {
		if(cmd.size() > 2 && (cmd[1] == ">" || cmd[2] == ">" || cmd[3] == ">" || cmd[1] == "<" || cmd[2] == "<" || cmd[3] == "<")) {
			if(cmd[1] == ">" || cmd[2] == ">" || cmd[3] == ">") {
				char* command,*fileName,*arg1,*arg2;
				if(cmd[1] == ">") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					fileName = new char[cmd[2].length() + 1];
					strcpy(fileName, cmd[2].c_str());
					
					arg1 = NULL;
					arg2 = NULL;
				}
				else if(cmd[2] == ">") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					fileName = new char[cmd[3].length() + 1];
					strcpy(fileName, cmd[3].c_str());
					
					
					arg1 = new char[cmd[1].length() + 1];
					strcpy(arg1, cmd[1].c_str());
					
					arg2 = NULL;
				}
				else if(cmd[3] == ">") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					fileName = new char[cmd[4].length() + 1];
					strcpy(fileName, cmd[4].c_str());
					
					arg1 = new char[cmd[1].length() + 1];
					strcpy(arg1, cmd[1].c_str());

					arg2 = new char[cmd[2].length() + 1];
					strcpy(arg2, cmd[2].c_str());
				}
				
				ofstream newFile(fileName);
				int file = open(fileName,O_WRONLY);
				pid = fork();
				if(pid == 0) {
					dup2(file,1);
					execlp(command,command,arg1,arg2,NULL);
					exit(0);
				}
				else if(pid > 0) {
					wait(NULL);
				}
			}
			else if(cmd[1] == "<" || cmd[2] == "<" || cmd[3] == "<") {
				char* command,*inputFile,*outputFile,*arg1,*arg2;
				if(cmd[1] == "<") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					inputFile = new char[cmd[2].length() + 1];
					strcpy(inputFile, cmd[2].c_str());
					
					arg1 = NULL;
					arg2 = NULL;
					
					outputFile = new char[cmd[4].length() + 1];
					strcpy(outputFile,cmd[4].c_str());
				}
				else if(cmd[2] == "<") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					inputFile = new char[cmd[3].length() + 1];
					strcpy(inputFile, cmd[3].c_str());
					
					
					arg1 = new char[cmd[1].length() + 1];
					strcpy(arg1, cmd[1].c_str());
					
					outputFile = new char[cmd[5].length() + 1];
					strcpy(outputFile,cmd[5].c_str());
					
					arg2 = NULL;
				}
				else if(cmd[3] == ">") {
					command = new char[cmd[0].length() + 1];
					strcpy(command, cmd[0].c_str());						
				
					
					inputFile = new char[cmd[4].length() + 1];
					strcpy(inputFile, cmd[4].c_str());
					
					arg1 = new char[cmd[1].length() + 1];
					strcpy(arg1, cmd[1].c_str());

					arg2 = new char[cmd[2].length() + 1];
					strcpy(arg2, cmd[2].c_str());
					
					outputFile = new char[cmd[6].length() + 1];
					strcpy(outputFile,cmd[6].c_str());
				}
				
				ofstream newFile(outputFile);
				int file1 = open(inputFile,O_RDONLY);
				int file2 = open(outputFile,O_WRONLY);
				pid = fork();
				if(pid == 0) {
					dup2(file1,0);
					dup2(file2,1);
					execlp(command,command,arg1,arg2,NULL);
					exit(0);
				}
				else if(pid > 0) {
					wait(NULL);
				}
			}
		}
		

		else if(cmd[0] == "exit") {
			exit(0);
		}
		else if(cmd[0] == "pwd") {
			printf("%s \n",getenv("PWD"));
		}
		else if(cmd[0] == "clear") {
			system("clear");
		}
		else if(cmd[0] == "ls") {
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir(cwd)) != NULL) {
  			while ((ent = readdir (dir)) != NULL) {
    			printf ("%s\n", ent->d_name);
  			}
  			closedir (dir);
			}
		}
		else if(cmd[0] == "cd") {
			if(cmd.size() == 2) {
				const char *path = cmd[1].c_str();
				chdir(path);	
			}
			else {
				chdir(getenv("HOME"));
			}
		}
		else if(cmd[0] == "env") {
			char **env = environ;
			while(*env) {
				printf("%s\n",*env);	
				**env++;
			}
		}
		else if(cmd[0] == "setenv") {
			if(cmd.size() > 2) {
				const char* name = cmd[1].c_str();
				const char* value = cmd[2].c_str();
				setenv(name,value,1);
			}
			else {
				const char* name = cmd[1].c_str();
				setenv(name,"",1);
			}
		}
		else if(cmd[0] == "unsetenv") {
			const char* name = cmd[1].c_str();
			unsetenv(name);
		}
		else {
			for(int i = 0; i<cmd.size(); i++) {
				cout << cmd[i] << endl;
			}	
			
			pid = fork();
			const char* command = cmd[0].c_str();
			if(pid == 0) {
				execlp(command,command,NULL);
				exit(0);
			}
			else if(pid > 0) {
				wait(NULL);
			}
		}
		}

		else {

			vector<string> commands;
			
			for(int i = 0; i<cmd.size(); i++) {
				if(cmd[i] != "|") {
					commands.push_back(cmd[i]);
				}
			}

			
  			pid_t pid;
			char* command;
  			int in, fd [2];

  			in = 0;
			pid = fork();
			if(pid == 0) {
  			for (int i = 0; i < pipeSize; ++i){
	  			pipe (fd);

				command = new char[commands[i].length() + 1];
				strcpy(command,commands[i].c_str());			

	      			spawn_proc (in, fd [1], command);
		
      				close (fd [1]);

      				in = fd [0];
    			}
  
  			if (in != 0)
    			dup2 (in, 0);

			command = new char[commands[pipeSize].length() + 1];
			strcpy(command,commands[pipeSize].c_str());
			
  			execlp (command,command,NULL);
			exit(0);
			}
			else if(pid > 0) {
				wait(NULL);
			}
			commands.clear();
			
		}

		cmd.clear();
		
	}

	exit(0); // exit normally	
}
