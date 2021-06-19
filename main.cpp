#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <fstream>
#include <cstring>

using namespace std;

//defining size for array
#define MAXSIZE 100
#define MAXARR 20


/*This program will create two childrens, which will recreate file from each other directory by passing the file name and content through a pipe */


//struct to hold file name and content from directory of both child
struct directory
{
  char filename[MAXARR][MAXSIZE];

} dir, dir2;




int main()
{
  //This part fill open the first directory and read all its content into a struct
  DIR * myDirectory;
  struct dirent * myFile;
  myDirectory = opendir("/home/hoy/assignment1/child/");
  
  
  //variables
  int countdir1 = 0, countdir2 = 0;
  
  //array to hold filename and content
  char path[MAXARR][MAXSIZE];
  char filecontentarray[MAXARR][MAXSIZE];
  char filenamearray[MAXARR][MAXSIZE];
  
  
  
  //if the directory opened
  if(myDirectory)
  {
      puts("child1 directory opened");

      while ((myFile = readdir(myDirectory)))
      {
          //display the list of files and storing it into struct
          strcpy(dir.filename[countdir1++], myFile->d_name);
      }


      //closing directory after usage
      if (closedir(myDirectory) == 0)
      {
          puts("child1 directory closed.");
      }
      else
      {
          puts("child1 directory can not be closed.\n\n");
      }
  }
  else //if the directory doesn't open
  {
    perror("Cound't open child1 directory");
    return EXIT_FAILURE;
  }
  


  printf("\n\n");
  
  

  //for loop to open the file and read its content into array
  for(int f = 0; f < countdir1-2; f++)
  {
    //string manipulation to store filepath into array
    strcpy(path[f], "/home/hoy/assignment1/child/");
    char * filename = strcat(path[f], dir.filename[f]);
    //strcpy(filenamearray[f], filename);
    strcpy(filenamearray[f], dir.filename[f]);

    //opening the file
    ifstream file(filename);
    string str;
    string file_contents;
    

    //reading its content and pushing it into a string
    while(getline(file, str))
    {
      file_contents += str;
      file_contents.push_back('\n');
    }  

    //converting the string into 2d char array
    for(int r = 0; r < file_contents.length(); r++)
    {
        filecontentarray[f][r] = file_contents[r];
    }

    
  }



  //-----------------------------------------------------------------


  //This part fill open the 2nd directory and read all its content into a struct
  DIR * myDirectory2;
  struct dirent * myFile2;
  myDirectory2 = opendir("/home/hoy/assignment1/child2/");
  
  
  //array to hold filename and content
  char path2[MAXARR][MAXSIZE];
  char filecontentarray2[MAXARR][MAXSIZE];
  char filenamearray2[MAXARR][MAXSIZE];
  
  
  //if the directory opened
  if(myDirectory2)
  {
      puts("child2 directory opened");

      while ((myFile2 = readdir(myDirectory2)))
      {
          //display the list of files and storing it into struct
          strcpy(dir2.filename[countdir2++], myFile2->d_name);
      }


      //closing directory after usage
      if (closedir(myDirectory2) == 0)
      {
          puts("child2 directory closed.");
      }
      else
      {
          puts("child2 directory can not be closed.\n\n");
      }
  }
  else //if the directory doesn't open
  {
    perror("Cound't open child2 directory");
    return EXIT_FAILURE;
  }
  
  

  printf("\n\n");
  


  //for loop to open the file and read its content into array
  for(int f = 0; f < countdir2-2; f++)
  {
    //string manipulation to store file name into array
    strcpy(path2[f], "/home/hoy/assignment1/child2/");
    char * filename = strcat(path2[f], dir2.filename[f]);
    //strcpy(filenamearray2[f], filename);
    strcpy(filenamearray2[f], dir2.filename[f]);
    

    //opening the file
    ifstream file(filename);
    string str;
    string file_contents;
    

    //reading its content and pushing it into a string
    while(getline(file, str))
    {
      file_contents += str;
      file_contents.push_back('\n');
    }  

    //converting the string into 2d char array
    for(int r = 0; r < file_contents.length(); r++)
    {
        filecontentarray2[f][r] = file_contents[r];
    }
    

    
  }


  printf("\n\n");
  
  

  //array to hold name/content when piping
  char pipename[MAXARR][MAXSIZE];
  char pipecontent[MAXARR][MAXSIZE];
  char pipename2[MAXARR][MAXSIZE];
  char pipecontent2[MAXARR][MAXSIZE];


  //variables and fork
  pid_t child1, child2;
  int p[2], p2[2], p3[2], p4[2];


  //creating the pipe
  pipe(p);
  pipe(p2);
  pipe(p3);
  pipe(p4);




  if(!(child1 = fork())) //child1 process
  {
    //array
    char newfilename2[MAXARR][MAXSIZE];

  
    //piping filename to child2
    for(int h = 0; h < countdir1-2; h++)
    {
      close(p[0]);
      close(p2[0]);

      write(p[1], filenamearray[h], sizeof(filenamearray[h]));
      write(p2[1], filecontentarray[h], sizeof(filecontentarray[h]));
    
    }



    //reading from child2
    for(int h = 0; h < countdir2-2; h++)
    {
      //reading from pipe
      close(p3[1]);
      close(p4[1]);


      read(p3[0], pipename2[h], sizeof(pipename2[h]));
      read(p4[0], pipecontent2[h], sizeof(pipecontent2[h]));


      //string manipulating to create and store the file into new child path
      strcpy(path2[h], "/home/hoy/assignment1/child/");
      char * filename2 = strcat(path2[h], pipename2[h]);
      strcpy(newfilename2[h], filename2);


      //writing out the content into the new directory
      ofstream file(newfilename2[h]);
      file << pipecontent2[h];
      file.close();
    }





  }
  else if(!(child2 = fork())) //child2 process
  {
    //array
    char newfilename[MAXARR][MAXSIZE];
    char newpath[MAXARR][MAXSIZE];
    

    
    //reading file name and content from child1
    for(int h = 0; h < countdir1-2; h++)
    {
      //reading from pipe
      close(p[1]);
      close(p2[1]);

      read(p[0], pipename[h], sizeof(pipename[h]));
      read(p2[0], pipecontent[h], sizeof(pipecontent[h]));

      //string manipulating to create and store the file into new child path
      strcpy(newpath[h], "/home/hoy/assignment1/child2/");
      char * filename = strcat(newpath[h], pipename[h]);
      strcpy(newfilename[h], filename);


      //writing out the content into the new directory
      ofstream file2(newfilename[h]);
      file2 << pipecontent[h];
      file2.close();

    

    }
    

    //piping file name and content to child1
    for(int h = 0; h < countdir2-2; h++)
    {
      close(p3[0]);
      close(p4[0]);

      write(p3[1], filenamearray2[h], sizeof(filenamearray2[h]));
      write(p4[1], filecontentarray2[h], sizeof(filecontentarray2[h]));
    }
    

  }
  else//parent process
  {
    //waiting the child processes to finish
    wait(NULL);

    printf("done\n");
  }

}

