
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#define SENTINEL_KEYWORD					"_VERSION_INFO_H_"
#define DEFINE_KEYWORD						":KEY"
#define MAJOR_VERSION_KEYWORD				"MAJOR_VERSION"
#define MINOR_VERSION_KEYWORD				"MINOR_VERSION"
#define BUILD_NUMBER_KEYWORD				"LAST_BUILD_NUMBER"
#define REVISION_NUMBER_KEYWORD				"REVISION_NUMBER"
#define REVISION_STRING_KEYWORD				"REVISION_STRING"

#define NEVISTA_VERSION_KEYWORD				"GAME_VERSION"
#define FILEVER_KEYWORD						"FILEVER"				// 1,0,0,1
#define PRODUCTVER_KEYWORD					"PRODUCTVER"			// 1,0,0,1
#define STRFILEVER_KEYWORD					"STRFILEVER"			// "1, 0, 0, 1\0"
#define STRPRODUCTVER						"STRPRODUCTVER"			// "1, 0, 0, 1\0"

#define TOOL_NAME							"IncrementBuildNumber"

#define LINE_BUFFER_MAX_LENGTH				1024
#define REVISION_STRING_MAX_LENGTH			5

#define MAJOR_VERSION_MASK					(1 << 0)
#define MINOR_VERSION_MASK					(1 << 1)
#define BUILD_NUMBER_MASK					(1 << 2)
#define REVISION_NUMBER_MASK				(1 << 3)
#define REVISION_STRING_MASK				(1 << 4)


int gnMajorVersionNumber = 0;
int gnMinorVersionNumber = 0;
int gnBuildNumber = 0;
int gnRevisionNumber = 0;
char gstrRevisionString[REVISION_STRING_MAX_LENGTH] = {0};
char gstrLineBuffer[LINE_BUFFER_MAX_LENGTH];



void SkipWhitespace(char** ppLine)
{
	while (**ppLine == '\r' || **ppLine == '\n' || **ppLine == ' ' || **ppLine == '\t' || **ppLine == '/')
	{
		(*ppLine)++;
	}
}


