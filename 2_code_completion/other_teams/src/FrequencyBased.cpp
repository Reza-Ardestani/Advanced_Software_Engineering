/*
 *CPSC4210 Fall 2022
 *Author: Kevin Masson
 *Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#include"FrequencyBased.h"
#include"Exceptions.h"
#include"rgxFinds.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

/*
 * Recursive function to get all the file names from a directory.
 * Skips over specified files or folders.
 */
void FrequencyBased::explore(std::string directory) {
  /*
   * Converting directory string into a char array
   */
  int n = directory.length();
  //char dirr[n + 1];
  //strcpy(dirr, directory.c_str());
  const char *dirname;
  dirname = directory.c_str();

  DIR *dir = opendir(dirname);
  if (dir == NULL) {
    return;
  }
  struct dirent *entity;
  entity = readdir(dir);
  while (entity != NULL) {
    const char *nm = entity->d_name;
    setCppFileNames(nm);
    if (entity->d_type == DT_DIR && strcmp(nm, ".") != 0
        && strcmp(nm, "..") != 0 && strcmp(nm, "game") != 0) {
      //char path[100] = { 0 };
      std::string path;
      path.append(dirname);
      path.append("/");
      path.append(entity->d_name);
      explore(path);
    }
    entity = readdir(dir);
  }
  closedir(dir);
}

/*
 * Retrieving all the text from .cpp files, using regex functions to find the method names and frequency
 * they're called, then placing them in a sortable container.
 */
void FrequencyBased::readData(std::string directory) {
  rgxFinds r;

  /*
   * Get file contents
   */
  explore(directory);

  /*
   * Move through the files and extract all the data
   */
  for (int i = 0; i < getCppFileNames().size(); i++) {
    if (getCppFileNames()[i].at(getCppFileNames()[i].size() - 1) == 'p') {
      std::string temp = "";
      std::string file = "";
      std::string temp2 = directory + "//" + getCppFileNames()[i];
      std::ifstream f(temp2);
      while (!f.eof()) {
        f >> temp;
        file += temp + " ";
      }
      setCppFileData(file);
    }
  }

  /*
   * Hard code way of getting the few contents from the main.cpp
   */
  std::string temp = "";
  std::string file = "";
  std::string temp2 = directory + "//game//main.cpp";
  std::ifstream f(temp2);
  while (!f.eof()) {
    f >> temp;
    file += temp + " ";
  }
  setCppFileData(file);

  /*
   * Using regex to find all the method names
   */
  for (unsigned int i = 0; i < cppFileData.size(); i++) {
    r.findAll(cppFileData[i]);
  }

  /*
   * Taking contents from methodData and putting them in a new, sortable container
   */
  for (auto const &i : r.getMethodData()) {
    orderedData.push_back(std::pair<int, std::string>(i.second, i.first));
  }

  /*
   * Sorting the contents (based on frequency)
   */
  std::sort(orderedData.begin(), orderedData.end());
}

/*
 * Compares a query to method names in the code completion database.
 * If the query matches the beginning (or all) of a method name, the
 * method (with it's frequency appended) is added to a vector of strings,
 * which is then returned
 */
std::vector<std::string>* FrequencyBased::getSuggestions(
    const std::string query) {
  // if (query.size() < 3)
  // throw input_error(
  //   "Error: Size of code must be greater than two characters.");

  suggestions->clear();

  for (unsigned int i = 0; i < orderedData.size(); i++) {
    if (orderedData[i].second.compare(query) == 0)
      suggestions->push_back(
          orderedData[i].second + ":" + std::to_string(orderedData[i].first));
  }

  for (int i = orderedData.size() - 1; i >= 0; i--) {
    if (orderedData[i].second.compare(query) == 0)
      continue;
    if (orderedData[i].second[0] == query[0]) {
      bool check = true;
      for (unsigned int j = 1; j < query.length(); j++) {
        if (query[j] != orderedData[i].second[j]) {
          check = false;
          break;
        }
      }
      if (check)
        suggestions->push_back(
            orderedData[i].second + ":" + std::to_string(orderedData[i].first));
    }
    if (suggestions->size() == 7)
      break;
  }
  return suggestions;
}
