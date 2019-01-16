/******************************************************************************
 *
 * Name: 	Zaid Albirawi
 * Email: 	zalbiraw@uwo.ca
 *
 * simulate.c
 *
 ******************************************************************************/

#include "simulate.h"

pthread_mutex_t mLock;
int memory, max_memory, mode, time_quantum;
FILE *fp;
d_linked_list_t *jobs;

void* run(void *j) {

  int required_memory;
  int number;

  /*
   * lock mutex, allocate memory, and unlock mutex
   */
  pthread_mutex_lock(&mLock);
  job_t *job = get_next_job(mode, jobs);
  pthread_mutex_unlock(&mLock);

  while (job != NULL)
  {

    number = job->number;
    required_memory = job->required_memory;

    /**********************************************************************
     * checks if the memory requested exceeds maximum memory
     **********************************************************************/
    if (required_memory > max_memory)
    {
      /******************************************************************
       * inform user that the job won't run, clean and terminate
       ******************************************************************/
      print_exceed_memory(fp, number);
      free(job);
      return NULL;
    }

    /**********************************************************************
     * runs job
     **********************************************************************/
    if (required_memory <= memory) {
	execute_job(job);
    }


    /**********************************************************************
     * checks if the memory requested exceeds current available memory
     **********************************************************************/
    else
    {
      /******************************************************************
       * inform user that the job doesn't have enough resources at the
       * moment, add the job back to the list
       ******************************************************************/
      print_insufficient_memory(fp, number);

      enqueue(jobs, job);

    }

    /*
     * lock mutex, allocate memory, and unlock mutex
     */
    pthread_mutex_lock(&mLock);
    job = get_next_job(mode, jobs);
    pthread_mutex_unlock(&mLock);

  }

  return NULL;

}

void simulate(int memory_value, int mode_value, int time_quantum_value, d_linked_list_t *list) {
  /**************************************************************************
   * opens output file
   **************************************************************************/
  fp = fopen(SYSTEM_OUTPUT, WRITE);

  /**************************************************************************
   * check if the system file is in the directory
   **************************************************************************/
  if (fp == NULL)
  {
    printf("Unable to open %s file\n", SYSTEM_OUTPUT);
    exit(EXIT_FAILURE);
  }

  /**************************************************************************
   * set global variables
   **************************************************************************/
  memory = max_memory = memory_value;

  /**************************************************************************
   * set executing mode
   **************************************************************************/
  mode = mode_value;
  time_quantum = time_quantum_value;
  print_mode(fp, mode);
  jobs = list;

  /*
   * make mutex
   */
  if(pthread_mutex_init(&mLock, NULL) > 0) {
    fprintf(stderr, "mutex issue\n");
    exit(EXIT_FAILURE);
  }

  /**************************************************************************
   * create threads and run jobs
   **************************************************************************/

  pthread_t threads[NUMBER_OF_THREADS];
  for (int i = 0; i < NUMBER_OF_THREADS; ++i)
  {
    if (pthread_create(&threads[i], NULL, run, NULL))
    {
      printf("Error: failed to create thread.\n");
      exit(EXIT_FAILURE);
    }
  }

  /**********************************************************************
   * wait for the jobs to finish executing
   **********************************************************************/
  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  /*
   * destroy mutex
   */
  pthread_mutex_destroy(&mLock);

}

void execute_job(job_t *job) {

  int required_memory = job->required_memory;
  int number = job->number;

  /*
   * FCFS, LIFO, SJF
   */
  if (mode == FCFS || mode == LIFO || mode == SJF) {

    /*
     * lock mutex, allocate memory, and unlock mutex
     */
    pthread_mutex_lock(&mLock);
    print_starting(fp, number);
    allocate_memory(required_memory);
    pthread_mutex_unlock(&mLock);

    /*
     * pause job until other job is finished
     */
    sleep(job->required_time);

    /*
     * lock mutex, allocate memory, and unlock mutex
     */
    pthread_mutex_lock(&mLock);
    print_completed(fp, number);
    free(job);
    deallocate_memory(required_memory);
    pthread_mutex_unlock(&mLock);

  }

  /*
   * RR
   */
  else if (mode == RR) {

    /*
     * lock mutex and allocate memory
     */
    pthread_mutex_lock(&mLock);
    print_starting(fp, number);
    allocate_memory(required_memory);

    if (job->required_time <= time_quantum){

      /*
       * pause job for other jobs to finish, deallocate memory
       */
      sleep(job->required_time);
      print_completed(fp, number);
      free(job);
      deallocate_memory(required_memory);

    }

    else {

      /*
       * pause job for other jobs to finish, deallocate memory
       */
      sleep(time_quantum);
      job->required_time -= time_quantum;
      enqueue(jobs, job);
      deallocate_memory(required_memory);

    }

    /*
     * unlock mutex
     */
    pthread_mutex_unlock(&mLock);

  }

  /*
   * check if improper mode
   */
  else {

    fprintf(stderr, "improper mode input\n");
    exit(EXIT_FAILURE);

  }

}

/*
 * allocate memory
 */
void allocate_memory(int r) {
  memory -= r;
  print_allocate_memory(fp, memory, r);
}

/*
 * deallocate memory
 */
void deallocate_memory(int r) {
  memory += r;
  print_deallocate_memory(fp, memory, r);
}
