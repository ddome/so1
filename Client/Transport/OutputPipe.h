#ifndef __OUTPUT_FIFO_H__
#define __OUTPUT_FIFO_H__

#define _FIFO_MODE_ 0666

#define _DEFAULT_PATH_ "/tmp/comm/PromptReaderClient"
#define MAX_PROMPT_MSG 150

int InitPromptCommunication(pid_t pid);

/* Mensajes de error, menores a 150 */
int WritePrompt(char * msg);

char * ReadMessage(void);

#endif









