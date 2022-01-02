#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/****************
-This is CMPE 351 Project program
-This program simulates the CPU Scheduler methods
*/

/*TODO
-Need to implement a way to check if the passed input argument exists (Not gonna check if the file is txt, just
check file exists(DONE!!!)

-Need to implement a linked list structure and funtions(DONE!!!)
-Need to implement a function to read from input file and insert them to Linked List(DONE!!!)
-Need to make a menu (Main menu, Methods menu, and Preemtive Mode menu are done) (IMPLEMENT menu3 and menu4)
-FCFS, SJF(Non-Preemtive), PS(Non-Preemtive) and RR are DONE!!!!!
-Implement Bubble Sort for selected mode(AT is Done!! PID is Done!! SJF is Done!! Priority is Done!!)
*/

// GLOBALS//
enum CPUMethods
{
	NONE,
	FCFS,
	SJF,
	PS,
	RR
} method = NONE; // CPU Scheduling Methods Enumeration
enum PMode
{
	OFF,
	ON
} mode = OFF; // Preemtive Mode Enumeration

// GLOBALS
int time_quantum;
char *input_filename = NULL;
char *output_filename = NULL;
char buffer_output[99999];
char buff[1000];
// GLOBALS//

struct node
{
	int process_id;
	int burst_time;
	int arrival_time;
	int priority;
	int waiting_time;
	int turnaround_time;
	int first_response;
	int how_much_left;
	int time_slices;
	int last_slice_burst;
	bool is_terminated;
	bool in_cpu;
	struct node *next;
};
struct node *header_original = NULL;

// Prototypes for Linked List
struct node *create_node(int, int, int, int);
struct node *insert_back(struct node *, int, int, int, int);
struct node *delete_front(struct node *);
void display_LL(struct node *);
struct node *clone_LL();
// Prototypes for Linked List

// Prototypes
void print_usage();									   // Usage showing Function
void menu();										   // Main Menu Function
void menu1();										   // Scheduling Methods Menu Function
void menu2();										   // Preemtive Mode Menu Function
void menu3();										   // Show Results Menu Function
void menu4();										   // End Program Menu Functions
void tq_menu();										   // Asking the user for time quantum if RR method is selected
void write_input_to_LL(char *);						   // Reading the input file and then writing it to LL
int total_burst_time(struct node *);				   // Getting total burst time of the input file
char *fcfs();										   // FirstComeFirstServe Function
char *sjf_np();										   // Shortest-Job-First Non-Preemtive
char *sjf_p();										   // Shortest-Job-First Preemtive
char *ps_np();										   // Priority Scheduling Non-Preemtive
char *ps_p();										   // Priority Scheduling Preemtive
char *rr();											   // Round-Robin Scheduling
int process_counter(struct node *);					   // Process counter
struct node *swap_nodes(struct node *, struct node *); // Swap Funcion
void bubble_sort(struct node **, int, char *);		   // Bubble Sort (AT/PID/SJF/PS)
bool is_all_done(struct node *);					   // Checking if all the processes are done
bool is_previous_ones_done(struct node *, int);		   // Checking if previous processes are terminated
struct node *find_in_cpu(struct node *);			   // Finding the node which is in the CPU
struct node *find_least_left(struct node *, int);	   // Finding the node which has the least time left
struct node *find_least_priority(struct node *, int);  // Finding the node which has the least priority
// Prototypes

int main(int argc, char *argv[])
{
	int options = 0;

	// Here we check if the correct options are used
	while ((options = getopt(argc, argv, "f:o:")) != -1)
	{
		switch (options)
		{
		case 'f':
			input_filename = optarg;
			break;
		case 'o':
			output_filename = optarg;
			break;
		default:
			print_usage();
			break;
		}
	}

	// Here we check if the arguments are passed for options
	if (input_filename == NULL || output_filename == NULL)
	{
		print_usage();
	}

	FILE *finput = fopen(input_filename, "r");
	if (finput == NULL) // Checking if the input file argument exists.
	{
		printf("The argument that you passed as input file does not exists.\n");
		printf("Please check the input file argument and run the program again\n");
		exit(1);
	}
	fclose(finput);

	write_input_to_LL(input_filename);

	menu();

	return 0;
}

