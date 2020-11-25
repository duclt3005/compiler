#include<stdio.h>
#include<string.h>
typedef struct Word{
	char word[20];
	int count;
	int line[100];
}Word;

typedef struct Node  
{ 
  Word data; 
  struct Node *left; 
  struct Node *right; 
} Node;
FILE *fw;   

int sizeArray(int arr[]){
	int count =0;
	int i=0;
	while(i<100){
		if(arr[i]!=0){
			i++;
		}
		else break;
	}
	return i;
}
int check(int line[], int data){
	int i=0;
	int size = sizeArray(line);
	while(i<size){
		if(data==line[i]) return 1;
		i++;
	}
	return 0;
}
Node* getTheNode(char* data, int lineCur){
	Node* newNode = (Node*)malloc(sizeof(Node));
	strcpy(newNode->data.word, data);
	newNode->data.count=1;
	newNode->data.line[0]=lineCur;
	int i=1;
	while(i<100){
		newNode->data.line[i]=0;
		i++;
	}
	newNode->left= NULL;
	newNode->right= NULL;
	return newNode;
}
Node* Insert(Node* root, char* data, int lineCur){
	if(root==NULL){
		root= getTheNode(data, lineCur);
		return root;
	}
	if(strcmp(root->data.word, data)==0){
		root->data.count++;		
		int h= sizeArray(root->data.line);
		if(!check(root->data.line, lineCur)){
			root->data.line[h]=lineCur;
		}
	}
	else if(strcmp(data, root->data.word) <0) {
		root->left = Insert(root->left,data, lineCur);
	}
	// else, insert in right subtree. 
	else {
		root->right = Insert(root->right,data, lineCur);
	}
	return root;
}
int Search(Node* root, char* data) {
	if(root == NULL) {
		return 0;
	}
	else if(strcmp(root->data.word, data)==0) {
		return 1;
	}
	else if(strcmp(root->data.word, data)<0) {
		return Search(root->left,data);
	}
	else {
		return Search(root->right,data);
	}
}

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
void Preoder(Node* root){
	if(root== NULL) return;
	printf("%s %d \n", root->data.word, root->data.count);
	Preoder(root->left);
	Preoder(root->right);
}
void Inorder(Node* root){
	if(root== NULL) return;
	Inorder(root->left);

	fprintf(fw,"%s ..... %d - ", root->data.word, root->data.count);
	printf("%s ..... %d - ",root->data.word, root->data.count);
	int n= sizeArray(root->data.line);
	int i=0; 
	while(i<n){
		fprintf(fw,"%d ",root->data.line[i] );
		printf("%d ",root->data.line[i] );
		i++;
//		int j=0;
//		while(j<100){
//			if(j==root->data.line[i]){
//				fprintf(fw," (%d, %d) ",x[j], y[j]);
//				printf(" (%d, %d) ",x[j], y[j]);
//			}
//			j++;
//		}
	}
	fprintf(fw,"\n");
	printf("\n");

	Inorder(root->right);
}
void Postorder(Node* root){
	if(root== NULL) return;
	Postorder(root->left);
	Postorder(root->right);
	printf("%s %d \n", root->data.word, root->data.count);
}
int main(){
	fw = fopen("index.txt","w");
	Node* root = NULL;
	int index[1000][1000];
	char c[1000][20];		// luu word doc ra tu file vanban.txt
	int wordCheck[1000];	// kiem tra tinh hop le cua word trc khi cho vao index
	int posCheck[1000];		// line xuat hien word
	char stop[20][10];		// luu word trong file stopw.txt
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
	}
	fptr = fopen("vanban.txt","r");
	if(fptr == NULL)
   	{
       printf("Error!");             
    } 
    int line =0;
	char ch;
	char temp2='a';
	char temp=' '; // ki tu trc ki tu hien tai
	int col=1;
	while((ch=getc(fptr))!=EOF){
		//printf("==%d=%c=%c=%c=%d-\n",ch, ch, temp, temp2,line );
    	if(64< ch && ch<91){
    		char character=ch+32;
    		strncat(c[line], &character, 1); 
			if((temp2==' ')&&(temp!=46)){
				wordCheck[line]=1;
			}
		}
		else if(96< ch && ch<123){
    		strncat(c[line], &ch, 1); 
    		temp=ch;
		}
		else if(47< ch && ch<58){
			if(temp2==46){
					strncat(c[line], &temp2, 1); 
				}
    		strncat(c[line], &ch, 1); 
    		temp=ch;
		}
		else if(ch==39){
			strncat(c[line], &ch, 1); 
    		temp=ch;
		}
		else if( ch==' '){
			if( c[line][0] ){
				posCheck[line]=lineNum+1;
				line++;
			}
			col++;
		}
		else if(ch=='-'){
			strncat(c[line], &ch, 1); 
		}
		else if(ch=='\n' ){
			if( c[line][0]){
				posCheck[line]=lineNum+1;
				line++;
			}
			lineNum++;
			col=1;
		}
		
		else if(ch==46){
			temp=ch;
		}
		//printf("%d ", line);
		temp2= ch;
	}
	line--;
	//printf("--%d-%c\n", ch);
	posCheck[line]=lineNum+1;	
	
	int i=0;
		while(i<=line){
			if(!checkWord(stop,c[i], d-1)){
				if(wordCheck[i]==0){
					//printf("%s\n", c[i]);
					if(c[i][0] >57 || c[i][0]<48){
					root = Insert(root,c[i], posCheck[i]);
					x[count]=i;
					y[count]=posCheck[i];
					count++;
					}	
				}
			}
		 i++;
	}
//	int j=0;
//	while(j<count){
//		printf("%d--%d\n", x[j], y[j]);
//		j++;	
//	}
	Inorder(root);
    
	fclose(fptr);
	return 0;
}
