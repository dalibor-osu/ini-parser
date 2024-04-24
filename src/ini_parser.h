#ifndef INI_PARSER_H
#define INI_PARSER_H

#define MAX_SECTION_NAME_LENGTH 32
#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 64

typedef struct IniKeyValue {
	char key[MAX_KEY_LENGTH];
	char value[MAX_VALUE_LENGTH];

	struct IniKeyValue* next;
	struct IniKeyValue* previous;
} IniKeyValue;

typedef struct IniSection {
	char sectionName[MAX_SECTION_NAME_LENGTH];

	int keyValueCount;
	IniKeyValue* firstKeyValue;

	struct IniSection* next;
	struct IniSection* previous;
} IniSection;

typedef struct IniFile {
	int sectionCount;
	IniSection* firstSection;
} IniFile;

IniFile* ParseFile(const char* filePath);
IniSection* GetSection(const char* sectionName, const IniFile* iniFile);

char* GetValue(const char* key, const IniSection* section);
char* GetValueInFile(const char* key, const char* sectionName, const IniFile* iniFile);

int* GetValueAsInt(const char* key, const IniSection* section);
int* GetValueInFileAsInt(const char* key, const char* sectionName, const IniFile* iniFile);
#endif
