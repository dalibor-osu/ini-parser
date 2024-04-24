#include "ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum number of chars that will be read per line
const int CURRENT_LINE_LENGTH = 256; 

// Adds a section to the beginning of a linked list in IniFile
void AddSection(IniFile* file, IniSection* section) {
	if (file == NULL || section == NULL) {
		fprintf(stderr, "IniFile or Section pointer was NULL");
		return;
	}

	if (file->firstSection == NULL) {
		file->firstSection = section;
		return;
	}

	section->next = file->firstSection;
	file->firstSection->previous = section;
	file->firstSection = section;
	file->sectionCount++;
}

// Adds a KeyValue to the beginning of a linked list in IniSection
void AddKeyValue(IniSection* section, IniKeyValue* keyValue) {
	if (section == NULL || keyValue == NULL) {
		fprintf(stderr, "Section or KeyValue pointer was NULL");
		return;
	}

	if (section->firstKeyValue == NULL) {
		section->firstKeyValue = keyValue;
		return;
	}

	keyValue->next = section->firstKeyValue;
	section->firstKeyValue->previous = keyValue;
	section->firstKeyValue = keyValue;
	section->keyValueCount++;
}

// Walks through the char array until it finds a char thats not whitespace
void SkipWhitespace(int* index, char* currentChar, const char* currentLine) {
	for (; *index < CURRENT_LINE_LENGTH; (*index)++) {
		*currentChar = currentLine[*index];
		if (*currentChar == ' ' || *currentChar == '\t') {
			continue;
		}

		break;
	}
}

// Parses an IniSection name and returns its pointer
IniSection* ParseSection(int* index, char* currentChar, const char* currentLine) {
	// Advance to a new char
	*currentChar = currentLine[++(*index)];

	// Allocate memory for a new section
	IniSection* newSection = (IniSection*)malloc(sizeof(IniSection));

	if (newSection == NULL) {
		fprintf(stderr, "Failed to allocate memory");
		return NULL;
	}

	// Initialize the new section
	newSection->next = NULL;
	newSection->previous = NULL;
	newSection->firstKeyValue = NULL;
	newSection->keyValueCount = 0;

	// Read section chars
	for (int j = 0; j < MAX_SECTION_NAME_LENGTH; j++) {
		if (*currentChar == ']') {
			break;
		}

		if (j + 1 == MAX_SECTION_NAME_LENGTH && currentLine[*index + 1] != ']') {
			fprintf(stderr, "Error when parsing an INI file");
			return NULL;
		}

		newSection->sectionName[j] = *currentChar;
		*currentChar = currentLine[++(*index)];
	}
	
	return newSection;
}

// Parses a key from IniKeyValue and returns its pointer. If error occures, returns NULL
IniKeyValue* ParseKey(int* index, char* currentChar, const char* currentLine, IniKeyValue* iniKeyValue) {
	// Key must not start with '='
	// TODO: Check more invalid chars
	if (*currentChar == '=') {
		fprintf(stderr, "Invalid key name (key starts with '=')");
		return NULL;
	}

	// Read key chars
	for (int j = 0; j < MAX_KEY_LENGTH; j++) {
		if (*currentChar == '=') {
			break;
		}

		// Handle whitespace between key and '='
		if (*currentChar == ' ' || *currentChar == '\t') {
			SkipWhitespace(index, currentChar, currentLine);
			
			// Char after whitespace must be '=' 
			if (*currentChar != '=') {
				fprintf(stderr, "Invalid key");
				return NULL;
			}
			
			break;
		}

		if (j + 1 == MAX_KEY_LENGTH && currentLine[*index + 1] != '=') {
			fprintf(stderr, "Invalid key (too long)");
			return NULL;
		}

		iniKeyValue->key[j] = *currentChar;
		*currentChar = currentLine[++(*index)];
	}

	return iniKeyValue;
}

// Parses a value from IniKeyValue and returns its pointer. If error occures, returns NULL
IniKeyValue* ParseValue(int* index, char* currentChar, const char* currentLine, IniKeyValue* iniKeyValue) {
	// Advance to a next char
	*currentChar = currentLine[++(*index)];
	int stringLiteral = 0;

	// Skip whitespace between '=' and value
	SkipWhitespace(index, currentChar, currentLine);

	// Read value chars
	for (int j = 0; j < MAX_KEY_LENGTH; j++) {
		// Handle double quotes
		if (*currentChar == '"') {
			if (stringLiteral == 1) {
				break;
			}

			stringLiteral = 1;
			*currentChar = currentLine[++(*index)];
			j--;
			continue;
		}

		// Handle comments
		if (*currentChar == '#') {
			if (stringLiteral != 1) {
				if (currentLine[*index - 1] == ' ' || currentLine[*index - 1] == '\t') {
					break;
				}

				fprintf(stderr, "Failed to parse INI value (found # in non string value)");
				return NULL;
			}
		}

		if (*currentChar == '\n') {
			break;
		}

		if (j + 1 == MAX_VALUE_LENGTH && currentLine[*index + 1] != '=') {
			fprintf(stderr, "Invalid value (too long)");
			return NULL;
		}

		iniKeyValue->value[j] = *currentChar;
		*currentChar = currentLine[++(*index)];
	}

	return iniKeyValue;
}

