#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <dirent.h>


//might have to do this as a linked list
typedef struct macro {
	char *name;
	char *value;
	int defined;
} macro;


char *storefile(char *filename)
{
	char buffer[256];
	char *input;
	int inputsize = 1;
	input = malloc(sizeof(char)*1);
	input[0] = '\0';

	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		return NULL;
	}
	while (fgets(buffer, 256, fp)) {
		inputsize += strlen(buffer);
		input = realloc(input, sizeof(char)*inputsize);
		strcat(input, buffer);
	}
	fclose(fp);
	return input;
}

char *storestdin()
{
	char buffer[256];
	char *input;
	int inputsize = 1;
	input = malloc(sizeof(char)*1);
	input[0] = '\0';
	while (fgets(buffer, 256, stdin)) {
		inputsize += strlen(buffer);
		input = realloc(input, sizeof(char)*inputsize);
		strcat(input, buffer);
	}
	return input;
}

typedef struct element {
	struct element *previous;
	struct element *next;
	char content; 
} element;

typedef struct firstandlast {
	element *first;
	element *last;
} firstandlast;

void addnext(element *input, char c)
{
	element *new;
	new = malloc(sizeof(element));
	new->previous = input;
	new->content = c;
	new->next = NULL;
	input->next = new;
	//printf("%c added\n", c);
	//printf("testing that it was added: %c\n", input->next->content);
	//fflush(stdout);
}

void addemptyprevious(element *input) 
{
	element *new;
	new = malloc(sizeof(element));
	new->next = input;
	new->previous = NULL;
	input->previous = new;
}

firstandlast stringtoll (char *string)
{
	//printf("string given to stringtoll is %s\n", string);
	int i;
	element *first;
	element *last;
	element *current;
	current = malloc(sizeof(element));
	current->content = string[0];
	current->next = NULL;
	current->previous = NULL;
	first = current;
	int length = strlen(string);
	for (i = 1; i < length; ++i) {
		//printf("address: %p, current.previous is %p, the char is %c and current.next is %p\n", current, current->previous, current->content, current->next);
		//printf("current contains character %c\n", current.content);
		addnext(current, string[i]);
		current = current->next;
		//printf("now current contains character %c\n", current.content);
		//fflush(stdout);
	}

	//printf("done with adding chars\n");

	last = current;
	firstandlast output;
	output.first = first;
	output.last = last;

	while (first->previous != NULL) {
		///printf("first contains %c\n", first->content);
		first = first->previous;

		//printf("going back!\n");
	}

	//printf("\n first contains %c and last contains %c\n", first->content, last->content);
	//fflush(stdout);
	return output;
}


typedef struct macroresult {
	char *macroname;
	int ismacro;
	int ERROR;
	int isspecialchar;
	char specialchar;
	int isdoublyspecial;
	char doublyspecial1;
	char doublyspecial2;
} macroresult;

void freeallbefore(element *start)
{
	// element *initial;
	/*element *start2;
	int i;
	if (start->previous == NULL) {
		return;
	}
	int counter = 0;
	start = start->previous;

	while (start->previous != NULL) {
		free(start->next->previous);
		printf("freeing pointing from left of %c\n", start->content);
		++counter;
		start = start->previous;
	}

	free(start->next->previous);
	
	free(start->previous);
	start2 = start;
	printf("start is %p and start2 is %p\n", start, start2);
	start = start->next;
	printf("now start is %p and start2 is %p\n", start, start2);

	for (i = 0; i <= counter; ++i) {
		printf("freeing %c\n", start2->content);
		free(start2);
		element *start2;
		start2 = start;
		start = start->next;
	}
	start->previous = NULL;

	//TODO: show this error to the TF*/

	element *first = start->previous;
	while (first != NULL) {
		start->previous = first->previous;
		free(first);
		first = start->previous;
	}




	// if (start->previous == NULL) {
	// 	return;
	// }
	// element *start2;
	// start = start->previous;
	// while (start != NULL) {
	// 	start2 = start->previous;
	// 	printf("freeing %c\n", start->content);
	// 	free(start);
	// 	start = start2;
	// }
	// start = initial;


	//start->previous = NULL;
}