int ReadAndStore(char* pHeaderFilename)
{
	FILE* pFile = fopen(pHeaderFilename, "r");
	if (!pFile)
	{
		printf("  Could not open file \"%s\".");
		return -1;
	}

	char* pLine = NULL;
	int bitStored = 0;

	do
	{
		pLine = fgets(gstrLineBuffer, LINE_BUFFER_MAX_LENGTH, pFile);
		if (pLine)
		{
			//-- skip whitespace
			SkipWhitespace(&pLine);
			
			if (!strncmp(pLine, DEFINE_KEYWORD, strlen(DEFINE_KEYWORD)))
			{
				//-- Now we're talking.
				pLine += strlen(DEFINE_KEYWORD);
				SkipWhitespace(&pLine);

				if (!strncmp(pLine, MAJOR_VERSION_KEYWORD, strlen(MAJOR_VERSION_KEYWORD)))
				{
					pLine += strlen(MAJOR_VERSION_KEYWORD);
					SkipWhitespace(&pLine);
					gnMajorVersionNumber = atoi(pLine);
					bitStored |= MAJOR_VERSION_MASK;
				}
				else if (!strncmp(pLine, MINOR_VERSION_KEYWORD, strlen(MINOR_VERSION_KEYWORD)))
				{
					pLine += strlen(MINOR_VERSION_KEYWORD);
					SkipWhitespace(&pLine);
					gnMinorVersionNumber = atoi(pLine);
					bitStored |= MINOR_VERSION_MASK;
				}
				else if (!strncmp(pLine, BUILD_NUMBER_KEYWORD, strlen(BUILD_NUMBER_KEYWORD)))
				{
					pLine += strlen(BUILD_NUMBER_KEYWORD);
					SkipWhitespace(&pLine);
					gnBuildNumber = atoi(pLine);
					bitStored |= BUILD_NUMBER_MASK;
				}
				else if (!strncmp(pLine, REVISION_NUMBER_KEYWORD, strlen(REVISION_NUMBER_KEYWORD)))
				{
					pLine += strlen(REVISION_NUMBER_KEYWORD);
					SkipWhitespace(&pLine);
					gnRevisionNumber = atoi(pLine);
					bitStored |= REVISION_NUMBER_MASK;
				}
				else if (!strncmp(pLine, REVISION_STRING_KEYWORD, strlen(REVISION_STRING_KEYWORD)))
				{
					pLine += strlen(REVISION_STRING_KEYWORD);
					SkipWhitespace(&pLine);
					strncpy(gstrRevisionString, pLine, REVISION_STRING_MAX_LENGTH - 1);
					gstrRevisionString[REVISION_STRING_MAX_LENGTH - 1] = '\0';
					bitStored |= REVISION_STRING_MASK;

					for (int i = 0; i < REVISION_STRING_MAX_LENGTH; i++)
					{
						if (gstrRevisionString[i] == '\r' || gstrRevisionString[i] == '\n' || gstrRevisionString[i] == ' ' || gstrRevisionString[i] == '\t')
						{
							gstrRevisionString[i] = '\0';
						}
					}
				}
			}
		}
	} while (pLine);

	fclose(pFile);

	if (bitStored == (MAJOR_VERSION_MASK | MINOR_VERSION_MASK | BUILD_NUMBER_MASK | REVISION_NUMBER_MASK | REVISION_STRING_MASK))
	{
		//-- successfully read and stored all values
		return 0;
	}
	else 
	{
		if (!(bitStored & MAJOR_VERSION_MASK))
		{
			printf("  %s %s not found.\n", DEFINE_KEYWORD, MAJOR_VERSION_KEYWORD);
		}
		if (!(bitStored & MINOR_VERSION_MASK))
		{
			printf("  %s %s not found.\n", DEFINE_KEYWORD, MINOR_VERSION_KEYWORD);
		}
		if (!(bitStored & BUILD_NUMBER_MASK))
		{
			printf("  %s %s not found.\n", DEFINE_KEYWORD, BUILD_NUMBER_KEYWORD);
		}
		if (!(bitStored & REVISION_NUMBER_MASK))
		{
			printf("  %s %s not found.\n", DEFINE_KEYWORD, REVISION_NUMBER_KEYWORD);
		}
		if (!(bitStored & REVISION_STRING_MASK))
		{
			printf("  %s %s not found.\n", DEFINE_KEYWORD, REVISION_STRING_KEYWORD);
		}

		return -2;
	}
}


