#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char** argv)
{

	// 2.1.1
	if(argv[1][1]=='c'){
		//working on directory
		struct dirent* de;
		struct stat sbuf;
		char** files=(char**)malloc(30000);
		DIR* dr = opendir(argv[2]);

		// Error handling
		if(dr==NULL){
			printf("Failed to complete creation operation\n");
			exit(-1);
		}

        chdir(argv[2]);
		int fd=open(argv[3],O_RDWR|O_CREAT,0644);
		int n_files=0;
		while((de=readdir(dr))!=NULL){
			if(strcmp(de->d_name,argv[3]) && strcmp(de->d_name,".") && strcmp(de->d_name,"..")){
			    n_files++;
			}
		}
		char num_of_files[6];
		sprintf(num_of_files,"%d",n_files);
		num_of_files[5]='\0';
		if(write(fd,num_of_files,6)<0){
			printf("Failed to complete creation operation\n");
			exit(-1);
		}
		closedir(dr);
        dr=opendir(".");

		while((de=readdir(dr))!=NULL){
			if(de->d_type==DT_REG && strcmp(de->d_name,argv[3]) && strcmp(de->d_name,".") && strcmp(de->d_name,"..")){
			    int fd2=open(de->d_name,O_RDONLY);

				//get the file size
				int c=fstat(fd2,&sbuf);
				long size=sbuf.st_size;

				// to store the file data
				char buf[512]; //Content of the file
				char* file=de->d_name;
				char file_name[17]; //Name of the file
				char file_size[11]; //size of the file
				sprintf(file_size,"%ld",size);
				file_size[11]='\0';
				//sprintf(file_name,"%s",file);
				/*for(int i=0;i<11;i++){
					if(i<strlen(filesize))file_size[i]=filesize[i];
					else file_size[i]='\0';
				}*/
				for(int i=0;i<17;i++){
					if(i<strlen(file))file_name[i]=file[i];
					else file_name[i]='\0';
				}

				//writing file name to the tar file
				if(write(fd,file_name,17)<=0){
					printf("Failed to complete creation operation\n");
			        exit(-1);
				}
				//writing file size to the tar file
				if(write(fd,file_size,11)<=0){
					printf("Failed to complete creation operation\n");
			        exit(-1);
				}
				// Read as chunks and write as chunks
				long pressize=size;
                while(1){
					if(pressize<=512){
						//read from the file
				        if(read(fd2,buf,pressize)<0){
					        printf("Failed to complete creation operation\n");
			                exit(-1);
				        }
				        //write content to tar file
				        if(write(fd,buf,pressize)<0){
					        printf("Failed to complete creation operation\n");
			                exit(-1);
				        }
						pressize=0;
						break;
					}
					else{
						//read from the file
				        if(read(fd2,buf,512)<0){
					        printf("Failed to complete creation operation\n");
			                exit(-1);
				        }
				        //write content to tar file
				        if(write(fd,buf,512)<0){
					        printf("Failed to complete creation operation\n");
			                exit(-1);
				        }
						pressize-=512;
					}

				}
				/*//read from the file
				if(read(fd2,buf,size)<=0){
					printf("Failed to complete creation operation\n");
			        exit(-1);
				}
				//write content to tar file
				if(write(fd,buf,size)<=0){
					printf("Failed to complete creation operation\n");
			        exit(-1);
				}*/
				close(fd2);
			}
		}
		close(fd);
	}

	// 2.1.2
	else if(argv[1][1]=='d'){
		int j=0;
		for(int i=0;i<strlen(argv[2]);i++){
			if(argv[2][i]=='/'){
				j=i;
			}
		}
		char path[j+1];
		path[j]='\0';
		for(int i=0;i<j;i++){
			path[i]=argv[2][i];
		}
		int filenamesize=strlen(argv[2])-j;
		// directory name
		char direc[filenamesize];
		direc[filenamesize-1]='\0';
		for(int i=filenamesize-6;i>=0;i--){
			direc[i]=argv[2][i+j+1];
		}
		direc[filenamesize-5]='D';
		direc[filenamesize-4]='u';
		direc[filenamesize-3]='m';
		direc[filenamesize-2]='p';
		//printf("%s %s",direc,path);

		int fd=open(argv[2],O_RDONLY);
		if(fd<0){
			printf("Failed to complete extraction operation\n");
			exit(-1);
		}

		// go to the newly created directory
		chdir(path);
		int check=mkdir(direc,0777);
		chdir(direc);

		//find the number of files in the tar file
		char num_of_files[6];
		if(read(fd,num_of_files,6)<0){
			printf("Failed to complete extraction operation\n");
			exit(-1);
		}
		int n_files=atoi(num_of_files);

		//Now extract each file
		char file_name[17];
		char file_size[11];
		long filesize;
		for(int i=1;i<=n_files;i++){
			if(read(fd,file_name,17)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			if(read(fd,file_size,11)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			int fd2=open(file_name,O_WRONLY|O_CREAT,0644);
			filesize=atol(file_size);
			char buf[512];
			/*if(read(fd,buf,filesize)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			if(write(fd2,buf,filesize)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }*/

			// Read as chunks and write as chunks
			long pressize=filesize;
            while(1){
			    if(pressize<=512){
					//read from the file
				    if(read(fd,buf,pressize)<0){
					    printf("Failed to complete creation operation\n");
			            exit(-1);
				    }
				    //write content to tar file
				    if(write(fd2,buf,pressize)<0){
					    printf("Failed to complete creation operation\n");
			            exit(-1);
				    }
					pressize=0;
					break;
				}
				else{
					//read from the file
				    if(read(fd,buf,512)<0){
					    printf("Failed to complete creation operation\n");
			            exit(-1);
				    }
				    //write content to tar file
				    if(write(fd2,buf,512)<0){
					    printf("Failed to complete creation operation\n");
			            exit(-1);
				    }
					pressize-=512;
					}
				}
			close(fd2);
		}
		close(fd);
	}

	// 2.2
	else if(argv[1][1]=='e'){
		int j=0;
		for(int i=0;i<strlen(argv[2]);i++){
			if(argv[2][i]=='/'){
				j=i;
			}
		}
		char path[j+1];
		path[j]='\0';
		for(int i=0;i<j;i++){
			path[i]=argv[2][i];
		}
		//int filenamesize=strlen(argv[2])-j;
		// directory name
		char *direc="IndividualDump";
		/*direc[filenamesize-1]='\0';
		for(int i=filenamesize-6;i>=0;i--){
			direc[i]=argv[2][i+j+1];
		}
		direc[filenamesize-5]='D';
		direc[filenamesize-4]='u';
		direc[filenamesize-3]='m';
		direc[filenamesize-2]='p';*/
		//printf("%s %s",direc,path);

		int fd=open(argv[2],O_RDONLY);
		if(fd<0){
			printf("Failed to complete extraction operation\n");
			exit(-1);
		}

		// go to the newly created directory
		chdir(path);
		int check=mkdir(direc,0777);
		chdir(direc);

		//find the number of files in the tar file
		char num_of_files[6];
		if(read(fd,num_of_files,6)<0){
			printf("Failed to complete extraction operation\n");
			exit(-1);
		}
		int n_files=atoi(num_of_files);

		//Now extract the given file
		char file_name[17];
		char file_size[11];
		long filesize;
		int file_existence=0;
		for(int i=1;i<=n_files;i++){
			if(read(fd,file_name,17)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			if(read(fd,file_size,11)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			filesize=atol(file_size);
			//char buf[512];
			if(strcmp(file_name,argv[3])){
				if(lseek(fd,filesize,SEEK_CUR)<0){
			        printf("Failed to complete extraction operation\n");
					exit(-1);
		        }
				continue;
			}
			int fd2=open(file_name,O_WRONLY|O_CREAT,0644);
			/*if(read(fd,buf,filesize)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }
			if(write(fd2,buf,filesize)<0){
			    printf("Failed to complete extraction operation\n");
			    exit(-1);
		    }*/
			long pressize=filesize;
			char buf[512];
            while(1){
			    if(pressize<=512){
					//read from the file
				    if(read(fd,buf,pressize)<0){
					    printf("Failed to complete extraction operation\n");
			            exit(-1);
				    }
				    //write content to tar file
				    if(write(fd2,buf,pressize)<0){
					    printf("Failed to complete extraction operation\n");
			            exit(-1);
				    }
					pressize=0;
					break;
				}
				else{
					//read from the file
				    if(read(fd,buf,512)<0){
					    printf("Failed to complete extraction operation\n");
			            exit(-1);
				    }
				    //write content to tar file
				    if(write(fd2,buf,512)<0){
					    printf("Failed to complete extraction operation\n");
			            exit(-1);
				    }
					pressize-=512;
				}
			}
			close(fd2);
			file_existence=1;
			exit(0);
		}
		if(!file_existence){
			printf("No such file is present in tar file.\n");
		}
		close(fd);
	}

	// 2.3
	else if(argv[1][1]=='l'){
		struct stat sbuf;
		int j=0;
		for(int i=0;i<strlen(argv[2]);i++){
			if(argv[2][i]=='/'){
				j=i;
			}
		}
		char path[j+1];
		path[j]='\0';
		for(int i=0;i<j;i++){
			path[i]=argv[2][i];
		}
		int fd=open(argv[2],O_RDONLY);
		if(fd<0){
			printf("Failed to complete list operation\n");
			exit(-1);
		}
		chdir(path);
		int fd2=open("tarStructure",O_WRONLY|O_CREAT,0644);

		// print tar file size
		int c=fstat(fd,&sbuf);
		long tar_size=sbuf.st_size;
		char* tarsize=(char*)malloc(100);
		sprintf(tarsize,"%ld\n",tar_size);
		if(write(fd2,tarsize,strlen(tarsize))<0){
			printf("Failed to complete list operation\n");
			exit(-1);
		}

		//find the number of files in the tar file
		char* num_of_files=(char*)malloc(7);
		if(read(fd,num_of_files,6)<0){
			printf("Failed to complete list operation\n");
			exit(-1);
		}
		int n_files=atoi(num_of_files);
		sprintf(num_of_files,"%d\n",n_files);
		if(write(fd2,num_of_files,strlen(num_of_files))<0){
			printf("Failed to complete list operation\n");
			exit(-1);
		}

		//Now extract each file name and size
		char* file_name=(char*)malloc(17);
		char* file_size=(char*)malloc(9);
		for(int i=1;i<=n_files;i++){
		    long filesize;
			if(read(fd,file_name,17)<0){
			    printf("Failed to complete list operation\n");
			    exit(-1);
		    }
			if(read(fd,file_size,11)<0){
			    printf("Failed to complete list operation\n");
			    exit(-1);
		    }
			filesize=atol(file_size);
			if(lseek(fd,filesize,SEEK_CUR)<0){
			    printf("Failed to complete list operation\n");
			    exit(-1);
		    }
			sprintf(file_size," %ld\n",filesize);
			if(write(fd2,file_name,strlen(file_name))<0){
			    printf("Failed to complete list operation\n");
			    exit(-1);
		    }
			if(write(fd2,file_size,strlen(file_size))<0){
			    printf("Failed to complete list operation\n");
			    exit(-1);
		    }

		}
		close(fd);
		close(fd2);
	}

	else{
		printf("UNKNOWN ARGUMENT GIVEN. PLEASE PROVIDE VALID ARGUMENT");
		exit(-1);
	}

	return 0;
}
