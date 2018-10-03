//Journal that keeps track of meetings, classes and skypecalls

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/********************** LITERALS *************************/
#define DAYS 30  /*number of days that the journal covers*/

#define STR_MAX 31 /*maximum string size, including tou '\0' */

#define DAY_START 8  /*Earliest time an appointment can start is 8:00 */
#define DAY_END 15  /*Latest time an appointment can finish is 15:00 */
#define MIN_MIN 0
#define MAX_MIN 60

/********************** TYPES *************************/

enum {FALSE, TRUE};

struct time{
	int hour;
	int minute;
};

enum category{ MEETING, CLASS, SKYPECALL};

struct appointment{
	char *description;
	enum category category;
	struct time beginning;
	struct time end;
	struct appointment *next;
	struct appointment *prev;
};

/* type enum: actionT
 * All the possible operations of the journal.
 * ADD: Add a new appointment
 * REMOVE_BY_DESCR: Remove appointments according to their description
 * REMOVE_BY_CAT: Remove appointments according to their category
 * PRINT: Print all appointments
 * EXIT: Exit the programm
 */
typedef enum { ADD=1, REMOVE_BY_DESCR, REMOVE_BY_CAT, PRINT, EXIT } actionT;

/********************** PROTOTYPES *************************/

void printMenu();

void free_all(struct appointment *journal[DAYS]);

/*********************** GLOBALS ************************/

/* debug: check if helping messages will print or not
 * If FALSE they do not print, if TRUE they print*/
int debug = FALSE;


/*********************** FUNCTIONS ************************/

//Initializes the journal
void init_journal(struct appointment *journal[DAYS]) {
	int i;
	
	for (i = 0 ; i < DAYS ; i++) {
		journal[i]=NULL;
	}
	
}

//Prints the option menu in case ADD is chosen
int printCat() {
	int choice;
	
	do {
		printf("\nSelect category:\n");
		printf("\t%d. MEETING\n", MEETING);
		printf("\t%d. CLASS\n", CLASS);
		printf("\t%d. SKYPE CALL\n--> ", SKYPECALL);
		
		scanf("%d", &choice);
	} while (choice != MEETING && choice != CLASS && choice != SKYPECALL);
	
	return (choice);
}

//Adds an appointment to the journal
void add_one(struct appointment *journal[DAYS], int current_day, char *descr, int category, int hh, int mm, int HH, int MM) {
	struct appointment *new_node, *curr;
	/* curr: current appointment
	 * new_node: new appointment */
	
	//if there is no other appointment in that day
	if (journal[current_day - 1] == NULL) {
		journal[current_day - 1] = (struct appointment *) malloc(sizeof(struct appointment));
		if (journal[current_day - 1] == NULL) {
			printf("\nAdd: Problem allocating memory.\n");
			free_all(journal);
			return;
		}
		
		journal[current_day - 1]->description = (char *) malloc(strlen(descr)*sizeof(char));
		if (journal[current_day - 1]->description == NULL) {
			printf("\nAdd: Problem allocating memory.\n");
			free_all(journal);
			return;
		}
		
		strcpy( journal[current_day - 1]->description, descr);
		
		journal[current_day - 1]->category = category;
		
		journal[current_day - 1]->beginning.hour = hh;
		journal[current_day - 1]->beginning.minute = mm;
		
		journal[current_day - 1]->end.hour = HH;
		journal[current_day - 1]->end.minute = MM;
		
		journal[current_day - 1]->next = NULL;
		journal[current_day - 1]->prev = NULL;
		
		if(debug==TRUE) {
			printf("\nAdded appointment on day %d.\n", current_day);
		}
	}
	else { //if there is/are other appointment/s that day
		for (curr = journal[current_day - 1] ; curr != NULL ; curr = curr->next) {
			
			if (hh < curr->beginning.hour || (hh == curr->beginning.hour && mm <= curr->beginning.minute)) {
				
				new_node = (struct appointment *) malloc(sizeof(struct appointment));
				if (new_node == NULL) {
					printf("\nAdd: Problem allocating memory.\n");
					free_all(journal);
					return;
				}
				
				new_node->description = (char *) malloc(strlen(descr)*sizeof(char));
				if (new_node->description == NULL) {
					printf("\nAdd: Problem allocating memory.\n");
					free_all(journal);
					return;
				}
				
				strcpy( new_node->description, descr);
				
				new_node->category = category;
				
				new_node->beginning.hour = hh;
				new_node->beginning.minute = mm;
				
				new_node->end.hour = HH;
				new_node->end.minute = MM;
				
				new_node->next = curr;
				new_node->prev = curr->prev;
				
				new_node->next->prev = new_node;
				if (new_node->prev != NULL) {
					new_node->prev->next = new_node;
				}
				
				if(debug == TRUE) {
					printf("\nAdded appointment on day %d.\n", current_day);
				}
			}
			else {
				new_node = (struct appointment *) malloc(sizeof(struct appointment));
				if (new_node == NULL) {
					printf("\nAdd: Problem allocating memory.\n");
					free_all(journal);
					return;
				}
				
				new_node->description = (char *) malloc(strlen(descr)*sizeof(char));
				if (new_node->description == NULL) {
					printf("\nAdd: Problem allocating memory.\n");
					free_all(journal);
					return;
				}
				
				strcpy( new_node->description, descr);
				
				new_node->category = category;
				
				new_node->beginning.hour = hh;
				new_node->beginning.minute = mm;
				
				new_node->end.hour = HH;
				new_node->end.minute = MM;
				
				new_node->prev = curr;
				new_node->next = NULL;
				
				new_node->prev->next = new_node;
				
				if(debug == TRUE) {
					printf("\nAdded appointment on day %d.\n", current_day);
				}
			}
		}
	}
}