int UpdateAndWrite(char* pHeaderFilename)
{
	FILE* pFile = fopen(pHeaderFilename, "w");
	if (!pFile)
	{
		printf("  Could not open file \"%s\".");
		return -3;
	}

	time_t rawtime;
	time(&rawtime);

	//-- Print some comments.
	//-- Autogen warning + date should do.
	fprintf(pFile, "// THIS FILE IS AUTOMATICALLY GENERATED - DO NOT EDIT.\n");
	fprintf(pFile, "//\n");
	fprintf(pFile, "// (unless you know what you're doing.)\n");
	fprintf(pFile, "//\n");
	fprintf(pFile, "// This file is maintained by the %s tool.\n", TOOL_NAME);
	fprintf(pFile, "// %s, %s and %s may be manually modified.\n", MAJOR_VERSION_KEYWORD, MINOR_VERSION_KEYWORD, REVISION_STRING_KEYWORD);
	fprintf(pFile, "// %s and %s are the only bits controlled by the tool.\n", BUILD_NUMBER_KEYWORD, REVISION_NUMBER_KEYWORD);
	fprintf(pFile, "//\n");
	fprintf(pFile, "// Last update time: %s", ctime(&rawtime));
	fprintf(pFile, "//\n");
	fprintf(pFile, "// These are used to generate game defines.\n");
	fprintf(pFile, "// %s %s \t\t%d\n", DEFINE_KEYWORD, MAJOR_VERSION_KEYWORD, gnMajorVersionNumber);
	fprintf(pFile, "// %s %s \t\t%d\n", DEFINE_KEYWORD, MINOR_VERSION_KEYWORD, gnMinorVersionNumber);
	fprintf(pFile, "// %s %s \t\t%d\n", DEFINE_KEYWORD, BUILD_NUMBER_KEYWORD, gnBuildNumber + 1);
	fprintf(pFile, "// %s %s \t\t%d\n", DEFINE_KEYWORD, REVISION_NUMBER_KEYWORD, gnRevisionNumber);
	fprintf(pFile, "// %s %s \t\t%s\n", DEFINE_KEYWORD, REVISION_STRING_KEYWORD, gstrRevisionString);
	fprintf(pFile, "\n");
	fprintf(pFile, "\n");
	fprintf(pFile, "#ifndef %s\n", SENTINEL_KEYWORD);
	fprintf(pFile, "#define %s\n", SENTINEL_KEYWORD);
	fprintf(pFile, "#pragma once\n");
	fprintf(pFile, "\n");
	fprintf(pFile, "\n");
	fprintf(pFile, "// These are generated from the values above. DO NOT MANUALLY EDIT.\n");

	//-- Human readable string
	fprintf(pFile, "#define %s \t\t\"%d.%d%s (Build %d)\"\n", NEVISTA_VERSION_KEYWORD, gnMajorVersionNumber, gnMinorVersionNumber, gstrRevisionString, gnBuildNumber);

	fprintf(pFile, "#define %s \t\t%d,%d,%d,%d\n", FILEVER_KEYWORD, gnMajorVersionNumber, gnMinorVersionNumber, gnBuildNumber, gnRevisionNumber);

	fprintf(pFile, "#define %s \t\t%d,%d,%d,%d\n", PRODUCTVER_KEYWORD, gnMajorVersionNumber, gnMinorVersionNumber, gnBuildNumber, gnRevisionNumber);

	fprintf(pFile, "#define %s \t\t\"%d, %d, %d, %d\\0\"\n", STRFILEVER_KEYWORD, gnMajorVersionNumber, gnMinorVersionNumber, gnBuildNumber, gnRevisionNumber);

	fprintf(pFile, "#define %s \t\t\"%d, %d, %d, %d\\0\"\n", STRPRODUCTVER, gnMajorVersionNumber, gnMinorVersionNumber, gnBuildNumber, gnRevisionNumber);

	fprintf(pFile, "\n");
	fprintf(pFile, "\n");
	fprintf(pFile, "#endif //%s\n", SENTINEL_KEYWORD);

	fclose(pFile);

	return 0;
}


int main(int argc, char* argv[])
{
	bool bRead = true;

	if (argc != 2 && argc != 3)
	{
		printf("%s:\n", TOOL_NAME);
		printf("  usage: %s [-W] <headerfile>\n", TOOL_NAME);
		printf("\n");
		printf("  -W\t\tCreate or overwrite <headerfile> with initial values instead of updating existing version.\n");
		
		getchar();
		return -1;
	}
	if (argc == 3)
	{
		if (!strcmp(argv[1], "-W"))
		{
			bRead = false;
		}
	}

	printf("%s running...\n", TOOL_NAME);
	
	int ret = 0;
	
	if (bRead == true)
	{
		ret = ReadAndStore(argv[argc-1]);
	}
	if (ret == 0)
	{
		ret = UpdateAndWrite(argv[argc-1]);
		if (ret == 0)
		{
			printf("Completed successfully.\n");
		}
		else
		{
			printf("Failed!\n");
		}
	}
	else
	{
		printf("Failed!\n");
	}

	return ret;
}

