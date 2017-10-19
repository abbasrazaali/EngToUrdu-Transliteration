//////////////////////////////////////////////////////////
//														//
//	Title: English-Urdu Tranliteration System			//
//	Description: English to Urdu Transliterateration	//
//				 using CMU Pronunciation Dictionary		//
//														//
//	File Name: EnUrTrans.c								//
//														//
//	Developed By: Abbas Raza Ali						//
//														//
//														//
//	Start Date:	March 10, 2008							//
//	End Date: August 31, 2008							//
//														//
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include "engurtrans.h"

// main function
int main()
{
	char input = '\0';
		
	printf("\n*************************************\n");
	printf("\n English-Urdu Tranliteration Project\n");
	printf("\n*************************************\n\n\n");

	/*printf("Generate English to Urdu Transliteraterated Dictionary (y/n): ");
	input = getch();
	printf("\n");

	if(input == 'Y' || input == 'y')
	{
		if(Create_Dictionary("ARPA_IPA_URDU.txt", "CMUDictEn.txt", "CMUDictUr.txt"))
		{
			printf("Urdu Dictionary is created successfully.\n\n");
		}
		else
		{
			printf("CMU Dictionary is not created.\n\n");
		}
	}

	// Out-Of-Vocabulary
	
	printf("Generate English Text to Pronunciation Alignment Data (y/n): ");
	input = getch();
	printf("\n");

	if(input == 'Y' || input == 'y')
	{
		if(OOV_Align("OOV/oov_cmu_align.txt", "OOV/oov_eng.txt", "OOV/oov_pron.txt", "OOV/oov_eng_pron.txt"))
		{
			printf("Alignment Data is created successfully.\n\n");
			
			if(OOV_Trans_Model("OOV/oov_eng_pron_freq.txt", "OOV/oov_eng_freq.txt", "OOV/oov_trans_model.txt"))
			{
				printf("OOV Transliteration Model is created successfully.\n\n");
			}
			else
			{
				printf("OOV Transliteration Model is not created.\n\n");
			}
		}
		else
		{
			printf("Alignment Data is not created.\n\n");
		}
	}*/

	if(Load_OOV_Prob("OOV/oov_phoneme.txt", "OOV/oov_trans_model.txt", "OOV/oov_lang_model.txt"))
	{
		printf("OOV Transliteration and Language Model files loaded successfully.\n\n");
	}
	else
	{
		printf("OOV Transliteration and Language Model files are not loaded.\n\n");
	}
	
	if(Initialization("CMUDictUr.txt"))
	{
		printf("Hash table loaded successfully.\n\n");

		if(Transliterate_File("Input.txt", "Output.txt"))
		{
			printf("English text is Transliteraterated in Urdu.\n\n\n");
		}
		else
		{
			printf("Transliterateration failed.\n\n");		
		}
	}
	else
	{
		printf("Hash table loading failed.\n\n");
	}
	Finalization();

	return SUCCESS;
}

// initialization
int Initialization(char* fileName)
{
	return Create_Hash_Table(fileName);
}

// finalization
int Finalization()
{
	if(conv)
		free(conv);

	if(dict)
		free(dict);

	if(ht)
		free(ht);

	conv_size = 0, conv_length = 0, dict_size = 0, dict_length = 0;

	return SUCCESS;
}

/////////////////////
// Transliteration //
/////////////////////

// Transliteraterates English text in Urdu
int Transliterate_File(char* inputFileName, char* outputFileName)
{
	long count = 1, tcount = 0, buff_size;
	wchar_t enWord[30] = {0}, urWord[60] = {0};
	
	FILE* file = fopen(outputFileName, "wb");

	if(file == NULL)
		return FAIL;

	buff_size = File_Size(inputFileName);

	if((buff = (wchar_t*) malloc(buff_size * sizeof(wchar_t))) != NULL)
	{
		Read_File(inputFileName, buff_size);

		while(count < buff_size - 1)
		{
			if(tcount < 30 && count != 1 && (buff[count] == 0 || buff[count] == 9 || buff[count] == 10 || buff[count] == 13 
				|| buff[count] == 32 || buff[count] == 33 || buff[count] == 34 || buff[count] == 37 || buff[count] == 38 
				|| buff[count] == 39 || buff[count] == 40 || buff[count] == 41 || buff[count] == 44 || buff[count] == 45 
				|| buff[count] == 46 || buff[count] == 47 || buff[count] == 58 || buff[count] == 59 || buff[count] == 60 
 				|| buff[count] == 62 || buff[count] == 63 || buff[count] == 123 || buff[count] == 125))
			{
				if(tcount == 0)
				{
					enWord[tcount++] = buff[count++];
				}
				enWord[tcount] = 0;

				wcscpy(urWord, Transliterate_Word(wcslwr(enWord)));
	
				if(wcscmp(wcslwr(urWord), wcslwr(enWord)) == 0)
				{
					//wcscpy(urWord, OOV(enWord));
					OOV(enWord);
				}

				fwrite(wcsupr(enWord), wcslen(enWord) * sizeof(wchar_t), 1, file);
				fwrite("\t", wcslen("\t") * sizeof(wchar_t), 1, file);
				fwrite(urWord, wcslen(urWord) * sizeof(wchar_t), 1, file);
	
				if(buff[count] == 46 && buff[count + 1] == 46 && buff[count + 2] == 46)
				{
					tcount = 0;
					enWord[tcount++] = buff[count++], enWord[tcount++] = buff[count++], enWord[tcount++] = buff[count++];
				}
				else if(!((buff[count] >= 65 && buff[count] <= 90) || (buff[count] >= 97 && buff[count] <= 122)) && (buff[count] != 0 && buff[count] != 40 
					&& buff[count] != 41 && buff[count] != 44 && buff[count] != 45 && buff[count] != 46 && buff[count] != 59 
					&& buff[count] != 60 && buff[count] != 62 && buff[count] != 63 && buff[count] != 123 && buff[count] != 125))
				{
					if(buff[count] == 13 && buff[count + 1] == 10)
					{
						enWord[0] = 0x000D, enWord[1] = 0x000A, enWord[2] = 0;
						count += 2;
					}
					else
					{
						tcount = 0;
						enWord[tcount++] = buff[count++], enWord[tcount++] = 0;

						if(buff[count] == 13 && buff[count + 1] == 10)
						{
							tcount--;
							enWord[tcount++] = 0x000D, enWord[tcount++] = 0x000A, enWord[tcount++] = 0;
							count += 2;
						}
					}

					fwrite(enWord, wcslen(enWord) * sizeof(wchar_t), 1, file);

					tcount = 0;
				}
				else
				{
					tcount = 0;
				}
			}
			else
			{
				enWord[tcount++] = buff[count++];
			}
		} 
		free(buff);
	}
	fclose(file);

	return SUCCESS;
}

