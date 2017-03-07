#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <list.h>
#include <events.h>
#include <pbconsole.h>

enum {
	KeyInputEventTypeId = 47578,
	RandomNumberEventTypeId = 134134,
	CustomEventTypeId = 124090
};

void RandomEventGen_update(void * self, Event * event);
void TestUpdate_update(void * self, Event * event);
void InputManager_update(void * self, Event * event);
void CustomHandler_handleEvent(void * self, Event * event);
void Timer_handleEvent(void * self, Event * event);
void HitCounter_handleEvent(void * self, Event * event);

typedef struct Timer Timer;
struct Timer {
	int id;
	int timeCounter;
};

int main(void) {
	EventSystem_init();

	// add event handlers
	EventSystem_addHandler(HandlerObject_new(NULL, NULL, TestUpdate_update));
	EventSystem_addHandler(HandlerObject_new(NULL, NULL, InputManager_update));
	EventSystem_addHandler(HandlerObject_new(NULL, NULL, RandomEventGen_update));
	int spaceHitCounter = 0;
	EventSystem_addHandler(HandlerObject_new(&spaceHitCounter, NULL, HitCounter_handleEvent));
	Timer timer = {
		.id = 0,
		.timeCounter = 100
	};
	EventSystem_addHandler(HandlerObject_new(&timer, NULL, Timer_handleEvent));
	EventSystem_addHandler(HandlerObject_new(NULL, NULL, CustomHandler_handleEvent));

	// start infinite event loop
	EventSystem_loop();
	EventSystem_deinit();
	return 0;
}

void TestUpdate_update(void * self, Event * event) {
	switch (event->type) {
		case StartEventTypeId: {
			puts("<<<START!>>>");
			break;
		}
		case UpdateEventTypeId: {
			putchar('.');
			break;
		}
	}
}

void InputManager_update(void * self, Event * event) {
	if (conIsKeyDown()) {
		char * keyCode = malloc(sizeof(char));
		*keyCode = getchar();
		EventSystem_raiseEvent(Event_new(self, KeyInputEventTypeId, keyCode, free));
	}
}

void RandomEventGen_update(void * self, Event * event) {
	if (event->type == StartEventTypeId) {
		srand(time(0));
	}
	if (rand() % 33 == 0) {
		int * number = malloc(sizeof(int));
		*number = rand() % 200 - 100;
		EventSystem_raiseEvent(Event_new(self, RandomNumberEventTypeId, number, free));
	}
}

void HitCounter_handleEvent(void * self, Event * event) {
	switch (event->type) {
		case CustomEventTypeId: {
			int * counterPtr = (int *)self;
			(*counterPtr)++;
			printf(">>> Custom event! Counter: %i\n", *counterPtr);
			break;
		}
	}
}

void CustomHandler_handleEvent(void * self, Event * event) {
	switch (event->type) {
		case RandomNumberEventTypeId: {
			int * number = (int *)event->data;
			printf("Random number %i\n", *number);
			break;
		}
		case KeyInputEventTypeId: {
			char * keyCodePtr = (char *)event->data;
			char keyCode = *keyCodePtr;
			if (keyCode == ' ') {
				EventSystem_raiseEvent(Event_new(self, CustomEventTypeId, NULL, NULL));
			}
			if (keyCode == 'a') {
				Timer * timer = malloc(sizeof(Timer));
				timer->id = rand() % 100;
				timer->timeCounter = 50;
				EventSystem_addHandler(HandlerObject_new(timer, free, Timer_handleEvent));
			}
			if (keyCode == 'q') {
				EventSystem_raiseEvent(Event_new(self, ExitEventTypeId, NULL, NULL));
			}
			printf("Key pressed `%c` [%i]\n", keyCode, keyCode);
			break;
		}
	} 
}

void Timer_handleEvent(void * self, Event * event) {
	switch(event->type) {
		case UpdateEventTypeId: {
			Timer * timer = (Timer *)self;
			double elapsedMillis = *(double *)event->data;
			timer->timeCounter -= 1;
			if (timer->timeCounter % 10 == 0) {
				printf("\nTimer [%i]: %i {%.1lf}\n", timer->id, timer->timeCounter, elapsedMillis); 
			}
			if (timer->timeCounter == 0) {
				printf("\nTimer [%i] COMPLETED!\n", timer->id); 
				EventSystem_raiseEvent(Event_new(self, RemoveHandlerEventTypeId, self, NULL));
			}
			break;
		}
	}
}