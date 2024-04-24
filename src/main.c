#include "ini_parser.h"
#include <stdio.h>
#include <string.h>

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
	}
}

int main(void) {
	IniFile* iniFile = ParseFile("./test.ini");
	if (iniFile == NULL) {
		printf("failed");
		return 1;
	}
	
	PrintIniFile(iniFile);

	printf("\n");
	IniSection* testSection = GetSection("Test", iniFile);
	if (testSection == NULL) {
		printf("Test section not found\n");
	}
	else {
		printf("Test section found\n");
	}

	if (GetSection("NotFound", iniFile) == NULL) {
		printf("NotFound section not found\n");
	}
	else {
		printf("NotFound section found\n");
	}

	if (GetValue("test", testSection) == NULL) {
		printf("test value not found\n");
	}
	else {
		printf("test value found\n");
	}


	if (GetValue("NotFound", testSection) == NULL) {
		printf("NotFound value not found\n");
	}
	else {
		printf("NotFound value found\n");
	}

	if (GetValueInFile("test", "Test", iniFile) == NULL) {
		printf("test value not found in file\n");
	}
	else {
		printf("test value found in file\n");
	}


	if (GetValueInFile("NotFound", "Test", iniFile) == NULL) {
		printf("NotFound value not found\n");
	}
	else {
		printf("NotFound value found\n");
	}

	int* foundIntPtr = GetValueInFileAsInt("test", "Test", iniFile);
	int* notFoundIntPtr = GetValueInFileAsInt("notFound", "Test", iniFile);

	if (foundIntPtr == NULL) {
		printf("test int not found in file\n");
	}
	else {
		printf("test int found in file, value: %d\n", *foundIntPtr);
	}

	if (notFoundIntPtr == NULL) {
		printf("notFound int not found in file\n");
	}
	else {
		printf("notFound int found in file, value: %d\n", *notFoundIntPtr);
	}

	return 0;
}
