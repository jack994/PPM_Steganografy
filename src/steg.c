/*
 * steg.c
 *
 *  Created on: Feb 20, 2017
 *      Author: giacomobenso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * linked list to hold the comments.
 */
typedef struct NODE {
	char *val;
	struct NODE * next;
} NODE;

/**
 * structure to hold a pixel of the image.
 */
typedef struct PIXEL {
	unsigned int R, G, B;
} PIXEL;

/**
 * main structure to hold the PPM file.
 */
typedef struct PPM {
	char *format;
	NODE *comments;
	unsigned int width, height;
	unsigned int max;
	PIXEL * map;
} PPM;

//-----------------------------------------------------------------------

/**
 * function used to deepcopy the linkedlist holding the comments.
 * it returns a pointer to a deepcopy of the list passed as a parameter;
 */
NODE *copy(NODE *start1) {
	NODE *start2 = NULL, *previous = NULL;

	while (start1 != NULL) {
		NODE * temp = (NODE *) malloc(sizeof(NODE));
		temp->val = start1->val;
		temp->next = NULL;

		if (start2 == NULL) {
			start2 = temp;
			previous = temp;
		} else {
			previous->next = temp;
			previous = temp;
		}
		start1 = start1->next;
	}
	return start2;
}

/**
 * function to deepcopy the PPM file passed as a parameter.
 * it returns a pointer to the new PPM.
 */
struct PPM * copyPPM(PPM *old) {

	PPM *new = (PPM*) malloc(sizeof(PPM));

	new->format = (char *) malloc(sizeof(char) * 3);
	strncpy(new->format, old->format, 3); //copy the format

	new->comments = copy(old->comments); //copy the comments

	// copy the height, width and max

	new->height = old->height;
	new->width = old->width;
	new->max = old->max;

	//copy the array containing the pixels
	new->map = (PIXEL *) malloc(old->height * old->width * sizeof(PIXEL));
	memcpy(new->map, old->map, old->height * old->width * sizeof(PIXEL));

	return new;

}

/**
 * function to read from a PPM file and store it into a PPM structure.
 * we then return the pointer to the structure;
 */
struct PPM * getPPM(FILE * fd) {

	PPM *image = (PPM *) malloc(sizeof(PPM));

	//getting the format
	image->format = (char *) malloc(sizeof(char) * 3);
	fscanf(fd, "%s", image->format);
	if (strcmp(image->format, "P3") != 0) {
		printf("Invalid image Format.");
		exit(0);
	}
	char c = getc(fd); //there is the newline char.
	//storing all the comments into the linkedList
	image->comments = (NODE *) malloc(sizeof(NODE));
	NODE *temp = image->comments;
	while ((c = getc(fd)) == '#') {
		fseek(fd, -1, SEEK_CUR); //we need to go back one character to get the '#'
		char str[100];
		fgets(str, 100, fd);
		temp->val = (char *) malloc(strnlen(str, 100));
		strcpy(temp->val, str);
		temp->val[strlen(temp->val) - 1] = 0;
		temp->next = (NODE *) malloc(sizeof(NODE));
		temp = temp->next;
		temp->next = NULL;
	}

	fseek(fd, -1, SEEK_CUR);
	//get the width,height and max
	fscanf(fd, "%d", &image->width);
	fscanf(fd, "%d", &image->height);
	fscanf(fd, "%d", &image->max);

	image->map = (PIXEL *) malloc(image->height * image->width * sizeof(PIXEL));

	int t = 0;
	int j = 0;
	//inserting all the RGB values of the pixels into the array 'map'
	while (j < image->height * image->width) {
		t = fscanf(fd, "%d", &image->map[j].R);
		t = fscanf(fd, "%d", &image->map[j].G);
		t = fscanf(fd, "%d", &image->map[j].B);
		j = j + 1;
	}

	return image;
}

/**
 * function to encode the message into the ppm file stored in the structure.
 * a copy of the ppm file is returned with the message encrypted into the red field.
 */
struct PPM * encode(char * text, struct PPM * i) {

	PPM * str = copyPPM(i); //create a deepcopy of the structure
	int random;
	srand((unsigned) time(NULL));
	//create the maximum size of the random to avoid going over the end of the image.
	int randMax = (i->height * i->width) / (strlen(text)) - 1;
	int k = 0;
	int j = 0;

	//replace the red fields with the ASCII character casted as an int.
	//the character is inserted every time using a different random number.
	while (k < strlen(text)) {
		random = rand() % randMax; //get the random
		if (random < 1) {
			random = 1; //if the random is less than 1 we set it as 1
		}
		j = j + random;
		if (str->map[j].R == text[k]) {
			j = j + 1; // if the values are the same we encode in the next pixel.
		} else {
			str->map[j].R = text[k]; // set the value of the red field with the ASCII char.
			k = k + 1;
		}
	}

	return str;
}

/**
 * function to decode the message encripted in the ppm file comparing it with
 * the original file.
 */
char * decode(struct PPM * i1, struct PPM * i2) {

	int j = 0;
	int i = 0;
	char *str = (char *) malloc(255);

	while (j < i1->height * i1->width) {
		if (i1->map[j].R != i2->map[j].R) {
			str[i] = i2->map[j].R;
			i = i + 1;
		}
		j = j + 1;
	}

	str = realloc(str, i);

	return str;
}

/**
 * function to print the PPM structure
 */
void showPPM(struct PPM * i) {

	printf("%s\n", i->format); //print format

	//print comments
	NODE *n = i->comments;
	while (n->next != NULL) {
		printf("%s\n", n->val);
		n = n->next;
	}

	// print width, height and max
	printf("%d %d\n%d\n", i->width, i->height, i->max);

	//print the array containing the pixels
	int j;
	for (j = 0; j < i->height * i->width; ++j) {
		printf("%d %d %d\n", i->map[j].R, i->map[j].G, i->map[j].B);
	}

	return;
}

//-----------------------------------------------------------------------

int main (int argc, char ** argv){

	if(argc != 3 && argc != 4){ //check the arguments
		printf("Incorrect number of arguments\n");
		exit(0);
	}
	else if(argc ==3){
		if(strncmp(argv[1], "e",1) != 0){ // check that if the arguments are 3 we are trying to encode
			printf("Unknown command %s for %d arguments\n", argv[1], argc);
			exit(0);
		}
		// encode a message inserted from the user in the ppm passed a parameter
		FILE * f = fopen(argv[2], "r");
		PPM * ppmOne = getPPM(f);
		fclose(f);
		char * toEnc =(char*) malloc(255 * sizeof(char));
		fgets (toEnc, 255, stdin);
		PPM * encoded = encode(toEnc, ppmOne);
		showPPM(encoded); //output the encoded ppm
	}
	else{
		if(strncmp(argv[1], "d",1) != 0){ // check that if the arguments are 4 we are trying to decode
					printf("Unknown command %s for %d arguments\n", argv[1], argc);
					exit(0);
				}
		// compare the two arguments to decode the message encoded in the second file
		FILE * f1 = fopen(argv[2], "r");
		FILE * f2 = fopen(argv[3], "r");
		PPM * ppm1 = getPPM(f1);
		PPM * ppm2 = getPPM(f2);
		char * dec = decode(ppm1, ppm2);
		printf("%s", dec); // print the message
	}

}
