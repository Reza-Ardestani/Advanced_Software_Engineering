/*
 *CPSC4210 Fall 2022
 *Author: Kevin Masson
 *Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#include <string>
#include "rgxFinds.h"

void rgxFinds::findNew(std::string s) {
  std::regex pattern("(new\\s)\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 4);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

void rgxFinds::findArrow(std::string s) {
  std::regex pattern("(->)\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 2);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

void rgxFinds::findPeriod(std::string s) {
  std::regex pattern("\\.\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 1);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

void rgxFinds::findEqualSign(std::string s) {
  std::regex pattern("(=\\s)\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 2);
    temp.erase(temp.end() - 1);

    if (temp != "typeid")
      methodData[temp]++;
  }
}

void rgxFinds::findBraceSandwhich(std::string s) {
  std::regex pattern("[(]\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 1);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

void rgxFinds::findAmpersandSpace(std::string s) {
  std::regex pattern("&&\\s\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 3);
    temp.erase(temp.end() - 1);

    if (temp != "typeid")
      methodData[temp]++;
  }
}

void rgxFinds::findThrows(std::string s) {
  std::regex pattern("throw\\s\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 6);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

void rgxFinds::findruntime_error(std::string s) {
  std::regex pattern("throw std::runtime_error");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 11);

    methodData[temp]++;
  }
}

void rgxFinds::findColon(std::string s) {
  std::regex pattern(":\\s\\w+[(]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 2);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

/*
 * DC is dynamic_cast
 */
void rgxFinds::findDC(std::string s) {
  std::regex pattern("[=|?]\\s\\w+[<]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 2);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

/*
 * DC is dynamic_cast
 */
void rgxFinds::findDC2(std::string s) {
  std::regex pattern("&&\\s\\W\\w+[<]");
  auto start = std::sregex_iterator(s.begin(), s.end(), pattern);
  auto end = std::sregex_iterator();
  for (auto match = start; match != end; match++) {
    std::smatch m = *match;

    std::string temp = m[0];
    temp.erase(0, 4);
    temp.erase(temp.end() - 1);

    methodData[temp]++;
  }
}

/*
 * Calls each defined regex patterns.
 */
void rgxFinds::findAll(std::string s) {
  findNew(s);
  findArrow(s);
  findPeriod(s);
  findEqualSign(s);
  findBraceSandwhich(s);
  findAmpersandSpace(s);
  findThrows(s);
  findruntime_error(s);
  findColon(s);
  findDC(s);
  findDC2(s);
}