macroresult macrochecker(element **start)
{
	int j;
	macroresult output;
	output.ERROR = 0;
	output.isspecialchar = 0;
	int counter = 1;
	output.isdoublyspecial = 0;
	while(*start != NULL) {
		//printf("macrochecker is reading %c\n", (*start)->content);
		if ((*start)->content == '{') {
			if (counter == 1) {
				output.isspecialchar = 1;
				output.specialchar = '{';
				output.macroname = NULL;
				output.ismacro = 0;
				return output;
			}
			else {
				output.macroname = calloc(sizeof(char), counter);
				for (j = 1; j <= counter - 1; ++j) {
					*start = (*start)->previous;
					output.macroname[counter - j - 1] = (*start)->content;
				}
				for (j = 1; j <= counter - 1; ++j) {
					*start = (*start)->next;
				}
				//printf("\npointing at %c\n", start->content);

				//makes it so that it points back to the '{'
				output.ismacro = 1;
				//printf("result of macrochecker was %s\n", output.macroname);
				return output;
			}
		}
		else if ((*start)->content == '\\' || (*start)->content == '#' || (*start)->content == '%' || (*start)->content == '{' || (*start)->content == '}') {
			output.isspecialchar = 1;
			output.specialchar = (*start)->content;
			//putchar((*start)->content);
			output.macroname = NULL;
			output.ismacro = 0;
			return output;
		}
		else if(isalnum((*start)->content) == 0) {
			if (counter > 1) {
				//printf("got an error at %c\n", (*start)->content);
				//printf("TYPE1 error\n");
				output.macroname = NULL;
				output.ismacro = 0;
				output.ERROR = 1;
				return output;
			}
			else if (counter == 1) {
				output.macroname = NULL;
				output.ismacro = 0;
				output.isdoublyspecial = 1;
				output.doublyspecial1 = '\\';
				output.doublyspecial2 = (*start)->content;
				return output;
			}
		}
		++counter;
		*start = (*start)->next;
	}
	//printf("TYPE2 error\n");
	output.macroname = NULL;
	output.ismacro = 0;
	output.ERROR = 1;
	return output;
}

typedef struct bracebracketed{
	char *value;
	int error;
} bracebracketed;


bracebracketed readbracebracketed(element **start)
{
	//make *start point to the first element after the '{'.
	bracebracketed output;
	int numleftbraces = 1;
	int numrightbraces = 0;
	int counter2 = 1;
	int j;
	while (*start != NULL) {
		if ((*start)->content == '\\') {
			if ((*start)->next->content == '{' || (*start)->next->content == '}' || (*start)->next->content == '%' || (*start)->next->content == '#' || (*start)->next->content == '\\') {
				*start = (*start)->next;
				//printf("skipping over %c\n", (*start)->content);
				++counter2;
			}
		}
		else if ((*start)->content == '{') {
			numleftbraces++;
		}
		else if ((*start)->content == '}') {
			numrightbraces++;
		}
		if (numleftbraces == numrightbraces) {
			output.value = calloc(sizeof(char), counter2);
			//printf("callocing %d\n", counter2 - 1);
			for (j = 1; j <= counter2 - 1; ++j) {
				*start = (*start)->previous;
				output.value[counter2 - j - 1] = (*start)->content;
			}
			for (j = 1; j <= counter2 - 1; ++j) {
				*start = (*start)->next;
			}
			//printf("returning value is %s\n", output.value);
			output.error = 0;
			return output;
		}
		++counter2;
		*start = (*start)->next;
		if (*start == NULL) {
			//printf("type1 error\n");
			output.value = NULL;
			output.error = 1;
			return output;
		}
	}
	//printf("type2 error\n");
	output.value = NULL;
	output.error = 1;
	return output;
}

bracebracketed readalphanumeric(element **start)
{
	//pointing to the element after '{'
	bracebracketed output;
	int counter = 1;
	int j;
	while ((*start) != NULL) {
		if ((*start)->content == '}') {
			if (counter == 1) {
				//printf("alpha1\n");
				output.value = NULL;
				output.error = 1;
				return output;
			}
			else {
				output.value = calloc(sizeof(char), counter);
				for (j = 1; j <= counter - 1; ++j) {
					*start = (*start)->previous;
					output.value[counter-j-1] = (*start)->content;
				}
				for (j = 1; j <= counter - 1; ++j) {
					(*start) = (*start)->next;
				}
				output.error = 0;
				//printf("alphanumeric is %s\n", output.value);
				return output;
			}
		}
		else if (isalnum((*start)->content) == 0) {
			//fprintf(stderr, "not alphanumeric in def name\n");
			output.value = NULL;
			output.error = 1;
			//printf("alpha2\n");
			return output;
			//didn't work
		}
		

		++counter;
		(*start) = (*start)->next;
	}
	output.value = NULL;
	//printf("alpha3\n");
	output.error = 1;
	return output;
}