//Adds appointment to the journal
void add(struct appointment *journal[DAYS]) {
	char descr[STR_MAX], format_str[20];
	int category, first_day, repeat_interval, current_day;
	int hh, mm; //about starting time of the appointment
	int duration, hours, minutes; //about duration of the appointment
	int HH, MM; //about finishing time of the appointment
		
	category = printCat();
	
	//Read appointment description
	printf("\nEnter short description: ");
	sprintf(format_str, " %%%ds", STR_MAX-1);
	scanf(format_str, descr);
	if (debug == TRUE) {
		printf("\nYou entered %s.\n", descr);
	}
	
	//Read starting time of the appointment
	do {
		printf("\nEnter start time in the form HH:MM : ");
		scanf("%d:%d", &hh, &mm);
		if ( (hh < DAY_START) || (mm < 0) || (mm >= 60) || (hh > DAY_END) || ((hh == DAY_END) && (mm > 0)) ) {
			printf("\nERROR: Invalid hour or minute. Try again.\n");
		}
	} while ( (hh < DAY_START) || (mm < 0) || (mm >= 60) || (hh > DAY_END) || ((hh == DAY_END) && (mm > 0)) );
	
	//Read duration of appointment
	do {
		do {
			printf("\nEnter duration in minutes : ");
			scanf("%d", &duration);
			if (duration < 0) {
				printf("\nERROR: Invalid duration. Try again.\n");
			}
		} while (duration<0);
		
		hours = duration / 60;
		minutes = duration % 60;
		MM = mm + minutes;
		HH = hh + hours;
		if (MM >= 60) {
			HH ++;
			MM = MM - 60;
		}
		
		if (HH > DAY_END || (HH == DAY_END && MM > 0)) {
			printf("\nERROR: Appointment exceeds end of workday. Try again.\n");
		}
		
	} while (HH > DAY_END || (HH == DAY_END && MM > 0));
	
	if (debug == TRUE) {
		printf("You entered %02d:%02d - %02d:%02d.\n", hh, mm, HH, MM);
	}
	
	//Read the first day fo the appointment and its repeat interval
	do {
		printf("\nEnter day (1 - %d) and repeat interval (>= 0):\t", DAYS);
		scanf(" %d %d", &first_day, &repeat_interval);
		if ((first_day < 1 || first_day > DAYS) || (repeat_interval < 0)) {
			printf("ERROR: Invalid day or interval. Try again.");
		}
	} while ((first_day < 1 || first_day > DAYS) || (repeat_interval < 0));
	
	//Adds the new appointment to the journal
	if (repeat_interval != 0){
		for(current_day = first_day ; current_day <= DAYS ; current_day = current_day + repeat_interval) {
			add_one(journal, current_day, descr, category, hh, mm, HH, MM);
		}
	}
	else {
		add_one(journal, first_day, descr, category, hh, mm, HH, MM);
	}
}

