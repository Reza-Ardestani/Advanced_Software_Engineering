/*
 *CPSC4210 Fall 2022
 *Author: Kevin Masson
 *Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#ifndef INCLUDE_RGXFINDS_H_
#define INCLUDE_RGXFINDS_H_

#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include "FrequencyBased.h"

/*
 * Each function defines a different regex pattern for
 * extracting method data. Method names explain their
 * particular use.
 */

class rgxFinds {
 public:
  rgxFinds() {
  }
  /*
   * Regex Function that looks for "New " cases
   * @param s An input string of data
   */
  void findNew(std::string s);
  /*
   * Regex Function that looks for "->" cases
   * @param s An input string of data
   */
  void findArrow(std::string s);
  /*
   * Regex Function that looks for "." cases
   * @param s An input string of data
   */
  void findPeriod(std::string s);
  /*
   * Regex Function that looks for "=" cases
   * @param s An input string of data
   */
  void findEqualSign(std::string s);
  /*
   * Regex Function that looks for "(" cases
   * @param s An input string of data
   */
  void findBraceSandwhich(std::string s);
  /*
   * Regex Function that looks for "&&" cases
   * @param s An input string of data
   */
  void findAmpersandSpace(std::string s);
  /*
   * Regex Function that looks for "throw " cases
   * @param s An input string of data
   */
  void findThrows(std::string s);
  /*
   * Regex Function that finds the single std::runtime_error
   * @param s An input string of data
   */
  void findruntime_error(std::string s);
  /*
   * Regex Function that looks for "::" cases
   * @param s An input string of data
   */
  void findColon(std::string s);
  /*
   * Regex Function that looks for dynamic cast cases
   * @param s An input string of data
   */
  void findDC(std::string s);
  /*
   * Regex Function that looks for dynamic cast cases
   * @param s An input string of data
   */
  void findDC2(std::string s);
  /*
   * Regex Function that will run all regex case methods
   * @param s An input string of data
   */
  void findAll(std::string s);
  /*
   * Getter for the final methodData map
   * @return Map of the frequency data of method calls
   */
  std::map<std::string, int> getMethodData() {
    return methodData;
  }

 private:
  /*
   * Private methodData
   * Records method frequency in a map
   */
  std::map<std::string, int> methodData;
};

#endif
