/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca), Kevin Masson, Matthew Ursaki,
 * Mohammadreza Ardestani
 */


#include <restbed>
#include <memory>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "rest/rest_utils.h"
#include "rest/GoFishUI_REST.h"

static const bool DEBUG = false;

bool askPlayer(unsigned int turnPlayer, unsigned int askPlayer,
               unsigned int askRank) {
  // Ask player
  std::string askPlayerStr, askRankStr, turnPlayerStr;
  askPlayerStr = std::to_string(askPlayer);
  askRankStr = std::to_string(askRank);
  turnPlayerStr = std::to_string(turnPlayer);
  std::string endpoint = "/ask_Card/" + askPlayerStr + '/' + askRankStr;

  auto request = createRequest(endpoint);
  auto response = restbed::Http::sync(request);
  auto result = handle_response(response);

  nlohmann::json found = nlohmann::json::parse(result);

  if (found.size() > 0) {
    // Move card between players
    for (unsigned int i = 0; i < found.size(); i++) {
      int r = found[i]["rank"];
      int s = found[i]["suit"];
      request = createRequest(
          "add_Card/" + turnPlayerStr + '/' + std::to_string(r) + '/'
              + std::to_string(s));
      restbed::Http::sync(request);
    }

    request = createRequest("remove_Card/" + askPlayerStr + '/' + askRankStr);
    restbed::Http::sync(request);

    return true;
  } else {
    // Draw card
    auto requestDraw = createRequest("/deck");
    auto responseDraw = restbed::Http::sync(requestDraw);
    auto resultDraw = handle_response(responseDraw);
    nlohmann::json cardDrawn = nlohmann::json::parse(resultDraw);

    int cardSuit, cardRank;
    cardSuit = cardDrawn.value("suit", 0);
    cardRank = cardDrawn.value("rank", 0);
    std::string cardSuitStr, cardRankStr;
    cardSuitStr = std::to_string(cardSuit);
    cardRankStr = std::to_string(cardRank);

    auto requestAdd = createRequest(
        "add_Card/" + turnPlayerStr + "/" + cardRankStr + "/" + cardSuitStr);
    restbed::Http::sync(requestAdd);

    return false;
  }
}

bool gameOver() {
  std::shared_ptr<restbed::Request> request = createRequest("/over");
  auto response = restbed::Http::sync(request);
  auto length = response->get_header("Content-Length", 0);
  restbed::Http::fetch(length, response);
  std::string responseStr(reinterpret_cast<char*>(response->get_body().data()),
                          length);
  nlohmann::json resultJSON = nlohmann::json::parse(responseStr);
  return resultJSON["is_over"];
}

int main(const int, const char**) {
  std::cout << "STARTING CLIENT" << std::endl << std::endl;

  std::string names[] = { "Billy", "Bob" };

  GoFishUI_REST *ui = new GoFishUI_REST();

  // Create a new game
  std::shared_ptr<restbed::Request> createGameRequest = createRequest(
      "/create");
  auto responseCreateGame = restbed::Http::sync(createGameRequest);
  nlohmann::json resultJSON = nlohmann::json::parse(
      handle_response(responseCreateGame));
  std::cout << resultJSON["result"] << std::endl;

  // Setting names
  std::string temp;
  std::cout << "Enter player 0 name: ";
  std::cin >> temp;
  names[0] = temp;
  std::cout << "Enter player 1 name: ";
  std::cin >> temp;
  names[1] = temp;

// Add players
  std::string qp = "/player/" + names[0];
  std::string qp2 = "/player/" + names[1];
  std::shared_ptr<restbed::Request> addPlayerRequest = createRequest(qp);
  restbed::Http::sync(addPlayerRequest);
  addPlayerRequest = createRequest(qp2);
  restbed::Http::sync(addPlayerRequest);

  // Start game
  std::shared_ptr<restbed::Request> startGameRequest = createRequest("/start");
  restbed::Http::sync(startGameRequest);

  // Play game
  unsigned int numPlayers = 2;
  unsigned int turn = 0;
  while (!gameOver()) {
    // Choose a player to ask
    int playerAsked = ui->choosePlayer(turn, numPlayers);

    // Show current players hand and get asked rank
    std::string playerTurn;
    playerTurn = std::to_string(turn);
    std::shared_ptr<restbed::Request> playerHandRequest = createRequest(
        "/playerHand/" + playerTurn);
    auto responsePlayerhand = restbed::Http::sync(playerHandRequest);
    nlohmann::json hand = nlohmann::json::parse(
        handle_response(responsePlayerhand));
    int index = ui->requestCard(hand);
    nlohmann::json jsonObj = hand.at(index);

    // Ask the player
    unsigned int askedRank = jsonObj.value("rank", 0);
    if (askPlayer(turn, playerAsked, askedRank)) {
      ui->playSucceeded();
    } else {
      ui->playFailed();
    }

    // Check for sets
    auto request = createRequest("has_Set/" + std::to_string(turn));
    auto response = restbed::Http::sync(request);
    resultJSON = nlohmann::json::parse(
        handle_response(response));
    if (resultJSON["set"]) {
      int r = resultJSON["rank"];
      request = createRequest(
          "remove_Card/" + std::to_string(turn) + '/' + std::to_string(r));
      response = restbed::Http::sync(request);

      request = createRequest("add_Points/" + std::to_string(turn) + "/1");
      restbed::Http::sync(request);
    }
    turn = ++turn % numPlayers;
  }

  // Show scores
  std::cout << "===== Game Over =====" << std::endl;
  auto request = createRequest("get_Score");
  auto response = restbed::Http::sync(request);
  auto result = handle_response(response);
  nlohmann::json scores = nlohmann::json::parse(result);
  ui->showScores(scores);

  delete ui;

  return EXIT_SUCCESS;
}
