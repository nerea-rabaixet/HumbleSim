/*
	A humble logger
*/

component Logger : public TypeII
{
	public:
		void Start();
		void Stop();

	private:
		FILE *fileTraces;
		FILE *fileResults;

	public:
		char filenameTraces[500] = "traces.txt";
		char filenameResults[500] = "results.csv";
		inport void Trace(char* input);
		inport void Result(char* input);
};

void Logger :: Start()
{
	fileTraces = fopen(filenameTraces, "w"); // To clear the contents
	fclose(fileTraces);
	
	fileResults = fopen(filenameResults, "r");
    if (fileResults != NULL) {  // File exists, open in append mode
        fclose(fileResults);
        fileResults = fopen(filenameResults, "a");
    } else {  // File does not exist, open in write mode and write header
        fileResults = fopen(filenameResults, "w");
        fprintf(fileResults,"Seed,App,ID,PosX,PosY,BeacTX,BeacRX,PingTX,PingRX,DataTX,DataRX,IsolTX,RelayTX\n");
    }
    fclose(fileResults);
};

void Logger :: Stop()
{

};

// Saves a trace in a file
void Logger :: Trace(char* input)
{
	fileTraces = fopen(filenameTraces, "a");
	fprintf(fileTraces,"%s\n", input);
	fclose(fileTraces);
};

// Saves a result in a file
void Logger :: Result(char* input)
{
	fileResults = fopen(filenameResults, "a");
	fprintf(fileResults,"%s\n", input);
	fclose(fileResults);
};
