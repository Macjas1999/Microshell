#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/wait.h>

void comm_split(char *tin_point, char **command);
void help();
void touch(char **command);
void cd(char **command, char *curr_path);
void sysinfo(char **command);
void cp(char **command);


int main(int argc, char const *argv[])
{
  char terminal_in[1024];
  char *tin_point;
  char *command[255];
  char cwd[PATH_MAX];
  char *name = getlogin();
  char hostname[HOST_NAME_MAX + 1];
  char buff_hist_cd[2][255];

  do {
    getcwd(cwd, sizeof(cwd));
    gethostname(hostname, HOST_NAME_MAX + 1);
    printf("%s@%s:~%s $ ", name, hostname, cwd);

    tin_point = fgets(terminal_in, 1024, stdin);
    comm_split(tin_point,command);

    if(strcmp(command[0], "help") == 0)
    {
    	 help();
  	}
    else if(strcmp(command[0], "touch") == 0)
    {
			touch(command);
		}
    else if(strcmp(command[0], "sysinfo") == 0)
    {
			sysinfo(command);
		}
    else if(strcmp(command[0], "cd") == 0)
    {
			strcpy(buff_hist_cd[0], cwd);
			cd(command, buff_hist_cd[1]);
			strcpy(buff_hist_cd[1], buff_hist_cd[0]);
		}
    else if(strcmp(command[0], "cp") == 0)
    {
			cp(command);
		}
  	else if(strcmp(command[0], "exit") == 0)
    {
  	   exit(0);
    }
    else
    {
      if (fork() == 0)
      {
        exit(execvp(command[0], command));
      }
      else
      {
        int i = 0;
        wait(&i);
        if (i != 0)
        {
          printf("Error: %d\n", i);
        }
      }
    }
  } while(1);
  return 0;
}

void comm_split(char *tin_point, char **command)
{
  int i = 0;
  char div[] = " \n";
  char *single_w = strtok(tin_point,div);
  while (single_w != NULL)
  {
    command[i] = single_w;
    i++;
    single_w = strtok(NULL,div);
  }
  command[i]=NULL;
  return;
}

void help()
{
  printf("Command list:\n");
  printf("help\n");
  printf("touch\n");
  printf("sysinfo\n");
  printf("cd\n");
  printf("cp\n");
  printf("exit\n");
  printf("Author:\nMaciej Jasiniak\nNr indeksu - 445850\n");
  return;
}

void touch(char **command)
{
  if(open(command[1], O_RDONLY) != -1)
  {
		printf("This file already exist\n");
		return;
	}
	else
  {
		creat(command[1], 0666);
	}
}

void cd(char **command, char *curr_path)
{
	if(strcmp(command[1], "-")==0)
  {
		chdir(curr_path);
	}
	else{
		chdir(command[1]);
	}
	return;
}

void sysinfo(char **command)
{
  struct utsname sysinfo;
  uname(&sysinfo);

  if (command[1] == NULL)
  {
    printf("Help for sysinfo\n Arg:\n");
    printf("-a for all stats\n");
    printf("-sn for system name\n");
    printf("-nd for node name\n");
    printf("-r for release\n");
    printf("-v for version\n");
    printf("-mch form macine\n");
  }
  else if (strcmp(command[1], "-sn")==0)
  {
    printf("System name: %s\n", sysinfo.sysname);
  }
  else if (strcmp(command[1], "-nd")==0)
  {
    printf("Node name: %s\n", sysinfo.nodename);
  }
  else if (strcmp(command[1], "-r")==0)
  {
    printf("Release: %s\n", sysinfo.release);
  }
  else if (strcmp(command[1],"-v")==0)
  {
    printf("Version: %s\n", sysinfo.version);
  }
  else if (strcmp(command[1], "-mch")==0)
  {
    printf("Machine: %s\n", sysinfo.machine);
  }
  else if (strcmp(command[1], "-a")==0)
  {
    printf("System name: %s\n", sysinfo.sysname);
    printf("Machine: %s\n", sysinfo.machine);
    printf("Node name: %s\n", sysinfo.nodename);
    printf("Release: %s\n", sysinfo.release);
    printf("Version: %s\n", sysinfo.version);
  }
  else
  {
    printf("Wrong argument passed.\nFor help execute command without any arguments.\n");
  }
 return;
}

void cp(char **command)
{
	if(command[1] == NULL)
  {
		printf("No file to copy\n");
		return;
	}
	if(command[2] == NULL)
  {
		printf("There is no destination for a file to be copied to\n");
		return;
	}

	int file = open(command[1], O_RDONLY);
	char buff[1024];
	int x;
	if(file == -1)
  {
		printf("Invalid source file\n");
		return;
	}
	if(open(command[2], O_RDWR) != -1)
  {
		int file_name = open(command[2], O_WRONLY);
		while((x = read(file, buff, sizeof(buff))) > 0)
    {
		write(file_name, buff, x);
		}
		return;
	}
	else if(open(command[2], O_RDWR) == -1)
  {
		printf("Access denied\n");
		return;
	}
	int new = creat(command[2], 0666);
	if(new == -1)
  {
		printf("Error in func creat\n");
		return;
	}
	while((x=read(file, buff, sizeof(buff))) > 0)
  {
		write(new, buff, x);
	}
	close(new);
	close(file);
}

// else if (strcmp(command[0], "grep") == 0)
// {
//   if (fork() == 0)
//   {
//     execvp( "grep", command);
//     perror("Cannot exec grep");
//     exit( 1 );
//   }
// }
