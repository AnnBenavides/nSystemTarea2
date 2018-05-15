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
#include "fifoqueues.h"
#include <stdio.h>

void * nExchange(nTask t, void *msg, int timeout){

 	START_CRITICAL();
 	{
 		nTask this_task= current_task;
 		// completamos la estructura task con t y msg 
 		this_task->ex_task = t;
 		this_task->ex_msg = msg;
	 	if (t->status == WAIT_EXHANGE || t->status == WAIT_EXCHANGE_TIMEOUT){
	 		// caso 1.a. t estaba esperando este nExchange, las desbloqueo
	 		if (this_task->ex_waiting){
	 			t->ex_waiting = FALSE;
	 			this_task->ex_waiting = FALSE;
	 		}
	 		if (t->status == WAIT_EXCHANGE_TIMEOUT)
	 			CancelTask(t);
	 		t->status= READY;
      		PushTask(ready_queue, t); /* En primer lugar en la cola */
      		/* En nExchange se coloca ``t'' en la cola de tareas fifo */
	    	PutObj(t->ready_fifo, t);
	    	END_CRITICAL();
	    	return t->ex_msg;
		} else {
			// se llama a nExchange por primera vez
			// programar timeout y bloqueo
			if (timeout > 0){
				this-task->ex_waiting = TRUE;
				this_task->status = WAIT_EXCHANGE_TIMEOUT;
				ProgramTask(timeout);
				/* La tarea se despertara automaticamente despues de timeout */		
			} else 
				this_task->status= WAIT_EXCHANGE; /* La tarea espera indefinidamente */
			PutObj(t->ready_fifo, t); 
			ResumeNextReadyTask(); /* Se suspende indefinidamente hasta un nExchange correspondiente */
		}

		if (t->ex_waiting){
			END_CRITICAL();
			return NULL;
		}

		nTask send_task= GetObj(t->ready_fifo); // ultimo task de la cola
		// casos de retorno
	    if (t!=NULL) *t= send_task; 
	    msg= send_task==NULL ? NULL : send_task->ex_msg;
	    END_CRITICAL();
	    return msg;
	}
}