// Parses an INI file and returns a pointer to IniFile instance that represents it. If error occures returns NULL 
IniFile* ParseFile(const char* filePath) {
	// Allocate memory for IniFile
	IniFile* iniFile = (IniFile*)malloc(sizeof(IniFile));

	if (iniFile == NULL) {
		fprintf(stderr, "Failed to allocate memory");
	}

	iniFile->sectionCount = 0;

	// Open file to be read
	FILE* file = fopen(filePath, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file");
		return NULL;
	}

	// Initialize stuff
	char currentLine[CURRENT_LINE_LENGTH];
	IniSection* currentSection = NULL;

	// Read file line by line
	while(fgets(currentLine, CURRENT_LINE_LENGTH, file)) {
		// Initialize stuff
		int i = 0;
		char currentChar;

		// Skip whitespace at the beginning of a line
		SkipWhitespace(&i, &currentChar, currentLine);

		// Continue on comment and line end
		if (currentChar == '#' || currentChar == '\0' || currentChar == '\n') {
			continue;
		}

		// Parse new section
		if (currentChar == '[') {
			IniSection* newSection = ParseSection(&i, &currentChar, currentLine);

			if (newSection == NULL) {
				return NULL;
			}

			AddSection(iniFile, newSection);
			currentSection = newSection;
			
			continue;
		}

		// File must start with a valid section
		if (currentSection == NULL) {
			fprintf(stderr, "Error when parsing an INI file");
			return NULL;
		}

		// Allocate memory for IniKeyValue
		IniKeyValue* newKeyValue = (IniKeyValue*)malloc(sizeof(IniKeyValue));
		
		if (newKeyValue == NULL) {
			fprintf(stderr, "Failed to allocate memory");
			return NULL;
		}
		
		// Parse key value
		newKeyValue = ParseKey(&i, &currentChar, currentLine, newKeyValue);
		
		if (newKeyValue == NULL) {
			return NULL;
		}

		// Parse value
		newKeyValue = ParseValue(&i, &currentChar, currentLine, newKeyValue);
		
		if (newKeyValue == NULL) {
			return NULL;
		}

		// Add KeyValue to linked list of current section
		AddKeyValue(currentSection, newKeyValue);
	}

	fclose(file);
	return iniFile;
}

// Tries to find a section by its name in IniFile. If found, returns its pointer, otherwise returns NULL
IniSection* GetSection(const char* sectionName, const IniFile* iniFile) {
	if (sectionName == NULL || iniFile == NULL) {
		return NULL;
	}

	IniSection* currentSection = iniFile->firstSection;
	while (currentSection != NULL) {
		if (strcmp(sectionName, currentSection->sectionName) == 0) {
			return currentSection;
		}
		currentSection = currentSection->next;
	}

	return NULL;
}

// Tries to find a value inside a section by its key. If found, returns its pointer, otherwise returns NULL
char* GetValue(const char* key, const IniSection* section) {
	if (key == NULL || section == NULL) {
		return NULL;
	}

	IniKeyValue* currentKeyValue = section->firstKeyValue;
	while (currentKeyValue != NULL) {
		if (strcmp(key, currentKeyValue->key) == 0) {
			return currentKeyValue->value;
		}
		currentKeyValue = currentKeyValue->next;
	}

	return NULL;
}

// Tries to find a value inside the whole file by its section and key. Shortcut combination of GetSection() and GetValue()
char* GetValueInFile(const char* key, const char* sectionName, const IniFile* iniFile) {
	return GetValue(key, GetSection(sectionName, iniFile));
}

// Tries to find a value inside a section by its key and convert it to an integer. Returns NULL if value is not found
int* GetValueAsInt(const char* key, const IniSection* section) {
	char* strValue = GetValue(key, section);

	if (strValue == NULL) {
		return NULL;
	}

	int value = atoi(strValue);
	int* pValue = &value;
	return pValue;
}

// Tries to find a value inside the whole file by its section and key and convert it to an integer. Returns NULL of value is not found
int* GetValueInFileAsInt(const char* key, const char* sectionName, const IniFile* iniFile) {
	char* strValue = GetValueInFile(key, sectionName, iniFile);

	if (strValue == NULL) {
		return NULL;
	}

	int value = atoi(strValue);
	int* pValue = &value;
	return pValue;
}