// Creating node (Function)
struct node *create_node(int pid, int burst_time, int arrival_time, int priority)
{
	struct node *temp;
	temp = (struct node *)malloc(sizeof(struct node));

	temp->process_id = pid;
	temp->burst_time = burst_time;
	temp->arrival_time = arrival_time;
	temp->priority = priority;
	temp->waiting_time = 0;
	temp->turnaround_time = 0;
	temp->how_much_left = burst_time;
	temp->first_response = 0;
	temp->time_slices = 0;
	temp->last_slice_burst = 0;
	if (temp->burst_time == 0)
		temp->is_terminated = false;
	temp->in_cpu = false;
	temp->next = NULL;

	return temp;
}

// Insert back to the LL (Function)
struct node *insert_back(struct node *header, int id, int burst_time, int arrival_time, int priority)
{
	struct node *temp = create_node(id, burst_time, arrival_time, priority);
	struct node *header_temp;

	// Check if the linked list is empty
	if (header == NULL)
	{
		header = temp;
		return header;
	}

	header_temp = header;
	while (header_temp->next != NULL) // Iterate until we reach the last node
		header_temp = header_temp->next;

	header_temp->next = temp;
	return header;
}

// Delete front of the LL (Function)
struct node *delete_front(struct node *header)
{
	struct node *temp;

	if (header == NULL)
	{
		return header;
	}

	temp = header;
	header = header->next;
	free(temp);
	return header;
}

// Displaying the Linked List Items(For Debugging Purposes Only) (Function)
void display_LL(struct node *header)
{
	struct node *temp = header;
	while (temp != NULL)
	{
		int a, b, c, d, e, f, g, h, i, j;
		bool t;
		a = temp->process_id;
		b = temp->burst_time;
		c = temp->arrival_time;
		d = temp->priority;
		e = temp->waiting_time;
		f = temp->turnaround_time;
		g = temp->how_much_left;
		h = temp->first_response;
		i = temp->time_slices;
		j = temp->last_slice_burst;

		printf("ID:%d\tBurst:%d\tArrival:%d\tPriority:%d\tWait:%d\tTurn:%d\tLeft:%d\tResponse:%d\tSlices:%d\tLastSlice:%d\n", a, b, c, d, e, f, g, h, i, j);
		temp = temp->next;
	}

	getchar();
	getchar();
}

// Cloning Main LL (Function)
struct node *clone_LL(struct node *header)
{
	struct node *header_temp = header;
	struct node *clone_header = NULL;

	while (header_temp != NULL)
	{
		int pid = 0, burst = 0, arrival = 0, priority = 0;
		pid = header_temp->process_id;
		burst = header_temp->burst_time;
		arrival = header_temp->arrival_time;
		priority = header_temp->priority;
		clone_header = insert_back(clone_header, pid, burst, arrival, priority);

		header_temp = header_temp->next;
	}

	return clone_header;
}

// This funtions is used to print programs usage and what arguments are needed to pass (Function)
void print_usage()
{
	printf("Usage: cpuScheduler -f <*.txt> -o <*.txt>\n");
	exit(1);
}

