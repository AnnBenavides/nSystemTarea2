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
 	void *send_msg; //mensage a retornar
  	nTask send_task;
 	START_CRITICAL();
 	{
 		nTask this_task= current_task;
	 	if (timeout != -1  && (t->status == WAIT_SEND || t->status == WAIT_SEND_TIMEOUT)){
	 		//esperamos el nExchange de t hacia nosotros
	 		// o timeout
	 		if (t->status == WAIT_SEND_TIMEOUT)
	 			CancelTask(t);
	 		// recibir mensaje del nExchange de t
	 		send_task= GetObj(this_task->send_queue);
	    	send_msg= send_task==NULL ? NULL : send_task->send.msg;
		}
		/* En nReply se coloca ``this_task'' en la cola de tareas ready */
	    PutObj(task->send_queue, this_task);
	    this_task->send.msg= msg;
	    if (EmptyFifoQueue(this_task->send_queue) && timeout!=-1){
	      if (timeout>-1){
	        this_task->status= WAIT_SEND_TIMEOUT;
	        ProgramTask(timeout);
	        /* La tarea se despertara automaticamente despues de timeout */
	      }
	      else 
	      	this_task->status= WAIT_SEND; /* La tarea espera indefinidamente */
	      ResumeNextReadyTask(); /* Se suspende indefinidamente hasta un nExchange */
	}
	END_CRITICAL();
	return send_msg;
}