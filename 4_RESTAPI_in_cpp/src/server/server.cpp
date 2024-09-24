/**
 * Copyright 2022 Dr. Anvik (john.anvik@uleth.ca), Kevin Masson, Matthew Ursaki,
 * Mohammadreza Ardestani
 */

#include <restbed>
#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <cstring>
#include <cstdlib>
#include <list>
#include <nlohmann/json.hpp>
#include "Game.h"
#include "GoFish.h"
#include "Card.h"
#include "Exceptions.h"

// REST endpoint
static const char ENDPOINT[] = "/gofish";

const int PORT = 2112;  // Choose your own port number > 1024

// Response header to prevent a cross-site validation problem
#define ALLOW_ALL { "Access-Control-Allow-Origin", "*" }

// Response header to close connection
#define CLOSE_CONNECTION { "Connection", "close" }

Deck *deck = nullptr;
Game *game = nullptr;
restbed::Service service;
bool playerCheck(int p) {
  if (p >= 0 && p < game->getPlayers().size())
    return true;

  return false;
}
// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

// a. Shutting down the server. [PROVIDED]
void shutdown_handler_callback(
    const std::shared_ptr<restbed::Session> &session) {
  std::cout << "Shutting down server" << std::endl;
  service.stop();
  exit(0);
}

// b. Create a new game.
void create_game_callback(const std::shared_ptr<restbed::Session> &session) {
  //std::cout << "inside create function" << std::endl;
  if (game == nullptr) {
    deck = new Deck();
    deck->createStandard();
    deck->shuffle();
    game = new GoFish(deck);
    std::string resultStr = "GAME CREATED";
    nlohmann::json resultJSON;
    resultJSON["result"] = resultStr;
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) },
    CLOSE_CONNECTION });
    return;
  } else {
    delete game;
    deck = new Deck();
    deck->createStandard();
    game = new GoFish(deck);
    std::string resultStr = "recreated a game!";
    nlohmann::json resultJSON;
    resultJSON["result"] = resultStr;
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) },
    CLOSE_CONNECTION });
  }
}

// c. Start a game.
void start_game_callback(const std::shared_ptr<restbed::Session> &session) {
  nlohmann::json resultJSON;
  try {
    game->start();
    resultJSON["started"] = true;
    std::string Msg = resultJSON.dump();
    session->close(restbed::OK, Msg, { ALLOW_ALL, { "Content-Length",
        std::to_string(Msg.length()) },
    CLOSE_CONNECTION });
  } catch (const std::runtime_error &e) {
    std::cout << "/* detected an error after starting the game */" << '\n';
    std::string errorMsg = "Cannot start game, no players found";
    session->close(restbed::BAD_REQUEST, "No players in game.", { ALLOW_ALL, {
        "Content-Length", std::to_string(errorMsg.length()) },
    CLOSE_CONNECTION });
  }
}

// d. Check if the game is over.
void is_over_callback(const std::shared_ptr<restbed::Session> &session) {
  if (game->isOver()) {
    // is over; A player with No card in their hand exists
    nlohmann::json resultJSON;
    resultJSON["is_over"] = true;
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) },
    CLOSE_CONNECTION });
  } else {
    nlohmann::json resultJSON;
    resultJSON["is_over"] = false;
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) },
    CLOSE_CONNECTION });
  }
}
void addPlayers_callback(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();

  if (request->has_path_parameter("playerName")) {
    std::string playerName = request->get_path_parameter("playerName");
    game->addPlayer(new Player(playerName));
    nlohmann::json resultJSON;
    resultJSON["result"] = "Player added successfully!";
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) }, CLOSE_CONNECTION });
  }
}

//e. Getting a players name
void getPlayerName_callback(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  std::string playerName;

  if (request->has_path_parameter("playerIndex")) {
    unsigned int playerIndex;
    playerIndex = std::stoi(request->get_path_parameter("playerIndex"));
    if (!playerCheck(playerIndex)) {
      std::string errorMsg = "Player does not exist";
        session->close(restbed::BAD_REQUEST, "Invalid Player.", { ALLOW_ALL, {
            "Content-Length", std::to_string(errorMsg.length()) },
        CLOSE_CONNECTION });
        return;
    }
    nlohmann::json resultJSON;
    playerName = game->getPlayers()[playerIndex]->name;
    resultJSON["name"] = playerName;
    std::string response = resultJSON.dump();
    session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
        std::to_string(response.length()) }, CLOSE_CONNECTION });
    }
  }

// g. Drawing a card
void deck_get_callback(const std::shared_ptr<restbed::Session> &session) {
  // Instantiations
  Card *aCard;
  nlohmann::json resultJSON;
  resultJSON["added"] = false;
  int size = deck->size();

  // Draw card, get it's suit and rank in string
  aCard = deck->getCard();
  if (deck->size() < size)
    resultJSON["added"] = true;

  // Just JSON things
  resultJSON["suit"] = aCard->suit;
  resultJSON["rank"] = aCard->rank;

  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) },
  CLOSE_CONNECTION });
}