// Main Menu (Function)
void menu()
{
	while (true)
	{
		system("clear");
		int option;
		printf("\t\tCPU Scheduler Simulator\n");
		switch (method)
		{
		case 0:
			printf("1-) Scheduling Method (NONE)\n");
			break;
		case 1:
			printf("1-) Scheduling Method (FCFS)\n");
			break;
		case 2:
			printf("1-) Scheduling Method (SJF)\n");
			break;
		case 3:
			printf("1-) Scheduling Method (PS)\n");
			break;
		case 4:
			printf("1-) Scheduling Method (RR)\n");
			break;
		default:
			printf("1-) Scheduling Method (NONE)\n");
			break;
		}

		switch (mode)
		{
		case 0:
			printf("2-) Preemtive Mode (OFF)\n");
			break;
		case 1:
			printf("2-) Preemtive Mode (ON)\n");
			break;
		default:
			printf("2-) Preemtive Mode (OFF)\n");
			break;
		}

		printf("3-) Show Result\n");
		printf("4-) End Program\n");
		printf("Option > ");
		scanf("%1d", &option);

		// Will implement a switch syntax here
		switch (option)
		{
		case 1:
			menu1();
			break;
		case 2:
			menu2();
			break;
		case 3:
			menu3();
			break;
		case 4:
			menu4();
			break;
		default:
			printf("Please select a valid option!\n");
			system("sleep 2");
			break;
		}
	}
}

// Scheduling Methods Menu (Function)
void menu1()
{
	system("clear");
	int option = 0;
	// Modifying Menu according to if preemtive mode is on
	if (mode == 1)
	{
		printf("1-) None: None of scheduling method chosen\n");
		printf("2-) Shortest-Job-First Scheduling\n");
		printf("3-) Priority Scheduling\n");
		printf("Option > ");
		scanf("%1d", &option);

		switch (option)
		{
		case 1:
			method = NONE;
			break;
		case 2:
			method = SJF;
			break;
		case 3:
			method = PS;
			break;
		default:
			printf("Please select a valid option\n");
			system("sleep 1");
			break;
		}
	}
	else
	{
		printf("1-) None: None of scheduling method chosen\n");
		printf("2-) First Come, First Served Scheduling\n");
		printf("3-) Shortest-Job-First Scheduling\n");
		printf("4-) Priority Scheduling\n");
		printf("5-) Round-Robin Scheduling\n");
		printf("Option > ");
		scanf("%1d", &option);
		switch (option)
		{
		case 1:
			method = NONE;
			break;
		case 2:
			method = FCFS;
			break;
		case 3:
			method = SJF;
			break;
		case 4:
			method = PS;
			break;
		case 5:
			method = RR;
			tq_menu();
			break;
		default:
			printf("Please select a valid option\n");
			system("sleep 1");
			break;
		}
	}
}

// Preemtive Mode Menu (Function)
void menu2()
{
	system("clear");
	int option = 0;
	if (method == 0) // If any scheduling method is not selected this menu will be shown
	{
		printf("(Scheduling Methods Menu will be modified according to your preemtive mode choice.)\n");
		printf("Please! Next time when you are choosing preemtive mode, ");
		printf("Firstly select the scheduling method first.\n");
		printf("1-) OFF\n");
		printf("2-) ON\n");
		printf("Option > ");
		scanf("%1d", &option);
		switch (option)
		{
		case 1:
			mode = OFF;
			break;
		case 2:
			mode = ON;
			break;
		default:
			printf("Please select a valid option\n");
			system("sleep 1");
			break;
		}
	}
	else if (method == 2 || method == 3) // If SJF or PS is selected this menu will be shown
	{
		printf("1-) OFF\n");
		printf("2-) ON\n");
		printf("Option > ");
		scanf("%1d", &option);
		switch (option)
		{
		case 1:
			mode = OFF;
			break;
		case 2:
			mode = ON;
			break;
		default:
			printf("Please select a valid option\n");
			system("sleep 1");
			break;
		}
	}
	else // If FCFS or RR is selected user will be informed that preemtive mode is not available for those methods
	{
		printf("Preemtive mode is not available for selected Scheduling Method\n");
		system("sleep 1");
	}
}

