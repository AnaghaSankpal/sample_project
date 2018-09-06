#define _CRT_SECURE_NO_WARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>



typedef struct superblock
 { 
	int TotalInodes;
	int FreeInode;
 }SUPERBLOCK,*PSUPERBLOCK;


typedef struct inode
 {
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenceCount;
	int permission;
	
 	struct inode *next;
 }INODE,*PINODE,**PPINODE;

 
typedef struct filetable
 {
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
 }FILETABLE,*PFILETABLE;


typedef struct ufdt
 {
	PFILETABLE ptrfiletable;
 }UFDT;



UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;


void man(char *name)
 {
	if(name == NULL)
	 {
		return;
	 }
 
	if(_stricmp(name,"create") == 0)
	 {
		printf("Description : Used to Create New Regular File \n");
		printf("Usage : Create File_name Permission \n");
	 }

	else if(_stricmp(name,"read") == 0)
	 {
		printf("Description : Used To Read Data From Regular File \n");
		printf("Usage : Read File_name No_Of_Bytes_To_Read \n");
	 }
	
	else if(_stricmp(name,"write") == 0)
	 {
		printf("Description : Used To Write InTo Regular File \n");
		printf("Usage : Write File_name\n After this Enter the date that we want to write\n");
	 }

	else if(_stricmp(name,"ls") == 0)
	 {
		printf("Description : Used to list all Information of Files\n");
		printf("Usage : ls\n");
	 }

	else if(_stricmp(name,"stat") == 0)
	 {
		printf("Description : Used to Display Information of File\n");
		printf("Usage : Stat File_name\n");
	 }

	else if(_stricmp(name,"fstat") == 0)
	 {
		printf("Description : Used to Display Information of Files\n ");
		printf("Usage : stat File_Descriptor\n");
	 }

	else if(_stricmp(name,"truncate") == 0)
	 {
		printf("Description : Used to remove Existing file\n");
		printf("Usage : truncate File_name\n");
 	 }
	
	else if(_stricmp(name,"open") == 0)
	 {
		printf("Description : Used to open the Existing File\n");
		printf("Usage : open File_name mode\n");
	 }

	else if(_stricmp(name,"close") == 0)
	 {
		printf("Description : Used to close the opened File\n");
		printf("Usage : close File_name\n");
	 }
	
	else if(_stricmp(name,"closeall") == 0)
	 {
		printf("Description : Used to Close All Opend Files\n");
		printf("Usage : closeall\n");
	 }

	else if(_stricmp(name,"lseek") == 0)
	 {
		printf("Description : Used to Change File Offset\n");
		printf("Usage : lseek File_name ChangeInOffset StartPoint\n");
	 }

	else if(_stricmp(name,"rm") == 0)
	 {
		printf("Description : Used to delete the File \n");
		printf("Usage :rm File_name \n");
	 }

	else
 	 {
		printf("Error : No Manual entry Available \n");
	 }
 }	



void DisplayHelp()
 {
	printf("ls : To List out all Files\n");
	printf("clear : To clear Console\n");
	printf("open : To open the File \n");
	printf("close : To Close the File\n");
	printf("closeall : To Close all Opened Files\n");
	printf("read : To read the contents of file \n");
	printf("write : To write contents int Files\n");
	printf("exit : To Terminate File System\n");
	printf("stat : To Display Information of using name\n");
	printf("fstat : To Display Information of using file Descriptor\n");
	printf("truncate : To Remove all data from file\n");
	printf("rm : To Delete the file\n");

 }


int GetFDFromName(char *name)
 {
	int i = 0;
	while(i < 50)
	 {
		if(UFDTArr[i].ptrfiletable != NULL)
		 {
			if(_stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name) == 0)
			 break;
		 }

		i++;
	 }
	if(i == 50)
	 {
	     return -1;
	 }
	else
	 {
 	     return i;
	 }
 }


PINODE Get_Inode(char * name)
 {
	PINODE temp=NULL;
	
	if(name = NULL)
	 {
	  return NULL;
	 }

	while(temp != NULL)
	 {
		if(strcmp(name,temp->FileName) == 0)
			 break;
		temp = temp->next;
	 }
	return temp;
 }





