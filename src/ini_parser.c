#include "ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int CURRENT_LINE_LENGTH = 256; 

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
}

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
}

IniFile* ParseFile(const char* filePath) {
	IniFile* iniFile = (IniFile*)malloc(sizeof(IniFile));

	if (iniFile == NULL) {
		fprintf(stderr, "Failed to allocate memory");
	}

	iniFile->sectionCount = 0;

	FILE* file = fopen(filePath, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file");
		return NULL;
	}

	char currentLine[CURRENT_LINE_LENGTH];
	IniSection* currentSection = NULL;

	while(fgets(currentLine, CURRENT_LINE_LENGTH, file)) {
		int i = 0;
		char currentChar;
		// Skip whitespace in the beginning of the line
		for (; i < CURRENT_LINE_LENGTH; i++) {
			currentChar = currentLine[i];
			if (currentChar == ' ' || currentChar == '\t') {
				continue;
			}

			break;
		}

		// Continue on comment
		if (currentChar == '#' || currentChar == '\0' || currentChar == '\n') {
			continue;
		}
		
		if (currentChar == '[') {
			i++;
			currentChar = currentLine[i];
			IniSection* newSection = (IniSection*)malloc(sizeof(IniSection));

			if (newSection == NULL) {
				fprintf(stderr, "Failed to allocate memory");
				return NULL;
			}

			newSection->next = NULL;
			newSection->previous = NULL;
			newSection->firstKeyValue = NULL;
			newSection->keyValueCount = 0;

			for (int j = 0; j < MAX_SECTION_NAME_LENGTH; j++) {
				if (currentChar == ']') {
					break;
				}

				if (j + 1 == MAX_SECTION_NAME_LENGTH && currentLine[i + 1] != ']') {
					fprintf(stderr, "Error when parsing an INI file");
					return NULL;
				}

				newSection->sectionName[j] = currentChar;
				currentChar = currentLine[++i];
			}

			AddSection(iniFile, newSection);
			currentSection = newSection;
			continue;
		}

		if (currentSection == NULL) {
			fprintf(stderr, "Error when parison an INI file");
			return NULL;
		}

		IniKeyValue* newKeyValue = (IniKeyValue*)malloc(sizeof(IniKeyValue));
		if (newKeyValue == NULL) {
			fprintf(stderr, "Failed to allocate memory");
			return NULL;
		}
		
		if (currentChar == '=') {
			fprintf(stderr, "Invalid key name (key starts with '=')");
			return NULL;
		}

		for (int j = 0; j < MAX_KEY_LENGTH; j++) {
			if (currentChar == '=') {
				break;
			}

			if (j + 1 == MAX_KEY_LENGTH && currentLine[i + 1] != '=') {
				fprintf(stderr, "Invalid key (too long)");
				return NULL;
			}

			newKeyValue->key[j] = currentChar;
			currentChar = currentLine[++i];
		}

		currentChar = currentLine[++i];
		int stringLiteral = 0;

		for (int j = 0; j < MAX_KEY_LENGTH; j++) {
			if (currentChar == '"') {
				if (stringLiteral == 1) {
					break;
				}

				stringLiteral = 1;
				currentChar = currentLine[++i];
				j--;
				continue;
			}

			if (currentChar == '#') {
				if (stringLiteral != 1) {
					if (currentLine[i - 1] == ' ' || currentLine[i - 1] == '\t') {
						break;
					}

					fprintf(stderr, "Failed to parse INI value (found # in non string value)");
				}
			}

			if (currentChar == '\n') {
				break;
			}

			if (j + 1 == MAX_VALUE_LENGTH && currentLine[i + 1] != '=') {
				fprintf(stderr, "Invalid value (too long)");
				return NULL;
			}

			newKeyValue->value[j] = currentChar;
			currentChar = currentLine[++i];
		}
		
		AddKeyValue(currentSection, newKeyValue);
	}

	fclose(file);
	return iniFile;
}


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

char* GetValueInFile(const char* key, const char* sectionName, const IniFile* iniFile) {
	return GetValue(key, GetSection(sectionName, iniFile));
}

int* GetValueAsInt(const char* key, const IniSection* section) {
	char* strValue = GetValue(key, section);

	if (strValue == NULL) {
		return NULL;
	}

	int value = atoi(strValue);
	int* pValue = &value;
	return pValue;
}

int* GetValueInFileAsInt(const char* key, const char* sectionName, const IniFile* iniFile) {
	char* strValue = GetValueInFile(key, sectionName, iniFile);

	if (strValue == NULL) {
		return NULL;
	}

	int value = atoi(strValue);
	int* pValue = &value;
	return pValue;
}
