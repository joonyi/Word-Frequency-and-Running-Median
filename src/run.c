//compile with "gcc run.c -o run"
//run with "./run wc_input
//1. Program will concatenate all the files, in alphabetical order, in folder wc_input into one files
//2. Program will count word frequency and create wc_result in wc_output
//3. Program will count median of each line and create med_result in wc_output

#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MAXWORDS	8888
#define MAXSTRING	200

struct st { 
    char s[MAXSTRING];
    int count;
};
void insert_word (struct st *words, int *n, char *s) {
	int	i;
	/* linear search for the word */
	for (i=0; i<*n; i++) if (strcmp (s, words[i].s) == 0) {
		/* if found increment and return. */
		words[i].count++;
		return;
	}
	/* error conditions... */
	if (strlen (s) >= MAXSTRING) {
		fprintf (stderr, "word too long!\n");
		exit (1);
	}
	if (*n >= MAXWORDS) {	
		fprintf (stderr, "too many words!\n");
		exit (1);
	}
	strcpy (words[*n].s, s); //copy the word into struct
	words[*n].count = 1; //count the word +1
	(*n)++;
}
/* return 1 if c is alphabetic (a..z or A..Z), 0 otherwise */
int is_alpha (char c) {
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') return 1;
	return 0;
}
/* remove the i'th character from the string s */
void remove_char (char *s, int i) {
	while (s[i]) {
		i++;
		s[i-1] = s[i];
	}
	s[i] = 0;
}
 /* remove non-alphabetic characters from the string s */
void remove_non_alpha (char *s) {
	int	i;
	for (i=0; s[i]; i++) if (!is_alpha (s[i])) remove_char (s, i);
}
void make_lowercase (char *s) {
	int	i;
	for (i=0; s[i]; i++) s[i] = tolower (s[i]);
}

 
/* qsort struct comparison function */ 
int struct_cmp(const void *a, const void *b) 
{ 
    struct st *ia = (struct st *)a;
    struct st *ib = (struct st *)b;
    return strcmp(ia->s, ib->s);
} 
 
/* Function of struct array printing and writing into file */ 
void print_struct_array(struct st *array, size_t len, FILE *f) 
{ 
    size_t i=0, j=0;
	for(i=0; i<len; i++) {
		if(array[i].count==0) j++;
		else break;
	}
	fseek(f, 0, SEEK_SET);  //go back to the beginning of file to starte writting
	for(i=j; i<len; i++){
        printf("%s\t\t  %d\n", array[i].s, array[i].count);//printout result
		fprintf(f, "%s\t\t %d\n", array[i].s, array[i].count);//write into wc_output.txt
	}
 
    puts("----------");
} 

