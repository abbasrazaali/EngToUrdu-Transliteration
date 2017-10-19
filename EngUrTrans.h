//////////////////////////////////////////////////////////
//							
//	Title: English-Urdu Tranliteration System			
//	Description: English to Urdu Transliterateration	
//		using CMU Pronunciation Dictionary		
//													    
//	File Name: EnUrTrans.h								
//														
//	Developed By: Abbas Raza Ali								
//														
//	Email: abbas4s@yahoo.com							
//														
//	Start Date: March 10, 2008							
//	End Date: August 31, 2008							
//														
//////////////////////////////////////////////////////////

#ifndef ENGURTRANS_H
#define ENGURTRANS_H

// global variables
#define SUCCESS 1
#define FAIL 0

#define HTEMPTY  -1
#define HTNIL -2

#define MAX_WORD 200
#define MAX_PRON 40

unsigned int conv_size = 0, conv_length = 0;							// size in lines of ARPAbet-Urdu conversion
long dict_size = 0, dict_length = 0, phoneme_count = 0, trans_count = 0, lang_count = 0;	// size in lines of CMU dictionary
wchar_t* buff;															// buffer for file reading
char** phonemes;
int word_len, phone_len, backPtr[MAX_WORD][MAX_PRON], seq[MAX_WORD], max_indx;
double viterbi[MAX_WORD][MAX_PRON];

// structures
struct ARPA_URDU														// contains ARPAbet alphabets and its alternate Urdu alphabet
{
	wchar_t arpa[5];  
	wchar_t ipa[5];  
	wchar_t urdu_init[5];	
	wchar_t urdu_mid[5];	
	wchar_t urdu_end[5];
	wchar_t type;
}* conv;

struct Eng_Dict															// contains English word and its pronunciation
{
	wchar_t word[30];  
	wchar_t pron[60];
	int usyllb[60];
	int usyllb_len;
	wchar_t ipa[60];
	wchar_t urpron[60];
}* dict;

struct Hash_Table														// contains hash tables
{
	wchar_t word[30];  
	wchar_t urpron[60];
	int next;
}* ht;

struct Trans_Model														// Transliteration Model
{
	char eng[10];  
	char pron[10];
	float prob;
	int next;
}* trans;

struct Lang_Model														// Transliteration Model
{
	char prev[10];  
	char curr[10];
	float prob;
	int next;
}* lang;

// functions 
int Initialization(char* fileName);										// initialization
int Finalization();														// finalization

// Transliterateration
int Transliterate_File(char* inputFileName, char* outputFileName);		// Transliteraterates English text in Urdu

// hashing
int Create_Hash_Table(char* fileName);									// create hash table
int Add_Word_HT(wchar_t* enWord, wchar_t* urWord);						// add a word and its pronounciation in hash table
long Hash_Function(wchar_t* word, long length);							// hash function
wchar_t* Transliterate_Word(wchar_t* word);								// search a word from hash table

// syllabification
void Syllabification(wchar_t* word, long index);
wchar_t* Syllb_Pron(long index);

// out-of-vocabulary
int OOV_Align(char* iFileName, char* eFileName, char* pFileName, char* epFileName);
int OOV_Trans_Model(char* oov_eng_pron_freq, char* oov_eng_freq, char* oov_trans_model);
int Load_OOV_Prob(char* oov_phoneme, char* oov_trans_model, char* oov_lang_model);
int Load_WordPron_Add(char* word, char* pron, float prob);
int Load_Pron_pPron_Add(char* pron, char* pPron, float prob);
int OOV(char* word);
double Max(int w, int t);
long Search_Phone(char* word, char* pron);
long Search_pPhone(char* pron, char* pPron);

// load dictionary
int Create_Dictionary(char* convFileName, char* dictFileName, char* transFileName);	// create Urdu pronunciation dictionary 
int Load_ARPA_IPA_URDU(char* fileName);									// loads ARPAbet alphabets, IPA's and their alternate Urdu alphabet
int Load_CMUDict(char* fileName);										// loads CMU dictionary
int ARPA_URDU_Conversion();												// converts English tranliteration into its alternate Urdu alphabet
int Generate_UrDict(char* fileName);									// creates a file of Urdu pronounication dictionary

// file handling 
long Read_File(char* fileName, long size);								// reads a file and place its data in buff array
long File_Line(char* fileName);											// return number of lines in a file
long File_Size(char* fileName);											// return number of characters in a file

#endif