/////////////
// hashing //
/////////////

// create hash table 
int Create_Hash_Table(char* fileName)
{
	int check = 0;
	long i = 0, j = 0, count = 0, buff_count = 0; 
	wchar_t en[30] = {0}, ur[60] = {0};
	
	if(ht)
	{
		free(ht);
	}

	dict_size = File_Size(fileName);

	if((buff = (wchar_t*) malloc(dict_size * sizeof(wchar_t))) != NULL)
	{
		dict_length = Read_File(fileName, dict_size) - 1;

		if((ht = (struct Hash_Table*) malloc(dict_length * sizeof(struct Hash_Table))) != NULL)
		{			
			for(i = 0; i < dict_length; i++)
			{
				ht[i].next = HTEMPTY;
			}

			i = 0;
			while(count < dict_length && buff[buff_count])
			{
				if((buff[buff_count] == 32 || buff[buff_count] == 9) && !check)
				{	
					en[i] = 0, i = 0;
					check = 1;
					while(i < 4)
					{
						if(buff[buff_count++] == 9)
							i++;	
					}
					buff_count--;
					i = 0;
				}
				else if(buff[buff_count] == 13)
				{	
					ur[j] = 0, j = 0;
					
					if(Transliterate_Word(en) == en)
					{
						Add_Word_HT(en, ur);
					}
					count++;
					check = 0;
				}
				else if(buff[buff_count] == 10)
				{ }
				else
				{		
					if(check == 0)	
						en[i++] = buff[buff_count];
					else
						ur[j++] = buff[buff_count];
				}
				buff_count++;
			}	
		}
		else
		{
			return FAIL;
		}
		free(buff);
	}
	return SUCCESS;
}

// add a word and its pronounciation in hash table
int Add_Word_HT(wchar_t* enWord, wchar_t* urWord)
{	
	long index = 0, trail;

	if(enWord == '\0' && urWord == '\0')
		return FAIL;

	index = Hash_Function(enWord, dict_length);
	
	if(ht[index].next == HTEMPTY)
	{
		wcscpy(ht[index].word, enWord);
		wcscpy(ht[index].urpron, urWord);
		ht[index].next = HTNIL; 
	}
	else
	{
		trail = index;
		while(ht[index].next != HTNIL)
		{
			index = ht[index].next;
			trail = index;
		}
		
		while(ht[(index + 1) % dict_length].next != HTEMPTY)
		{
			index = (index + 1) % dict_length;
		}
		index = (index + 1) % dict_length;
		
		ht[trail].next = index;
		
		wcscpy(ht[index].word, enWord);
		wcscpy(ht[index].urpron, urWord);
		ht[index].next = HTNIL;
	}

	return SUCCESS;
}

// hash function
long Hash_Function(wchar_t* word, long length)
{
	unsigned long hValue = 1315423911;
	
	if(length > 0)
	{
		for(hValue = 0; *word != '\0'; word++)
		{
			hValue ^= ((hValue << 5) + (*word) + (hValue >> 2));
		}
	}
	return hValue % length;
}

// search a word from hash table
wchar_t* Transliterate_Word(wchar_t* Word)
{
	long index = 0;

	if(dict_length == 0) 
	{
		return FAIL;
	}
	
	index = Hash_Function(Word, dict_length);
	
	while(wcscmp(ht[index].word, Word) != FAIL)
	{
		if(ht[index].next == HTNIL || ht[index].next == HTEMPTY)
		{
			return Word;
		}
		index = ht[index].next;
	}		
	return ht[index].urpron;
}

/////////////////////
// syllabification //
/////////////////////