// Show Result Menu (Function)
void menu3()
{
	char buffer[500];
	switch (method)
	{
	case 1:
		strcpy(buffer, fcfs());
		printf("%s", buffer);
		printf("Press Enter to return to the main menu.\n");
		getchar();
		getchar();
		break;

	case 2:
		if (mode == 0)
		{
			strcpy(buffer, sjf_np());
			printf("%s", buffer);
			printf("Press Enter to return to the main menu.\n");
			getchar();
			getchar();
		}
		else
		{
			strcpy(buffer, sjf_p());
			printf("%s", buffer);
			printf("Press Enter to return to the main menu.\n");
			getchar();
			getchar();
		}
		break;

	case 3:
		if (mode == 0)
		{
			strcpy(buffer, ps_np());
			printf("%s", buffer);
			printf("Press Enter to return to the main menu.\n");
			getchar();
			getchar();
		}
		else
		{
			strcpy(buffer, ps_p());
			printf("%s", buffer);
			printf("Press Enter to return to the main menu.\n");
			getchar();
			getchar();
		}
		break;

	case 4:
		strcpy(buffer, rr());
		printf("%s", buffer);
		printf("Press Enter to return to the main menu.\n");
		getchar();
		getchar();
		break;
	}
}

// End Program Menu (Function)
void menu4()
{
	if (time_quantum == 0)
		tq_menu();
	strcat(buffer_output, fcfs());
	strcat(buffer_output, sjf_np());
	strcat(buffer_output, sjf_p());
	strcat(buffer_output, ps_np());
	strcat(buffer_output, rr());

	printf("%s", buffer_output);
	FILE *f = fopen(output_filename, "w");
	fprintf(f, "%s", buffer_output);
	fclose(f);

	exit(0);
}

// Time Quantum Asking Menu (Function)
void tq_menu()
{
	while (true)
	{
		system("clear");
		printf("Please enter the time quantum for Round-Robin Method\n");
		printf("Time Quantum > ");
		scanf("%d", &time_quantum);
		break;
	}
}

// Reading from Input File to Write it to LL (Function)
void write_input_to_LL(char *input_filename)
{
	FILE *finput = fopen(input_filename, "r");
	int id_counter = 1;
	if (feof(finput))
	{
		printf("The input file is empty\n");
		exit(1);
	}
	else
	{
		while (!feof(finput)) // Reading the input file and recording the values to Linked List
		{
			int a, b, c;
			fscanf(finput, "%d:%d:%d\n", &a, &b, &c);
			header_original = insert_back(header_original, id_counter, a, b, c);
			id_counter++;
		}
	}
	fclose(finput);
}

// Getting Total Burst Time (Function)
int total_burst_time(struct node *header)
{
	struct node *temp = header;
	int ret = 0;
	if (temp == NULL)
	{
		return ret;
	}

	while (temp != NULL)
	{
		ret += temp->burst_time;
		temp = temp->next;
	}

	return ret;
}

