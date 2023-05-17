#ifndef SEM_H
#define SEM_H

#include "synch.h"

class Semaphore;

class Sem {
private:
  char name[50];  // The semaphore name
  Semaphore *sem; // Create semaphore for management
public:
  // Initial the Sem object, the started value is null
  // Remember to initial the Sem to use
  Sem(char *na, int i);
  /*{
      strcpy(this->name, na);
      sem = new Semaphore(this->name, i);
  }*/
  ~Sem(); // Destruct the Sem object
  /*
  {
    if (sem)
      delete sem;
  }
  */
  void wait();
  /*
    {
        sem->P(); // Conduct the waiting function
    }
    */
  void signal();
  /*
  {
    sem->V(); // Release semaphore
  }
  */

  char *GetName(); // Return the semaphore name
                   /*
                    {
                      return this->name;
                    }
                    */
};

#endif