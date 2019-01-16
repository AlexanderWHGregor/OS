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

	switch (mode) {
	case 0: 
	{

		j = dequeue(jobs);
		break;

	}

	case 1:
	{

		j = pop(jobs);
		break;

	}

	case 2:
	{

		j = dequeue(jobs);
		job_t *temp;
		//d_linked_list_t *temp_list = init_d_linked_list();

		//for (int i = 0; i < jobs->size; i++){
   		while (jobs->size > 0) {

			temp = dequeue(jobs);

    			if (j == NULL) {
				//printf("\nmode");
				//enqueue(jobs, j);
				enqueue(jobs, temp);
				//j == temp;

    			}

			else {
				//printf("\nmode2");
      				if(j->required_time > temp->required_time){

					//enqueue(jobs, temp);
					enqueue(jobs, j);
        				j = temp;

      				}

    			}

  		}

		//jobs = temp_list;

		break;

	}

	case 3:
	{

		j = dequeue(jobs);
		break;

	}

	default:
	{

		fprintf(stderr, "invalid mode");
		exit(EXIT_FAILURE);

	}
	}

	return j;

}
