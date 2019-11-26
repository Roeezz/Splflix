#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"
#include "User.h"
#include "json.hpp"
#include <list>
#include <climits>

class User;

class Watchable;

class Session {

public:
    //ctor
    Session(const std::string &configFilePath);

    //Copy ctor
    Session(const Session &other);

    //Copy assignment
    Session& operator=(const Session& other);

    //Move copy ctor
    Session(Session &&other);

    //Move copy assignment
    Session& operator=(Session &&other);

    //Destructor
    ~Session();

    //Event loop
    void start();

    //userMap methods
    /**
     * Finds a user in the user map.
     * @param userName the name of the user to find.
     * @return a pointer to the user if found, otherwise null.
     */
    User *getUser(std::string &userName);

    bool addUser(std::string &name, User *user);

    bool deleteUser(std::string &userName);

    //actionsLog Methods
    void addActionToLog(BaseAction *action);

    //content methods
    Watchable *getWatchable(long &id);

    //Getters and Setters
    std::vector<Watchable *> const& getContent() const;

    std::vector<BaseAction *> const& getActionsLog() const;

    std::unordered_map<std::string, User *>const& getUserMap() const;

    User *getActiveUser() const;

    void setActiveUser(User *newUser);

    bool getEndSession() const;

    void setEndSession(bool set);

    //Recommendation methods
    Watchable *GetRecommendationLength(const LengthRecommenderUser &user, int average);

    Watchable *GetRecommendationGenre(const GenreRecommenderUser &user, const std::string &tag);

    static std::string watchableVectorToString(const std::vector<Watchable *> &vec);

    std::string actionsLogToString();

private:

    std::vector<Watchable *> content;
    std::vector<BaseAction *> actionsLog;
    std::unordered_map<std::string, User *> userMap;
    User *activeUser;
    bool endSession;

    //ctor, assignment and destructor methods
    void clear();

    void copy(const Session &other);

    void move(Session &&other);


    //Content creating methods
    void createContent(const std::string &configFilePath);

    void extractMoviesContent(nlohmann::json &j, long &id);

    void extractTVContent(const nlohmann::basic_json<> &j, long id);

    void setDefaultUser();

    //event loop methods
    void eventLoop();

    /**
     * Picks the correct action that matches the inserted string command.
     * If the command is invalid the event loop will print a message to the screen and clear the cin buffer.
     * @param command the command for the action to choose.
     */
    void actionChooser(const std::string &command);

    void createUser();

    void changeActiveUser();

    void deleteUserAct();

    void duplicateUser();

    void printContentList();

    void printWatchHistory();

    void printActionsLog();

    void exitSession();



    void watch(long &&recommendId);
};

#endif