//Delete appointment according to category
void remove_by_cat(struct appointment *journal[DAYS]) {
	int category, current_day;
	struct appointment *curr, *next;
		
	category = printCat();
	
	for (current_day = 1 ; current_day <= DAYS ; current_day ++) {
		
		if (journal[current_day - 1] != NULL) {
			
			for (curr = journal[current_day - 1] ; curr != NULL ; ) {
				
				if (curr->category == category) {
					
					next = curr->next;
					curr->next->prev = curr->prev;
					curr->prev->next = curr->next;
					free(curr->description);
					free(curr);
					curr = next;
					
					if (debug == TRUE) {
						printf ("Removed apppointment on day %d\n", current_day);
					}
				}
			}
		}
	}
}

//Delete appointment according to description
void remove_by_descr(struct appointment *journal[DAYS]) {
	int current_day;
	char description[STR_MAX], format_str[13];
	struct appointment *curr, *next;
		
	printf ("\nEnter short description: ");
	sprintf(format_str, " %%%ds", STR_MAX - 1);
	scanf(format_str, description);
	
	for (current_day = 1 ; current_day <= DAYS ; current_day ++) {
		
		if (journal[current_day-1]!=NULL) {
			
			for (curr = journal[current_day - 1] ; curr != NULL ; ) {
				
				if (strcmp(curr->description, description) == 0) {
					
					next = curr->next;
					curr->next->prev = curr->prev;
					curr->prev->next = curr->next;
					free(curr->description);
					free(curr);
					curr = next;
					
					if (debug == TRUE) {
						printf ("Removed apppointment on day %d\n", current_day);
					}
				}
			}
		}
	}
}

//Print all appointments of journal
void print(struct appointment *journal[DAYS]) {
	int i, current_day;
	char category[10];
	struct appointment *curr;
		
	putchar('\n');
	for (current_day = 1 ; current_day <= DAYS ; current_day ++) {
		
		if (journal[current_day - 1] != NULL) {
			
			for (i=0 ; i<5 ; i++) {
				printf ("=");
			}
			printf ("\nDAY %d\n", current_day);
				
			for (curr = journal[current_day - 1] ; curr != NULL ; curr = curr->next) {
				
				switch (curr->category) {
					case MEETING : {strcpy(category, "MEETING"); break; }
					case CLASS : {strcpy(category, "CLASS"); break; }
					case SKYPECALL : {strcpy(category, "SKYPECALL"); break;}
				}
				
				printf ("\t%s (%s), %02d:%02d - %02d:%02d\n", 
						category, curr->description, curr->beginning.hour,
						curr->beginning.minute, curr->end.hour, curr->end.minute);
			}
		}
	}
}

//Free all dynamically allocated memory of the program
void free_all(struct appointment *journal[DAYS]) {
	int current_day;
	struct appointment *curr;
		
	for (current_day = 1 ; current_day <= DAYS ; current_day ++) {
		for (curr = journal[current_day - 1] ; curr != NULL ; curr = curr->next) {
			if (curr != NULL) {
				free(curr->description);
				free(curr);
			}
		}
	}
}

//Print menu of user options, read and execute user's option
void action(struct appointment *journal[DAYS]) {
	int choice;
	do {
		printMenu();
		scanf("%d", &choice);
		switch (choice) {
			case ADD: {
				add(journal);
				break;
			}
			case REMOVE_BY_DESCR: {
				remove_by_descr(journal);
				break;
			}
			case REMOVE_BY_CAT: {
				remove_by_cat(journal);
				break;
			}
			case PRINT: {
				print(journal);
				break;
			}
			case EXIT: {
				free_all(journal);
				break;
			}
			default: {
				printf("\nERROR: Incorrect selection. Try again.\n");
			}
		}
	} while (choice != EXIT);
}

int main (int argc, char *argv[]) {
	
	struct appointment *journal[DAYS];
	
	if (argc!=1) {
		if (strcmp( argv[1], "-debug")==0) {
			if (strcmp( argv[2], "on")==0) {
				debug = TRUE;
			}
			else {
				if (strcmp( argv[2], "off")==0) {
					debug = FALSE;
				}
			}
		}
		else {
			printf("Ignoring incorrect arguments.\n");
		}
	}
	
	init_journal(journal);
	
	action(journal);
	
	return 0;
}


/*********************************
 * printMenu()                   *
 * Print menu operations  *
 * Parameters: none             *
 * Return value: none           *
 *********************************/ 
void printMenu() {
	printf("\nSelect action:\n");
	printf("\t%d. Add appointment.\n", ADD);
	printf("\t%d. Cancel appointments by description.\n", REMOVE_BY_DESCR);
	printf("\t%d. Cancel appointments by category.\n", REMOVE_BY_CAT);
	printf("\t%d. Print appointments.\n", PRINT);
	printf("\t%d. Exit.\n--> ", EXIT);
}