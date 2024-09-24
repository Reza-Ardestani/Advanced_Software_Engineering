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
#include <exception>
#include <nlohmann/json.hpp>
#include "gtest/gtest.h"
#include "rest/rest_utils.h"

TEST(TestServer, setPlayerName) {
  std::shared_ptr<restbed::Request> createGameRequest = createRequest(
      "/create/");
  restbed::Http::sync(createGameRequest);
  std::string playerName = "Ursa";

  std::shared_ptr<restbed::Request> addPlayerRequest = createRequest(
      "/player/" + playerName);
  auto response = restbed::Http::sync(addPlayerRequest);
  int status_code = response->get_status_code();
  ASSERT_EQ(200, status_code);

  auto request = createRequest("player/");
  response = restbed::Http::sync(request);
  EXPECT_EQ(404, response->get_status_code());
}

TEST(TestServer, getPlayerName) {
  std::shared_ptr<restbed::Request> createGameRequest = createRequest(
      "/create/");
  restbed::Http::sync(createGameRequest);
  std::string playerName = "Ursa";

  std::shared_ptr<restbed::Request> addPlayerRequest = createRequest(
      "/player/" + playerName);
  restbed::Http::sync(addPlayerRequest);

  std::string test = "0";
  std::shared_ptr<restbed::Request> getPlayerNameRequest = createRequest(
      "/getPlayerName/" + test);
  auto getPlayerNameResponse = restbed::Http::sync(getPlayerNameRequest);
  nlohmann::json playerNameJSON = nlohmann::json::parse(handle_response(
          getPlayerNameResponse));
  std::string returnedPlayerName = playerNameJSON.value("name", "");

  EXPECT_EQ(playerName, returnedPlayerName);
}
TEST(TestServer, handIllegalPlayer) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("/player/testname");
  auto response = restbed::Http::sync(request);
  EXPECT_EQ(200, response->get_status_code());

  // Get the name of an illegal player
  request = createRequest("getPlayerName/1");
  response = restbed::Http::sync(request);
  EXPECT_EQ(400, response->get_status_code());
}

TEST(TestServer, gameStart) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  //starting game with no players
  request = createRequest("start");
  auto response = restbed::Http::sync(request);
  EXPECT_EQ(400, response->get_status_code());

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // start the game
  request = createRequest("/start");
  response = restbed::Http::sync(request);
  EXPECT_EQ(200, response->get_status_code());
}

TEST(TestServer, gameOverSuccess) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player whose hand is empty
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // check if the game is over
  request = createRequest("/over");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_TRUE(resultJSON["is_over"]);
}

TEST(TestServer, gameOverFailure) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player whose hand is empty
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // start the game
  request = createRequest("/start");
  restbed::Http::sync(request);

  // check if the game is over
  request = createRequest("/over");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_FALSE(resultJSON["is_over"]);
}

TEST(TestServer, addCardSuccess) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  // Add the Card
  request = createRequest("add_Card/0/2/2");
  response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_TRUE(resultJSON["hand"]);
}

TEST(TestServer, addCardFailure) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add Card
  request = createRequest("add_Card/1/2/2");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(400, response->get_status_code());
}

TEST(TestServer, getHand) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/1/3");
  restbed::Http::sync(request);

  // Get Hand
  request = createRequest("playerhand/0");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  std::string result = handle_response(response);
  nlohmann::json resultJson = nlohmann::json::parse(result);
  EXPECT_EQ(resultJson.size(), 2);
  EXPECT_EQ(resultJson[0]["rank"], 2);
  EXPECT_EQ(resultJson[1]["rank"], 1);
}

TEST(TestServer, removeCardSuccess) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add the Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);

  // Remove the Card
  request = createRequest("remove_Card/0/2");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_TRUE(resultJSON["hand"]);
}
//
TEST(TestServer, removeCardFailure) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add the Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);

  // Remove the Card from invalid player
  request = createRequest("remove_Card/1/3");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(400, response->get_status_code());

  //Remove invalid card
  request = createRequest("remove_Card/0/3");
  response = restbed::Http::sync(request);
  ASSERT_EQ(400, response->get_status_code());
}

