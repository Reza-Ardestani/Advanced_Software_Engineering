// Minimal source file that allows the pipeline to pass.
// Copyright 2022 Matthew Ursaki and Kevin Masson

#include<iostream>
#include"Exceptions.h"
#include"FrequencyBased.h"
#include"rgxFinds.h"
#include<vector>
#include<string>

int main() {
  std::string in = "", end;
  FrequencyBased *f = new FrequencyBased();
  std::vector<std::string> *suggestions;
  f->readData("data//chess//src");

  std::cout << "\nWelcome to Code Completion! \n \n";

  while (1) {
  try {
    std::cout << "Input code to complete (or 'END!' to quit) \n";
    std::cin >> in;
    if (in.size() <3)
    throw input_error("Error: size of input must be greater than 2\n");
    if (in == "END!")
    break;

    suggestions = f->getSuggestions(in);
    if (suggestions->size() != 0) {
      std::cout << "Here are your suggestions: \n";

      for (unsigned int i = 0; i < suggestions->size(); i++)
      std::cout << suggestions->at(i) << "\n";
    } else {
      std::cout << "No suggestions! \n";
    }
    std::cout << "\n";
  }
  catch () {
    continue;
  }
}

std::cout << "Goodbye! \n";
delete f, suggestions;
return 0;
}