void Syllabification(wchar_t* word, long dict_index)
{
	int i, j = 0, k, l = 0, prevType, wlen = 0, ctoken, cvowel, index[30], syllbi[60];
	wchar_t token[10];

	// getting indexes of the phonemes from ARPA_IPA_URDU file
	wlen = wcslen(word);
	for(i = 0; i < wlen; i++)
	{
		if(word[i] == 32)
		{
			token[j] = 0;
			for(k = 0; k < conv_length; k++)
			{
				if(wcscmp(conv[k].arpa, wcsupr(token)) == 0)
				{
					index[l++] = k;
					break;
				}
			}
			j = 0, i++;
		}
		token[j++] = word[i];
	}
	token[j] = 0;
	for(k = 0; k < conv_length; k++)
	{
		if(wcscmp(conv[k].arpa, wcsupr(token)) == 0)
		{
			index[l++] = k;
			break;
		}
	}
	
	// syllabification algorithm
	prevType = 0;
	ctoken = l;
	l = 0;
	syllbi[l++] = -1;
	for(i = ctoken - 1; i >= 0; i--)
	{
		if(prevType == 1 && conv[index[i]].type == L'C')
		{
			syllbi[l++] = index[i];
			syllbi[l++] = -1;

			prevType = 0;
		}
		else if(prevType == 1)
		{
			syllbi[l++] = -1;
			syllbi[l++] = index[i];

			prevType = 0;			
		}
		else
		{
			syllbi[l++] = index[i];
		}

		if(i == -1)
		{
			syllbi[l++] = -1;
			break;
		}

		if(conv[index[i]].type != L'C')
		{
			prevType = 1;
		}
	}
	if(syllbi[l - 1] != -1)
		syllbi[l++] = -1;
	ctoken = l;
	
	// special cases
	// 1. (a) S C.* -> I S.C*
	if((wcscmp(conv[syllbi[ctoken - 2]].arpa, "S") == 0 && conv[syllbi[ctoken - 3]].type == L'C') || (wcscmp(conv[syllbi[ctoken - 2]].arpa, "S") == 0 && syllbi[ctoken - 3] == -1 && conv[syllbi[ctoken - 4]].type == L'C'))
	{
		for(k = 0; k < conv_length; k++)
		{
			if(wcscmp(conv[k].arpa, L"IH") == 0)
				break;
		}
		syllbi[ctoken] = -1, syllbi[ctoken - 1] = syllbi[ctoken - 2], syllbi[ctoken - 2] = k, ctoken += 1;
	}

	// 2. (a) *.C.(R | L)* -> *.C IH.(R | L)* | (b) *.C.* [~(R | L)] -> *.C AH.C* [~(R | L)] 
	for(i = ctoken - 1; i >= 3; i--)
	{
		if(syllbi[i] == -1 && conv[syllbi[i - 1]].type == L'C' && syllbi[i - 2] == -1 && (wcscmp(conv[syllbi[i - 3]].arpa, "R") == 0 || wcscmp(conv[syllbi[i - 3]].arpa, "L") == 0))
		{																					
			for(k = ctoken - 1; k >= i - 1; k--)
			{
				syllbi[k + 1] = syllbi[k];	
			}
			ctoken++;
			
			for(k = 0; k < conv_length; k++)
			{
				if(wcscmp(conv[k].arpa, L"IH") == 0)
					break;
			}
			syllbi[--i] = k;
		}
		else if(syllbi[i] == -1 && conv[syllbi[i - 1]].type == L'C' && syllbi[i - 2] == -1 && conv[syllbi[i - 3]].type == L'C')
		{
			for(k = ctoken - 1; k >= i - 1; k--)
			{
				syllbi[k + 1] = syllbi[k];	
			}
			ctoken++;
			
			for(k = 0; k < conv_length; k++)
			{
				if(wcscmp(conv[k].arpa, L"AH") == 0)
					break;
			}
			syllbi[--i] = k;
		}
	}

	// create syllabified pronunciation
	j = 0;
	for(i = ctoken - 1; i >= 0; i--)
	{
		dict[dict_index].usyllb[j++] = syllbi[i];
	}
	dict[dict_index].usyllb_len = j;
}

wchar_t* Syllb_Pron(long index)
{
	int i, j, k;
	wchar_t retWord[80];

	j = 0;
	for(i = 0; i < dict[index].usyllb_len; i++)
	{
		if(dict[index].usyllb[i] == -1)
		{
			if(retWord[j - 1] == L' ')
				retWord[j - 1] = L'.';
			else
				retWord[j++] = L'.';
		}
		else
		{
			k = 0;
			while(conv[dict[index].usyllb[i]].arpa[k])		
			{
				retWord[j++] = conv[dict[index].usyllb[i]].arpa[k++];
			}
			retWord[j++] = L' ';
		}
	}
	retWord[j++] = 0;

	return retWord;
}

///////////////////////
// out-of-vocabulary //
///////////////////////