int deffunc(element **start, macro *macroarray)
{
	bracebracketed NAME;
	bracebracketed ARG;
	int j;
	//char *value;
	*start = (*start)->next;
	NAME = readalphanumeric(start);
	if (NAME.error == 1) {
		//printf("def1\n");
		return -1;
	}
	//printf("name is %s\n", NAME.value);

	*start = (*start)->next;
	if ((*start)->content != '{') {
		//printf("def2\n");
		return -1;
	}
	//printf("going fine\n");
	//fflush(stdout);
	*start = (*start)->next;
	ARG = readbracebracketed(start);
	if (ARG.error == 1) {
		//printf("def3\n");
		return -1;
	}
	//printf("ARG is %s\n", ARG.value);
	//printf("going fine\n");
	//fflush(stdout);
	for (j = 0; j < 100; ++j) {
		if (macroarray[j].name == NULL) {
			continue;
		}
		if (strcmp(macroarray[j].name, NAME.value) == 0) {
			//fprintf(stderr, "Attempts to redefine a macro\n");
			//printf("def4\n");
			return -1;
		}
	}
	for (j = 0; j < 100; ++j) {
		if (macroarray[j].name == NULL) {
			macroarray[j].name = NAME.value;
			macroarray[j].value = ARG.value;
			macroarray[j].defined = 1;
			//printf("%s defined as %s in position %d\n", NAME.value, ARG.value, j);
			break;
		}
	}


	return 1;
}

int undeffunc(element **start, macro *macroarray)
{
	bracebracketed NAME;
	*start = (*start)->next;
	NAME = readalphanumeric(start);
	if (NAME.error == 1) {
		return -1;
	}
	int j;
	for (j = 0; j < 100; ++j) {
		if (macroarray[j].name == NULL) {
			continue;
		}
		if (strcmp(macroarray[j].name, NAME.value) == 0) {
			macroarray[j].name = NULL;
			macroarray[j].value = NULL;
			macroarray[j].defined = 0;
			//printf("undefined %s\n", NAME.value);
			fflush(stdout);
			return 1;
		}
	}
	return -1;
}

int ifdeffunc(element **start, macro *macroarray)
{
	bracebracketed NAME;
	bracebracketed THEN;
	bracebracketed ELSE;
	int j, i;
	*start = (*start)->next;
	NAME = readalphanumeric(start);
	if(NAME.error == 1) {
		return -1;
	}
	*start = (*start)->next;
	if ((*start)->content != '{') {
		return -1;
	}
	*start = (*start)->next;
	THEN = readbracebracketed(start);
	if (THEN.error == 1) {
		return -1;
	}
	*start = (*start)->next;
	if ((*start)->content != '{') {
		return -1;
	}
	*start = (*start)->next;
	ELSE = readbracebracketed(start);
	//printf("else %s\n", ELSE.value);

	if (ELSE.error == 1) {
		return -1;
	}

	int length;
	for (j = 0; j < 100; ++j) {
		if (macroarray[j].name == NULL) {
			continue;
		}
		if (strcmp(macroarray[j].name, NAME.value) == 0) {
			//replace everything 
			//printf("size of THEN is %lu\n", strlen(THEN.value));
			length = strlen(THEN.value);
			for (i = 0; i < length; ++i) {
				(*start)->content = THEN.value[length - i - 1];
				if ((*start)->previous != NULL) {
					*start = (*start)->previous;
				}
				else {
					addemptyprevious(*start);
					*start = (*start)->previous;
				}
			}
			freeallbefore(*start);
			//printf("added %s\n", THEN.value);
			//addemptyprevious(*start);
			//*start = (*start)->previous;
			return 1;
		}
	}
	length = strlen(ELSE.value);
	for (i = 0; i < length; ++i) {
		(*start)->content = ELSE.value[length - i - 1];
		if ((*start)->previous != NULL) {
			*start = (*start)->previous;
		}
		else {
			addemptyprevious(*start);
			*start = (*start)->previous;
		}
	}
	//printf("added %s\n", ELSE.value);
	freeallbefore(*start);
	//addemptyprevious(*start);
	//*start = (*start)->previous;
	//printf("size of ELSE is %lu\n", strlen(ELSE.value));
	return 1;


}

