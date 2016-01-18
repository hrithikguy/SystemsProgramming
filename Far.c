#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>

void chararrayprinter(char ** array, int length) {
	int i;
	for (i = 0; i < length; ++i) {
		printf("%d: %s\n", i, array[i]);
	}
}

void intarrayprinter(int * array, int length) {
	int i;
	for (i = 0; i < length; ++i) {
		printf("%d: %d\n", i, array[i]);
	}
}

int filesizetoint(char *filesize) {
	int i = 0;
    while (isdigit(filesize[i]) == 1) {
            ++i;
    }
    int filesizer = 0;
    int j;
    for (j = 0; j <= i-1; ++j) {
            filesizer += pow(10, i-1-j) * (filesize[j] - '0');
    }
    return filesizer;
}

int main(int argc, char *argv[] )
{
	if (strcmp(argv[1], "r") == 0) {
		int i, j;
		char **names;
		names = malloc(sizeof(char*)*(argc - 3));
		//chararrayprinter(names, 1);

		for (i = 0; i < argc - 3; ++i) {
			names[i] = malloc(sizeof(strlen(argv[i+3]))+1);
			strcpy(names[i], argv[i+3]);
		}

		int numelementsnames = argc - 3;
		int sizeofnames = argc-3;
		//names = realloc(names, sizeof(char*)*sizeofnames);

		char **names2;
		names2 = malloc(sizeof(char*)*sizeofnames);
		for (i = 0; i < argc-3; ++i) {
			names2[i] = (char *) malloc(strlen(".") + 1);
			strcpy(names2[i], ".");
		}

		//chararrayprinter(names, sizeofnames);
		//chararrayprinter(names2, sizeofnames);

		struct dirent *entry;
		DIR *dp;
		char permacwd[100];
		getcwd(permacwd,100);
		char cwd[100];
		char currentreldir[1000]= "";
		struct stat lstatbuf;

		for (i = 0; i < numelementsnames; ++i) {
			if (names[i] == NULL) {
				break;
			}
			getcwd(cwd, 100);
			chdir(names2[i]);
			lstat(names[i], &lstatbuf);
			getcwd(cwd, 100);

			if (S_ISDIR(lstatbuf.st_mode)) {
				dp = opendir(names[i]);
				while((entry = readdir(dp)) != NULL) {
					//printf("current entry is %s\n", entry->d_name);
					if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
                                		continue;
                    }
					
					strcpy(currentreldir, names2[i]);
					strcat(currentreldir, "/");
					strcat(currentreldir, names[i]);

					if(numelementsnames == sizeofnames) {
						sizeofnames *= 3;
						names = realloc(names, sizeof(char*)*sizeofnames);
						names2 = realloc(names2, sizeof(char*) *sizeofnames);
					}


					//chararrayprinter(names, sizeofnames);
					//chararrayprinter(names2, sizeofnames);

					names[numelementsnames] = malloc(strlen(entry->d_name) + 1);
					names2[numelementsnames] = malloc(strlen(currentreldir) + 1);
					strcpy(names[numelementsnames], entry->d_name);
					strcpy(names2[numelementsnames], currentreldir);
					++numelementsnames;
					
					
					//printf("printingboth\n");
					//chararrayprinter(names, sizeofnames);
					//chararrayprinter(names2, sizeofnames);			
					//TODO: figure out when to closedir(dp)
					//closedir(dp);
				}
				
			}
			chdir(permacwd);
		}
		


		for (i = 0; i < numelementsnames; ++i) {
			strcat(names2[i], "/");
			strcat(names2[i], names[i]);
		}
		//chararrayprinter(names2, sizeofnames);


		FILE *fptr;
		FILE *fptr2;
		FILE *currentfile;
		int matched;
		int filesizer;
		char c;
		int names2markers[numelementsnames];
        for (i = 0; i < numelementsnames; ++i) {
            names2markers[i] = 0;
        }

        //intarrayprinter(names2markers, numelementsnames);

        if((fptr = fopen(argv[2], "r")) == NULL) {
			fptr = fopen(argv[2], "w");
			for (i = 0; i < numelementsnames; ++i) {
				lstat(names2[i], &lstatbuf);
				if (S_ISREG(lstatbuf.st_mode) || S_ISDIR(lstatbuf.st_mode)) {
					fprintf(fptr, names2[i]);
					if (S_ISREG(lstatbuf.st_mode)) {
						fprintf(fptr, "\nf\n");
						fprintf(fptr, "%lld\n", lstatbuf.st_size);
						currentfile = fopen(names2[i], "r");
						while ((c = fgetc(currentfile)) != EOF) {
							fputc(c, fptr);
						}
					}
					else if (S_ISDIR(lstatbuf.st_mode)) {
						fprintf(fptr, "\nd\n0\n");
					}
				}
			}
		}

		else {
			fptr2 = fopen("TEMPARKIV", "w");
			int EOFreached = 0;
			char *filename;
			filename = malloc(sizeof(char)*100);
			char filetype[10];
			char filesize[10];
			char *filenamehelper;
			filenamehelper = malloc(sizeof(char)*100);
			while (EOFreached == 0) {
				c = fgetc(fptr);
				if (c == EOF) {
					break;
				}
				fgets(filenamehelper, 100, fptr);
				filename = realloc(filename, strlen(filenamehelper) + 1);
				filename[0] = c;
				for (i = 0; i < strlen(filenamehelper) + 1; ++i){
					filename[i+1] = filenamehelper[i];
				}
				//printf("filenamehelper is %s and filename is %s", filenamehelper, filename);

				fgets(filetype, 10, fptr);
				fgets(filesize, 10, fptr);
				
				filename[strlen(filename) - 1] = '\0';


				for (i = 0; i < numelementsnames; ++i) {
					matched = 0;
					if (strcmp(filename, names2[i]) == 0) {
						matched = 1;
						//printf("matched %s", filename);
						names2markers[i] = 1;
					
						lstat(names2[i], &lstatbuf);
              	 	        if (S_ISREG(lstatbuf.st_mode) || S_ISDIR(lstatbuf.st_mode)) {
               		                 fprintf(fptr2, names2[i]);
               		                 //fprintf(fptr2, "TYPE1");
                       		         if (S_ISREG(lstatbuf.st_mode)) {
                      		                 fprintf(fptr2, "\nf\n");
                               		         fprintf(fptr2, "%lld\n", lstatbuf.st_size);
                                    	         currentfile = fopen(names2[i], "r");
                                	         while ((c = fgetc(currentfile)) != EOF) {
                                       		         fputc(c, fptr2);
                              		       	  }
                              		  }
                                   	  else if (S_ISDIR(lstatbuf.st_mode)) {
                                	        fprintf(fptr2, "\nd\n0\n");
                             	 	  }
                                }

						
                        filesizer = filesizetoint(filesize);
                        
                        for (i = 0; i < filesizer; ++i) {
                                c = fgetc(fptr);
                        }

						break;
					}
				}
				if (matched == 0) {
					strcat(filename, "\n");
					fprintf(fptr2, filename);
					//fprintf(fptr2, "TYPE2");
					fprintf(fptr2, filetype);
					fprintf(fptr2, filesize);
				

					filesizer = filesizetoint(filesize);
					
					for (i = 0; i < filesizer; ++i) {
						c = fgetc(fptr);
						
						fputc(c, fptr2);
						
					}
				}
			}

			for (i = 0; i < numelementsnames; ++i) {
				if (names2markers[i] == 0) {
					lstat(names2[i], &lstatbuf);
					if(S_ISREG(lstatbuf.st_mode) || S_ISDIR(lstatbuf.st_mode)) {
						fprintf(fptr2, names2[i]);
						//fprintf(fptr2, "TYPE3");
						if (S_ISREG(lstatbuf.st_mode)) {
							fprintf(fptr2, "\nf\n");
							fprintf(fptr2, "%lld\n", lstatbuf.st_size);
							currentfile = fopen(names2[i], "r");
							while ((c = fgetc(currentfile)) != EOF) {
								fputc(c, fptr2);
							}
						}
						else if (S_ISDIR(lstatbuf.st_mode)) {
							fprintf(fptr2, "\nd\n0\n");
						}
					}
				}
			}
			unlink(argv[2]);
			rename("TEMPARKIV", argv[2]);
		}
	}
	else if (strcmp(argv[1], "x") == 0) {
		int i, j;
		char **names;
		names = malloc(sizeof(char*)*(argc - 3));

		for (i = 0; i < argc - 3; ++i) {
			names[i] = malloc(sizeof(strlen(argv[i+3]))+1);
			strcpy(names[i], argv[i+3]);
		}
		int nameslength = argc - 3;

		FILE *fptr;
		fptr = fopen(argv[2], "r");
		char *filename;
		filename = malloc(sizeof(char)*100);
		char filetype[10];
		char filesize[10];
		char *filenamehelper;
		filenamehelper = malloc(sizeof(char)*100);
		int EOFreached = 0;
		int c;
		int filesizer;
		int matched;

		struct stat lstatbuf;
		DIR *dp;
		struct dirent *entry;


		while (EOFreached == 0) {
			c = fgetc(fptr);
			if (c == EOF) {
				break;
			}
			fgets(filenamehelper, 100, fptr);
			filename = realloc(filename, strlen(filenamehelper) + 1);
			filename[0] = c;
			for (i = 0; i < strlen(filenamehelper) + 1; ++i) {
				filename[i+1] = filenamehelper[i];
			}
			fgets(filetype, 10, fptr);
			fgets(filesize, 10, fptr);
			//printf("filename is %s", filename);

			char **filenamebreakdown;
			filenamebreakdown = malloc(sizeof(char*));
			filename[strlen(filename) - 1] = '\0';
			filenamebreakdown[0] = malloc(strlen(filename) + 1);
			strcpy(filenamebreakdown[0], filename);
			

			int count = 0;
			//char *temp;
			//temp = malloc(1);
			for (i = strlen(filenamebreakdown[0]); i >= 0; --i) {
				//printf("step %d\n", i);
				if (filenamebreakdown[0][i] == '/') {
					//printf("found / at position %d\n", i);
					count++;
					//printf("count is now %d\n", count);
					filenamebreakdown = realloc(filenamebreakdown, sizeof(char*) * (count + 1));
					//printf("the new filebreakdown is\n");
					//chararrayprinter(filenamebreakdown, count+1);
					//printf("size is now %lu\n", sizeof(filenamebreakdown));
					filenamebreakdown[count] = malloc(sizeof(char) * (i-1));
					//temp = realloc(temp, i);
					for(j = 0; j < i; ++j) {
						filenamebreakdown[count][j] = filenamebreakdown[0][j];
					}
					filenamebreakdown[count][i] = '\0';
					
				}
			}

			count++;
			matched = 0;
			for (i = 0; i < count; ++i) {
				for (j = 0; j < argc - 3; ++j) {
					if (strcmp(filenamebreakdown[i], names[j]) == 0) {
						//printf("match found with %s and %s\n", filenamebreakdown[0], names[j]);
						matched = 1;
					}
				}
			}

			//if it doesn't match, keep going through the file
			if (matched == 0) {
				filesizer = filesizetoint(filesize);
				
				for (i = 0; i < filesizer; ++i) {
					c = fgetc(fptr);
					if (c == EOF) {
						EOFreached = 1;
					}
					
				}
			}
			
			else if (matched == 1) {
				int found;
				system("pwd");
				if (filetype[0] == 'd') {
					dp = opendir(".");
					for (i = count - 1; i >= 0; --i) {
						found = 0;
						printf("i is at %d\n", i);
						printf("we are looking for %s\n", filenamebreakdown[i]);
						while((entry = readdir(dp)) != NULL){
							printf("checking %s\n", entry->d_name);
							if(strcmp(entry->d_name, filenamebreakdown[i]) == 0) {
								found = 1;
								printf("directory %s found!\n", entry->d_name);
								break;
							}
						}
						if (found == 0) {
							mkdir(filenamebreakdown[i], "r+");
						}
					}
						printf("directory!\n");
				}
				else if (filetype[0] == 'f') {
					printf("file!\n");
				}




				for (i = 0; i < filesizer; ++i) {
					c = fgetc(fptr);
					if (c == EOF) { 
						EOFreached = 1;
					}

				}
			}
		}
	}
	else if (strcmp(argv[1], "d") == 0) {
		int i, j;
		char **names;
		names = malloc(sizeof(char*)*(argc - 3));

		for (i = 0; i < argc - 3; ++i) {
			names[i] = malloc(sizeof(strlen(argv[i+3]))+1);
			strcpy(names[i], argv[i+3]);
		}
		int nameslength = argc - 3;

		FILE *fptr;
		fptr = fopen(argv[2], "r");
		char *filename;
		filename = malloc(sizeof(char)*100);
		char filetype[10];
		char filesize[10];
		char *filenamehelper;
		filenamehelper = malloc(sizeof(char)*100);
		int EOFreached = 0;
		int c;
		int filesizer;
		int matched;


		FILE *fptr2;
		fptr2 = fopen("TEMPARKIV", "w");

		while (EOFreached == 0) {
			c = fgetc(fptr);
			if (c == EOF) {
				break;
			}
			fgets(filenamehelper, 100, fptr);
			filename = realloc(filename, strlen(filenamehelper) + 1);
			filename[0] = c;
			for (i = 0; i < strlen(filenamehelper) + 1; ++i) {
				filename[i+1] = filenamehelper[i];
			}
			fgets(filetype, 10, fptr);
			fgets(filesize, 10, fptr);
			//printf("filename is %s", filename);

			char **filenamebreakdown;
			filenamebreakdown = malloc(sizeof(char*));
			filename[strlen(filename) - 1] = '\0';
			filenamebreakdown[0] = malloc(strlen(filename) + 1);
			strcpy(filenamebreakdown[0], filename);
			
			//int breakdownsize;
			//breakdownsize = breakdownexpander(filenamebreakdown);
			//printf("%s\n", filenamebreakdown[0]);
			//printf("size is %d\n", breakdownsize);
			//chararrayprinter(filenamebreakdown, breakdownsize);
			//printf("good\n");
			//printf("breakdownsize is %d\n", breakdownsize);
			//printf("original is %s\n", filenamebreakdown[0]);


			int count = 0;
			//char *temp;
			//temp = malloc(1);
			for (i = strlen(filenamebreakdown[0]); i >= 0; --i) {
				//printf("step %d\n", i);
				if (filenamebreakdown[0][i] == '/') {
					//printf("found / at position %d\n", i);
					count++;
					//printf("count is now %d\n", count);
					filenamebreakdown = realloc(filenamebreakdown, sizeof(char*) * (count + 1));
					//printf("the new filebreakdown is\n");
					//chararrayprinter(filenamebreakdown, count+1);
					//printf("size is now %lu\n", sizeof(filenamebreakdown));
					filenamebreakdown[count] = malloc(sizeof(char) * (i-1));
					//temp = realloc(temp, i);
					for(j = 0; j < i; ++j) {
						filenamebreakdown[count][j] = filenamebreakdown[0][j];
					}
					filenamebreakdown[count][i] = '\0';
					
				}
			//printf("temp is %s\n", temp);
			//strcpy(filenamebreakdown[count], temp);
			//printf("copied %s\n", filenamebreakdown[count]);
			//printf("original is still %s\n", filenamebreakdown[0]);
			//printf("here's the whole array\n");
			//chararrayprinter(filenamebreakdown, count+1);
			//printf("done\n");
			}

			count++;
			matched = 0;
			for (i = 0; i < count; ++i) {
				for (j = 0; j < argc - 3; ++j) {
					if (strcmp(filenamebreakdown[i], names[j]) == 0) {
						//printf("match found with %s and %s\n", filenamebreakdown[0], names[j]);
						matched = 1;
					}
				}
			}


			if (matched == 0) {
				strcat(filename, "\n");
				fprintf(fptr2, filename);
				//fprintf(fptr2, "TYPE2");
				fprintf(fptr2, filetype);
				fprintf(fptr2, filesize);
			

				filesizer = filesizetoint(filesize);
				
				for (i = 0; i < filesizer; ++i) {
					c = fgetc(fptr);
					
					fputc(c, fptr2);
					
				}
			}
			else if (matched == 1) {
				filesizer = filesizetoint(filesize);
				for (i = 0; i < filesizer; ++i) {
					c = fgetc(fptr);
					if (c == EOF) { 
						EOFreached = 1;
					}

				}
			}
		}
		unlink(argv[2]);
		rename("TEMPARKIV", argv[2]);
	}
	else if (strcmp(argv[1], "t") == 0) {

		FILE *fptr;
		fptr = fopen(argv[2], "r");
		char *filename;
		filename = malloc(sizeof(char)*100);
		char filetype[10];
		char filesize[10];
		char *filenamehelper;
		filenamehelper = malloc(sizeof(char)*100);
		int EOFreached = 0;
		int c;
		int filesizer;
		int i;


		while (EOFreached == 0) {
			c = fgetc(fptr);
			if (c == EOF) {
				break;
			}
			fgets(filenamehelper, 100, fptr);
			filename = realloc(filename, strlen(filenamehelper) + 1);
			filename[0] = c;
			for (i = 0; i < strlen(filenamehelper) + 1; ++i) {
				filename[i+1] = filenamehelper[i];
			}
			fgets(filetype, 10, fptr);
			fgets(filesize, 10, fptr);
		
			filesizer = filesizetoint(filesize);
			if (filetype[0] == 'd') {
				filename[strlen(filename) -1] = '/';
			}
			printf("%8d %s", filesizer, filename);
			if (filetype[0] == 'd') {
				printf("\n");
			}
			for (i = 0; i < filesizer; ++i) {
				c = fgetc(fptr);
				if (c == EOF) { 
					EOFreached = 1;
				}

			}
		}
	}
}


