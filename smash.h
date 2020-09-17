


void executeCommand(char* str);


int executeExternalCommand(int argc,char* argv[],int numPipes,int hasIn,int hasOut, char* inFile, char* outFile);


void getArgs(char* args[], int argc, char* argv[], int numPipes);


void signalHandler(int sigNumber);