int iffunc(element **start)
{
	bracebracketed NAME;
	bracebracketed THEN;
	bracebracketed ELSE;
	int i;
	*start = (*start)->next;
	NAME = readbracebracketed(start);
	if(NAME.error == 1) {
		return -1;
	}
	*start = (*start)->next;
	if ((*start)->content != '{') {
		return -1;
	}
	*start = (*start)->next;
	THEN = readbracebracketed(start);
	if (THEN.error == 1) {
		return -1;
	}
	*start = (*start)->next;
	if ((*start)->content != '{') {
		return -1;
	}
	*start = (*start)->next;
	ELSE = readbracebracketed(start);
	if (ELSE.error == 1) {
		return -1;
	}


	int length;
	if (strlen(NAME.value) > 0) {
		//replace everything 
		//printf("size of THEN is %lu\n", strlen(THEN.value));
		length = strlen(THEN.value);
		for (i = 0; i < length; ++i) {
			(*start)->content = THEN.value[length - i - 1];
			if ((*start)->previous != NULL) {
				*start = (*start)->previous;
			}
			else {
				addemptyprevious(*start);
				*start = (*start)->previous;
			}
		}
		freeallbefore(*start);
		//addemptyprevious(*start);
		//*start = (*start)->previous;
		return 1;
	}
	length = strlen(ELSE.value);
	for (i = 0; i < length; ++i) {
		(*start)->content = ELSE.value[length - i - 1];
		if ((*start)->previous != NULL) {
			*start = (*start)->previous;
		}
		else {
			addemptyprevious(*start);
			*start = (*start)->previous;
		}
	}
	freeallbefore(*start);
	//addemptyprevious(*start);
	//*start = (*start)->previous;
	//printf("size of ELSE is %lu\n", strlen(ELSE.value));
	return 1;

}




int custommacrofunc(element **start, macro *macroarray, int index)
{
	//printf("custommacrofunc is called\n");
	bracebracketed ARG;
	*start = (*start)->next;
	ARG = readbracebracketed(start);
	if(ARG.error == 1) {

		//printf("error 1\n");
		return -1;
	}
	//printf("ARG is %s\n", ARG.value);
	//printf("the unexpanded macro is %s\n", macroarray[index].value);

	char *output;
	int i;
	int sizetomalloc = 0;
	int numberexpands = 0;
	int length = strlen(macroarray[index].value);
	int length2 = strlen(ARG.value);
	for (i = 0; i < length; ++i) {
		if (macroarray[index].value[i] != '#') {
			sizetomalloc++;
		}
		else if (macroarray[index].value[i] == '#' && i == 0) {
			sizetomalloc += length2;
			++numberexpands;
		}
		else if (macroarray[index].value[i] == '#' && macroarray[index].value[i-1] == '\\') {
			sizetomalloc++;
		}
		else if (macroarray[index].value[i] == '#' && macroarray[index].value[i-1] != '\\') {
			sizetomalloc += length2;
			++numberexpands;
		}
	}

	//printf("numberexpands is %d\n", numberexpands);

	output = calloc(sizeof(char), sizetomalloc + 1);
	int counter = 0;
	i = 0;
	int j;
	while (counter < sizetomalloc) {
		if (macroarray[index].value[i] != '#') {
			output[counter] = macroarray[index].value[i];
			++counter;
		}
		else if (macroarray[index].value[i] == '#' && i == 0) {
			for (j = 0; j < strlen(ARG.value); ++j) {
				output[counter] = ARG.value[j];
				++counter;
			}
		}
		else if (macroarray[index].value[i] == '#' && macroarray[index].value[i-1] == '\\') {
			output[counter] = macroarray[index].value[i];
			++counter;
		}
		else if (macroarray[index].value[i] == '#' && macroarray[index].value[i-1] != '\\') {
			for (j = 0; j < strlen(ARG.value); ++j) {
				output[counter] = ARG.value[j];
				++counter;
			}
		}
		++i;
	}
	length = strlen(output);
	for (i = 0; i < length; ++i) {
		(*start)->content = output[length - i - 1];
		if ((*start)->previous != NULL) {
			*start = (*start)->previous;
		}
		else {
			addemptyprevious(*start);
			*start = (*start)->previous;
		}
	}
	//printf("expanded to %s\n", output);
	freeallbefore(*start);


	//printf("the argument to the custom macro %s is %s\n", macroarray[index].name, ARG.value);
	return 1;
}

