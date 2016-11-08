#include "document.h"
#include "text_processor.h"
#include "porter2_stemmer.h"
#include <stack>
#include <algorithm>
#include <cstdio>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#include <set>

#include <chrono> // this is for check execution time
#include <ctime> // this is for check execution time

list<string> Document::stopwords = {};
string Document::outputDirectory = "";
int Document::documentNumber = 0;
map<string, int> Document::wordIds = {{"asd", 0}};
vector<term*> Document::wordList;
int Document::wordId = 0;
term* getWord(string word);

using namespace text_processor;

Document::Document(string _docno, string headline, string text) {
	id = ++documentNumber;
	docno = _docno;
	removePunctuation(headline);
	removePunctuation(text);
	headlineWords = tokenize(headline);
	textWords = tokenize(text);
}

Document::Document(int _id, string _docno, float _denominator) {
	id = _id;
	docno = _docno;
	denominator = _denominator;
}

int Document::getDocumentNumber() {
	return documentNumber;
}

list<string> Document::tokenize(string str) {
	list<string> result;
	char * c_str = strdup(str.c_str());
	char* tokenizer = " -\n\t,.:_()'`\"/{}[]";

	for(char * ptr = strtok(c_str, tokenizer); ptr != NULL; ptr = strtok(NULL, tokenizer)) {
		string temp = string(ptr);
		::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		if(isStopword(stopwords, temp))
			continue;
		result.push_back(temp);
	}

	free(c_str);
	return result;
}


void Document::increaseDocumentFrequency() {
	set<term*>::iterator iter = words.begin();
	while( iter != words.end()) {
		(*iter)->df++;
		iter++;
	}
}


void Document::transform() {
	removeNumberWords(headlineWords);
	removeNumberWords(textWords);
	stem(headlineStems, headlineWords);
	stem(textStems, textWords);

	//headlineWords.unique();
	//textStems.unique();
}

void Document::stem(list<string> &stemList, list<string> words) {
	list<string>::iterator iter = words.begin();
	while( iter != words.end()) {
		string word = *iter;
		Porter2Stemmer::trim(word);
		Porter2Stemmer::stem(word);

		if(!word.empty()) {
			stemList.push_back(word);
			term *temp = getWord(word);
			temp->cf++;;
			temp->tf[id]++;

			this->words.insert(temp);
		}

		iter++;
	}
}

term* getWord(string word) {
	if(Document::wordIds[word] == 0) {
		term *temp = new term;
		temp->id = ++Document::wordId;
		Document::wordIds[word] = Document::wordId;
		temp->str = word;
		Document::wordList.push_back(temp);
		return temp;
	} else {
		return Document::wordList[Document::wordIds[word] - 1];
	}
}

void Document::writeDocInfoFile() {
	ofstream outputFile (outputDirectory + "/doc.dat", ios_base::app);
	outputFile.close();
}

string Document::preFileString() {
	return to_string(id) + "\t" + docno + "\t" + to_string(words.size()) + "\t" + to_string(denominator);
}

string Document::toString() {
	return to_string(id) + "\t" + docno + "\t" + to_string(words.size());
}

void Document::calculateDenominator(float dValue) { // tf idf formula's denominator
	set<term*>::iterator iter = words.begin();
	while( iter != words.end()) {
		denominator += pow((log((*iter)->tf[id]) + 1.0f) * dValue, 2.0f);
		iter++;
	}
	denominator = sqrt(denominator);
}