// h. Getting a player's hand.
void playerHand_callback(const std::shared_ptr<restbed::Session> &session) {
  //std::cout << std::endl << "in playerHand_callback" << std::endl;
  const auto request = session->get_request();
  std::string playerIndexSTR = request->get_path_parameter("ph");
  int playerIndexINT = std::stoi(playerIndexSTR);
  int playerHandSize = game->getPlayers()[playerIndexINT]
                                          ->getHand()->size();
  nlohmann::json resultJSON;

  for (int i = 0; i < playerHandSize; i++) {
    Card *aCard = game->getPlayers()[playerIndexINT]->getCard(i);

    resultJSON[i]["rank"] = aCard->rank;
  }
  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

// i. Adding a card to a player's hand.
void add_Card(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  Card::Rank r = (Card::Rank) (stoi(request->get_path_parameter("rank")));
  Card::Suit s = (Card::Suit) (stoi(request->get_path_parameter("suit")));
  int p = stoi(request->get_path_parameter("player"));
  nlohmann::json resultJSON;
  resultJSON["player"] = playerCheck(p);
  resultJSON["hand"] = false;

  if (playerCheck(p)) {
    int size = game->getPlayers()[p]->getHand()->size();

    game->getPlayers()[p]->addCard(new Card(s, r));

    if (game->getPlayers()[p]->getHand()->size() > size)
      resultJSON["hand"] = true;
  } else {
    std::string errorMsg = "Invalid Player";
    session->close(restbed::BAD_REQUEST, "Player doesn't exist.", { ALLOW_ALL, {
        "Content-Length", std::to_string(errorMsg.length()) },
    CLOSE_CONNECTION });
    return;
  }

  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

// j. Removing a card from a player's hand
void remove_Card(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  Card::Rank r = (Card::Rank) stoi(request->get_path_parameter("rank"));
  int p = stoi(request->get_path_parameter("player"));
  nlohmann::json resultJSON;

  if (playerCheck(p)) {
    int size = game->getPlayers()[p]->getHand()->size();

    //deletes the card from the player's hand if it exists
    for (std::list<Card*>::iterator it =
        game->getPlayers()[p]->getHand()->begin();
        it != game->getPlayers()[p]->getHand()->end(); ++it) {
      if (r == (*it)->rank) {
        delete (*it);
        it = game->getPlayers()[p]->getHand()->erase(it);
        --it;
      }
    }

    //verifies whether a card was removed or not
    if (game->getPlayers()[p]->getHand()->size() < size) {
      resultJSON["hand"] = true;
    } else {
      std::string errorMsg = "Card not removed or doesn't exist";
      session->close(restbed::BAD_REQUEST, "Card removal error.", { ALLOW_ALL, {
          "Content-Length", std::to_string(errorMsg.length()) },
      CLOSE_CONNECTION });
    }

  } else {
    std::string errorMsg = "Player does not exist";
    session->close(restbed::BAD_REQUEST, "Invalid Player.", { ALLOW_ALL, {
        "Content-Length", std::to_string(errorMsg.length()) },
    CLOSE_CONNECTION });
  }

  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

// k. Checking if a player has a set.
void has_Set(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  int p = stoi(request->get_path_parameter("player"));
  nlohmann::json resultJSON;
  resultJSON["player"] = playerCheck(p);
  resultJSON["set"] = false;
  std::map<Card::Rank, int> hand;

  if (playerCheck(p)) {
    for (int i = 0; i < game->getPlayers()[p]->getHand()->size(); i++) {
      Card *aCard = game->getPlayers()[p]->getCard(i);

      if (hand.find(aCard->rank) == hand.end()) {
        hand.emplace(aCard->rank, 1);
      } else {
        hand[aCard->rank]++;
        if (hand[aCard->rank] == 4) {
          resultJSON["rank"] = aCard->rank;
          resultJSON["set"] = true;
        }
      }
    }
  }
  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

// l. Getting the players' scores.
void get_Score(const std::shared_ptr<restbed::Session> &session) {
  nlohmann::json resultJSON;

  for (unsigned int i = 0; i < game->getPlayers().size(); i++) {
    resultJSON[i]["name"] = game->getPlayers()[i]->name;
    resultJSON[i]["score"] = game->getPlayers()[i]->getScore();
  }

  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

// m. Asking a player for a specific card.
void ask_Card(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  int p = stoi(request->get_path_parameter("player"));
  Card::Rank r = (Card::Rank) stoi(request->get_path_parameter("rank"));
  nlohmann::json resultJSON;
  int it = 0;
  if (!playerCheck(p)) {
    std::string errorMsg = "Player does not exist";
    session->close(restbed::BAD_REQUEST, "Invalid Player.", { ALLOW_ALL, {
        "Content-Length", std::to_string(errorMsg.length()) },
    CLOSE_CONNECTION });
    return;
  }
  Player *player = game->getPlayers()[p];
  int playerHandSize = player->getHand()->size();

  for (int i = 0; i < playerHandSize; i++) {
    if (r == player->getCard(i)->rank) {
      Card *aCard = player->getCard(i);

      resultJSON[it]["rank"] = aCard->rank;
      resultJSON[it]["suit"] = aCard->suit;
      it++;
    }
  }

  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

void add_Points(const std::shared_ptr<restbed::Session> &session) {
  const auto request = session->get_request();
  int p = stoi(request->get_path_parameter("player"));
  int s = stoi(request->get_path_parameter("points"));
  nlohmann::json resultJSON;
  resultJSON["added"] = false;

  if (playerCheck(p)) {
    game->getPlayers()[p]->addPoints(s);
    resultJSON["added"] = true;
  }
  std::string response = resultJSON.dump();
  session->close(restbed::OK, response, { ALLOW_ALL, { "Content-Length",
      std::to_string(response.length()) }, CLOSE_CONNECTION });
}

int main(const int, const char**) {
//Setup service and request handlers
  std::cout << "\n\n" << "\t ***SERVER START***\n" << std::endl;

  auto shutdown = std::make_shared<restbed::Resource>();
  shutdown->set_path("/shutdown");
  shutdown->set_method_handler("GET", shutdown_handler_callback);

  auto create = std::make_shared<restbed::Resource>();
  create->set_path("/create");
  create->set_method_handler("GET", create_game_callback);

  auto start = std::make_shared<restbed::Resource>();
  start->set_path("/start");
  start->set_method_handler("GET", start_game_callback);

  auto over = std::make_shared<restbed::Resource>();
  over->set_path("/over");
  over->set_method_handler("GET", is_over_callback);

  auto addPlayer = std::make_shared<restbed::Resource>();
  addPlayer->set_path("/player/{playerName: \\w+}");
  addPlayer->set_method_handler("GET", addPlayers_callback);

  auto getPlayerName = std::make_shared<restbed::Resource>();
  getPlayerName->set_path("/getPlayerName/{playerIndex: \\d+.?\\d*}");
  getPlayerName->set_method_handler("GET", getPlayerName_callback);

  auto makedeck = std::make_shared<restbed::Resource>();
  makedeck->set_path("/deck");\
  makedeck->set_method_handler("GET", deck_get_callback);

  auto playerHand = std::make_shared<restbed::Resource>();
  playerHand->set_path("/playerHand/{ph: \\w}");
  playerHand->set_method_handler("GET", playerHand_callback);

  auto addCard = std::make_shared<restbed::Resource>();
  addCard->set_path(
      "/add_Card/{player: \\d+.?\\d*}/{rank: \\d+.?\\d*}/{suit: \\d+.?\\d*}");
  addCard->set_method_handler("GET", add_Card);

  auto removeCard = std::make_shared<restbed::Resource>();
  removeCard->set_path("/remove_Card/{player: \\d+.?\\d*}/{rank: \\d+.?\\d*}");
  removeCard->set_method_handler("GET", remove_Card);

  auto hasSet = std::make_shared<restbed::Resource>();
  hasSet->set_path("/has_Set/{player: \\d+.?\\d*}");
  hasSet->set_method_handler("GET", has_Set);

  auto askCard = std::make_shared<restbed::Resource>();
  askCard->set_path("/ask_Card/{player: \\d+.?\\d*}/{rank: \\d+.?\\d*}");
  askCard->set_method_handler("GET", ask_Card);

  auto getScore = std::make_shared<restbed::Resource>();
  getScore->set_path("/get_Score");
  getScore->set_method_handler("GET", get_Score);

  auto addPoints = std::make_shared<restbed::Resource>();
  addPoints->set_path("/add_Points/{player: \\d+.?\\d*}/{points: \\d+.?\\d*}");
  addPoints->set_method_handler("GET", add_Points);

  auto settings = std::make_shared<restbed::Settings>();
  settings->set_port(PORT);
  settings->set_root(ENDPOINT);

// Publish and start service
  service.publish(shutdown);
  service.publish(create);
  service.publish(start);
  service.publish(over);
  service.publish(addPlayer);
  service.publish(getPlayerName);
  service.publish(makedeck);
  service.publish(playerHand);
  service.publish(addCard);
  service.publish(removeCard);
  service.publish(hasSet);
  service.publish(askCard);
  service.publish(getScore);
  service.publish(addPoints);
  service.start(settings);
  return EXIT_SUCCESS;
}