double median(int n, int x[]) {
	double temp;
    int i, j;
    // sort the array x in ascending order
    for(i=0; i<n-1; i++) {
        for(j=i+1; j<n; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }
    if(n%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[n/2] + x[n/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[n/2];
    }
}

int main(int argc, char *argv[]){
	DIR *dip;
	struct dirent *dit;
	int i=0, j=0;
	struct st structs[MAXWORDS] = {};
	char	s[MAXWORDS];
	int n=0;
	int num_word; //count number of word each line
	int x[MAXWORDS];
	double median(int,int[]);
	FILE *f;
	FILE *f2;
	FILE *f3;
    char ch;
/****Files Path Name****************/
	char text_name[MAXSTRING]= "./wc_input/"; 
	char input_path[MAXSTRING] = "./wc_input/"; 
	char wc_result[MAXSTRING] ="./wc_output/wc_result.txt";
	char median_temp[MAXSTRING]="median_temp.txt";
	char med_result[MAXSTRING]="./wc_output/med_result.txt";
	char wc_temp[MAXSTRING] ="wc_temp.txt";
/*********************************************/
	 int line=0,word=0;
	if (argc < 2){	//check to see if user entered a directory name
		printf("Usage: %s <directory>\n", argv[0]);
		return 0;
	}
	if((dip=opendir(argv[1])) == NULL){
		perror("opendir");
		return 0;
	}
/******Concatenate all text files into one file****/
	printf("Directory wc_input is now open\n");
    f2 = fopen(wc_temp,"w");

	while((dit = readdir(dip))  != NULL){ //read all the text files
		if (!strcmp (dit->d_name, ".")) //skip parental file
            continue;
        if (!strcmp (dit->d_name, "..")) //skip parental file
            continue;
		j++;
	   //printf("%d\n", j); //count how many files inside
	   strcpy(text_name, input_path); //prepare correct file names
	   strcat(text_name,dit->d_name);
	   //printf("%s\n",text_name); //display files name

	   f = fopen(text_name,"r"); //open files
	   while( ( ch = fgetc(f) ) != EOF ){
		   ch=tolower(ch); 
		   if(ch=='\n'){ //below three lines will put a separator '.' for word counting later
			   fseek(f2,SEEK_CUR-2,SEEK_END);
				fputc('.',f2); 
				fputc('\n',f2);
		   }
		   else if (ch=='.'||ch==',') continue; //remove punctuation
			else fputc(ch,f2); //write to wc_temp;		  
	   }
	   fputc('\n',f2);  //adjust off

	}
	printf("Found a total of %d files \n", j);
	fclose(f);	
	fclose(f2);
/***************************************************/
/*** Resort the temp file for counting word frequency *****/
	f = fopen (wc_temp, "r"); //read from wc_temp
	if (f==NULL) perror ("Error opening file");
	f2 = fopen (wc_result, "w"); //write to wc_output

	while (!feof (f)) { 
		ch=fgetc(f) ;
		if(ch==' '||ch=='\t'||ch=='.'||ch==',')  {
			ch='\n'; //put \n after every words
		}
		fputc (ch, f2); //write into wc_output
	}
	printf("---------- \n", j);
	fclose(f);
	fclose(f2);
/********************************************/
/*** reopen sorted file to count word frequency ***/
	f = fopen (wc_result, "r+"); //read and update wc_result
	if (f==NULL) printf ("Error opening file");
	while (!feof (f)) {
		fgets(s , MAXWORDS , f) ;		//get input to string s		
		if (is_alpha (s[0])) {		
			remove_non_alpha (s);		//get rid of non-letters 		
			make_lowercase (s);	//make all letters lowercase 		
			insert_word (structs, &n, s);	//put this word in the list 
		}
	}
/* quick sort using other comparision function by alphabert*/ 
	size_t structs_len = sizeof(structs) / sizeof(struct st);
	qsort(structs, structs_len, sizeof(struct st), struct_cmp);    
/**************************************************/
/****** print and write into wc_result **************/ 
	print_struct_array(structs, structs_len,f);
	fclose(f);
/**************************************************/
/****** count the words and median*******************/
	f = fopen (wc_temp, "r"); //read from wc_temp
	f2 = fopen (med_result, "w"); //write to med_result
	if (f==NULL) printf ("Error opening file\n");
	i=0;
	j=0;
	num_word=0;
	while (!feof (f)) {
		ch=fgetc(f) ;
		if(ch==' '||ch=='\t')  {
			num_word++;
		}
		
		if(ch=='\n') {	
			num_word++;
			x[i]=num_word;
			//printf("word count: %d, i:%d\n",x[i],i);
			i++;
			num_word=0;
			printf("%.1f\n",median(i,x));
			fprintf(f2, "%.1f\n",median(i,x));
		}
		
	}
	printf("----------");
	fclose(f);
 	fclose(f2);
/**************************************************/
/****** Close directory and End ************************/ 
	if(closedir(dip) == -1){
		perror("closedir");
		return 0;
	}
	printf("\nDirectory wc_input is now close\n");

	return 1;

}
