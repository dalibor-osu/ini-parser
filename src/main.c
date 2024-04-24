// File for testing purposes

#include "ini_parser.h"
#include <stdio.h>
#include <string.h>

void PrintSuccess(const char* restrict message) {
	printf("\x1B[92m");
	printf("%s", message);
	printf("\033[0m\t\n");
}

void PrintError(const char* restrict message) {
	printf("\x1B[91m");
	printf("%s", message);
	printf("\033[0m\t\n");
}

void PrintIniFile(const IniFile* file) {
	IniSection* currentSection = file->firstSection;
	IniKeyValue* currentKeyValue;

	while (currentSection->next != NULL) {
		currentSection = currentSection->next;
	}

	while (currentSection != NULL) {
		printf("[%s]\n", currentSection->sectionName);

		currentKeyValue = currentSection->firstKeyValue;
		while (currentKeyValue->next != NULL) {
			currentKeyValue = currentKeyValue->next;
		}

		while (currentKeyValue != NULL) {
			printf("%s = %s\n", currentKeyValue->key, currentKeyValue->value);
			currentKeyValue = currentKeyValue->previous;
		}

		currentSection = currentSection->previous;
		printf("\n");
	}
}

int main(void) {
	IniFile* iniFile = ParseFile("./test.ini");
	if (iniFile == NULL) {
		PrintError("failed");
		return 1;
	}
	
	PrintIniFile(iniFile);

	printf("\n");
	IniSection* testSection = GetSection("Test", iniFile);
	if (testSection == NULL) {
		PrintError("Test section not found");
	}
	else {
		PrintSuccess("Test section found");
	}

	if (GetSection("NotFound", iniFile) == NULL) {
		PrintSuccess("NotFound section not found");
	}
	else {
		PrintError("NotFound section found");
	}

	printf("\n");
	
	if (GetValue("test", testSection) == NULL) {
		PrintError("test value not found");
	}
	else {
		PrintSuccess("test value found");
	}

	if (GetValue("NotFound", testSection) == NULL) {
		PrintSuccess("NotFound value not found");
	}
	else {
		PrintError("NotFound value found");
	}

	printf("\n");
	
	if (GetValueInFile("test", "Test", iniFile) == NULL) {
		PrintError("test value not found in file");
	}
	else {
		PrintSuccess("test value found in file");
	}


	if (GetValueInFile("NotFound", "Test", iniFile) == NULL) {
		PrintSuccess("NotFound value not found");
	}
	else {
		PrintError("NotFound value found");
	}

	int* foundIntPtr = GetValueInFileAsInt("test", "Test", iniFile);
	int* notFoundIntPtr = GetValueInFileAsInt("notFound", "Test", iniFile);

	printf("\n");
	
	if (foundIntPtr == NULL) {
		PrintError("test int not found in file");
	}
	else {
		PrintSuccess("test int found in file");
	}

	if (notFoundIntPtr == NULL) {
		PrintSuccess("notFound int not found in file");
	}
	else {
		PrintError("notFound int found in file");
	}

	return 0;
}