void CreateDILB()
 {
	 int i=0;
	PINODE newn=NULL;
	PINODE temp=head;

	while(i <= MAXINODE)
	 {
		newn= (PINODE)malloc(sizeof(INODE));
		newn->LinkCount = newn->ReferenceCount = 0;
		newn->FileType = newn->FileSize = 0;
		newn->Buffer = NULL;
		newn->next = NULL;
		newn->InodeNumber = i;
		
		if(temp == NULL)
		 {
			head = newn;
			temp = head;
		 }
		else
		 {
			temp->next = newn;
			temp=temp->next;
		 }	
		i++;
	 }

 }





void InitializeSuperBlock()
 {
	int i = 0;
	
	while(i <= 50)
	 {
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	 }

	SUPERBLOCKobj.TotalInodes = MAXINODE;
	SUPERBLOCKobj.FreeInode = MAXINODE;

 }





int CreateFile(char *name,int permission)
 {
	int i = 0;
	PINODE temp = head;

	if((name == NULL) || (permission == 0) || (permission > 3))
	 {
		return -1;
	 }
	
	if(SUPERBLOCKobj.FreeInode == 0)
	 {
		return -2;
	 }

	if(Get_Inode(name) != NULL)
	 {
		return -3;
	 }

	(SUPERBLOCKobj.FreeInode)--;

	while(temp != NULL)
	 {
		if(temp->FileType == 0)
		 {
			break;
		 }
		temp = temp->next;
	 }

	while(i < 50)
	 {
		if(UFDTArr[i].ptrfiletable == NULL)
		 {
			break;
		 }
	   i++;
	 }

	UFDTArr[i].ptrfiletable->count = 1;
        UFDTArr[i].ptrfiletable->mode = permission;
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;

        UFDTArr[i].ptrfiletable->ptrinode = temp;
		strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
        UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
        UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
        UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
        UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
        UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
        UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
        UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
        memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);


      return i;
 }




int rm_File(char*name)
 {
	int fd = 0;
	
	fd=GetFDFromName(name);

	if(fd == -1)
	 {
		return -1;
	 }
        (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount )--;

	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
	 {
		UFDTArr[fd].ptrfiletable->ptrinode->FileType  = 0;
		free(UFDTArr[fd].ptrfiletable);
	 }

	        UFDTArr[fd].ptrfiletable = NULL;
		(SUPERBLOCKobj.FreeInode)++;
 
 }



int ReadFile(int fd,char *arr,int isize)
 {
	int read_size = 0;
	if(UFDTArr[fd].ptrfiletable == NULL)
	 {
		return -1;
	 }
	
	if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
	 {
		return -2;
	 }

	if((UFDTArr[fd].ptrfiletable->ptrinode->permission != READ) &&
	   (UFDTArr[fd].ptrfiletable->ptrinode->permission !=READ+WRITE))
	 {
	return -2;
	 }

	if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
	 {
		return -3;
	 }

	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
	 {
		return -4;
	 }

	read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset);

	if(read_size < isize)
	 {

		strcpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),read_size);

		UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
	 }
	else
	 {
		strcpy( arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),isize);
	 
		(UFDTArr[fd].ptrfiletable->readoffset) =(UFDTArr[fd].ptrfiletable->readoffset) + isize;
	 }
	return isize;

 }





int WriteFile(int fd,char *arr,int isize)
 {
	if(((UFDTArr[fd].ptrfiletable->mode) != WRITE) &&
	   ((UFDTArr[fd].ptrfiletable->mode) !=READ+WRITE))
	 {
		return -1;
	 }

	if(((UFDTArr[fd].ptrfiletable->ptrinode->permission) !=WRITE)&&
	   ((UFDTArr[fd].ptrfiletable->ptrinode->permission) !=READ+WRITE))
	 {
		return -1;
	 }

	if((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFILESIZE)
	 {
		return -2;
	 }

	if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
	 {
		return -3;
	 }

	strcpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);

	(UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset+isize);

	(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;

	return isize;
 }






int OpenFile(char *name,int mode)
 {
	int i = 0;
	PINODE temp = NULL;

	if(name == NULL || mode <= 0)
	 {
		return -1;
	 }

	temp = Get_Inode(name);

	if(temp == NULL)
	 {
		return -2;
	 }

	while(i<50)
	 {
		if(UFDTArr[i].ptrfiletable == NULL)
		 {
			break;
		 }
	 	i++;
	 }

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable == NULL)
	 {
		return -1;
	 }

	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = mode;

	if(mode == READ+WRITE)
	 {
		UFDTArr[i].ptrfiletable->readoffset = 0;
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	 }
	else if(mode == READ)
	 {
		UFDTArr[i].ptrfiletable->readoffset = 0;
	 }
	else if(mode = WRITE)
	 {
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	 }

	UFDTArr[i].ptrfiletable->ptrinode = temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

	return i;

 }





