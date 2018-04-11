//Maxcharm decides to write some comments after she realized she just could't understand her previous codes.
//2018.04.04
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

int status;
int fnt = 0;
const char PATH[] = "./Functions";
// char storage_for_functions[4096];

void make_directory(){ //prepare a directory to store all the functions.
	status = mkdir(PATH, 0777);
	if(status == -1){
	    perror("errors in mkdir.\n");
	    exit(0);
	}
}

void delete_directory(){ //when running crepl multiple times it's essential to delete the original directory
   /* status = rmdir(PATH);
    if(status == -1){
      perror("errors in rmdir.\n");
      exit(0);
    }*/
   char cmd[] = "rm -rf ./Functions";
   if(system(cmd) != 0){
      perror("delete files error.\n");
   }
//   else printf("file deleted!\n");
}



void *process_lib(char *body, char *name, int is_func){
  char cmd[512];
 // make_directory();
  if(is_func){
	  char filename[40];
	  sprintf(filename, "%s/f%d.c",PATH, fnt);
	  char libpath[40];
	  sprintf(libpath, "%s/f%d.so",PATH, fnt);
     FILE *file_pointer = fopen(filename, "w+");
     if(file_pointer == NULL){
        perror("creating file4func error");
	exit(0);
     }
//     fseek(file_pointer, 0L, SEEK_END);
//     int length = ftell(file_pointer);
//     fread(storage_for_functions, 1, length + 1, file_pointer);
//     storage_for_functions[length + 1] = '\0';
     fwrite(body, strlen(body), 1, file_pointer);
//     fwrite("\n", strlen("\n"), 1, file_pointer);
     fclose(file_pointer);
     sprintf(cmd, "gcc -shared -fPIC -Wno-implicit-function-declaration -o %s %s", libpath, filename);
     if(system(cmd) != 0){   //linking failure
         printf("Please type in the right function.\n");
//	 file_pointer = fopen(file4functions, "w+");
//	 fwrite(storage_for_functions, strlen(storage_for_functions), 1, file_pointer);
//	 fclose(file_pointer);
	 return NULL;
     }
     else{
         void *handle = dlopen(libpath, RTLD_LAZY | RTLD_GLOBAL);
         return handle;
     }
  }
  else{
     char expr_path[256];
     sprintf(expr_path, "%s/%s.c", PATH, name);
//     printf("%s\n", expr_path);
     FILE *file_pointer = fopen(expr_path, "w+");
     if(file_pointer == NULL){
        perror("creating exprfunc error.\n");
	exit(0);
     }
     fwrite(body, strlen(body), 1, file_pointer);
     fclose(file_pointer);
     char lib_expr[256];
     sprintf(lib_expr, "%s/%s.so", PATH, name);
     sprintf(cmd,  "gcc -shared -fPIC -Wno-implicit-function-declaration -o %s %s", lib_expr, expr_path);
     if(system(cmd) != 0){
        printf("Please type in a valid expression.\n");
	return NULL;
     }
     else{
        void *handle = dlopen(lib_expr, RTLD_LAZY | RTLD_GLOBAL);
        return handle;
     }
  }
}

void process_func(char *content){
      char *file4func = "functions.c";
      process_lib(content, file4func, 1);
}
void process_expr(char *content, int number){
     char func_name[30]; 
     sprintf(func_name, "_expr_wrap_%03d", number); // wrap the expression up as a function too
     char body[256];
//     sprintf(expr, "expr_no_%03d.c", number);
     sprintf(body, "int %s() {\n return (%s);\n}", func_name, content);
//     printf("%s\n", body);
     void *handle = process_lib(body, func_name, 0);
     if(handle == NULL){
     }
     else{
        int (*func)() = dlsym(handle, func_name);
	int value = func();           //sample code
	printf(">> %s = %d.\n", content, value);
     }
     return;
}

int main() {
   //printf(">>> ");
   make_directory();
   char buf[1024] = {0};
   int count = 0;   //count the expressions typed in to name them.
   while(fgets(buf, 1024, stdin) != NULL){
      for(int i = 0; i < 1024; i++){
          if(buf[i] == '\n'){
	     buf[i] = '\0';
	     break;
	  }
      }
    //  printf("%s\n", buf);
      if(!strcmp(buf, "exit")){
	 printf("Thank you for using crepl.\n");
	 delete_directory(); 
	 exit(0);
      }
      if(!strncmp("int", buf, 3)){ //if the user typed in a function
        process_func(buf);
	fnt++;
//	printf(">>> ");
      }
      else{ // the user typed in a expression
           process_expr(buf, count);
           count++;
//	   printf(">>> ");
      }
   }
   delete_directory();
   return 0;
}
