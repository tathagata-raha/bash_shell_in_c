#include <sys/types.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
// #include <readline/readline.h>
// #include <readline/history.h>
int max=21;
typedef struct{
	char name[25][100];
	int size;
}his;
char delimit[]=" \t\r\n\v\f";
char p_dir[1000]="~";
char bg_processname[1000][100];
int bg_processid[1000];
int bg=0;
int currprocid=-1;
char currprocname[100];
char tempup[1000]="";
char home[1000];
void handler(int sig)
{
	if(sig == SIGTSTP && currprocid >= 0)
	{
		int invalid=0;
		// for(int i=0;i<bg;i++)
		// {
		// 	if(bg_processid[i]==currprocid)
		// 	{
		// 		invalid=1;
		// 		break;
		// 	}
		// }
		if(invalid==0)
		{
			bg_processid[bg]=currprocid;
			sprintf(bg_processname[bg],"%s",currprocname);
			bg++;
		}
		kill(currprocid, SIGTSTP);
		currprocid = -1;
		strcpy(currprocname, "");
		signal(SIGTSTP, SIG_IGN);
	}
	else if(sig == SIGINT && currprocid >= 0)
	{
		kill(currprocid,9);
		printf("Process with pid %d terrminated by Ctrl+C\n", currprocid);
		currprocid = -1;
		strcpy(currprocname, "");
		signal(SIGINT, SIG_IGN);
	}
	fflush(stdout);
	return;
}
char *LastcharDel(char *name)
{
	int i=0;
	while(name[i]!='\0')
	{
		i++;
	}
	name[i-1]='\0';
	return name;
}
int insq(char queue[max][500], int *rear, char data[80])
{
      if(*rear == max -1)
            return(-1);
      else
      {
            *rear = *rear + 1;
            strcpy(queue[*rear], data);
            return(1);
      }
}
char issspace (unsigned char c) 
{
  if ( c == ' '
    || c == '\f'
    || c == '\n'
    || c == '\r'
    || c == '\t'
    || c == '\v' ) 
      return 1;

  return 0;
}
char* itoa(long long int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
int  getSubString(char *source, char *target,int from, int to)
{
	int length=0;
	int i=0,j=0;
	
	//get length
	while(source[i++]!='\0')
		length++;
	
	if(from<0 || from>length){
		printf("Invalid \'from\' index\n");
		return 1;
	}
	if(to>length){
		printf("Invalid \'to\' index\n");
		return 1;
	}	
	
	for(i=from,j=0;i<=to;i++,j++){
		target[j]=source[i];
	}
	
	//assign NULL at the end of string
	target[j]='\0'; 
	
	return 0;	
}
char *isunderhome(char *path)
{
	if(strncmp(path, home, strlen(home))==0)
	{
		char *target = (char *)malloc(1000 * sizeof(char));	
		getSubString(path,target,strlen(home), strlen(path)-1);
		char temp[1000]="~";
		strcat(temp,target);
		strcpy(p_dir,temp);
	}
	else
	{
		strcpy(p_dir,path);
	}
}
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(issspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && issspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
void invalid(char *str)
{
	printf("Invalid instance of %s\n", str);
}
void run_pipe(char piped[100][100][100], int pipes)
{
	int pipe_arr[2]={0,0};
	int pipe_var=0;
	int i=0;
	while(i<pipes)
	{
		char abc[]="abc\n";
		pipe(pipe_arr);
		int id=fork();
		if(id<0)
		{
			perror("Fork error");
			return;
		}
		else if(id==0)
		{
			// printf("abc\n");
			
			int k=0;
			dup2(pipe_var,0);
			close(pipe_arr[0]);
			if(i<(pipes-1))
			{
				dup2(pipe_arr[1],1);
			}
			int fprr;
			//Ahh shit!!! Here we go again
			int arct=0;
			char **arvt=(char **)malloc(1000 * sizeof(char *));
			while(1)
			{
				arvt[arct] = (char *)malloc(sizeof(char));
				if(strcmp(piped[i][arct],"end")==0)
					break;
				strcpy(arvt[arct],piped[i][arct]);
				arct++;
				
			}
	    	int ip=0,op=0,ap=0,ipfile,opfile,apfile;
	    	char ipfilepath[1000]="";
	    	char opfilepath[1000]="";
	    	char apfilepath[1000]="";
	    	for(int j=0;j<arct;j++)
	    	{
	    		if(arvt[j][0]=='<' && arvt[j][1]=='\0')
	    		{
	    			ip++;
	    			strcpy(ipfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}
	    		else if(arvt[j][0]=='>' && arvt[j][1]=='\0')
	    		{
	    			op++;
	    			strcpy(opfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}
	    		else if(arvt[j][0]=='>' && arvt[j][1]=='>' && arvt[j][2]=='\0')
	    		{
	    			ap++;
	    			strcpy(apfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}	
	    	}
	    	char *t_input= (char *)malloc(10000 * sizeof(char));
	    	char *token= (char *)malloc(10000 * sizeof(char));
	    	sprintf(t_input,"");
	    	for(int j=0;j<arct;j++)
	    	{
	    		sprintf(t_input + strlen(t_input),"%s ",arvt[j]);
	    	}
			t_input=trimwhitespace(t_input);
			int argc=0;
			char **argv = (char **)malloc(1000 * sizeof(char *));
			token = strtok(t_input, delimit);  
		    while (token != NULL) 
		    {
		        argv[argc]=token;
		        token = strtok(NULL, delimit);
		        argc++; 
	    	}
	    	if(ap>1 || op>1 || ip>1 || (ap+op)>1)
	    	{
	    		printf("Invalid redirection\n");
	    		continue;
	    	}
	    	if(ip==1)
	    	{
	    		ipfile=open(ipfilepath, O_RDONLY, 0);
	    		if(ipfile < 0)
				{
					printf("Error in input file\n");
					continue;
				}
	    	}
	    	if(op==1)
	    	{

	    		opfile=creat(opfilepath,  0777);
	    		if(opfile < 0)
				{
					printf("Error in output file\n");
					continue;
				}
	    	}
	    	else if(ap==1)
	    	{
	    		opfile=open(apfilepath, O_WRONLY | O_APPEND | O_CREAT, 0777);
	    		if(opfile < 0)
				{
					printf("Error in output file\n");
					continue;
				}
	    	}
    		int id=fork();    
    		if(id<0)
    		{
    			perror("Fork error");
    		}
    		if(id==0)
    		{
    			
    			argv[argc]=NULL;
	    		if(ip == 1)
				{
					int check=dup2(ipfile, 0);
					if(check < 0)
					{
						printf("Error in dup\n");
						continue;
					}
				}

				if(ap == 1 || op ==1)
				{
					int check=dup2(opfile, 1);
					if(check < 0)
					{
						printf("Error in dup\n");
						continue;
					}
				}
				int check=execvp(argv[0],argv);
				if(check==-1)
				{
					perror("Error in execvp");
				}
				close(opfile);
				close(ipfile);
			}
			int stat;
			waitpid(id, &stat, WUNTRACED);
			if(op==0 && ap==0)
				printf("");
			exit(EXIT_FAILURE);
			//Ahh shit!!! Done at last
		}
		else
		{
			wait(NULL);
			close(pipe_arr[1]);
			pipe_var = pipe_arr[0];
			i++;
		}
		int stat;
		waitpid(id, &stat, WUNTRACED);
	}
}
void pwd()
{
	char *path = (char *)malloc(1000 * sizeof(char));
	char *check=getcwd(path,1000);
	// printf("%s\n", path);
	isunderhome(path);
}
void pwd2()
{
	char *path = (char *)malloc(1000 * sizeof(char));
	char *check=getcwd(path,1000);
	if(check==NULL)
	{
		perror("pwd didn't happen");
		return;
	}
	printf("%s\n", path);
}
int cd(char *path)
{
	char *before=(char *)malloc(1000 * sizeof(char));
	strcpy(before, p_dir);
	if(path[0]=='~')
	{
		char *temp=(char *)malloc(1000 * sizeof(char));
		char *temp2=(char *)malloc(1000 * sizeof(char));
		getSubString(path,temp,1, strlen(path)-1);
		strcpy(temp2,home);
		strcat(temp2,temp);
		strcpy(path,temp2);
	}
	int state = chdir(path);
	if(state == -1)
	{
		cd(before);
		
		return 0;
	}
	pwd();
	return 1;
	// printf("%s\n", p_dir);
}
void echo(int argc,char **argv)
{
	if(argc!=1)
		for(int i=1;i<argc;i++)
			printf("%s ",argv[i]);
	printf("\n");
}
void ls(int a,int l,char *path)
{
	char *before=(char *)malloc(1000 * sizeof(char));
	strcpy(before, p_dir);
	int fol_exists=cd(path);
	cd(before);
	if(!fol_exists)
	{
		perror("Illegal usage of ls");
		return;
	}
	if(path[0]=='~')
	{
		char *temp=(char *)malloc(1000 * sizeof(char));
		char *temp2=(char *)malloc(1000 * sizeof(char));
		getSubString(path,temp,1, strlen(path)-1);
		strcpy(temp2,home);
		strcat(temp2,temp);
		strcpy(path,temp2);
	}
	// char *temp=(char *)malloc(1000 * sizeof(char));
	// int fol_exists=cd(path);
	// cd(before);
	DIR *folder;
	folder=opendir(path);
	struct dirent *myfile;
    struct stat mystat;
    if(folder==NULL)
    	printf("%s\n",path);
    while((myfile = readdir(folder)) != NULL)
    {
    	if((strcmp(myfile->d_name, "..") == 0 || strcmp(myfile->d_name, ".") == 0))
    	{
    		if(a==0)
    			continue;
    	}
    	if(l)
    	{
    		char *temp=(char *)malloc(2000 * sizeof(char));
    		char *line=(char *)malloc(10000 * sizeof(char));
    		sprintf(temp, "%s/%s", path, myfile->d_name);
        	stat(temp, &mystat);
    		strcat(line,(S_ISDIR(mystat.st_mode)) ? "d" : "-");
			strcat(line,(mystat.st_mode & S_IRUSR) ? "r" : "-");
			strcat(line,(mystat.st_mode & S_IWUSR) ? "w" : "-");
			strcat(line,(mystat.st_mode & S_IXUSR) ? "x" : "-");
			strcat(line,(mystat.st_mode & S_IRGRP) ? "r" : "-");
			strcat(line,(mystat.st_mode & S_IWGRP) ? "w" : "-");
			strcat(line,(mystat.st_mode & S_IXGRP) ? "x" : "-");
			strcat(line,(mystat.st_mode & S_IROTH) ? "r" : "-");
			strcat(line,(mystat.st_mode & S_IWOTH) ? "w" : "-");
			strcat(line,(mystat.st_mode & S_IXOTH) ? "x" : "-");
			char temp2[10000]="\t";
			strcat(temp2,itoa((int)(mystat.st_nlink),10));
			strcat(line,temp2);
			strcpy(temp2,"\t");
			strcat(temp2,getpwuid(mystat.st_uid)->pw_name);
			strcat(line,temp2);
			strcpy(temp2,"\t");
			strcat(temp2,getgrgid(mystat.st_gid)->gr_name);
			strcat(line,temp2);
			strcpy(temp2,"\t");
			strcat(temp2,itoa((long long int)(mystat.st_size),10));
			strcat(line,temp2);
			strcpy(temp2,"\t");
			time_t t = time(NULL);
			struct tm *tmp = localtime(&t);
			char time[200];
			if(tmp->tm_year == localtime(&mystat.st_ctime)->tm_year)
				strftime(time, 200, "%b %e %R", localtime(&mystat.st_mtime));
			else
				strftime(time, 200, "%b %e %Y", localtime(&mystat.st_mtime));			
			strcat(temp2,time);
			strcat(line,temp2);
			strcpy(temp2,"\t");
			strcat(temp2,myfile->d_name);
			strcat(line,temp2);
			strcpy(temp2,"\t");
			printf("%s\n",line);
    	}
    	else
    	{
    		printf("%s\n", myfile->d_name);
    	}
    }
}
void pinfo(char *arg){
	char p_name[1000]="/proc/",path2[2000];
	strcat(p_name,arg);
	sprintf(path2,"%s%s",p_name,"/exe");
	strcat(p_name,"/stat");
	char result[1000]="";
	//printf("%s\n", path2);
	int check=readlink(path2, result, sizeof(result));
	int handle = open(p_name, O_RDONLY);
	if(handle==-1 || check==-1)		
	{
		perror("Pinfo error");
		return;
	}
	char data[5000];
	read(handle, data, 5000);
	// printf("%s\n", data);
	int i = 0;
	char **tokenset = (char**)malloc(5000 * sizeof(char* ));
	char *token=(char *)malloc(5000 * sizeof(char));
	token = strtok(data, " ");
	while(token != NULL)
	{
		tokenset[i] = token;
		++i;
		token = strtok(NULL, " ");
	}
	if(strncmp(result, home, strlen(home))==0)
	{
		char *target = (char *)malloc(1000 * sizeof(char));	
		getSubString(result,target,strlen(home), strlen(result)-1);
		char temp[1000]="~";
		strcat(temp,target);
		strcpy(result,temp);
	}
	printf("pid -- %s\n", tokenset[0]);
	printf("Process Status -- %s\n", tokenset[2]);
	printf("memory-- %s\n", tokenset[23]);
	printf("Executable Path -- %s\n", result);
}
void jobs()
{
	int valid=0,count=1;
	for(int i=0;i<bg;i++)
	{
		// printf("%d\n", i);
		// int temp = kill(bg_processid[i], 0);
		if(bg_processid[i]!=-1)
		{
			valid=1;
			char p_name[1000]="/proc/",path2[2000];
			char temp2[1000];
			sprintf(temp2, "%d", bg_processid[i]);
			strcat(p_name,temp2);
			strcat(p_name,"/stat");
			char result[1000]="";
			//printf("%s\n", path2);
			int handle = open(p_name, O_RDONLY);
			char data[5000];
			read(handle, data, 5000);
			// printf("%s\n", data);
			int j = 0;
			char **tokenset = (char**)malloc(5000 * sizeof(char* ));
			char *token=(char *)malloc(5000 * sizeof(char));
			token = strtok(data, " ");
			while(token != NULL)
			{
				tokenset[j] = token;
				++j;
				token = strtok(NULL, " ");
			}
			if(strcmp(tokenset[2], "T") == 0)
				printf("[%d] %s %s [%d]\n", count, "Stopped", bg_processname[i], bg_processid[i]);
			else
				printf("[%d] %s %s [%d]\n", count, "Running", bg_processname[i], bg_processid[i]);
			count++;
		}
	}
	if(valid==0)
		printf("No background jobs\n");
}
void kjob(int id,int sig)
{
	int valid=0,count=0;
	for(int i=0;i<bg;i++)
	{
		if(bg_processid[i]!=-1)
		{
			count++;
			if(id==count)
			{
				int check=kill(bg_processid[i], sig);
				if(check==-1 && errno==EINVAL)
				{
					perror("Kjob error");
					break;
				}
				valid=1;
				int stat;
				while(waitpid(-1, &stat, WNOHANG) > 0);
				fprintf(stderr,"Process with id %d killed succesfully with signal %d\n", bg_processid[i],sig);
				if(sig==9)
					bg_processid[i]=-1;
				break;
			}
		}
	}
	if(valid==0)
		printf("Process not found\n");
}
void fg(int id)
{
	int valid=0,count=0;
	for(int i=0;i<bg;i++)
	{
		if(bg_processid[i]!=-1)
		{
			count++;
			if(id==count)
			{
				int check=kill(bg_processid[i], SIGCONT);
				printf("Process with id %d moved to foreground.\n", bg_processid[i]);
				valid=1;
				int stat;
				currprocid=bg_processid[i];
				sprintf(currprocname,"%s",bg_processname[i]);
				bg_processid[i]=-1;
				waitpid(currprocid, &stat, WUNTRACED);
				currprocid=-1;
				sprintf(currprocname,"");
				break;
			}
		}
	}
	if(valid==0)
		printf("Process not found\n");
}
void bg1(int id)
{
	int valid=0,count=0;
	for(int i=0;i<bg;i++)
	{
		if(bg_processid[i]!=-1)
		{
			count++;
			if(id==count)
			{
				int check=kill(bg_processid[i], SIGCONT);
				printf("Process with id %d moved to background.\n", bg_processid[i]);
				valid=1;
				break;
			}
		}
	}
	if(valid==0)
		printf("Process not found\n");
}
void cronjob(char **arg,int c_pres,int t_pres,int p_pres)
{
	int t=atoi(arg[t_pres+1]);
	int p=atoi(arg[p_pres+1]);
	char **argv = (char **)malloc(1000 * sizeof(char *));
	int i=0;
	// argv[0]=arg[0];
	// printf("%s",argv[0]);
	for(;i<t_pres-c_pres-1;i++)
	{
		argv[i]=arg[c_pres+i+1];
	}
	// printf("abc %d %d %d %s %s\n" ,p/t,t,p,arg[p_pres],arg[p_pres+1]);
	argv[i]=NULL;
	int time=0; 
	for(i=0;;i++)
	{
		// printf("abc\n");
		sleep(t);
		time+=t;
		int k=fork();
		if(k==0)
		{
			int temp=execvp(argv[0],argv);
			if(temp==-1)
				printf("Command not found\n");
			exit(1);
		}
		int status;
		currprocid=k;
		waitpid(k, &status, WUNTRACED);
		currprocid=-1;
		sprintf(currprocname,"");
		if(time<=p && time>(p-t))
		{
			sleep(p-t);
			break;
		}
	}
}
void overkill()
{
	int valid=0,count=0;
	for(int i=0;i<bg;i++)
	{
		if(bg_processid[i]!=-1)
		{
			count++;
			kill(bg_processid[i], 9);
			fprintf(stderr,"Process with id %d killed succesfully\n", bg_processid[i]);
			bg_processid[i]=-1;
		}
	}
	return;
}
void up(int count,char history[max][500],int cnt)
{
	// printf("%d %d\n",cnt,count );
	if(count>10)
		printf("Invalid times given");
	else if(count>cnt)
		return;
	else{
		if(cnt>=20)
		{
			sprintf(tempup,"%s",history[21-count]);
			sprintf(tempup,"%s",LastcharDel(tempup));
		}
		else
		{
			sprintf(tempup,"%s",history[cnt+1-count]);
			sprintf(tempup,"%s",LastcharDel(tempup));
		}

	}
	// printf("%s",tempup);
}
int main()
{
	char *user,*machine;
	char history[21][500];
	int historycount=0;
	int front, rear, reply;
	user = (char *)malloc(1000 * sizeof(char));
	machine = (char *)malloc(1000 * sizeof(char));
	getlogin_r(user, 1000);
	gethostname(machine, 1000);
	getcwd(home, 1000);
	int upvar=0;
	while(1) 
	{
		signal(SIGINT, handler);
		signal(SIGTSTP, handler);
		signal(SIGSTOP, handler);
		signal(SIGABRT, handler);
		int status;
		// printf("*\n");
		int pid=waitpid(-1, &status, WNOHANG );
		while (pid > 0)
        {
            char *pname;
            // printf("%d\n", pid);
            for (int i = 0; i < bg; ++i)
            {
                if (bg_processid[i] == pid)
                {
                    char pname[100]="";
                    strcat(pname,bg_processname[i]);
                    bg_processid[i] = -1;
                    fprintf(stderr, "%s with pid %d exited normally.\n", pname, (int)pid);
                    break;
                }
            }
            pid=waitpid(-1, &status, WNOHANG );
        }
        char *temptotal = (char *)malloc(10000 * sizeof(char));
        char *total = (char *)malloc(10000 * sizeof(char));
        if(upvar)
        {
        	sprintf(total,"%s\n",tempup);
        	strcpy(temptotal,total);
        	// printf("%s\n", total);
        	upvar=0;
        }
        else
        {
			printf("<%s@%s:%s>", user, machine, p_dir);
			// add_history(tempup);
			sprintf(tempup,"");
			// char *total = (char *)malloc(10000 * sizeof(char));
			size_t length=0;
			int got_back = getline(&total, &length, stdin);
			// char *temptotal = (char *)malloc(10000 * sizeof(char));
			strcpy(temptotal,total);
			if(got_back <= 1)
				continue;
		}
		// printf("%d%d%d\n", (int)total[0],(int)total[1],(int)total[2]);
		char *temp_input = (char *)malloc(1000 * sizeof(char));
		temp_input = strtok(total, ";");
		char **commands = (char **)malloc(1000 * sizeof(char *));
		int cmdcount=0;
		while(temp_input != NULL)
		{
			commands[cmdcount] = temp_input;
			temp_input = strtok(NULL, ";");
			cmdcount++;
		}
		commands[cmdcount-1][strlen(commands[cmdcount-1])-1]=0;
		for(int i=0;i<cmdcount;i++)
		{
			
			// printf("%s\n", commands[i]);
		}
		if((int)temptotal[0]!=27)
		{	
			reply = insq(history, &rear, temptotal);
			historycount++;
    	}
    	if(reply==-1)
    	{
    		for(int i=2;i<=rear;i++)
    		{
    			sprintf(history[i-1],"%s",history[i]);
    		}
    		sprintf(history[rear],"%s",temptotal);
    	}
    	// for(int i=rear;i>(rear-10<0?0:rear-10);i--)
	    // 	printf("%s", history[i]);
	    int k=0;
		for(int i=0;i<cmdcount;i++)
		{
			int pipe=0;
			char t_input[1000];
			int len=strlen(commands[i]);
			for(int j=0;j<len;j++)
			{
				if(commands[i][j]=='|')
					pipe=1;
				if(commands[i][j]=='<')
				{
					t_input[k]=' ';
					t_input[k+1]='<';
					t_input[k+2]=' ';
					k+=3;
				}
				else if(commands[i][j]=='>')
				{
					if((j+1)!=len && commands[i][j+1]=='>')
					{
						t_input[k]=' ';
						t_input[k+1]='>';
						t_input[k+2]='>';
						t_input[k+3]=' ';
						k+=4;
						j++;
					}
					else
					{
						t_input[k]=' ';
						t_input[k+1]='>';
						t_input[k+2]=' ';
						k+=3;
					}
				}
				else
				{
					t_input[k]=commands[i][j];
					k++;
				}
			}
			t_input[k]=0;
			// printf("%s\n", t_input);
			sprintf(commands[i],"%s",t_input);
			commands[i]=trimwhitespace(commands[i]);
			if(commands[i][0]=='\0')
				continue;
			if(pipe==1)
			{
				// printf("%s\n", commands[i]);
				char piped[100][100][100];
				char *t;
				char *token=strtok_r(commands[i],"|",&t);
				int i=0;
				while(token!=NULL)
				{
					int j=0;
					char temp4[1000];
					sprintf(temp4,"%s",token);
					token=strtok_r(NULL,"|",&t);
					char *s;
					char *token2=strtok_r(temp4,delimit,&s);
					while(token2!=NULL)
					{
						sprintf(piped[i][j],"%s",token2);
						token2=strtok_r(NULL,delimit,&s);
						j++;
					}
					strcpy(piped[i][j],"end");
					i++;
				}
				// printf("%d\n", i);
				run_pipe(piped,i);
				continue;
			}

			char cmdcopy[1000]="",cmdcopy2[1000]="";
			sprintf(cmdcopy2,"%s",commands[i]);
			int arct=0;
			char **arvt=(char **)malloc(1000 * sizeof(char *));
			char* token = strtok(cmdcopy2, delimit);  
		    while (token != NULL) 
		    { 
		        arvt[arct]=token;
		        token = strtok(NULL, delimit);
		        arct++; 
	    	}
	    	// for(int j=0;j<arct;j++)
	    	// {
	    	// 	printf("%s\n", arvt[j]);
	    	// }
	    	int ip=0,op=0,ap=0,ipfile,opfile,apfile;
	    	char ipfilepath[1000]="";
	    	char opfilepath[1000]="";
	    	char apfilepath[1000]="";
	    	for(int j=0;j<arct;j++)
	    	{
	    		if(arvt[j][0]=='<' && arvt[j][1]=='\0')
	    		{
	    			ip++;
	    			strcpy(ipfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}
	    		else if(arvt[j][0]=='>' && arvt[j][1]=='\0')
	    		{
	    			op++;
	    			strcpy(opfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}
	    		else if(arvt[j][0]=='>' && arvt[j][1]=='>' && arvt[j][2]=='\0')
	    		{
	    			ap++;
	    			strcpy(apfilepath,arvt[j+1]);
	    			arvt[j]="";
	    			arvt[j+1]="";
	    		}	
	    	}
	    	sprintf(t_input,"");
	    	for(int j=0;j<arct;j++)
	    	{
	    		sprintf(t_input + strlen(t_input),"%s ",arvt[j]);
	    	}
	    	sprintf(commands[i],"%s",t_input);
			commands[i]=trimwhitespace(commands[i]);
			int argc=0;
			char **argv = (char **)malloc(1000 * sizeof(char *));
			token = strtok(commands[i], delimit);  
		    while (token != NULL) 
		    { 
		        argv[argc]=token;
		        token = strtok(NULL, delimit);
		        argc++; 
	    	}
	    	// printf("%s*\n%s*\n%s*\n%s*\n", t_input,ipfilepath,opfilepath,apfilepath);
	    	if(ap>1 || op>1 || ip>1 || (ap+op)>1)
	    	{
	    		printf("Invalid redirection\n");
	    		continue;
	    	}
	    	if(ip==1)
	    	{
	    		ipfile=open(ipfilepath, O_RDONLY, 0);
	    		if(ipfile < 0)
				{
					printf("Error in input file\n");
					continue;
				}
	    	}
	    	if(op==1)
	    	{

	    		opfile=creat(opfilepath,  0777);
	    		if(opfile < 0)
				{
					printf("Error in output file\n");
					continue;
				}
	    	}
	    	else if(ap==1)
	    	{
	    		opfile=open(apfilepath, O_WRONLY | O_APPEND | O_CREAT, 0777);
	    		if(opfile < 0)
				{
					printf("Error in output file\n");
					continue;
				}
	    	}
	    	if(ip==1 || ap==1 || op==1)
	    	{
	    		int id=fork();
	    		if(id<0)
	    		{
	    			perror("Fork error");
	    		}
	    		if(id==0)
	    		{
	    			argv[argc]=NULL;
		    		if(ip == 1)
					{
						int check=dup2(ipfile, 0);
						if(check < 0)
						{
							printf("Error in dup\n");
							continue;
						}
					}

					if(ap == 1 || op ==1)
					{
						int check=dup2(opfile, 1);
						if(check < 0)
						{
							printf("Error in dup\n");
							continue;
						}
					}
					int check=execvp(argv[0],argv);
					if(check==-1)
					{
						perror("Error in execvp");
					}
					close(opfile);
					close(ipfile);
				}
				int stat;
				waitpid(id, &stat, WUNTRACED);
				if(op==0 && ap==0)
					printf("\n");
	      		continue;
	    	}
	    	
	    	// printf("%d\n", argc);
	    	// for(int j=0;j<argc;j++)
	    	// {
	    	// 	printf("%s\n", argv[j]);
	    	// } 
	    	int amp=0;
	    	if(argv[argc-1][0]=='&')
	    		{amp=1;argc--;}
	    	for(int j=0;j<argc;j++)
	    	{
	    		sprintf(cmdcopy + strlen(cmdcopy),"%s ",argv[j]);
	    	}
	    	if(strcmp(argv[0],"pwd")==0)
	    	{
	    		if(argc==1)
	    		{
	    			pwd2();
	    		}
	    		else
	    			invalid("pwd");
	    	}
	    	else if(strcmp(argv[0],"cd")==0)
	    	{
	    		if(argc==2)
	    		{
	    			int check=cd(argv[1]);
	    			if(!check)
	    				perror("Illegal cd");
	    		}
	    		else
	    			invalid("cd");
	    	}
	    	else if(strcmp(argv[0],"echo")==0)
	    	{
	    		echo(argc,argv);
	    	}
	    	else if(strcmp(argv[0],"ls")==0)
	    	{
	    		if(argc>4)
	    			{invalid("ls");continue;}
	    		int ls_a=0,ls_l=0,folcheck=0;
	    		char folder[1000];
	    		for (int i = 1; i < argc; ++i)
	    		{
	    			if(strcmp(argv[i],"-a")==0)
	    				ls_a=1;
	    			else if(strcmp(argv[i],"-l")==0)
	    				ls_l=1;
	    			else if(strcmp(argv[i],"-la")==0 || strcmp(argv[i],"-al")==0)
	    			{
	    				ls_l=1;
	    				ls_a=1;
	    			}
	    			else
	    			{
	    				if(!folcheck)
	    				{
	    					folcheck=1;
	    					strcpy(folder,argv[i]);
	    				}
	    				else
	    				{
	    					invalid("ls");
	    					continue;
	    				}
	    			}
	    		}
	    		if(folcheck==1)
	    			ls(ls_a,ls_l,folder);
	    		else
	    		{
	    			char *temp=(char *)malloc(1000 * sizeof(char));
	    			sprintf(temp,"%s",p_dir);
	    			ls(ls_a,ls_l,temp);
	    		}
	    	}
	    	else if(strcmp(argv[0],"pinfo")==0)
	    	{
	    		if(argc>2)
	    			{invalid("pinfo");continue;}
	    		char *temp=(char *)malloc(1000 * sizeof(char));
	    		if(argc==1)
	    		{
	    			sprintf(temp,"%s",itoa(getpid(),10));
	    			pinfo(temp);
	    		}
	    		else
	    		{
	    			sprintf(temp,"%s",argv[1]);
	    			pinfo(temp);
	    		}
	    	}
	    	else if(strcmp(argv[0],"setenv")==0)
	    	{
	    		if(argc==1 || argc >3)
	    			{invalid("setenv");continue;}
	    		char *temp=(char *)malloc(1000 * sizeof(char));
	    		if(argc==2)
	    		{
	    			setenv(argv[1],"",1);
	    			printf("Environment variable %s set up with empty string\n", argv[1]);
	    		}
	    		else
	    		{
	    			setenv(argv[1],argv[2],1);
	    			printf("Environment variable %s set up with %s\n", argv[1],argv[2]);
	    		}

	    	}
	    	else if(strcmp(argv[0],"unsetenv")==0)
	    	{
	    		if(argc!=2)
	    			{invalid("unsetenv");continue;}
	    		int temp=unsetenv(argv[1]);
	    		if(temp==-1)
	    			perror("unsetenv error");
	    		else
	    			printf("Variable %s removed\n", argv[1]);

	    	}
	    	else if(strcmp(argv[0],"jobs")==0)
	    	{
	    		if(argc!=1)
	    			{invalid("jobs");continue;}
	    		jobs();
	    	}
	    	else if(strcmp(argv[0],"kjob")==0)
	    	{
	    		if(argc!=3)
	    			{invalid("kjob");continue;}
	    		kjob(atoi(argv[1]),atoi(argv[2]));
	    	}
	    	else if(strcmp(argv[0],"fg")==0)
	    	{
	    		if(argc!=2)
	    			{invalid("fg");continue;}
	    		fg(atoi(argv[1]));
	    	}
	    	else if(strcmp(argv[0],"bg")==0)
	    	{
	    		if(argc!=2)
	    			{invalid("bg");continue;}
	    		bg1(atoi(argv[1]));
	    	}
	    	else if(strcmp(argv[0],"overkill")==0)
	    	{
	    		if(argc!=1)
	    			{invalid("overkill");continue;}
	    		overkill();
	    	}
	    	else if(strcmp(argv[0],"quit")==0)
	    	{
	    		if(argc!=1)
	    			{invalid("quit");continue;}
	    		overkill();
	    		printf("See you again. Bye!!!\n");
	    		exit(0);
	    	}

	    	else if(strcmp(argv[0],"history")==0)
	    	{
	    		if(argc==1)
	    		{
	    			for(int i=rear;i>((rear-10<0)?0:rear-10);i--)
	    				printf("%s", history[i]);
	    		}
	    		else if(argc==2 && (strlen(argv[1])==1 || strlen(argv[1])==2))
	    		{
	    			int num=0;
	    			if(strlen(argv[1])==1)
	    				num+=(int)argv[1][0]-48;
	    			else if(strlen(argv[1])==2)
	    				num+=((int)argv[1][0]-48)*10+(int)argv[1][1]-48;
	    			if(num>1 && num<=10)
	    			{
	    				int temp=((rear-num)<0)?0:rear-num;
	    				for(int i=rear;i>(temp);i--)
	    					printf("%s",history[i]);	
	    			}
	    			else
	    			{
	    				invalid("history");
	    			}
	    		}
	    		else
	    		{
	    			invalid("history");
	    		}
	    	}
	    	else if(strcmp(argv[0],"cronjob")==0)
	    	{
	    		if(argc<7)
	    			{invalid("cronjob");continue;}
	    		int c_pres=0,t_pres=0,p_pres=0;
	    		for(int l=0;l<argc;l++)
	    		{
	    			if(strcmp(argv[l],"-c")==0)
	    				c_pres=l;
	    			if(strcmp(argv[l],"-t")==0)
	    				t_pres=l;
	    			if(strcmp(argv[l],"-p")==0)
	    				p_pres=l;
	    		}
	    		if(c_pres!=0 && t_pres!=0 && p_pres!=0)
	    			cronjob(argv,c_pres,t_pres,p_pres);
	    		else
	    			{invalid("cronjob");continue;}
	    	}
	    	else if(((int)argv[0][0]==27) && ((int)argv[0][1]==91) && ((int)argv[0][2]==65) && (strlen(argv[0])%3==0))
	    	{
	    		if(argc>1)
	    			{printf("Command not found\n");continue;}
	    		int count=0,valid=1;
	    		for(int j=0;j<strlen(argv[0]);j+=3)
	    		{
	    			if(((int)argv[0][j]==27) && ((int)argv[0][j+1]==91) && ((int)argv[0][j+2]==65))
	    				count++;
	    			else
	    				{printf("Command not found\n");valid=0;break;}
	    		}
	    		if(valid==0)
	    			continue;
	    		up(count,history,historycount);
	    		upvar=1;
	    	}
	    	else
	    	{
	    		argv[argc]=NULL;
	    		int status;
	    		if(!amp)
	    		{
	    			int pid=fork();
		    		if(pid==0)
		    		{
		    			// setpgid(0,0);
		    			int temp=execvp(argv[0],argv);
		    			if(temp==-1)
		    				printf("Command not found\n");
		    			exit(1);
		    		}
		    		sprintf(currprocname, "%s",cmdcopy);
					currprocid=pid;
		    		waitpid(pid, &status, WUNTRACED);
		    		currprocid=-1;
					sprintf(currprocname,"");
		    	}
		    	else
		    	{
		    		int pid=fork();
		    		int sdf=0;
		    		if(pid==0)
		    		{
		    			int temp=execvp(argv[0],argv);
		    			if(temp==-1)
		    				printf("Command not found\n");
		    			exit(1);
		    		}
		    		else
		    			printf("Process with pid %d has started\n",pid);
		    		bg_processid[bg] = (int)pid;
					sprintf(bg_processname[bg], "%s",cmdcopy);
					bg++;
					waitpid(pid, &status, WNOHANG);

		    	}
	    	}
	    } 
		
	}
}