int includefunc(element **start)
{
	int i;
	bracebracketed ARG;
	*start = (*start)->next;
	ARG = readbracebracketed(start);
	if(ARG.error == 1) {
		return -1;
	}

	char *filecontents;
	filecontents = storefile(ARG.value);

	int length = strlen(filecontents);
	for (i = 0; i < length; ++i) {
		(*start)->content = filecontents[length - i - 1];
		if ((*start)->previous != NULL) {
			*start = (*start)->previous;
		}
		else {
			addemptyprevious(*start);
			*start = (*start)->previous;
		}
	}
	freeallbefore(*start);

	//printf("the file that was added is %s", filecontents);
	return 1;

}



char * m15(element *start, macro *macroarray) {
	char *output;
	output = calloc(sizeof(char), 100);
	int counter = 0;
	int j;
	macroresult currentmacroresult;
	int error;
	int length, length2;
	while (start != NULL) {
		if (start->content == '\\') {
			//printf("\ncalling macrochecker\n");
			freeallbefore(start);
			start = start->next;
			currentmacroresult = macrochecker(&start);
			if (currentmacroresult.isspecialchar == 1) {
				output[counter] = start->content;
				++counter;
			}
			if (currentmacroresult.isdoublyspecial == 1) {
				output[counter] = currentmacroresult.doublyspecial1;
				++counter;
				output[counter] = currentmacroresult.doublyspecial2;
				++counter;
			}
			if (currentmacroresult.ERROR == 1) {
				fprintf(stderr, "There was an error trying to read a macro\n");
				return NULL;
			}
			if (currentmacroresult.ismacro == 1) {
				//start = start->previous;
				//printf("in m15, start is passed at %c\n", start->content);

				if (strcmp(currentmacroresult.macroname, "def") == 0) {
					error = deffunc(&start, macroarray);
					if (error == -1) {
						fprintf(stderr, "Error with def macro\n");
						return NULL;
					}
					//printf("DEF! worked");
				}
				else if (strcmp(currentmacroresult.macroname, "ifdef") == 0) {
					//printf("IFDEF! being called");
					error = ifdeffunc(&start, macroarray);
					if (error == -1) {
						fprintf(stderr, "Error with ifdef macro\n");
						return NULL;
					}
				}
				else if (strcmp(currentmacroresult.macroname, "undef") == 0) {
					error = undeffunc(&start, macroarray);
					if (error == -1) {
						fprintf(stderr, "Error with undef macro\n");
						return NULL;
					}
					//printf("UNDEF!");
				}
				else if (strcmp(currentmacroresult.macroname, "if") == 0) {
					error = iffunc(&start);
					if (error == -1) {
						fprintf(stderr, "Error with if macro\n");
						return NULL;
					}
					//printf("UNDEF!");
				}
				else if (strcmp(currentmacroresult.macroname, "include") == 0) {
					error = includefunc(&start);
					if (error == -1) {
						fprintf(stderr, "Error with if macro\n");
						return NULL;
					}
					//printf("UNDEF!");
				}
				else if (strcmp(currentmacroresult.macroname, "expandafter") == 0) {
					bracebracketed BEFORE;
					bracebracketed AFTER;
					int i;
					start = start->next;
					BEFORE = readbracebracketed(&start);
					if(BEFORE.error == 1) {
						return NULL;
					}
					start = start->next;
					if (start->content != '{') {
						return NULL;
					}
					start = start->next;
					AFTER = readbracebracketed(&start);
					if (AFTER.error == 1) {
						return NULL;
					}

					firstandlast newll = stringtoll(AFTER.value);
					char *expandedafter;
					//freeallbefore(llinput.last);
					expandedafter = m15(newll.first, macroarray);
					if (expandedafter == NULL) {
						return NULL;
					}
					char *output = calloc(sizeof(char), strlen(BEFORE.value) + strlen(expandedafter)+1);
					length = strlen(BEFORE.value);
					length2 = strlen(expandedafter);
					for (i = 0; i < length; ++i) {
						output[i] = BEFORE.value[i];
					}
					for (i = length; i < length + length2; ++i) {
						output[i] = expandedafter[i - length];
					}
					//printf("output is %s and strlen(output) is %lu\n", output, strlen(output));
					length = strlen(output);
					for (i = 0; i < length; ++i) {
						start->content = output[length - i - 1];
						if (start->previous != NULL) {
							start = start->previous;
						}
						else {
							addemptyprevious(start);
							start = start->previous;
						}
					}
					freeallbefore(start);

					/*error = iffunc(&start);
					if (error == -1) {
						fprintf(stderr, "Error with if macro\n");
						return NULL;
					}*/
					//printf("UNDEF!");
				}
				else { 
					for (j = 0; j < 100; ++j) {
						if (macroarray[j].name == NULL) {
							continue;
						}
						if (strcmp(currentmacroresult.macroname, macroarray[j].name) == 0) {
							error = custommacrofunc(&start, macroarray, j);
							if (error == -1) {
								fprintf(stderr, "Error with custom macro\n");
							}
							break;
						}
					}
					if (j == 100) {
						fprintf(stderr, "Macro not defined\n");
						return NULL;
					}
				}
			}
			//printf("\npointing at %c\n", start->content);
			//printf("macroresult.ismacro is %d and the string is %s\n", currentmacroresult.ismacro, currentmacroresult.macroname);
			//printf("currently pointing to %c\n", start->content);
			//current = macrochecker(start);
		}
		else if (start->content == '%') {
			if (start->previous != NULL && start->previous->content != '\\'){
				while (start->content != '\n') {
					start = start->next;
					if (start == NULL) {
						return output;
					}
				}
				start = start->next;
				if (start == NULL) {
					return output;
				}
				while (start->content == ' ' || start->content == '\t') {
					start = start->next;
					if (start == NULL) {
						return output;
					}
				}
				start = start->previous;
			}
		}
		else {
			output[counter] = start->content;
			++counter;
			//putchar(start->content);
		}

		if (start == NULL) {
			return output;
		}
		start = start->next;
		if (start == NULL) {
			return output;
		}
	}
	return output;
}