TEST(TestServer, askPlayerSucceed) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add the Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);

  //Ask for Card
  request = createRequest("ask_Card/0/2");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_EQ(resultJSON.size(), 1);
  EXPECT_EQ(resultJSON[0]["rank"], 2);
  EXPECT_EQ(resultJSON[0]["suit"], 2);
}

TEST(TestServer, askPlayerFail) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  request = createRequest("ask_Card/0/3");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(400, response->get_status_code());

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  //Add the Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);

  //Ask for Card
  request = createRequest("ask_Card/0/3");
  response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_EQ(resultJSON.size(), 0);
}

TEST(TestServer, hasSetSucceed) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add the Card
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/2/3");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/2/0");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/2/1");
  restbed::Http::sync(request);

  //Check for Set
  request = createRequest("has_Set/0");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  ASSERT_TRUE(resultJSON["player"]);
  EXPECT_TRUE(resultJSON["set"]);
}

TEST(TestServer, hasSetFail) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);

  // Add the Card
  request = createRequest("add_Card/0/2/0");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/2/1");
  restbed::Http::sync(request);
  request = createRequest("add_Card/0/2/2");
  restbed::Http::sync(request);

  //Check for Set
  request = createRequest("has_Set/0");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  ASSERT_TRUE(resultJSON["player"]);
  EXPECT_FALSE(resultJSON["set"]);
}

TEST(TestServer, initialState) {
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  request = createRequest("player/testname");
  restbed::Http::sync(request);

  request = createRequest("start");
  restbed::Http::sync(request);

  request = createRequest("playerhand/0");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  EXPECT_EQ(resultJSON.size(), 7);
}

TEST(TestServer, drawCard) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  //Draw a card
  request = createRequest("deck");
  auto response = restbed::Http::sync(request);
  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);

  EXPECT_TRUE(resultJSON["added"]);
  EXPECT_NE(resultJSON["suit"], "");
  EXPECT_NE(resultJSON["rank"], "");
}

TEST(TestServer, testScore) {
  // Create a new game
  std::shared_ptr<restbed::Request> request = createRequest("/create");
  restbed::Http::sync(request);

  // Add a player
  request = createRequest("player/testname");
  restbed::Http::sync(request);
  request = createRequest("player/testname2");
  restbed::Http::sync(request);

  //Get default score
  request = createRequest("get_Score");
  auto response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  std::string result = handle_response(response);
  nlohmann::json resultJSON = nlohmann::json::parse(result);
  ASSERT_EQ(resultJSON.size(), 2);
  ASSERT_EQ(resultJSON[0]["score"], 0);
  ASSERT_EQ(resultJSON[1]["score"], 0);

  //Add points
  request = createRequest("add_Points/0/20");
  response = restbed::Http::sync(request);
  ASSERT_EQ(200, response->get_status_code());

  request = createRequest("add_Points/1/30");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  resultJSON = nlohmann::json::parse(result);
  ASSERT_TRUE(resultJSON["added"]);

  request = createRequest("get_Score");
  response = restbed::Http::sync(request);
  result = handle_response(response);
  resultJSON = nlohmann::json::parse(result);
  EXPECT_EQ(resultJSON[0]["name"], "testname");
  EXPECT_EQ(resultJSON[1]["name"], "testname2");
  EXPECT_EQ(resultJSON[0]["score"], 20);
  EXPECT_EQ(resultJSON[1]["score"], 30);
}

/**
 * A pseudo-test to shutdown server when testing is complete
 * Needed for computing code coverage of tests
 */
TEST(TestServer, shutdown) {
  std::shared_ptr<restbed::Request> request = createRequest("shutdown");
  restbed::Http::sync(request);
}
