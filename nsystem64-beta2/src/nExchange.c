/**
ENUNCIADO
t1 -> nExchange(t2,m1,dt1)
	t1 se bloquea hasta que
		1. t2 llama nExchange(t1,m2,dt2)
			-> t1 retorna m2
			!   a. t2 retorna m1 inmediatamente
				b. t2 espera bloqueado la llamada de t1 o el timeout de dt2
		2. tiempo de espera supera a dt1
			-> t1 retorna NULL
		3. si dt1 = -1, t1 espera el nEchange de t2 indefinidamente
**/

#include "nSysimp.h"
#include "nSystem.h"
#include "fifoqueues.h"

void * nExchange(nTask t, void *msg, int timeout){
 	void *msg; //mensage a retornar
  	nTask send_task;
 	START_CRITICAL();
 	{
 		nTask this_task= current_task;
	 	if (t->status == WAIT_SEND && timeout == -1){
	 			//esperar hasta recibir nEchange  de t
	 		// recibir mensaje del nExchange de t
	 		send_task= GetTask(this_task->send_queue);
	    	msg= send_task==NULL ? NULL : send_task->send.msg;
	 	}
	 	else if (timeout != -1  && (t->status == WAIT_SEND || t->status == WAIT_SEND_TIMEOUT)){
	 		//esperamos el nExchange de t hacia nosotros
	 		// o timeout
	 		if (t->status == WAIT_SEND_TIMEOUT)
	 			CancelTask(t);
	 		// recibir mensaje del nExchange de t
	 		send_task= GetTask(this_task->send_queue);
	    	msg= send_task==NULL ? NULL : send_task->send.msg;
		}
	}
}



int nSend(nTask task, void *msg)
{
  int rc;

  START_CRITICAL();
  pending_sends++;
  { nTask this_task= current_task;

    if (task->status==WAIT_SEND || task->status==WAIT_SEND_TIMEOUT)
    {
      if (task->status==WAIT_SEND_TIMEOUT)
        CancelTask(task);
      task->status= READY;
      PushTask(ready_queue, task); /* En primer lugar en la cola */
    }
    else if (task->status==ZOMBIE)
      nFatalError("nSend", "El receptor es un ``zombie''\n");

    /* En nReply se coloca ``this_task'' en la cola de tareas ready */
    PutTask(task->send_queue, this_task);
    this_task->send.msg= msg;
    this_task->status= WAIT_REPLY;
    ResumeNextReadyTask();

    rc= this_task->send.rc;
  }
  pending_sends--;
  END_CRITICAL();

  return rc;
}

void *nReceive(nTask *ptask, int timeout)
{
  void *msg;
  nTask send_task;

  START_CRITICAL();
  pending_receives++;
  { nTask this_task= current_task;

    if (EmptyQueue(this_task->send_queue) && timeout!=0)
    {
      if (timeout>0)
      {
        this_task->status= WAIT_SEND_TIMEOUT;
        ProgramTask(timeout);
        /* La tarea se despertara automaticamente despues de timeout */
      }
      else this_task->status= WAIT_SEND; /* La tarea espera indefinidamente */

      ResumeNextReadyTask(); /* Se suspende indefinidamente hasta un nSend */
    }

    send_task= GetTask(this_task->send_queue);
    if (ptask!=NULL) *ptask= send_task;
    msg= send_task==NULL ? NULL : send_task->send.msg;
  }
  pending_receives--;
  END_CRITICAL();

  return msg;
}    