// First-Come-First-Serve (Function)
char *fcfs()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp1, *temp2;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	bool is_first = true;
	bubble_sort(&clone_header, number_of_process, "AT");
	temp1 = clone_LL(clone_header);
	temp2 = temp1;

	while (temp1 != NULL)
	{
		if (temp1->arrival_time <= program_counter)
		{
			program_counter += temp1->burst_time;
			temp1->turnaround_time = program_counter;
			if (is_first)
			{
				if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time) < 0)
					temp1->waiting_time = 0;
				is_first = false;
			}
			else
			{
				if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time) < 0)
					temp1->waiting_time = 0;
			}
		}

		else
		{
			program_counter = temp1->arrival_time;
			program_counter += temp1->burst_time;
			temp1->turnaround_time = program_counter;
			if ((temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time) < 0)
				temp1->waiting_time = 0;
		}

		temp1 = temp1->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp2, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: First Come First Served\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp2 != NULL)
	{
		int pid = temp2->process_id;
		int wait = temp2->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp2 = temp2->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Shortes-Job-First Non-Preemtive (Function)
char *sjf_np()
{
	// I have first tried selection sort but could not figure it out...
	//...(There were complications regarding to non-adjacent nodes)
	// So I changed the sorting algorithm to bubble sort
	struct node *clone_header = clone_LL(header_original);
	struct node *temp;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	bool is_first = true;
	bubble_sort(&clone_header, number_of_process, "AT");
	bubble_sort(&clone_header, number_of_process, "SJF");
	temp = clone_LL(clone_header);
	struct node *temp1 = temp;

	while (temp != NULL)
	{
		if (temp->arrival_time <= program_counter)
		{
			program_counter += temp->burst_time;
			temp->turnaround_time = program_counter;
			if (is_first)
			{
				if ((temp->waiting_time = temp->turnaround_time - temp->burst_time) < 0)
					temp->waiting_time = 0;
				is_first = false;
			}
			else
			{
				if ((temp->waiting_time = temp->turnaround_time - temp->burst_time - temp->arrival_time) < 0)
					temp->waiting_time = 0;
			}
		}

		else
		{
			program_counter = temp->arrival_time;
			program_counter += temp->burst_time;
			temp->turnaround_time = program_counter;
			if ((temp->waiting_time = temp->turnaround_time - temp->burst_time - temp->arrival_time) < 0)
				temp->waiting_time = 0;
		}

		temp = temp->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp1, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: Shortest Job First (Non-Preemtive)\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp1 != NULL)
	{
		int pid = temp1->process_id;
		int wait = temp1->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp1 = temp1->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Shortest-Job-First Preemtive (Function)
char *sjf_p()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp, *temp1, *temp2;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	int total_time = total_burst_time(clone_header);
	bubble_sort(&clone_header, number_of_process, "AT");
	bubble_sort(&clone_header, number_of_process, "SJF");
	temp = temp1 = temp2 = clone_header;

	while (!is_all_done(clone_header))
	{
		struct node *in_cpu_node = find_least_left(clone_header, program_counter);
		bool is_found = true;
		if (in_cpu_node == NULL)
		{
			temp = clone_header;
			while (temp != NULL)
			{
				if (!temp->is_terminated)
				{
					if (temp->arrival_time > program_counter && is_found)
					{
						is_found = false;
						program_counter = temp->arrival_time;
						in_cpu_node = find_least_left(clone_header, program_counter);
						in_cpu_node->how_much_left--;
						program_counter++;
						if (in_cpu_node->how_much_left == 0)
						{
							in_cpu_node->turnaround_time =program_counter;
							in_cpu_node->is_terminated = true;
						}
					}
				}

				temp = temp->next;
			}
		}
		else
		{
			program_counter++;
			in_cpu_node->how_much_left--;

			if (in_cpu_node->how_much_left == 0)
			{
				in_cpu_node->turnaround_time = program_counter;
				in_cpu_node->is_terminated = true;
			}
		}
	}

	bool is_first = true;
	while (temp1 != NULL)
	{
		if (is_first)
		{
			temp1->waiting_time = temp1->turnaround_time - temp1->burst_time;
			if (temp1->waiting_time < 0)
				temp1->waiting_time = 0;

			is_first = false;
		}

		else
		{
			temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time;
			if (temp1->waiting_time < 0)
				temp1->waiting_time = 0;
		}

		temp1 = temp1->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp2, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: Shortest Job First Scheduling (Preemtive)\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp2 != NULL)
	{
		int pid = temp2->process_id;
		int wait = temp2->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp2 = temp2->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Priority Scheduling Non-Preemtive (Function)
char *ps_np()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp, *temp1;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	bubble_sort(&clone_header, number_of_process, "AT");
	bubble_sort(&clone_header, number_of_process, "PS");
	temp = clone_LL(clone_header);
	temp1 = temp;
	bool is_first = true;
	while (temp != NULL)
	{
		if (temp->arrival_time <= program_counter)
		{
			program_counter += temp->burst_time;
			temp->turnaround_time = program_counter;
			if (is_first)
			{
				if ((temp->waiting_time = temp->turnaround_time - temp->burst_time) < 0)
					temp->waiting_time = 0;
				is_first = false;
			}
			else
			{
				if ((temp->waiting_time = temp->turnaround_time - temp->burst_time - temp->arrival_time) < 0)
					temp->waiting_time = 0;
			}
		}

		else
		{
			program_counter = temp->arrival_time;
			program_counter += temp->burst_time;
			temp->turnaround_time = program_counter;
			if ((temp->waiting_time = temp->turnaround_time - temp->burst_time - temp->arrival_time) < 0)
				temp->waiting_time = 0;
		}

		temp = temp->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp1, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: Priority Scheduling (Non-Preemtive)\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp1 != NULL)
	{
		int pid = temp1->process_id;
		int wait = temp1->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp1 = temp1->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Priority Scheduling Preemtive (Function)
char *ps_p()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp, *temp1;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	int total_time = total_burst_time(clone_header);
	bubble_sort(&clone_header, number_of_process, "AT");
	bubble_sort(&clone_header, number_of_process, "PS");
	temp = clone_LL(clone_header);
	temp1 = temp;

	temp->in_cpu = true;
	while (!is_all_done(temp))
	{
		struct node *temp2 = temp;
		struct node *in_cpu_node = find_in_cpu(temp);

		in_cpu_node->how_much_left--;
		program_counter++;

		if (in_cpu_node->how_much_left > 0)
		{
			in_cpu_node->in_cpu = false;
			in_cpu_node = find_least_priority(temp, program_counter);
			if (in_cpu_node == NULL)
			{
				while (temp2 != NULL)
				{
					if (!temp2->is_terminated)
					{
						if (temp2->arrival_time > program_counter)
						{
							program_counter = temp2->arrival_time;
							in_cpu_node = find_least_priority(temp, program_counter);
							in_cpu_node->in_cpu = true;
							break;
						}
					}
					temp2 = temp2->next;
				}
			}
			else
			{
				in_cpu_node->in_cpu = true;
			}
		}

		else if (in_cpu_node->how_much_left == 0)
		{
			in_cpu_node->turnaround_time = program_counter;
			in_cpu_node->is_terminated = true;
			in_cpu_node->in_cpu = false;
			in_cpu_node = find_least_priority(temp, program_counter);
			if (in_cpu_node == NULL)
			{
				while (temp2 != NULL)
				{
					if (!temp2->is_terminated)
					{
						if (temp2->arrival_time > program_counter)
						{
							program_counter = temp2->arrival_time;
							in_cpu_node = find_least_priority(temp, program_counter);
							in_cpu_node->in_cpu = true;
							break;
						}
					}
					temp2 = temp2->next;
				}
			}
			else
			{
				in_cpu_node->in_cpu = true;
			}
		}
	}

	bool is_first = true;
	while (temp != NULL)
	{
		if (is_first)
		{
			temp->waiting_time = temp->turnaround_time - temp->burst_time;
			if (temp->waiting_time < 0)
				temp->waiting_time = 0;

			is_first = false;
		}

		else
		{
			temp->waiting_time = temp->turnaround_time - temp->burst_time - temp->arrival_time;
			if (temp->waiting_time < 0)
				temp->waiting_time = 0;
		}

		temp = temp->next;
	}

	strcpy(buff, "");
	bubble_sort(&temp1, number_of_process, "PID");
	system("clear");
	strcat(buff, "Scheduling Method: Priority Scheduling (Preemtive)\n");
	strcat(buff, "Process Waiting Times:\n");
	while (temp1 != NULL)
	{
		int pid = temp1->process_id;
		int wait = temp1->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp1 = temp1->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Round-Robin Scheduling (Function)
char *rr()
{
	struct node *clone_header = clone_LL(header_original);
	struct node *temp1, *temp2, *temp3;
	int program_counter = 0;
	float average_wait = 0.0f;
	int number_of_process = process_counter(clone_header);
	int total_time = total_burst_time(clone_header);
	bool is_first = true;
	bool previous_ones_done = false;
	bubble_sort(&clone_header, number_of_process, "AT");
	temp1 = temp2 = temp3 = clone_header;

	while (temp3 != NULL)
	{
		temp3->time_slices = temp3->burst_time / time_quantum;
		temp3->last_slice_burst = temp3->burst_time % time_quantum;
		temp3 = temp3->next;
	}

	while (!is_all_done(clone_header))
	{
		temp1 = clone_header;
		is_first = true;
		while (temp1 != NULL)
		{
			if (!temp1->is_terminated)
			{
				if (temp1->arrival_time <= program_counter)
				{
					if (is_first)
					{
						if (temp1->time_slices == 0)
						{
							program_counter += temp1->last_slice_burst;
							if (temp1->last_slice_burst != 0)
								temp1->turnaround_time = program_counter;
							temp1->waiting_time = temp1->turnaround_time - temp1->burst_time;
							if (temp1->waiting_time < 0)
								temp1->waiting_time = 0;
							temp1->is_terminated = true;
						}
						else
						{
							program_counter += time_quantum;
							temp1->turnaround_time = program_counter;
							temp1->time_slices--;
						}
						is_first = false;
					}

					else
					{
						if (temp1->time_slices == 0)
						{
							program_counter += temp1->last_slice_burst;
							if (temp1->last_slice_burst != 0)
								temp1->turnaround_time = program_counter;
							temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time;
							if (temp1->waiting_time < 0)
								temp1->waiting_time = 0;
							temp1->is_terminated = true;
						}
						else
						{
							program_counter += time_quantum;
							temp1->turnaround_time = program_counter;
							temp1->time_slices--;
						}
					}
				}

				else
				{
					previous_ones_done = is_previous_ones_done(clone_header, program_counter);
					if (previous_ones_done)
					{
						program_counter = temp1->arrival_time;
						if (temp1->time_slices == 0)
						{
							program_counter += temp1->last_slice_burst;
							if (temp1->last_slice_burst != 0)
								temp1->turnaround_time = program_counter;
							temp1->waiting_time = temp1->turnaround_time - temp1->burst_time - temp1->arrival_time;
							if (temp1->waiting_time < 0)
								temp1->waiting_time = 0;
							temp1->is_terminated = true;
						}
						else
						{
							program_counter += time_quantum;
							temp1->turnaround_time = program_counter;
							temp1->time_slices--;
						}
					}
				}
			}

			is_first = false;
			temp1 = temp1->next;
		}
	}

	strcpy(buff, "");
	bubble_sort(&clone_header, number_of_process, "PID");
	system("clear");
	snprintf(buff, 999, "Scheduling Method: Round-Robin (Time quantum: %d)\n", time_quantum);
	strcat(buff, "Process Waiting Times:\n");
	while (temp2 != NULL)
	{
		int pid = temp2->process_id;
		int wait = temp2->waiting_time;
		average_wait += wait;
		char buff_1[20];
		snprintf(buff_1, 19, "PS%d: %d ms\n", pid, wait);
		strcat(buff, buff_1);
		temp2 = temp2->next;
	}
	average_wait /= number_of_process;
	char buff_2[40];
	snprintf(buff_2, 39, "Average Waiting Time: %.3f ms\n\n", average_wait);
	strcat(buff, buff_2);
	return buff;
}

// Counts How many process' are in the LL (Function)
int process_counter(struct node *header)
{
	struct node *temp = header;
	int counter = 0;
	while (temp != NULL)
	{
		counter++;
		temp = temp->next;
	}

	return counter;
}

// Swapping nodes (Function)
struct node *swap_nodes(struct node *temp1, struct node *temp2)
{
	struct node *tmp = temp2->next;
	temp2->next = temp1;
	temp1->next = tmp;
	return temp2;
}

// Sorts LL in ascending order (Function)
void bubble_sort(struct node **header, int counter, char *sort_mode)
{
	struct node **header_temp;
	int swapped, max_at = 0;
	int i, j;

	for (i = 0; i < counter; i++)
	{
		header_temp = header;
		swapped = 0;
		max_at = 0;

		for (j = 0; j < counter - 1 - i; j++)
		{
			struct node *temp1 = *header_temp;
			struct node *temp2 = temp1->next;

			if (!strcmp(sort_mode, "PID"))
			{
				if (temp1->process_id >= temp2->process_id)
				{
					*header_temp = swap_nodes(temp1, temp2);
					swapped = 1;
				}
				header_temp = &(*header_temp)->next; // Setting the header_temp's addres to the address of next node which is in the header_temp's address
			}

			else if (!strcmp(sort_mode, "AT"))
			{
				if (temp1->arrival_time > temp2->arrival_time)
				{
					*header_temp = swap_nodes(temp1, temp2);
					swapped = 1;
				}

				else if (temp1->arrival_time == temp2->arrival_time)
				{
					if (temp1->process_id > temp2->process_id)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}
				}
				header_temp = &(*header_temp)->next;
			}

			else if (!strcmp(sort_mode, "SJF"))
			{
				if (temp1->arrival_time <= max_at && temp2->arrival_time <= max_at)
				{
					if (temp1->burst_time > temp2->burst_time)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}

					else if (temp1->burst_time == temp2->burst_time)
					{
						if (temp1->process_id > temp2->process_id)
						{
							*header_temp = swap_nodes(temp1, temp2);
							swapped = 1;
						}
					}
					max_at += (*header_temp)->burst_time;
				}
				else
				{
					if (temp2->arrival_time > max_at)
						max_at = temp2->arrival_time;
				}

				header_temp = &(*header_temp)->next;
			}

			else if (!strcmp(sort_mode, "PS"))
			{
				if (temp1->arrival_time <= max_at && temp2->arrival_time <= max_at)
				{
					if (temp1->priority > temp2->priority)
					{
						*header_temp = swap_nodes(temp1, temp2);
						swapped = 1;
					}

					else if (temp1->priority == temp2->priority)
					{
						if (temp1->process_id > temp2->process_id)
						{
							*header_temp = swap_nodes(temp1, temp2);
							swapped = 1;
						}
					}
					max_at += (*header_temp)->burst_time;
				}
				else
				{
					if (temp2->arrival_time > max_at)
						max_at = temp2->arrival_time;
				}

				header_temp = &(*header_temp)->next;
			}
		}

		if (swapped == 0)
		{
			break;
		}
	}
}

// Checking if all the processes are done returning true if all done (Function)
bool is_all_done(struct node *header)
{
	bool done = true;
	while (header != NULL)
	{
		if (!header->is_terminated)
			done = false;
		header = header->next;
	}

	return done;
}

// Checking if all the processes before arrival time is done (Function)
bool is_previous_ones_done(struct node *header, int at_limit)
{
	bool done = true;
	while (header != NULL)
	{
		if (header->arrival_time <= at_limit)
		{
			if (!header->is_terminated)
			{
				done = false;
			}
		}
		header = header->next;
	}

	return done;
}

// Finding the node which is in the CPU
struct node *find_in_cpu(struct node *header)
{
	while (header != NULL)
	{
		if (!header->is_terminated)
		{
			if (header->in_cpu == true)
				return header;
		}
		else
			header = header->next;
	}
}

// Finding the node which has the least time left
struct node *find_least_left(struct node *header, int at_limit)
{
	struct node *temp = NULL;
	int x = INT_MAX;
	while (header != NULL)
	{
		if (!header->is_terminated)
		{
			if (header->arrival_time <= at_limit)
			{
				if (header->how_much_left < x)
				{
					temp = header;
					x = header->how_much_left;
				}
			}
		}
		header = header->next;
	}

	return temp;
}

// Finding the node which has the least priority
struct node *find_least_priority(struct node *header, int at_limit)
{
	struct node *temp = NULL;
	int x = INT_MAX;
	while (header != NULL)
	{
		if (!header->is_terminated)
		{
			if (header->arrival_time <= at_limit)
			{
				if (header->priority < x)
				{
					temp = header;
					x = header->how_much_left;
				}
			}
		}
		header = header->next;
	}

	return temp;
}