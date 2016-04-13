#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>

int slave;
int main(int argc, char *argv[]){
	int i = 0;
	int status;
	FILE *fptr;
	if(strcmp(argv[1],"1")==0){
		slave = 0;
		fptr = fopen("count.txt","w");
		fclose(fptr);
		i = 1;
	}
	else
	{
		slave = 1;
		printf("Yo from the slave\n");
	}

	
	while(1){
		
		if(slave == 0){
			if(fork() == 0){ 
	            // Child process will return 0 from fork()
	            status = system("gnome-terminal -x ./jaman 0");
	            printf("Backup spawned.\n");
	            exit(0);
	        }
	            else{
	            // Parent process will return a non-zero value from fork()
	            printf("I'm the big dog.\n");
	        }
		}
		while(slave == 0){
			printf("Halla fra Big Boss\n");
			fptr = fopen("count.txt","a");
			fprintf(fptr,"%d\n",i);
			printf("i: %d\n",i);
			i++;
			fclose(fptr);
			sleep(1);
		}
		
		FILE *fptrs;
		int same_read=0;
		int new=0;
		int old=0;
		char read[5];
		while(same_read<1&&slave==1){
			fptrs = fopen("count.txt","r");
			fseek(fptrs,0,SEEK_SET);
			while(!feof(fptrs)){
				memset(read,0x00,5);
				fscanf(fptrs,"%[^\n]\n", read);
			}
		
			fclose(fptrs);

			new = atoi(read);
			if(new==old)
				same_read+=1;
			else
				same_read=0;
			old = new;
			sleep(1);
		}
		slave = 0;
		i = new+1;
	}
	return 0;



}