void CloseFileByFd(int fd)
 {
	UFDTArr[fd].ptrfiletable->readoffset = 0;
	UFDTArr[fd].ptrfiletable->writeoffset = 0;
	(UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
 }




int CloseFileByName(char *name)
 {
	int i = 0;
	i = GetFDFromName(name); 

	if(i == -1)
	 {
		return -1;
	 }

	UFDTArr[i].ptrfiletable->readoffset = 0;
	UFDTArr[i].ptrfiletable->writeoffset = 0;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

	return 0;
 }





void CloseAllFile()
 {
	int i = 0;
	while(i<50)
	 {
		if(UFDTArr[i].ptrfiletable != NULL)
		 {
			UFDTArr[i].ptrfiletable->readoffset = 0;
			UFDTArr[i].ptrfiletable->writeoffset = 0;
			(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
		 }
		i++;
	 }

 }






int LseekFile(int fd,int size,int from)
 {
	if((fd<0) || (from > 0))
	 {
		return -1;
	 }
	if(UFDTArr[fd].ptrfiletable == NULL)
	 {
		return -1;
	 }

	if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
	 {
		if(from == CURRENT)
		 {
		    if(((UFDTArr[fd].ptrfiletable->readoffset) + size) >
		      (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
		     {
			return -1;
		     }
		    if(((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
		     {
			return -1;
		     }

		    (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset) +size;
		 }
		else if(from == START)
		 {
		    if(size > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
		     {
			return -1;
		     }
		    if(size < 0)
		     {
			return -1;
		     }
		
		    UFDTArr[fd].ptrfiletable->readoffset = size;	
		 }
		else if(from == END)
		 {
		     if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
		      {
			return -1;
		      }
		     if(((UFDTArr[fd].ptrfiletable->readoffset)+size) < 0)
		      {
			return -1;
		      }
	
		     (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
		 }
	
	 }
	else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
	 {
		if(from == CURRENT)
		 {
		  if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
		   {
			return -1;
		   }
		  if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
		   {
			return -1;
		   }
		  if(((UFDTArr[fd].ptrfiletable->writeoffset) + size ) >
		      (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
		   {
		    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->writeoffset) + size;
		   }
		  (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;		

		 }
		else if(from == START)
		 {
		  if(size > MAXFILESIZE)
		   {
			return -1;
		   }
		  if(size < 0)
		   {
			return -1;
		   }
		  if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
		   {
			(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
		   }

		  (UFDTArr[fd].ptrfiletable->writeoffset)=size;

		 }
		else if(from == END)
		 {
		   if(((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) > MAXFILESIZE)
		     {
			return -1;

		    }
		   if(((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
		    {
			return -1;
		    }
		   
	       	   (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;

		 }

	 }
 }





void ls_file()
 {
	PINODE temp = head;

	if(SUPERBLOCKobj.FreeInode == MAXINODE)
	 {
		printf("Error : there are no Files\n");
		return;
	 }

	printf("\nFile name\tInode Number\tFile Size\tLink Count\n");
	printf("--------------------------------------------------------\n");

	while(temp != NULL)
	 {
		if(temp->FileType != 0)
		 {
		    printf("%s\t\t%d\t\t%d\t\t%d\n",
		    temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
	  	 }
		temp = temp->next;
	 }
	printf("--------------------------------------------------------\n");

 }



int fstat_file(int fd)
 {
	PINODE temp = head;

	if(fd < 0)
	 {
	    return -1;
	 }

	if(UFDTArr[fd].ptrfiletable == NULL)
	 {
	    return -2;
	 }

	temp = UFDTArr[fd].ptrfiletable->ptrinode;

	printf("------------Statistical Information About File----------\n");
	printf("File Name : %s\n",temp->FileName);
	printf("InodeNumber : %d\n",temp->InodeNumber);
	printf("File Size : %d\n",temp->FileSize);
	printf("Actual File Size : %d\n",temp->FileActualSize);
	printf("Link Count : %d\n",temp->LinkCount);
	printf("Reference Count : %d\n",temp->ReferenceCount);

	if(temp->permission == 1)
	 {
		printf("File Permission : Read only\n");
	 }
	else if(temp->permission == 2)
	 {
		printf("File Permission : Write Only\n");
	 }
	else if(temp->permission == 3)
	 {
		printf("File Permission : Read & Write\n");
	 }
	printf("--------------------------------------------------------\n");

	return 0;
 }




int stat_file(char *name)
 {
	PINODE temp = head;

	if(name == NULL)
	 {
		return -1;
	 }

	while(temp != NULL)
	 {
	  if(strcmp(name,temp->FileName) == 0)
	   {
		break;
	   }
	temp = temp->next;
	 }

	if(temp == NULL)
	 {
	return -1;
	 }

        printf("------------Statistical Information About File----------\n");
        printf("File Name : %s\n",temp->FileName);
        printf("InodeNumber : %d\n",temp->InodeNumber);
        printf("File Size : %d\n",temp->FileSize);
        printf("Actual File Size : %d\n",temp->FileActualSize);
        printf("Link Count : %d\n",temp->LinkCount);
        printf("Reference Count : %d\n",temp->ReferenceCount);

        if(temp->permission == 1)
         {
                printf("File Permission : Read only\n");
         }
        else if(temp->permission == 2)
         {
                printf("File Permission : Write Only\n");
         }
        else if(temp->permission == 3)
         {
                printf("File Permission : Read & Write\n");
         }
        printf("--------------------------------------------------------\n");

        return 0;

 }




int truncate_File(char *name)
 {
	int fd =GetFDFromName(name);
	if(fd == -1)
	 {
		return -1;
	 }
	
	memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
	UFDTArr[fd].ptrfiletable->readoffset = 0;
	UFDTArr[fd].ptrfiletable->writeoffset = 0;
	UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;

 }

int main()
 {
	char *ptr = NULL;
	int ret = 0,fd = 0,count = 0;
	char command[4][80],str[80],arr[1024];

	InitializeSuperBlock();
	CreateDILB();

	while(1)
	 {
		flush(stdin);
		strcpy_s(str,"");

		printf("\n Marvellous VFS : >");
		fgets(str,80,stdin);

		count = sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

		if(count == 1)
		 {
		   if(_stricmp(command[0],"ls") == 0)
		    {
			ls_file();
		    }
		   else if(_stricmp(command[0],"closeall") == 0)
		    {
			CloseAllFile();
			printf("All Files Closed Successfully\n");
			continue;
	   	    }
                   else if(_stricmp(command[0],"clear") == 0)
                    {
                        system("cls");
                        continue;
                    }
                   else if(_stricmp(command[0],"help") == 0)
                    {
                        DisplayHelp();
                        continue;
                    }
                   else if(_stricmp(command[0],"exit") == 0)
                    {
                        printf("Terminating the Marvellous Virtual File System\n");
                        continue;
                    }
		   else 
		    {
			printf("\nERROR : Command Not Found !!!\n");
		    }

		 }
		else if(count == 2)
		 {
                   if(_stricmp(command[0],"stat") == 0)
                    {
                        ret = stat_file(command[1]);
			if(ret == -1)
			 {
                    		printf("ERROR : Incorrect Parameters\n");
			 }
			if(ret == -2)
			 {
				printf("ERROR : There is No Such File\n");
		 	 }
                        continue;
                    }
                   else if(_stricmp(command[0],"fstat") == 0)
                    {
                        ret = fstat_file(atoi(command[1]));
                        if(ret == -1)
                         {
                                printf("ERROR : Incorrect Parameters\n");
                         }
                        if(ret == -2)
                         {
                                printf("ERROR : There is No Such File\n");
                         }
                        continue;
                    }
                   else if(_stricmp(command[0],"close") == 0)
                    {
                        ret = CloseFileByName(command[1]);
                        if(ret == -1)
                         {
                   		printf("ERROR : There is No Such File\n");
                         }
                        continue;
                    }
                   else if(_stricmp(command[0],"rm") == 0)
                    {
                        ret = rm_File(command[1]);
                        if(ret == -1)
                         {
                                printf("ERROR : There is No Such File\n");
                         }
                        continue;
                    }
                   else if(_stricmp(command[0],"man") == 0)
                    {
                        man(command[1]);
                    }
                   else if(_stricmp(command[0],"write") == 0)
                    {
                        fd = GetFDFromName(command[1]);
                        if(ret == -1)
                         {
                                printf("ERROR : Incorrect Parameters\n");
				continue;
                         }

			printf("Enter the Data : \n");
			scanf("%[^\n]",arr);

			ret = strlen(arr);
			if(ret == 0)
			 {
				printf("ERROR : Incorrect Parameters\n");
				continue;
			 }
			ret = WriteFile(fd,arr,ret);
			if(ret == -1)
			 {
				printf("ERROR : Permission Denied \n");
			 }
                        if(ret == -2)
                         {
			 	printf("ERROR : There Is No Sufficient memory to Write \n");
			 }
			if(ret == -3)
			 {
                                printf("ERROR : There is No Such File\n");
                         }
                        
                    }
                    else if(_stricmp(command[0],"truncate") == 0)
                    {
                        ret = truncate_File(command[1]);
                        if(ret == -1)
                         {
                                printf("ERROR : Incorrect Parameters\n");
                         }
                        else
                         {
                                printf("\nERROR : Command Not Found !!!\n");
  	                        continue;
			 }
                    }

		 }
		else if(count == 3)
		 {
		   if(_stricmp(command[0],"create") == 0)
		    {
			ret = CreateFile(command[1],atoi(command[2]));
			if(ret >= 0)
			 {
				printf("File is Successfully Created With File Descriptor : %d\n",ret);
			 }
			if(ret == -1)
			 {
				printf("ERROR : Incorrect Parameters\n");
			 }
                        if(ret == -2)
                         {
                                printf("ERROR : There is No Inodes\n");
                         }
                        if(ret == -3)
                         {
                                printf("ERROR : File AllReady Exists\n");
                         }
                        if(ret == -4)
                         {
                                printf("ERROR : Memry Allocation Failure\n");
                         }
			continue;

		    }
		   else if(_stricmp(command[0],"open") == 0)
		    {
			ret = OpenFile(command[1],atoi(command[2]));
                        if(ret >= 0)
                         {
                                printf("File is Successfully opened with File Descriptor : %d\n",ret);
                         }
                        if(ret == -1)
                         {
                                printf("ERROR : Incorrect Parameters\n");
                         }
                        if(ret == -2)
                         {
                                printf("ERROR : File Not Present \n");
                         }
                        if(ret == -3)
                         {
                                printf("ERROR : Permission Denied\n");
                         }
			continue;

		    }

		   else if(_stricmp(command[0],"read") == 0)
		    {
			fd = GetFDFromName(command[1]);
			if(fd == -1)
			 {
				printf("ERROR : Incorrect Parameters\n");
				continue;
			 }
			ptr = (char *)malloc(sizeof(atoi(command[2]))+1);
			if(ptr == NULL)
			 {
				printf("ERROR : Memory Allocation Failure \n");
				continue;
			 }
			ret = ReadFile(fd,ptr,atoi(command[2]));
                        if(ret == -1)
                         {
                                printf("ERROR : Fie Not Existint \n");
                         }
                        if(ret == -2)
                         {
                                printf("ERROR : Permission Denied \n");
                         }
                        if(ret == -3)
                         {
                                printf("ERROR : Reached at End Of File\n");
                         }
                        if(ret == -4)
                         {
                                printf("ERROR : It is Not Regular File\n");
                         }
                        if(ret == -0)
                         {
                                printf("ERROR : File Empty \n");
                         }
                        if(ret > 0)
                         {
                                _write(2,ptr,ret);
                         }
			continue;
			
		    }
		   else
		    {
			printf("ERROR : Command Not Found !!!\n");
			continue;
		    }


		 }
		else if(count == 4)
		 {
		   if(_stricmp(command[0],"lseek")==0)
		    {
			fd = GetFDFromName(command[1]);
                        if(fd == -1)
                         {
                                printf("ERROR : Incorrect Parameters\n");
				continue;
                         }
			ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
                        if(ret == -1)
                         {
                                printf("ERROR : Unable to Perform lseek\n");
                         }

		    }
		   else
		    {
			printf("\nERROR : Command Not Found !!!\n");
			continue;
		    }


		 }
		else 
		 {
			printf("\nERROR : Command Not Found !!!\n");
			continue;
		 }


	 }

	return 0;

 }
