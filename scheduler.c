/******************************************************************************
*
* Name: 	Zaid Albirawi
* Email: 	zalbiraw@uwo.ca
*
* scheduler.c
*
******************************************************************************/

#include "scheduler.h"

job_t *get_next_job(int mode, d_linked_list_t* jobs) {

	job_t *j;

	/*
	 * first come first serve
	 */
	if (mode == FCFS) {

		j = dequeue(jobs);

	}

	/*
	 * last in first out
	 */
	else if (mode == LIFO) {

		j = pop(jobs);

	}

	/*
	 * second job first
	 */
	else if (mode == SJF) {

		j = dequeue(jobs);
		int jobSize = jobs->size;
		job_t *tempVal;

   		while (jobSize > 0) {

			jobSize--;

			tempVal = dequeue(jobs);

    			if (tempVal->required_time < j->required_time) {
				enqueue(jobs, j);
				j = tempVal;

    			}

			else {

				enqueue(jobs, tempVal);

    			}

  		}

	}

	/*
	 * round robin
	 */
	else {

		j = dequeue(jobs);

	}

	return j;

}
