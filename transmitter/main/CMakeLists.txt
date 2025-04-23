idf_component_register(
	SRCS "main.c"
	     "can_interface.c"
	     "can_reader.c"
		 "obd2_pid_scanner.c"
		 "obd2_writer.c"
        INCLUDE_DIRS "."
)
