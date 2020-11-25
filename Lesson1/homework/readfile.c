#include<stdio.h>
#include<string.h>

int checkWord(char word[][10], char* data, int size){
	int i=0; 
	while(i<size){
		if(strcmp(word[i], data) ==0){
			return 1;
		}
		i++;
	}
	return 0;
}
int main(){
	char c[1000][20];
	int wordCheck[1000];
	char stop[20][10];
	char a[20];
	char str[20];
	int lineNum=0;
	FILE *fptr;   
    fptr = fopen("stopw.txt","r");
    if(fptr == NULL)
   	{
       printf("Error!");             
    } 
    int d=0;
    char ch1;
	while((ch1=getc(fptr))!=EOF){
		
    	if(64< ch1 && ch1<91){
    		strncat(stop[d], &ch1, 1); 
		}
		else if(96< ch1 && ch1<123){
    		strncat(stop[d], &ch1, 1); 
		}
		else if(ch1=='\n'){
			if( stop[d][0]){
				d++;
			}
		}
    	//printf("%c", ch);	
	}
	//printf("%d--%s", d,stop[0]);
	fptr = fopen("vanban.txt","r");
	if(fptr == NULL)
   	{
       printf("Error!");             
    } 
    int line =0;
	char ch;
	char temp=' ';
	while((ch=getc(fptr))!=EOF){
	
    	if(64< ch && ch<91){
    		char character=ch+32;
    		strncat(c[line], &character, 1); 
			printf("%c--", temp);
			if(temp!=46 && temp!=' '){
				wordCheck[line]=1;
				printf("%d======%d", line, wordCheck[line]);
			}
    		printf("%c\n", ch);
		}
		else if(96< ch && ch<123){
    		strncat(c[line], &ch, 1); 
    		temp=ch;
		}
		else if( ch==' '){
			if( c[line][0] ){
				line++;
			}
		}
		else if(ch=='-'){
			strncat(c[line], &ch, 1); 
		}
		else if(ch=='\n' ){
			lineNum++;
			
			if( c[line][0]){
				line++;
			}
		}
		else if(ch==46){
			temp=ch;
		}
	}
	int i=0;
		while(i<line){
			if(!checkWord(stop,c[i], d-1)){
				if(wordCheck[i]==0){
					printf("%s__", c[i]);
				}
			}
		 i++;
	}

    
	fclose(fptr);
	return 0;
}
