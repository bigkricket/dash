#pragma once

// Starts the CAN reader task, which listens for and parses OBD2 responses.
void can_reader_task(void *arg);
