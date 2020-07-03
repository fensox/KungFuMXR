#pragma once

/* Handles SDL input events and sets internal flags that can be polled by main program loop. */
class Events
{
public:
	// process SDL event queue
	void doEvents();

	// Returns true if quit the application has been chosen
	bool getQuit();

private:
	// hold whether user has chosen to quit the app
	bool mQuit{ false };
};

