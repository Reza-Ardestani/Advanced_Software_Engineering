/*
 *CPSC4210 Fall 2022
 *Author: Kevin Masson
 *Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#ifndef INCLUDE_FREQUENCYBASED_H_
#define INCLUDE_FREQUENCYBASED_H_

#include<dirent.h>
#include<map>
#include<fstream>
#include<string>
#include<vector>
#include<utility>
#include"CodeCompletion.h"

class FrequencyBased : public CodeCompletion {
 public:
  /**
   * Constructor for FrequencyBased class.
   */
  FrequencyBased() {
    suggestions = new std::vector<std::string>;
  }
  /**
   * Destructor for FrequencyBased class.
   */
  ~FrequencyBased() {
    delete suggestions;
  }

  /**
   * Read files in a given directory (and sub-directories)
   * to populate the code completion database.
   * @param directory Relative path to the directory.
   */
  void readData(std::string directory);

  /**
   * Method of inputting a query and returning the top 5 ordered suggestions.
   * @param query A string with which to query the code completion system.
   * @return A pointer to an ordered list of suggestions for the query in descending order of
   * relevance. The maximum number of suggestions is 7.
   */
  std::vector<std::string>* getSuggestions(const std::string query);

  /**
   * Method for extracting the file data from a directory.
   * @param directory Input string of directory path
   */
  void explore(std::string directory);

  /*
   * Getter for the cppFileData
   * @return cppFileData
   */
  std::vector<std::string> getCppFileData() {
    return cppFileData;
  }
  /*
   * Getter for the cppFileNames
   * @return cppFileNames
   */
  std::vector<std::string> getCppFileNames() {
    return cppFileNames;
  }
  /*
   * Setter for the cppFileNames
   * @param name Name for the file being inputted
   */
  void setCppFileNames(std::string name) {
    cppFileNames.push_back(name);
  }
  /*
   * Setter for the cppFileData
   * @param name Data for the file being inputted
   */
  void setCppFileData(std::string file) {
    cppFileData.push_back(file);
  }
  /*
   * Getter for the orderedData
   * @return Vector of ordered data based on frequency
   */
  std::vector<std::pair<int, std::string>> getOrderedData() {
    return orderedData;
  }

 private:
  std::vector<std::pair<int, std::string>> orderedData;
  std::vector<std::string> cppFileNames;
  std::vector<std::string> cppFileData;
  std::vector<std::string> *suggestions;
};

#endif
