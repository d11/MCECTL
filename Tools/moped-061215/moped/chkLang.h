typedef struct IrDefLanguageStruct *IrLang;
typedef struct IrDefAlphabet *Alpha;
typedef struct InitListStruct *InitList;

struct IrDefAlphabet{
	char* word;
	char letter;
	Alpha next;
};

struct IrDefLanguageStruct {
	char* name;
	char* expression;
	int unaccept;
	IrLang next;
};

struct InitListStruct {
	int initNummer;
	int ifModule;
	char* initName;
	InitList next;

};

extern IrLang ir_lang;
extern Alpha alphabet;
extern int lineno;
extern int debuginfo;
extern void chkParse (char*);
extern void printInitialParameters();
extern void printStateTransfer(const char* stateName, char let);
extern void printUnacceptStates();
extern void printLocalStates();
extern int printForLetter(char*, const char*, char);
extern IrLang irLangCreate(char*, char*, IrLang);
extern Alpha irAlphaCreate(char*, char, Alpha);
extern InitList InitListIdentCreate(int, char*, InitList);
extern char getLetter(char*);
extern char* getWord(char) ;
extern void irLangFree(IrLang);
extern void irAlphaFree(Alpha);
extern void InitListFree(InitList);
extern void FreeStruct();








