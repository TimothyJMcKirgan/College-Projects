// Timmy McKirgan
// 11-05-2022
// Project 1 Problem 3
// Text File Interactive Spellchecker

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void GenerateText(vector<string>, string);
void PopulateDictVector(vector<string>&);
void PrintContext(unsigned, vector<string>);
bool InDictionary(vector<string>, string);
bool GenerateSuggest(vector<string>, string, vector<string>&);
bool OneLetterDif(string, string);
bool Transpose(string, string);
bool AddChar(string, string);
bool RemoveChar(string, string);
bool Continuation(string, string);
char UsersChoice(vector<string>);
string Replacement(vector<string>, char, vector<string>, int);
string AlterReplace(vector<string>, string, int);
string PopulateDocVector(vector<string>&, vector<string>&);

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/05/2022

Parameter Descriptions: N/A

Return Description: 0 if function executes properly

Function Description: Calls functions to populate vectors for dictionary and text file, iterates
over each text file word to check if it's in dictionary. If so, add it to corrected text file. If not,
check if it has already been corrected and add that word to the corrected text file. If not, give user choice
of suggested words, choose their own word, or keep the current word and add that to corrected text file.
*/

int main()
{
    vector<string> DictionaryList(1); // Vector containing the entire selected dictionary.
    vector<string> TextFileList(1);   // Vector containing each word in the text document.
    vector<string> RecommendList(1);  // Vector containing the list of recommended words for mispelled words.
    vector<string> UnalteredText(1);  // Vector containing each word, including characters that were removed.
    vector<string> CorrectedText(1);  // Vector containing each corrected word, used to create corrected text file.
    vector<string> PrevCorrected(1); // Vector containing list of previously corrected words, parallel to PrevMispelled.
    vector<string> PrevMispelled(1); // Vector containing list of previously mispelled words, parallel to PrevCorrected.
    string OriginalWord = "";        // Current word iteration before being altered for capitalization checks.
    string ReplacementWord = "";     // Word that replaces a mispelled word.
    string OutFileName = "";
    char Choice = 'x';
    bool MispelledWords = false; // Boolean representing whether or not mispelled words were present in the text file.
    bool CapFoundWord = false; // Boolean representing whether the capitalized word was found in the dictionary vector.
    bool LowFoundWord = false; // Boolean representing whether the lowercase word was found in the dictionary vector.
    bool AlreadyChecked = false; // Boolean representing whether same word was checked in previous iteration.
    int ListCount = 1;           // Helps output words in recommend list by representing the number in the list.
    int PreviousCount = 0;       // Increments over each previous mispelled word to keep track of where they are.
    int PreviousCheck = 0; // Location of same word in the PrevMispelled vector to find parallel PrevCorrected word.
    CorrectedText.clear();
    PrevMispelled.clear();
    PrevCorrected.clear();
    PopulateDictVector(DictionaryList);
    OutFileName = PopulateDocVector(TextFileList, UnalteredText);
    // For each word in TextFileList, check to see if it's correctly spelled, mispelled but already checked,
    // or mispelled for first time. Then, if it's mispelled, allows user to choose new replacement or use replacement
    // already used for same word.
    for(unsigned i = 0; i < TextFileList.size(); i++) {
        string Word = TextFileList.at(i); // Sets word equal to the current position in the text file list.
        OriginalWord = Word;
        PreviousCount = 0;
        Word[0] = toupper(Word[0]);
        CapFoundWord = InDictionary(DictionaryList, Word);
        Word[0] = tolower(Word[0]);
        LowFoundWord = InDictionary(DictionaryList, Word);
        // If capitalized and lowercase version of word were both not found, continue. Otherwise, append unaltered
        // word to the corrected text vector.
        if(!(CapFoundWord || LowFoundWord)) {
            AlreadyChecked = false;
            // For each previosly mispelled word, check to see if capitalized or lowercase version of new mispelled
            // word is the same as one already mispelled and indicate such with boolean AlreadyChecked.
            for(string MispelledWord : PrevMispelled) {
                Word[0] = tolower(Word[0]);
                // If lowercase word has already been checked, set AlreadyChecked to true and save location within the
                // vector.
                if(Word == MispelledWord) {
                    AlreadyChecked = true;
                    PreviousCheck = PreviousCount;
                }
                Word[0] = toupper(Word[0]);
                // If uppercase word has already been checked, set AlreadyChecked to true and save location within the
                // vector.
                if(Word == MispelledWord) {
                    AlreadyChecked = true;
                    PreviousCheck = PreviousCount;
                }
                PreviousCount++;
            }
            // If the word has already been checked, use already used replacement and alter it in case original word was
            // capitalized or ended with a period or comma, then add it to corrected text vector. Otherwise, generate a
            // suggestion list, and give the user the option to use one of those, write one in, or keep the word as is.
            // Alter that word, then add to corrected text vector.
            if(AlreadyChecked) {
                ReplacementWord = PrevCorrected.at(PreviousCheck);
                ReplacementWord = AlterReplace(UnalteredText, ReplacementWord, i);
                CorrectedText.push_back(ReplacementWord);
                AlreadyChecked = false;
            } else {
                cout << endl;
                PrevMispelled.push_back(OriginalWord);
                MispelledWords = true;
                GenerateSuggest(DictionaryList, OriginalWord, RecommendList);
                cout << "\"" << OriginalWord << "\""
                     << " is not in the dictionary.\n\n";
                PrintContext(i, UnalteredText);
                cout << "Select Action\n---------------\nK: Keep word as is.\nC: Input word to replace.\n";
                // For each word in the recommend list, print it out along with a label, but print 0 instead of 10.
                for(string Word : RecommendList) {
                    // If this is the tenth element of the list, have 0 as it's label. Otherwise, print 1-9.
                    if(ListCount == 10) {
                        cout << "0: " << Word << endl;
                    } else {
                        cout << ListCount << ": " << Word << endl;
                    }
                    ListCount++;
                }
                ListCount = 1;
                cout << endl;
                Choice = UsersChoice(RecommendList);
                ReplacementWord = Replacement(RecommendList, Choice, UnalteredText, i);
                PrevCorrected.push_back(ReplacementWord);
                ReplacementWord = AlterReplace(UnalteredText, ReplacementWord, i);
                CorrectedText.push_back(ReplacementWord);
            }
        } else {
            CorrectedText.push_back(UnalteredText.at(i));
        }
    }
    // If no mispelled words are present in the file, indicate such in an output.
    if(!(MispelledWords)) {
        cout << "No mispelled words.\n";
    }
    cout << "\nDone spell checking the file. Saving new file.\n";
    GenerateText(CorrectedText, OutFileName);
    cout << "Done.";
    return 0;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/04/2022

Parameter Descriptions: vector of corrected word strings, and the filename string accessed for text file to be
corrected.

Return Description: N/A

Function Description: Formats the corrected text vector into a text file with lines no longer
then 80 characters. Preconditions are that the CorrectedText vector is finished filling with words or replacements.
*/

void GenerateText(vector<string> CorrectedText, string FileName)
{
    bool FirstWord = true;                         // Determines whether this is the first word in a new line.
    unsigned LineLength = 0;                       // Represents current length of a line.
    unsigned WordLength = 0;                       // Represents current length of word in current iteration.
    string OutputFileName = "Checked_" + FileName; // Output file to send corrected text too.
    ofstream Output;                               // Represents output file.
    string Line = "";                              // Represents current line being generated.
    string CurrentWord = ""; // Represents previous word iteration to save it while a line is being output.
    Output.open(OutputFileName);
    // For each word in the corrected text vector, check to see if it plus current line is greater than 80 characters,
    // if so, end the line but save current word iteration to add to next one. Otherwise, add word to line.
    for(string Word : CorrectedText) {
        LineLength = Line.length();
        WordLength = Word.length();
        // If this is the first word of any other line, set the line equal to the word saved at the end of the last
        // line, and set FirstWord to false.
        if(FirstWord == true && CurrentWord != "") {
            Line = CurrentWord;
            FirstWord = false;
        }
        // If this is the first word of the first line and CurrentWord hasnt been set yet, set line equal to word, and
        // set FirstWord to false. If this is any other word in the line, go through else statement.
        if(FirstWord == true && CurrentWord == "") {
            Line = Word;
            FirstWord = false;
        } else {
            // If current line plus a space plus current word is less than or equal to 80, append a space and next word
            // to the line.
            if((LineLength + WordLength + 1) <= 80) {
                Line = Line + " " + Word;
                // If current line plus a space plus current word is greater than 80, send the line to output file,
                // then set line to blank, save the word, and set FirstWord to true to indicate start of new line.
            } else if((LineLength + WordLength + 1) > 80) {
                Output << Line << endl;
                Line = "";
                CurrentWord = Word;
                FirstWord = true;
            }
        }
    }
    // If at the end there exists a line that wasn't long enough to output, output it now.
    if(Line != "") {
        Output << Line << endl;
    }
    Output.close();
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/05/2022

Parameter Descriptions: Vector of recommended replacements, choice character, vector containing all unaltered text,
and int i to represent the location of the unaltered text vector.

Return Description: string representing the chosen replacement word.

Function Description: Using user's choice char, selects a replacement word or gives user option to write one in.
Precondition is that the user chose a word to replace the current word with, and that choice has already been error
checked so that no invalid vector positions can be chosen.
*/

string Replacement(vector<string> RecommendList, char Choice, vector<string> UnalteredText, int i)
{
    string CurrentWord = UnalteredText.at(i); // Represents word already in text file
    string ReplaceWord = "";                  // Return value representing the replacement.
    // Switch statement determining which word is the replacement.
    switch(Choice) {
    // If user chose 1, use word at 0th element of the recommend vector.
    case '1': {
        ReplaceWord = RecommendList.at(0);
        break;
    }
    // If user chose 2, use word at 1st element of the recommend vector.
    case '2': {
        ReplaceWord = RecommendList.at(1);
        break;
    }
    // If user chose 3, use word at 2nd element of the recommend vector.
    case '3': {
        ReplaceWord = RecommendList.at(2);
        break;
    }
    // If user chose 4, use word at 3rd element of the recommend vector.
    case '4': {
        ReplaceWord = RecommendList.at(3);
        break;
    }
    // If user chose 5, use word at 4th element of the recommend vector.
    case '5': {
        ReplaceWord = RecommendList.at(4);
        break;
    }
    // If user chose 6, use word at 5th element of the recommend vector.
    case '6': {
        ReplaceWord = RecommendList.at(5);
        break;
    }
    // If user chose 7, use word at 6th element of the recommend vector.
    case '7': {
        ReplaceWord = RecommendList.at(6);
        break;
    }
    // If user chose 8, use word at 7th element of the recommend vector.
    case '8': {
        ReplaceWord = RecommendList.at(7);
        break;
    }
    // If user chose 9, use word at 8th element of the recommend vector.
    case '9': {
        ReplaceWord = RecommendList.at(8);
        break;
    }
    // If user chose 0, use word at 9th element of the recommend vector.
    case '0': {
        ReplaceWord = RecommendList.at(9);
        break;
    }
    // If user chose K, use unaltered word from input text file.
    case 'K': {
        ReplaceWord = CurrentWord;
        break;
    }
    // If user chose C, use word user enters in as replacement.
    case 'C': {
        cout << "Input replacement word: ";
        cin >> ReplaceWord;
        break;
    }
    }
    return ReplaceWord;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/05/2022

Parameter Descriptions: string vector with unaltered words from text file vector, replacement word string, and int i
representing position within the unaltered text vector.

Return Description: returns altered replacement word string based on if the original word was capitalized, or ended with
a . or , so that the final text document has proper sentence structure.

Function Description: Takes the replacement word, capitalizes it if original word was capitalized, appends a ,
if original word ended with a , and appends a . if original word ended with a .
Precondition is that a replacement word has been chosen. Post Condition is that the replacement word is
now formatted to fit into a sentence in the place of the original word.
*/

string AlterReplace(vector<string> UnalteredText, string ReplacementWord, int i)
{
    string CurrentWord = UnalteredText.at(i);                           // Represents word used in original text file.
    char FirstCharCur = CurrentWord[0];                                 // First character of current word.
    char LastCharCur = CurrentWord[(CurrentWord.length() - 1)];         // Last character of current word.
    char LastCharRep = ReplacementWord[(ReplacementWord.length() - 1)]; // Last character of replacement word.
    // If the first character of the original word was capitalized, capitalize the replacement.
    if(FirstCharCur >= 'A' && FirstCharCur <= 'Z') {
        ReplacementWord[0] = toupper(ReplacementWord[0]);
    }
    // If the first character of the original word was in lowercase, make the replacement lowercase.
    if(FirstCharCur >= 'a' && FirstCharCur <= 'z') {
        ReplacementWord[0] = tolower(ReplacementWord[0]);
    }
    // If the last character of the original had a . and the last character of the current word doesnt already end with
    // a . append a . to the replacement.
    if(LastCharCur == '.' && LastCharRep != '.') {
        ReplacementWord += ".";
    }
    // If the last character of the original has a , and the last character of the current word doesnt already end with
    // a , append a , to the replacement.
    if(LastCharCur == ',' && LastCharRep != ',') {
        ReplacementWord += ",";
    }
    return ReplacementWord;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/04/2022

Parameter Descriptions: String vector containing list of recommendations for replacements.

Return Description: Returns a char representing the user's choice in what to do with the word.

Function Description: Allows the user to enter their choice and error checks to make sure that choice is a valid one
given the size of the recommendation list. Precondition is that recommend list has been populated with no more than 10
words.
*/

char UsersChoice(vector<string> RecommendList)
{
    char UserChoice; // Represents the choice the user makes for how to change the word.
    // If recommend list is maximum size, allow the user to enter any choice of C, K, or 0-9.
    // Otherwise, allow the user to enter a choice of K, C, and anywhere from 1 to the
    // char representing the size of the vector.
    if(RecommendList.size() == 10) {
        // Prompt user for choice and repeat if entry is invalid.
        do {
            cout << "Selection: ";
            cin >> UserChoice;
            UserChoice = toupper(UserChoice);
            // If entry is invalid, output message indicating that.
            if(UserChoice != 'K' && UserChoice != 'C' && (UserChoice < '0' || UserChoice > '9')) {
                cout << "Invalid Input: Try Again.\n";
            }
        } while(UserChoice != 'K' && UserChoice != 'C' && (UserChoice < '0' || UserChoice > '9'));
    } else {
        // Prompt user for choice and repeat if entry is invalid.
        do {
            cout << "Selection: ";
            cin >> UserChoice;
            UserChoice = toupper(UserChoice);
            // If entry is invalid, output message indicating that.
            if(UserChoice != 'K' && UserChoice != 'C' &&
                (UserChoice < '1' || UserChoice > (RecommendList.size() + '0'))) {
                cout << "Invalid Input: Try Again.\n";
            }
        } while(
            UserChoice != 'K' && UserChoice != 'C' && (UserChoice < '1' || UserChoice > (RecommendList.size() + '0')));
    }
    return UserChoice;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/03/2022

Parameter Descriptions: string vector with unaltered words and unsigned int i representing position within that vector.

Return Description: N/A

Function Description: Generates the context output based on where in the vector the word is.
*/

void PrintContext(unsigned i, vector<string> UnalteredText)
{
    // If this is the first word, output the word and 4 words after.
    if(i == 0) {
        cout << "Context: <<< " << UnalteredText.at(i) << " >>> " << UnalteredText.at(i + 1) << " "
             << UnalteredText.at(i + 2) << " " << UnalteredText.at(i + 3) << " " << UnalteredText.at(i + 4) << endl
             << endl;
        // If this is the second word, output 1 word before, the word, and 4 words after.
    } else if(i == 1) {
        cout << "Context:  " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << " " << UnalteredText.at(i + 3) << " "
             << UnalteredText.at(i + 4) << endl
             << endl;
        // If this is the third word, output 2 words before, the word, and 4 words after.
    } else if(i == 2) {
        cout << "Context:  " << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< "
             << UnalteredText.at(i) << " >>> " << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << " "
             << UnalteredText.at(i + 3) << " " << UnalteredText.at(i + 4) << endl
             << endl;
        // If this is the fourth word, output 3 words before, the word, and 4 words after.
    } else if(i == 3) {
        cout << "Context:  " << UnalteredText.at(i - 3) << " " << UnalteredText.at(i - 2) << " <<< "
             << UnalteredText.at(i) << " >>> " << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << " "
             << UnalteredText.at(i + 3) << " " << UnalteredText.at(i + 4) << endl
             << endl;
        // If this is the last word, output 4 words before, and then the word.
    } else if(i == (UnalteredText.size() - 1)) {
        cout << "Context:  " << UnalteredText.at(i - 4) << " " << UnalteredText.at(i - 3) << " "
             << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << endl
             << endl;
        // If this is the second to last word, output 4 words before, the word, and 1 word after.
    } else if(i == (UnalteredText.size() - 2)) {
        cout << "Context:  " << UnalteredText.at(i - 4) << " " << UnalteredText.at(i - 3) << " "
             << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << UnalteredText.at(i + 1) << endl
             << endl;
        // If this is the third to last word, output 4 words before, the word, and 2 words after.
    } else if(i == (UnalteredText.size() - 3)) {
        cout << "Context:  " << UnalteredText.at(i - 4) << " " << UnalteredText.at(i - 3) << " "
             << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << endl
             << endl;
        // If this is the fourth to last word, output 4 words before, the word, and 3 words after.
    } else if(i == (UnalteredText.size() - 4)) {
        cout << "Context:  " << UnalteredText.at(i - 4) << " " << UnalteredText.at(i - 3) << " "
             << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << " " << UnalteredText.at(i + 3) << endl
             << endl;
        // If this is in any other position, output 4 words before, the word, and 4 words after.
    } else {
        cout << "Context:  " << UnalteredText.at(i - 4) << " " << UnalteredText.at(i - 3) << " "
             << UnalteredText.at(i - 2) << " " << UnalteredText.at(i - 1) << " <<< " << UnalteredText.at(i) << " >>> "
             << UnalteredText.at(i + 1) << " " << UnalteredText.at(i + 2) << " " << UnalteredText.at(i + 3) << " "
             << UnalteredText.at(i + 4) << endl
             << endl;
    }
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/01/2022

Parameter Descriptions: Vector (not yet) containing a list of words in the chosen dictionary passed by reference
to populate the vector.

Return Description: N/A

Function Description: Populates the dictionary vector with words from the chosen dictionary file.
*/
void PopulateDictVector(vector<string>& WordList)
{
    WordList.clear();
    string WordFileName = ""; // User input file name directing the program to open a specific file.
    string DictWord = "";     // Variable storing each word before capitalizing and placing it in vector.
    ifstream WordFile;        // Variable given to represent the dictionary file.
    // If the file did not open properly, repeat the prompt.
    do {
        cout << "Input dictionary filename: ";
        cin >> WordFileName;
        WordFile.open(WordFileName);
        // If the file could not be opened, send output indicating such.
        if(!(WordFile)) {
            cout << "File could not be accessed or does not exist. Try again.\n";
        }
    } while(!(WordFile));
    // For each word in the dictionary file, capitalize it and append the word to the word list.
    while(WordFile >> DictWord) {
        WordList.push_back(DictWord);
    }
    sort(WordList.begin(), WordList.end());
}
/*
Function Author: Timmy McKirgan

Date of Last Update: 11/02/2022

Parameter Descriptions: Vector (not yet) containing a list of words in the chosen text file.

Return Description: String containing the name of the text file accessed.

Function Description: Populates the text file vector with words from the chosen text file as well as the
unaltered words vector. Returns file name string.
*/
string PopulateDocVector(vector<string>& WordList, vector<string>& UnalteredWords)
{
    UnalteredWords.clear();
    WordList.clear();
    string WordFileName = ""; // User input file name directing the program to open a specific file.
    string DocWord = "";      // Variable storing each word before capitalizing and placing it in vector.
    ifstream WordFile;        // Variable given to represent the text file.
    // If the file did not open properly, repeat the prompt.
    do {
        cout << "Input document filename: ";
        cin >> WordFileName;
        WordFile.open(WordFileName);
        // If the file could not be opened, send output indicating such.
        if(!(WordFile)) {
            cout << "File could not be accessed or does not exist. Try again.\n";
        }
    } while(!(WordFile));
    // For each word in the text file, capitalize it, remove non alphabetical characters except for " ' " and "-"
    // and append the word to the word list.
    while(WordFile >> DocWord) {
        UnalteredWords.push_back(DocWord);
        char ch;
        // For each character in the word, remove it if it's not alphabetical, " ' " , or "-".
        for(unsigned i = 0; i < DocWord.length(); i++) {
            ch = DocWord.at(i);
            // remove character if it's not alphabetical, " ' " , or "-". Then go back by one iterator so no characters
            // are skipped.
            if(!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '\'' || ch == '-')) {
                DocWord.replace(i, 1, "");
                i--;
            }
        }
        // If the removal of characters entirely deleted the word, don't add it to the vector. Also remove it from
        // unaltered words vector
        // so that the two vectors can be parallel.
        if(DocWord.length() > 0) {
            WordList.push_back(DocWord);
        } else {
            UnalteredWords.pop_back();
        }
    }
    return WordFileName;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/01/2022

Parameter Descriptions: Vector containing all of the words in the dictionary and the word being searched for.

Return Description: Returns the boolean DictWordFound to determine whether the search word was in the dictionary.

Function Description: Utilizes a binary search on the dictionary vector to find the search word. If it is found,
DictWordFound is set to true, and returns true. Otherwise, the function returns false. Precondition is that WordList is
sorted by ASCII.
*/
bool InDictionary(vector<string> WordList, string SearchWord)
{
    bool DictWordFound = false;       // Return boolean determining whether search word is in dictionary.
    int first = 0;                    // First within search range used for binary search
    int last = (WordList.size() - 1); // Last within search range used for binary search
    int middle = 0;                   // Middle of search range used for binary search.
    // Binary search for the search word over the dictionary vector.
    while(DictWordFound == false && first <= last) {
        middle = ((first + last) / 2);
        // If the middle if the vector is the search word, set DictWordFound to true.
        // If the middle word is greater than the search word on ASCII table, set the last word to the word before the
        // middle. If the middle word is less than the search word on ASCII table, set the first word to the word after
        // the middle.
        if(WordList.at(middle) == SearchWord) {
            DictWordFound = true;
        } else if(WordList.at(middle) > SearchWord) {
            last = middle - 1;
        } else {
            first = middle + 1;
        }
    }
    return DictWordFound;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 11/01/2022

Parameter Descriptions: Word suggestion vector passed by reference, word list vector from dictionary, and the mispelled
word.

Return Description: Returns a boolean WordsSuggested representing whether or not any words were suggested at all.

Function Description: Iterates through dictionary and Calls functions to determine possible suggestions, if a suggestion
occurs twice it will ignore the suggestion the second time. Precondition is that the search word is not in the
dictionary.
*/
bool GenerateSuggest(vector<string> WordList, string SearchWord, vector<string>& WordSuggest)
{
    WordSuggest.clear();
    bool SameWord = false;       // Boolean variable representing whether a word is already in the suggestion vector.
    bool WordsSuggested = false; // Return value indicating whether or not words were suggested.
    // Iterates over the word list checking to see if they could be recommended in place of the search word.
    for(string TargetWord : WordList) {
        /*
        if the target word results from transposing adjacent letters from the search word, adding a letter,
        removing a letter, adding letters to the end, of changing one of the letters, continue.
        */
        if(OneLetterDif(SearchWord, TargetWord) || Transpose(SearchWord, TargetWord) ||
            AddChar(SearchWord, TargetWord) || RemoveChar(SearchWord, TargetWord) ||
            Continuation(SearchWord, TargetWord)) {
            // Iterates over each word in the suggestion vector to compare to the new potential suggestion.
            for(string word : WordSuggest) {
                // If the new suggestion is one of the words in the vector, set SameWord to true.
                if(TargetWord == word)
                    SameWord = true;
            }
            // Only add the new suggestion if SameWord stays false, or, in other words, if the new word doesn't
            // equal any of the other words.
            if(SameWord == false && WordSuggest.size() < 10) {
                WordSuggest.push_back(TargetWord);
                WordsSuggested = true;
            }
            SameWord = false;
        }
    }
    return WordsSuggested;
}
/*
Function Author: Timmy McKirgan

Date of Last Update: 10/27/2022

Parameter Descriptions:
Search is the word input from the user to compare to the target word.
Target is the word from the vector containing the entire dictionary.

Return Description: A boolean value OneDif determining whether or not the target word is different
from the search word through changing a single letter.

Function Description: Checks that the search and target words are both the same length, then counts
each letter in both words whose indexes correspond to check if they different. If only one character is
different, than the function returns true. Precondition is that this function operates under the assumption that
Search does not equal Target.
*/
bool OneLetterDif(string Search, string Target)
{
    bool OneDif = true; // Return bool variable marking if the words are different by one character.
    int DifCount =
        0; // Counts the number of times the characters in Search and Target with the same index are different.
    // If the search word and the target word are the same length, continue, otherwise, set OneDif to false.
    if(Search.length() == Target.length()) {
        // Iterates through each letter in both the search and target words using their shared index numbers.
        for(unsigned i = 0; i < Search.length(); i++) {
            // If the two characters at any given index are not the same, increment DifCount.
            if(Search.at(i) != Target.at(i)) {
                DifCount++;
            }
        }
        // If there is more than one difference in character, set OneDif to false. Otherwise, OneDif remains true.
        if(DifCount > 1) {
            OneDif = false;
        }
    } else {
        OneDif = false;
    }
    return OneDif;
}

/*
Function Author: Timmy McKirgan

Date of Last Update: 10/27/2022

Parameter Descriptions:
Search is the word input from the user to compare to the target word.
Target is the word from the vector containing the entire dictionary.

Return Description: The return is a boolean value AdjSwap determining whether or not the target word is the
search word with two adjacent letters swapped

Function Description:
Swaps the first and second letter of the search word, determines if it is equal to the target word, then resets the
search word. Rinse and repeat for words 2 and 3, 3 and 4, 4 and 5, etc. If any of these result in
Search and Target being equal, then the return value AdjSwap is set to true, thus the function returns true.
Otherwise the function returns false. Precondition is that this function operates under the assumption that Search
does not equal Target.
*/
bool Transpose(string Search, string Target)
{
    char Temp = 'a';      // Temp variable used for swapping.
    bool AdjSwap = false; // Return boolean dictating whether or not the target word is the search word but transposed.
    string OriginalSearch = Search; // Sets a value so that Search can be reset after each swap and comparison.
    // If the two words are the same length, continue.
    if(Search.length() == Target.length() && Search.length() != 1) {
        // Iterates over the length of the words minus 1 swapping each character with the following character
        // Before checking to see if the new swapped word equals the target word. After this check is performed,
        // search is reset to it's original value.
        for(unsigned i = 0; i < (Search.length() - 1); i++) {
            Temp = Search.at(i);
            Search.at(i) = Search.at(i + 1);
            Search.at(i + 1) = Temp;
            // If at any point a swapped Search word equals the target word, set AdjSwap to true.
            if(Search == Target) {
                AdjSwap = true;
            }
            Search = OriginalSearch;
        }
    }
    return AdjSwap;
}
/*
Function Author: Timmy McKirgan

Date of Last Update: 10/27/2022

Parameter Descriptions:
Search is the word input from the user to compare to the target word.
Target is the word from the vector containing the entire dictionary.

Return Description: Boolean variable AddedChar dictates whether the target word is the search word but with one
letter added or not.

Function Description: Iterates through each letter in Target, removing it then checking to see if it is then equal
to Search before resetting the target word. if Search equals Target in any of these loop iterations, the target word
is just one letter added to the search, thus AddedChar is set to true. Precondition is that this function operates
under the assumption that Search does not equal Target.
*/
bool AddChar(string Search, string Target)
{
    string OriginalTarget = Target; // OriginalTarget allows Target to be reset after a character is removed.
    bool AddedChar =
        false; // Return value displaying whether or not the target word is the search word with a character added.
    // Iterates over each letter in the target word, removing that letter, checking for equality with Search word,
    // then resetting target word.
    for(unsigned i = 0; i < Target.length(); i++) {
        Target.replace(i, 1, "");
        // If in one iteration of the loop, the target word equals the search word, set AddedChar to true, otherwise
        // it remains false for the entirety of the function and the function returns false.
        if(Target == Search) {
            AddedChar = true;
        }
        Target = OriginalTarget;
    }
    return AddedChar;
}
/*
Function Author: Timmy McKirgan

Date of Last Update: 10/27/2022

Parameter Descriptions:
Search is the word input from the user to compare to the target word.
Target is the word from the vector containing the entire dictionary.

Return Description: Boolean variable RemovedChar dictates whether the search word is the target word but with one
letter added or not.

Function Description: Iterates through each letter in Search, removing it then checking to see if it is then equal
to Target before resetting the search word. if Target equals Search in any of these loop iterations, the search word
is just one letter added to the target, thus RemovedChar is set to true. Precondition is that this function operates
under the assumption that Search does not equal Target.
*/
bool RemoveChar(string Search, string Target)
{
    string OriginalSearch = Search; // OriginalSearch allows Search to be reset after a character is removed.
    bool RemovedChar =
        false; // Return value displaying whether or not the search word is the target word with a character added.
    // Iterates over each letter in the search word, removing that letter, checking for equality with Target word,
    // then resetting search word.
    for(unsigned i = 0; i < Search.length(); i++) {
        Search.replace(i, 1, "");
        // If in one iteration of the loop, the search word equals the target word, set RemovedChar to true,
        // otherwise it remains false for the entirety of the function and the function returns false.
        if(Search == Target) {
            RemovedChar = true;
        }
        Search = OriginalSearch;
    }
    return RemovedChar;
}
/*
Function Author: Timmy McKirgan

Date of Last Update: 10/27/2022

Parameter Descriptions:
Search is the word input from the user to compare to the target word.
Target is the word from the vector containing the entire dictionary.

Return Description: Boolean return variable Cont determines whether or not the target word is a continuation of the
search word.

Function Description: Sets the return to false by default. Searches the target word for the search word. Boolean
value is only set to true if the search word is found at the very beginning of the target word. Precondition is that
this function operates under the assumption that Search does not equal Target.
*/
bool Continuation(string Search, string Target)
{
    bool Cont = false; // Boolean return variable determining whether Target word is a continuation of Search Word.
    int SearchLoc = Target.find(Search); // Location of Search word within target Word. Used to determine if Search
                                         // word is at the start of Target word.
    // If search word is found at the start of target word, Cont is true and the function returns true. Otherwise
    // false is returned.
    if(SearchLoc == 0)
        Cont = true;
    return Cont;
}