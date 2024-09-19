/*
 * testFrequencyBased.cpp
 *
 *  Created on: Oct. 7, 2022
 *      Author: Kevin
 *
 *  Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#include <stdexcept>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include"rgxFinds.h"
#include"Exceptions.h"
#include"FrequencyBased.h"
#include"gtest/gtest.h"

TEST(TestFrequencyBased, TestreadData) {
  FrequencyBased *f = new FrequencyBased();
  f->readData("data//chess//src");
  EXPECT_NE(f->getOrderedData().size(), 0);
  EXPECT_EQ(f->getOrderedData()[f->getOrderedData().size() - 1].second, "getX");
  delete f;
}

TEST(TestFrequencyBased, TestgetSuggestions) {
  FrequencyBased *f = new FrequencyBased();
  std::vector<std::string> *v;
  std::string s = "fo";
  EXPECT_THROW(f->getSuggestions(s), input_error);
  f->readData("data//chess//src");
  v = f->getSuggestions("get");
  EXPECT_EQ(v->at(0), "getX:70");
  EXPECT_EQ(v->at(1), "getY:62");
  EXPECT_EQ(v->at(2), "getSide:12");
  EXPECT_EQ(v->at(3), "getPiece:4");
  v = f->getSuggestions("getY");
  EXPECT_EQ(v->at(0), "getY:62");
  EXPECT_EQ(v->size(), 1);
  delete f, v;
}
