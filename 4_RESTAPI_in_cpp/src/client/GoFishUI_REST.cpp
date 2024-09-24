/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca)
 */

#include <limits>
#include <string>
#include "rest/GoFishUI_REST.h"

unsigned int GoFishUI_REST::choosePlayer(unsigned int playerNum,
                                         unsigned int numPlayers) {
  unsigned int selection = playerNum;
  std::cout << "You are Player #" << playerNum << std::endl;
  while (selection == playerNum || selection >= numPlayers) {
    std::cout << "Which player (0-" << numPlayers - 1 << ")? ";
    std::cin >> selection;
    if (selection == playerNum)
      std::cout << "That is you! Please choose another player." << std::endl;
    if (selection >= numPlayers) {
      std::cout << "Player #" << selection
          << " doesn't exist! Please choose another player." << std::endl;
    } else if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');
      std::cout << "Invalid entry: no characters. Try again: \n";
    }
  }
  return selection;
}

unsigned int GoFishUI_REST::requestCard(nlohmann::json hand) {
  std::cout << "Your hand:" << std::endl;
  unsigned int index = 0;
  for (nlohmann::json::iterator card = hand.begin(); card != hand.end();
      ++card) {
    Card::Rank r = (*card)["rank"];
    std::cout << "\tChoice " << (++index) << ": " << Card::getRank(r)
        << std::endl;
  }
  std::cout << "Select a choice: ";
  unsigned int selection;
  bool check = false;
  while (!check) {
    std::cin >> selection;
    if (selection > 0 && selection <= hand.size()) {
      check = true;
    } else if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');
      std::cout << "Invalid entry: no characters. Try again: \n";
    } else {
      std::cout << "Invalid entry: out of range. Try again: \n";
    }
  }
  return selection - 1;
}

void GoFishUI_REST::playFailed() {
  std::cout << "Go Fish!" << std::endl;
}

void GoFishUI_REST::playSucceeded() {
  std::cout << "The player had some of those cards!" << std::endl;
}

void GoFishUI_REST::showScores(nlohmann::json players) {
  std::cout << "---- Scores ----" << std::endl;
  for (nlohmann::json::iterator player = players.begin();
      player != players.end(); ++player) {
    std::string name = (*player)["name"];
    unsigned int score = (*player)["score"];
    std::cout << name << ": " << score << std::endl;
  }
}
