#include <stdio.h>

uint8  gSmartDebugFlag = 0; 

int main(int argc, char *argv[])
{
    printf("Hello\n");
	
	static struct option long_options[] =
    {
        {"help",                    	no_argument,        NULL,       'h'},
        {"verbosity",               	no_argument,  		NULL,       'v'},
        { NULL, 0, NULL, 0}
    };
    signed char opt;
    int option_index;
    
    while ((opt = getopt_long(argc, argv, "hvDt:", long_options, &option_index)) != -1) 
    {
        switch (opt) 
        {
            case 'h':
                gSmartDebugFlag = 0;
                print_usage_exit(argv);
                break;
            case 'v':
                gSmartDebugFlag = 1;
                break;
            default: /* '?' */
                gSmartDebugFlag = 0;
                print_usage_exit(argv);
        }
    }

}

static void print_usage_exit(char *argv[])
{
    printf("Usage: %s\n", argv[0]);
    printf("  Arguments:\n");
    printf("  Options:\n");
    printf("    -h   --help                         Print this help.\n\r");
    printf("    -v --verbosity                      Verbosity level. Increses amount of debug information. Default close.\n");
    exit(EXIT_FAILURE);
}