int main(int argc, char *argv[])
{
	macro macroarray[100];
	int i;
	for (i = 0; i < 100; ++i) {
		macroarray[i].name = NULL;
		macroarray[i].value = NULL;
		macroarray[i].defined = 0;
	}

	/*
	for (i = 0; i < 100; ++i) {
		printf("%s\n%s\n%d\n----\n", macroarray[i].name, macroarray[i].value, macroarray[i].defined);
		fflush(stdout);
	}
	macroarray[0].name = "hello";*/
	//TODO: figure out how to initialize macroarray, how to realloc and modify it in other functions
	//macroarray[0].name = "XXXX";
	//printf("the number of elements in macroarray is %lu", sizeof(macroarray));
	//macroarray = realloc(macroarray, sizeof(macroarray)*2);
	//printf("the number of elements in macroarray is %lu", sizeof(macroarray));

	//macroarray[0].name = "asdf";

	//printf("argc is %d\n", argc);
	if (argc == 1) {
		char *input;
		input = storestdin();
		firstandlast llinput = stringtoll(input);
		char *output;

		//freeallbefore(llinput.last);
		output = m15(llinput.first, macroarray);
		free(input);
		if (output == NULL) {
			return 0;
		}
		else {
			printf("%s", output);
		}
	}
	else if (argc > 1) {
		for (i = 1; i < argc; ++i) {
			char *input;
			input = storefile(argv[i]);
			//printf("the contents of %s are %s\n", argv[i], input);
			firstandlast llinput = stringtoll(input);
			
			/*while (llinput.first != NULL) {
				putchar(llinput.first->content);
				llinput.first = llinput.first->next;
			}*/
			char *output;
			output = m15(llinput.first, macroarray);
			free(input);
			printf("%s", output);
			if (output == NULL) {
				//printf("it messed up");
				return 0;
			}
		}
	}
	
	//printf("macroarrayname is now %s", macroarray[0].name);
	return 0;
}





