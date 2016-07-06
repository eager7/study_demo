/*************************************************************************
	> File Name: strcpy.c
	> Author: 
	> Mail: 
	> Created Time: 2016年03月02日 星期三 14时00分49秒
 ************************************************************************/

#include<stdio.h>
 
 int main(int argc, char *argv[]) 
 { 
     int flag = 0; 
     char passwd[10]; 
  
     memset(passwd,0,sizeof(passwd)); 
  
     strcpy(passwd, argv[1]); 
  
     if(0 == strcmp("LinuxGeek", passwd)) 
     { 
             flag = 1; 
    } 
  
     if(flag) 
     { 
             printf("\n Password cracked \n"); 
         } 
     else 
     { 
             printf("\n Incorrect passwd \n"); 
      
         } 
     return 0; 
 }
