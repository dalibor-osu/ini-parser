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

// Parses an INI file and returns a pointer to IniFile instance that represents it. If error occures returns NULL 
IniFile* ParseFile(const char* filePath);
// Tries to find a section by its name in IniFile. If found, returns its pointer, otherwise returns NULL
IniSection* GetSection(const char* sectionName, const IniFile* iniFile);

// Tries to find a value inside a section by its key. If found, returns its pointer, otherwise returns NULL
char* GetValue(const char* key, const IniSection* section);
// Tries to find a value inside the whole file by its section and key. Shortcut combination of GetSection() and GetValue()
char* GetValueInFile(const char* key, const char* sectionName, const IniFile* iniFile);

// Tries to find a value inside a section by its key and convert it to an integer. Returns NULL if value is not found
int* GetValueAsInt(const char* key, const IniSection* section);
// Tries to find a value inside the whole file by its section and key and convert it to an integer. Returns NULL of value is not found
int* GetValueInFileAsInt(const char* key, const char* sectionName, const IniFile* iniFile);

#endif