int OOV_Align(char* iFileName, char* eFileName, char* pFileName, char* epFileName)
{
	long i = 0, j = 0, k = 0, l = 0, m = 0, buff_count = 0, size = 0, count = 0, elen = 0, plen = 0; 
	int check = 1, index[80], index1[80];
	char word[60], preWord[60], pron[80], align[10], word_align[80],* buffer, newline[3] = {13, 10};
	FILE* efile,* pfile,* epfile;

	// file size
	efile = fopen(iFileName, "r");
	if(!efile)
		return FAIL;
	fseek(efile, 0L, SEEK_END);
    size = ftell(efile);
    fclose(efile);

	if((buffer = (char*) malloc(size * sizeof(char))) != NULL)
	{
		// read file
		efile = fopen(iFileName, "r");
		for(buff_count = 0; buff_count < size; buff_count++)
		{
			buffer[buff_count] = 0;
		}
		buff_count = 0;
		fread(buffer, size, 1, efile);
		fclose(efile);

		preWord[0] = 0, size = 127120;

		efile = fopen(eFileName, "w");
		pfile = fopen(pFileName, "w");
		epfile = fopen(epFileName, "w");
		while(count < size && buffer[buff_count])
		{
			if(buffer[buff_count] == 9 && check == 1)
			{	
				word[i] = 0, i = 0, check = 2;
			}
			else if(buffer[buff_count] == 9 && check == 2)
			{	
				pron[j] = 0, j = 0, check = 3;
			}
			else if((buffer[buff_count] == 13 || buffer[buff_count] == 10) && check == 3)
			{	
				index[l] = -1, index1[m] = -1, check = 1, j = 0;
				
				if(strcmp(preWord, word) != 0)
				{
					for(l = 1; index1[l] != -1; l++)
					{
						if(index1[l] <= index1[l - 1])
						{
							j++;
						}
					}
					if(j == 0)
					{
						k = 0;
						for(l = 0; index[l] != -1; l++)
						{
							if(l != 0)
							{
								if(index[l] < index1[l] && (index1[l] - index1[l - 1]) == 2)
								{
									word_align[j++] = word[k++];		
								}
								else if(index[l] < index1[l] && (index1[l] - index1[l - 1]) == 3)
								{
									word_align[j++] = word[k++], word_align[j++] = word[k++];		
								}
								else if(index[l] < index1[l] && (index1[l] - index1[l - 1]) == 4)
								{
									word_align[j++] = word[k++], word_align[j++] = word[k++], word_align[j++] = word[k++];		
								}
								
								if(index[l] != index[l - 1])
								{
									word_align[j++] = 32;
								}
							} 
							word_align[j++] = word[k++];
						}	
						if(index1[l - 1] < strlen(word))
							while(word[k])
								word_align[j++] = word[k++];

						word_align[j++] = 0;
					
						plen = 0, elen = 0;

						for(j = 0; word_align[j]; j++)
							if(word_align[j] == 32)
								elen++;

						for(j = 0; pron[j]; j++)
							if(pron[j] == 32)
								plen++;
								
						if(elen == plen)
						{
							fwrite(word_align, strlen(word_align) * sizeof(char), 1, efile);
							fwrite(newline, strlen(newline) * sizeof(char), 1, efile);	

							fwrite(pron, strlen(pron) * sizeof(char), 1, pfile);
							fwrite(newline, strlen(newline) * sizeof(char), 1, pfile);

							m = 0, l = 0, j = 0, k = 0, plen = strlen(word_align) + strlen(pron);
							for(i = 0; word_align[j]; i++)
							{
								if(word_align[j] == 32 && l == 0)
								{
									align[m] = 0;
									fwrite(align, strlen(align) * sizeof(char), 1, epfile);
									l = 1, j++, m = 0;
								}
								else if(l == 0)
								{
									align[m++] = word_align[j++];
								}	
								else if(pron[k] == 32 && l == 1)
								{
									align[m] = 0;
									fwrite("_", strlen("_") * sizeof(char), 1, epfile);
									fwrite(align, strlen(align) * sizeof(char), 1, epfile);
									fwrite(" ", strlen(" ") * sizeof(char), 1, epfile);
									l = 0, k++, m = 0;
								}
								else if(l == 1)
								{
									align[m++] = pron[k++];
								}
							}
							align[m] = 0, m = 0;
							fwrite(align, strlen(align) * sizeof(char), 1, epfile);
							for(i = 0; pron[k]; i++)
							{
								if(pron[k] == 32)
								{
									align[m] = 0;
									fwrite("_", strlen("_") * sizeof(char), 1, epfile);
									fwrite(align, strlen(align) * sizeof(char), 1, epfile);
									fwrite(" ", strlen(" ") * sizeof(char), 1, epfile);
									l = 0, k++, m = 0;
								}
								else
								{
									align[m++] = pron[k++];
								}
							}
							align[m] = 0;
							fwrite("_", strlen("_") * sizeof(char), 1, epfile);
							fwrite(align, strlen(align) * sizeof(char), 1, epfile);

							fwrite(newline, strlen(newline) * sizeof(char), 1, epfile);
						}
					}
				}
				strcpy(preWord, word);
				i = 0, j = 0, l = 0, m = 0, k = 0;

				count++;
			}
			else
			{		
				if(check == 1)	
				{
					word[i++] = buffer[buff_count];
				}
				else if(check == 2)
				{
					pron[j++] = buffer[buff_count];
				}
				else if(check == 3)
				{
					if(buffer[buff_count] == '-')
					{
						k = 0;
						while(buffer[++buff_count] != 32 && buffer[buff_count] != 10 && buffer[buff_count] != 0)
							align[k++] = buffer[buff_count];
						align[k] = 0;

						if(buffer[buff_count] == 10)
							buff_count--;

						index[l++] = atoi(align);
					}
					else
					{
						k = 0;
						while(buffer[buff_count] != '-' && buffer[buff_count] != 0)
							align[k++] = buffer[buff_count++];
						align[k] = 0;

						buff_count--;

						index1[m++] = atoi(align);
					}
				}
			}
			buff_count++;
		}
		free(buffer);
		fclose(efile);
		fclose(pfile);
		fclose(epfile);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

int OOV_Trans_Model(char* oov_eng_pron_freq, char* oov_eng_freq, char* oov_trans_model)
{
	float frequ[1000];
	long i = 0, j = 0, k = 0, buff_count = 0, epsize = 0, esize = 0, count = 0, eplen = 0, elen = 0; 
	int check = 1;
	char fr[50], en[1000][10],* epbuffer,* ebuffer, newline[3] = {13, 10};
	
	FILE* epfile,* efile,* tmfile;

	// file size
	epfile = fopen(oov_eng_pron_freq, "r");
	if(!epfile)
		return FAIL;
	fseek(epfile, 0L, SEEK_END);
    epsize = ftell(epfile);
    fclose(epfile);

	efile = fopen(oov_eng_freq, "r");
	if(!efile)
		return FAIL;
	fseek(efile, 0L, SEEK_END);
    esize = ftell(efile);
    fclose(efile);

	if((epbuffer = (char*) malloc(epsize * sizeof(char))) != NULL && (ebuffer = (char*) malloc(esize * sizeof(char))) != NULL)
	{
		epfile = fopen(oov_eng_pron_freq, "r");
		for(buff_count = 0; buff_count < epsize; buff_count++)
		{
			epbuffer[buff_count] = 0;
		}
		fread(epbuffer, epsize, 1, epfile);
		fclose(epfile);

		efile = fopen(oov_eng_freq, "r");
		for(buff_count = 0; buff_count < esize; buff_count++)
		{
			ebuffer[buff_count] = 0;
		}
		fread(ebuffer, esize, 1, efile);
		fclose(efile);

		while(epbuffer[count])
		{
			if(epbuffer[count] == 10 || epbuffer[count + 1] == 0)
			{
				eplen++;
			}
			count++;
		}
		
		i = j = k = count = buff_count = 0;
		if((trans = (struct Trans_Model*) malloc(eplen * sizeof(struct Trans_Model))) != NULL)
		{
			while(epbuffer[buff_count])
			{
				if(epbuffer[buff_count] == 9 && check == 1)
				{	
					trans[count].eng[i] = 0, i = 0;
					check = 2;
				}
				else if(epbuffer[buff_count] == 9 && check == 2)
				{	
					trans[count].pron[j] = 0, j = 0;
					check = 3;
				}
				else if((epbuffer[buff_count] == 10 || epbuffer[buff_count] == 13) && check == 3)
				{	
					fr[k] = 0, k = 0, check = 1;
					trans[count].prob = atof(fr);
					count++;
				}
				else if(epbuffer[buff_count] == 10)
				{ }
				else
				{		
					if(check == 1)	
						trans[count].eng[i++] = epbuffer[buff_count];
					else if(check == 2)
						trans[count].pron[j++] = epbuffer[buff_count];
					else if(check == 3)
						fr[k++] = epbuffer[buff_count];
				}
				buff_count++;
			}
			free(epbuffer);	
		}
		eplen = count;

		i = j = count = buff_count = 0, elen = 609;
		while(ebuffer[buff_count] && count < elen)
		{
			if(ebuffer[buff_count] == 9 && check == 1)
			{	
				en[count][i] = 0, i = 0;
				check = 2;
			}
			else if((ebuffer[buff_count] == 10 || ebuffer[buff_count] == 13) && check == 2)
			{	
				fr[j] = 0, j = 0, check = 1;
				frequ[count] = atof(fr);
				count++;
			}
			else if(ebuffer[buff_count] == 10)
			{ }
			else
			{		
				if(check == 1)	
					en[count][i++] = ebuffer[buff_count];
				else if(check == 2)
					fr[j++] = ebuffer[buff_count];
			}
			buff_count++;
		}
		free(ebuffer);	
		
		for(i = 0; i < eplen; i++)
		{
			for(j = 0; j < elen; j++)
			{
				if(strcmp(trans[i].eng, en[j]) == 0)
				{
					trans[i].prob = (trans[i].prob / frequ[j]) * 1000;
					break;
				}
			}
		}
		tmfile = fopen(oov_trans_model, "w");
		for(i = 0; i < eplen; i++)
		{
			fwrite(trans[i].eng, strlen(trans[i].eng) * sizeof(char), 1, tmfile);
			fwrite("\t", strlen("\t") * sizeof(char), 1, tmfile);
			fwrite(trans[i].pron, strlen(trans[i].pron) * sizeof(char), 1, tmfile);
			fwrite("\t", strlen("\t") * sizeof(char), 1, tmfile);
		
			sprintf(fr, "%f", (trans[i].prob));
			fwrite(fr, strlen(fr) * sizeof(char), 1, tmfile);
			fwrite(newline, strlen(newline) * sizeof(char), 1, tmfile);
		}
		fclose(tmfile);
		free(trans);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

int Load_OOV_Prob(char* oov_phoneme, char* oov_trans_model, char* oov_lang_model)
{
	int i = 0, j = 0, k = 0, l = 0, check = 1;
	long count = 0, buff_count = 0;
	char* buffer, fr[50], wrd[100] = {0}, pr[15] = {0}, ppr[15] = {0};
	float prob = 0;
	FILE* file;

	// OOV Phonemes
	// file size
	file = fopen(oov_phoneme, "r");
	if(!file)
		return FAIL;
	fseek(file, 0L, SEEK_END);
    phoneme_count = ftell(file);
    fclose(file);

	if((buffer = (char*) malloc(phoneme_count * sizeof(char))) != NULL)
	{
		file = fopen(oov_phoneme, "r");
		for(buff_count = 0; buff_count < phoneme_count; buff_count++)
		{
			buffer[buff_count] = 0;
		}
		fread(buffer, phoneme_count, 1, file);
		fclose(file);

		phoneme_count = 0, count = 0;
		while(buffer[count])
		{
			if(buffer[count] == 10 || buffer[count + 1] == 0)
			{
				phoneme_count++;
			}
			count++;
		}
		count = 0, buff_count = 0;

		phonemes = (char **) malloc(phoneme_count * sizeof(char *));
		for(i = 0; i < phoneme_count; i++)
			phonemes[i] = (char *) malloc(5 * sizeof(char));

		i = 0;
		if(phonemes != NULL)
		{
			while(count < phoneme_count && buffer[buff_count])
			{
				if(buffer[buff_count] == 10)
				{
					phonemes[count][i] = 0, i = 0;
					count++;
				}
				else if(buffer[buff_count] == 10)
				{ }
				else
				{
					phonemes[count][i++] = buffer[buff_count];
				}
				buff_count++;
			}
		}
		free(buffer);
	}
	else
	{
		return FAIL;
	}

	// OOV Transliteration Model
	// file size
	file = fopen(oov_trans_model, "r");
	if(!file)
		return FAIL;
	fseek(file, 0L, SEEK_END);
    trans_count = ftell(file);
    fclose(file);

	if((buffer = (char*) malloc(trans_count * sizeof(char))) != NULL)
	{
		file = fopen(oov_trans_model, "r");
		for(buff_count = 0; buff_count < trans_count; buff_count++)
		{
			buffer[buff_count] = 0;
		}
		fread(buffer, trans_count, 1, file);
		fclose(file);

		trans_count = 0, count = 0;
		while(buffer[count])
		{
			if(buffer[count] == 13 || buffer[count + 1] == 0)
			{
				trans_count++;
			}
			count++;
		}
		count = 0, buff_count = 0;
		
		trans_count = 1144;
		j = k = count = buff_count = 0;
		if((trans = (struct Trans_Model*) malloc(trans_count * sizeof(struct Trans_Model))) != NULL)
		{
			for(i = 0; i < trans_count; i++)
			{
				trans[i].next = HTEMPTY;
			}
			i = 0;
			while(count < trans_count && buffer[buff_count])
			{
				if(buffer[buff_count] == 9 && check == 1)
				{	
					wrd[i] = 0, i = 0;
					check = 2;
				}
				else if(buffer[buff_count] == 9 && check == 2)
				{	
					pr[j] = 0, j = 0;
					check = 3;
				}
				else if((buffer[buff_count] == 10 || buffer[buff_count] == 13) && check == 3)
				{	
					fr[k] = 0, k = 0, check = 1;
					prob = atof(fr) / 1000;
					count++;

					Load_WordPron_Add(wrd, pr, prob);
				}
				else if(buffer[buff_count] == 10)
				{ }
				else
				{		
					if(check == 1)	
						wrd[i++] = buffer[buff_count];
					else if(check == 2)
						pr[j++] = buffer[buff_count];
					else if(check == 3)
						fr[k++] = buffer[buff_count];
				}
				buff_count++;
			}	
		}
		free(buffer);
	}
	else
	{
		return FAIL;
	}

	// OOV language Model
	// file size
	file = fopen(oov_lang_model, "r");
	if(!file)
		return FAIL;
	fseek(file, 0L, SEEK_END);
    lang_count = ftell(file);
    fclose(file);

	if((buffer = (char*) malloc(lang_count * sizeof(char))) != NULL)
	{
		file = fopen(oov_lang_model, "r");
		for(buff_count = 0; buff_count < lang_count; buff_count++)
		{
			buffer[buff_count] = 0;
		}
		fread(buffer, lang_count, 1, file);
		fclose(file);

		lang_count = 0, count = 0;
		while(buffer[count])
		{
			if(buffer[count] == 10 || buffer[count + 1] == 0)
			{
				lang_count++;
			}
			count++;
		}
		count = 0, buff_count = 0;
		
		lang_count = 23170;
		i = j = k = l = count = buff_count = 0, check = 1;
		if((lang = (struct Lang_Model*) malloc(lang_count * sizeof(struct Lang_Model))) != NULL)
		{
			for(i = 0; i < lang_count; i++)
			{
				lang[i].next = HTEMPTY;
			}

			i = 0;
			while(count < lang_count && buffer[buff_count])
			{
				if(buffer[buff_count] == 9 && check == 1)
				{	
					ppr[i] = 0, i = 0;
					check = 2;
				}
				else if(buffer[buff_count] == 9 && check == 2)
				{	
					pr[j] = 0, j = 0;
					check = 3;
				}
				
				else if((buffer[buff_count] == 10 || buffer[buff_count] == 13) && check == 3)
				{	
					fr[k] = 0, k = 0, check = 1;
					prob = atof(fr) / 1000;
					count++;

					Load_Pron_pPron_Add(pr, ppr, prob);
				}
				else if(buffer[buff_count] == 10)
				{ }
				else
				{		
					if(check == 1)	
						ppr[i++] = buffer[buff_count];
					else if(check == 2)
						pr[j++] = buffer[buff_count];
					else if(check == 3)
						fr[k++] = buffer[buff_count];
				}
				buff_count++;
			}	
		}
		free(buffer);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

// add a word and its pronunciation in hash table
int Load_WordPron_Add(char* word, char* pron, float prob)
{	
	long index = 0, trail;
	char pt[50];

	if(word == '\0' && pron == '\0')
		return FAIL;

	strcpy(pt, word);
	index = Hash_Function(strcat(pt, pron), trans_count);

	if(trans[index].next == HTEMPTY)
	{
		wcscpy(trans[index].eng, word);
		wcscpy(trans[index].pron, pron);
		trans[index].prob = prob;
		trans[index].next = HTNIL; 
	}
	else
	{
		trail = index;
		while(trans[index].next != HTNIL)
		{
			index = trans[index].next;
			trail = index;
		}
		
		while(trans[(index + 1) % trans_count].next != HTEMPTY)
		{
			index = (index + 1) % trans_count;
		}
		index = (index + 1) % trans_count;
		
		trans[trail].next = index;
		
		wcscpy(trans[index].eng, word);
		wcscpy(trans[index].pron, pron);
		trans[index].prob = prob;
		trans[index].next = HTNIL;
	}
	return SUCCESS;
}

int Load_Pron_pPron_Add(char* pron, char* pPron, float prob)
{	
	long index = 0, trail;
	wchar_t pt[20];

	if(pron == '\0' && pPron == '\0')
		return FAIL;

	wcscpy(pt, pron);
	index = Hash_Function(wcscat(pt, pPron), lang_count);
	
	if(lang[index].next == HTEMPTY)
	{
		wcscpy(lang[index].curr, pron);
		wcscpy(lang[index].prev, pPron);
		lang[index].prob = prob;
		lang[index].next = HTNIL; 
	}
	else
	{
		trail = index;
		while(lang[index].next != HTNIL)
		{
			index = lang[index].next;
			trail = index;
		}
		
		while(lang[(index + 1) % lang_count].next != HTEMPTY)
		{
			index = (index + 1) % lang_count;
		}
		index = (index + 1) % lang_count;
		
		lang[trail].next = index;
		
		wcscpy(lang[index].curr, pron);
		wcscpy(lang[index].prev, pPron);
		lang[index].prob = prob;
		lang[index].next = HTNIL;
	}
	return SUCCESS;
}

int OOV(char* word)
{
	int j, p, w = 0, wIndex = 0, pIndex = 0, UKN = 0;
	double max = 0, UKNProb;
	char ch[10], ph[10], pph[10];
	
	// Initialization
	for(p = 0; p < phoneme_count; p++)
	{
		ch[0] = word[0], ch[1] = 0; 
		wIndex = Search_Phone(ch, phonemes[p]);
		if(wIndex != -1)
		{
			viterbi[0][p] = trans[wIndex].prob;
		}
		else
		{
			viterbi[0][p] = 0;
		}
	}
	
	// Iteration
	UKNProb = lang[Search_pPhone(phonemes[phoneme_count - 1], phonemes[phoneme_count - 1])].prob;

	for(w = 1; w < word_len; w++)
	{
		UKN = 0;
		for(p = 0; p < phoneme_count; p++)
		{
			max = Max(w - 1, p);
			wIndex = Search_Phone(word[w], phonemes[p]);
			if(wIndex != -1 && strcmp(trans[wIndex].pron, phonemes[p]) == 0)
			{
				viterbi[w][p] = max * trans[wIndex].prob;
				backPtr[w - 1][p] = max_indx;
				UKN++;
			}
			else
			{
				viterbi[w][p] = 0;
			}
		}

		/*if(UKN == 0)
		{
			for(p = 0; p < phoneme_count; p++)
			{
				max = Max(w - 1, p);
				for(j = 0; j < tagset_len; j++)
				{
					if(strcmp(tagset[j], phonemes[p]) == 0)
					{
						viterbi[w][p] = max * UKNProb;
						backPtr[w - 1][p] = max_indx;
						break;
					}
					else 
					{
						viterbi[w][p] = 0;
					}
				}
			}
		}*/
	}

	// Sequence
	max = viterbi[w - 1][0];
	seq[word_len - 1] = 0;		
	for(j = 1; j < phoneme_count; j++)
	{
		if(viterbi[w - 1][j] > max)
		{
			max = viterbi[w - 1][j];
			seq[word_len - 1] = j;		
		}
	}
	
	for(w = word_len - 2; w >= 0; w--)
	{
		seq[w] = backPtr[w][seq[w + 1]];
	}

	return SUCCESS;
}

double Max(int w, int p)
{
	int j;
	long pIndex;
	double max = 0;
	max_indx = 0;

	pIndex = Search_pPhone(phonemes[p], phonemes[0]);
	if(pIndex != -1)
	{
		max = viterbi[w][0] * lang[pIndex].prob;
	}

	for(j = 1; j < phoneme_count; j++)
	{
		pIndex = Search_pPhone(phonemes[p], phonemes[j]);
		if(pIndex != -1 && (viterbi[w][j] * lang[pIndex].prob) > max)
		{
			max = viterbi[w][j] * lang[pIndex].prob;
			max_indx = j;
		}
	}
	return max;
}

// search a word | pron from hash table
long Search_Phone(char* word, char* phone)
{
	long index = 0;
	char wch[100];

	if(trans_count == 0) 
	{
		return FAIL;
	}
	
	strcpy(wch, word);
	index = Hash_Function(strcat(wch, phone), trans_count);
	
	while(strcmp(trans[index].eng, word) != FAIL || strcmp(trans[index].pron, phone) != FAIL)
	{
		if(trans[index].next == HTNIL || trans[index].next == HTEMPTY)
		{
			return -1;
		}
		index = trans[index].next;
	}		
	return index;
}

// search a pron | pPron from hash table
long Search_pPhone(char* Phone, char* pPhone)
{
	long index = 0;
	char wch[20];

	if(lang_count == 0) 
	{
		return FAIL;
	}
	
	strcpy(wch, Phone);
	index = Hash_Function(strcat(wch, pPhone), lang_count);
	
	while(strcmp(lang[index].curr, Phone) != FAIL || strcmp(lang[index].prev, pPhone) != FAIL)
	{
		if(lang[index].next == HTNIL || lang[index].next == HTEMPTY)
		{
			return -1;
		}
		index = lang[index].next;
	}		
	return index;
}

/////////////////////
// load dictionary //
/////////////////////

// create Urdu pronunciation dictionary
int Create_Dictionary(char* convFileName, char* dictFileName, char* transFileName)
{
	if(Load_ARPA_IPA_URDU(convFileName))
	{
		if(Load_CMUDict(dictFileName))
		{
			if(ARPA_URDU_Conversion())
			{
				if(Generate_UrDict(transFileName))
				{
					return SUCCESS;
				}	
			}
		}
	}
	return FAIL;
}

// loads ARPAbet alphabets, IPA's and their alternate Urdu alphabet
int Load_ARPA_IPA_URDU(char* fileName)
{
	unsigned int i = 0, j = 0, k = 0, l = 0, m = 0, count = 0, buff_count = 1, check = 1;

	if(conv)
	{
		free(conv);
	}

	conv_size = File_Size(fileName);

	if((buff = (wchar_t*) malloc(conv_size * sizeof(wchar_t))) != NULL)
	{
		conv_length = Read_File(fileName, conv_size) - 1;

		if((conv = (struct ARPA_URDU*) malloc(conv_length * sizeof(struct ARPA_URDU))) != NULL)
		{
			while(count < conv_length && buff[buff_count])
			{
				if(buff[buff_count] == 9 && check == 1)
				{	
					conv[count].arpa[i] = 0, i = 0;
					check = 2;
				}
				else if(buff[buff_count] == 9 && check == 2)
				{	
					conv[count].ipa[j] = 0, j = 0;
					check = 3;
				}
				else if(buff[buff_count] == 9 && check == 3)
				{	
					conv[count].urdu_end[k] = 0, k = 0;
					check = 4;
				}
				else if(buff[buff_count] == 9 && check == 4)
				{	
					conv[count].urdu_mid[l] = 0, l = 0;
					check = 5;
				}
				else if(buff[buff_count] == 9 && check == 5)
				{	
					conv[count].urdu_init[m] = 0, m = 0;
					check = 6;
				}
				else if(buff[buff_count] == 13 && check == 6)
				{	
					count++;
					check = 1;
				}
				else if(buff[buff_count] == 10)
				{ }
				else
				{		
					if(check == 1)	
						conv[count].arpa[i++] = buff[buff_count];
					else if(check == 2)
						conv[count].ipa[j++] = buff[buff_count];
					else if(check == 3)
						conv[count].urdu_end[k++] = buff[buff_count];
					else if(check == 4)
						conv[count].urdu_mid[l++] = buff[buff_count];
					else if(check == 5)
						conv[count].urdu_init[m++] = buff[buff_count];
					else if(check == 6)
						conv[count].type = buff[buff_count];
				}
				buff_count++;
			}
		}
		free(buff);
	}
	else
		return FAIL;

	return SUCCESS;
}

// loads CMU Dictionary
int Load_CMUDict(char* fileName)
{
	long i = 0, j = 0, count = 0, buff_count = 1; 
	int check = 0;

	if(dict)
	{
		free(dict);
	}

	dict_size = File_Size(fileName);

	if((buff = (wchar_t*) malloc(dict_size * sizeof(wchar_t))) != NULL)
	{
		dict_length = Read_File(fileName, dict_size) - 1;

		if((dict = (struct Eng_Dict*) malloc(dict_length * sizeof(struct Eng_Dict))) != NULL)
		{
			while(count < dict_length && buff[buff_count])
			{
				if((buff[buff_count] == 32 || buff[buff_count] == 9) && !check)
				{	
					dict[count].word[i] = 0, i = 0;
					check = 1;
				}
				else if(buff[buff_count] == 13)
				{	
					dict[count].pron[j] = 0;
					// syllabification
					Syllabification(dict[count].pron, count);
					count++, check = 0, j = 0;

				}
				else if(buff[buff_count] == 10)
				{ }
				else
				{		
					if(check == 0)	
					{
						dict[count].word[i++] = buff[buff_count];
					}
					else
					{
						dict[count].pron[j++] = buff[buff_count];
					}
				}
				buff_count++;
			}
		}
		free(buff);
	}
	else
	{
		return FAIL;
	}

	return SUCCESS;
}

// converts English tranliteration into its alternate Urdu alphabet
int ARPA_URDU_Conversion()
{
	long i;
	unsigned int j, l, m, n;
	wchar_t pron[7] = {0}, ipa[5] = {0};

	for(i = 0; i < dict_length; i++)
	{
		m = 0, n = 0;
		for(j = 1; j < dict[i].usyllb_len - 1; j++)
		{
			if(dict[i].usyllb[j] != -1)
			{
				if(dict[i].usyllb[j - 1] == -1)										// syllable initially
				{
					if(wcscmp(conv[dict[i].usyllb[j]].arpa, L"AH") == 0 && j >= 2 && j < dict[i].usyllb_len - 2)
					{
						pron[0] = 1574, pron[1] = 1614, pron[2] = 0; 
					}
					else if(wcscmp(conv[dict[i].usyllb[j]].arpa, L"IH") == 0 && j >= 2 && j < dict[i].usyllb_len - 2)
					{
						pron[0] = 1574, pron[1] = 1616, pron[2] = 0; 
					}
					else
					{
						wcscpy(pron, conv[dict[i].usyllb[j]].urdu_init);	
					}
				}
				else if(dict[i].usyllb[j + 1] == -1)								// syllable finally
				{
					/*if(conv[dict[i].usyllb[j]].type == L'C')
					{
						l = wcslen(pron);
						pron[l++] = 1618, pron[l] = 0;
					}*/

					if((conv[dict[i].usyllb[j]].type == L'L' || conv[dict[i].usyllb[j]].type == L'D') && j >= 2 && j < dict[i].usyllb_len - 2)
					{
						wcscpy(pron, conv[dict[i].usyllb[j]].urdu_mid);
					}
					else
					{
						wcscpy(pron, conv[dict[i].usyllb[j]].urdu_end);			
					}
				}
				else if(dict[i].usyllb[j - 1] != -1 && dict[i].usyllb[j + 1] != -1)	// syllable middially
				{
					wcscpy(pron, conv[dict[i].usyllb[j]].urdu_mid);		
				}

				for(l = 0; pron[l]; l++)
				{
					dict[i].urpron[m++] = pron[l];		
				}

				for(l = 0; conv[dict[i].usyllb[j]].ipa[l]; l++)
				{
					dict[i].ipa[n++] = conv[dict[i].usyllb[j]].ipa[l];
				}
				if(j != wcslen(dict[i].ipa))
				{
					dict[i].ipa[n++] = 32;
				}
			}
		}		
		dict[i].urpron[m] = 0, dict[i].ipa[n] = 0;
	}

	return SUCCESS;
} 

// creates a file of Urdu pronounication dictionary
int Generate_UrDict(char* fileName)		
{
	int i, j = 0;
	long count = 0;
	wchar_t tab[2] = {0x0009}, newline[3] = {0x000D, 0x000A}, syllb[80];

	FILE* file = fopen(fileName, "wb");

	if(file == NULL)
	{
		return FAIL;
	}

	for(i = 0; i < dict_length; i++)
	{
		fwrite(dict[i].word, wcslen(dict[i].word) * sizeof(wchar_t), 1, file);
		fwrite(tab, wcslen(tab) * sizeof(wchar_t), 1, file);
		fwrite(dict[i].pron, wcslen(dict[i].pron) * sizeof(wchar_t), 1, file);
		fwrite(tab, wcslen(tab) * sizeof(wchar_t), 1, file);
		wcscpy(syllb, Syllb_Pron(i));
		fwrite(wcslwr(syllb), wcslen(syllb) * sizeof(wchar_t), 1, file);
		fwrite(tab, wcslen(tab) * sizeof(wchar_t), 1, file);
		fwrite(wcslwr(dict[i].ipa), wcslen(dict[i].ipa) * sizeof(wchar_t), 1, file);
		fwrite(tab, wcslen(tab) * sizeof(wchar_t), 1, file);
		fwrite(dict[i].urpron, wcslen(dict[i].urpron) * sizeof(wchar_t), 1, file);
		fwrite(newline, wcslen(newline) * sizeof(wchar_t), 1, file);
	}

	fclose(file);

	return SUCCESS;
}

///////////////////
// file handling //
///////////////////

// reads a file and place its data in buff array
long Read_File(char* fileName, long size)
{
	int i;

	FILE* file = fopen(fileName, "rb");
	
	if(file == NULL)
	{
		return FAIL;
	}

	for(i = 0; i < size; i++)
	{
		buff[i] = 0;
	}
	
	fread(buff, size, 1, file);
	fclose(file);

	return File_Line(fileName);
} 

// return number of lines in a file
long File_Line(char* fileName)
{
	long len = 0, count = 0;

	if(wcslen(buff))
	{
		while(buff[count])
		{
			if(buff[count] == 13 || buff[count + 1] == 0)
			{
				len++;
			}
			count++;
		}
		return len;		
	}
	else
	{
		return FAIL;
	}
}

// return number of characters in a file
long File_Size(char* fileName)
{
	long size;

	FILE* file = fopen(fileName, "rb");
	
	if(file == NULL)
	{
		return FAIL;
	}

	fseek(file, 0L, SEEK_END);
    size = ftell(file);

    fclose(file);

	return size;
}
