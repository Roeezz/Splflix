#include "../include/Session.h"
#include "fstream"
#include "../include/Watchable.h"
#include "../include/json.hpp"
#include "../include/User.h"
#include <list>

//Constructors and assignments
Session::Session(const std::string &configFilePath)
        : content(), actionsLog(), userMap(), activeUser(nullptr), endSession(false) {
    createContent(configFilePath);
    createDefaultUser();
}

Session::Session(const Session &other) : content(), actionsLog(), userMap(), activeUser(nullptr), endSession(false) {
    copy(other);
}

Session &Session::operator=(const Session &other) {
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}

Session::Session(Session &&other) : content(), actionsLog(), userMap(), activeUser(nullptr), endSession(false) {
    move(std::move(other));
}

Session &Session::operator=(Session &&other) {
    if (this != &other) {
        clear();
        move(std::move(other));
    }
    return *this;
}

Session::~Session() {
    clear();
}

//Create content and default user methods
void Session::createContent(const std::string &configFilePath) {
    std::fstream ifs(configFilePath);
    nlohmann::json j = nlohmann::json::parse(ifs);
    long id;
    extractMoviesContent(j, id);

    extractTVContent(j, id);
}

void Session::extractMoviesContent(nlohmann::json &j, long &id) {
    id = 1;
    nlohmann::json movies = j["movies"];
    for (auto &mov : movies.items()) {
        nlohmann::json movie = mov.value();
        auto *newMovie = new Movie(id, movie["name"], movie["length"], movie["tags"]);
        content.push_back(newMovie);
        id++;
    }
}

void Session::extractTVContent(const nlohmann::json &j, long id) {
    nlohmann::json series = j["tv_series"];
    for (auto &episode : series.items()) {
        nlohmann::json seriesName = episode.value();
        std::vector<int> seasons = seriesName["seasons"];
        int seasonNumber = 1;
        for (int y :seasons) {
            for (int episodeNumber = 1; episodeNumber <= y; episodeNumber++) {
                auto *newEpisode = new Episode(id, seriesName["name"], seriesName["episode_length"],
                                               seasonNumber, episodeNumber, seriesName["tags"]);
                content.push_back(newEpisode);
                id++;
            }
            seasonNumber++;
        }
    }
}

void Session::createDefaultUser() {
    std::string defaultName = "default";
    auto createUser = CreateUser(defaultName, "len");
    createUser.act(*this);
    activeUser = getUser(defaultName);
}


//Event loop
void Session::start() {
    std::cout << "SPLFLIX is now on!" << std::endl;
    eventLoop();
}

//-Private event loop methods
void Session::eventLoop() {
    setEndSession(false);
    std::string command;
    while (!getEndSession()) {
        std::cout << "Please enter a command: ";
        std::cin >> command;
        actionChooser(command);
    }
}

void Session::actionChooser(const std::string &command) {
    if (command == "createuser") {
        createUser();
    } else if (command == "changeuser") {
        changeActiveUser();
    } else if (command == "deleteuser") {
        deleteUserAct();
    } else if (command == "dupuser") {
        duplicateUser();
    } else if (command == "content") {
        printContentList();
    } else if (command == "watchhist") {
        printWatchHistory();
    } else if (command == "log") {
        printActionsLog();
    } else if (command == "watch") {
        watch(-1);
    } else if (command == "exit") {
        exitSession();
    } else {
        std::cout << "'" + command + "' is not a valid command" << std::endl;
        std::cin.clear();
    }
}


//-Private actions methods
void Session::createUser() {
    std::string userName, algorithmType;
    std::cin >> userName >> algorithmType;
    auto *create = new CreateUser(userName, algorithmType);
    create->act(*this);
    addActionToLog(create);
}

void Session::changeActiveUser() {
    std::string userName;
    std::cin >> userName;
    auto *change = new ChangeActiveUser(userName);
    change->act(*this);
    addActionToLog(change);
}

void Session::deleteUserAct() {
    std::string userName;
    std::cin >> userName;
    auto *deleted = new DeleteUser(userName);
    deleted->act(*this);
    addActionToLog(deleted);
}

void Session::duplicateUser() {
    std::string oldUserName, newUserName;
    std::cin >> oldUserName >> newUserName;
    auto *duplicate = new DuplicateUser(oldUserName, newUserName);
    duplicate->act(*this);
    addActionToLog(duplicate);
}

void Session::printWatchHistory() {
    //todo: check why this prints an address
    auto *printHistory = new PrintWatchHistory;
    printHistory->act(*this);
    addActionToLog(printHistory);
}

void Session::printContentList() {
    auto *printContent = new PrintContentList();
    printContent->act(*this);
    addActionToLog(printContent);
}

void Session::printActionsLog() {
    auto printLog = new PrintActionsLog();
    printLog->act(*this);
    addActionToLog(printLog);
}

void Session::watch(long &&recommendId) {
    long id = recommendId;
    if (id < 0) {
        std::string idString;
        std::cin >> idString;
        id = std::stol(idString);
    }

    auto watchAct = new Watch(id);
    watchAct->act(*this);
    addActionToLog(watchAct);

    if (watchAct->getKeepWatching()) {
        watch(watchAct->getNextId());
    }
}

void Session::exitSession() {
    auto exitSession = new Exit();
    exitSession->act(*this);
    addActionToLog(exitSession);
}


//newRecommendation methods
//by length recommender
Watchable *Session::GetRecommendationLength(const LengthRecommenderUser &user, const int average) {
    Watchable *recommended = nullptr;
    int closest = std::numeric_limits<int>::max();
    for (auto const &toRecommend : content) {
        if (!user.isInHistory(toRecommend) && abs(toRecommend->getLength() - average) < closest) {
            closest = abs(toRecommend->getLength() - average);
            recommended = toRecommend;
        }
    }
    return recommended;
}

//by genre recommender
Watchable *Session::GetRecommendationGenre(const GenreRecommenderUser &user, const std::string &tag) {

    for (auto const &watchable_ptr : content) {
        if (watchable_ptr->checkInTags(tag) && !user.isInHistory(watchable_ptr)) {
            return watchable_ptr;
        }

    }
    return nullptr;

}


//userMap methods
User *Session::getUser(std::string &userName) {
    auto found = getUserMap().find(userName);
    if (found != getUserMap().end()) {
        User *output = found->second;
        return output;
    }
    return nullptr;
}

bool Session::addUser(std::string &name, User *user) {
    std::pair<std::string, User *> newUser(name, user);
    bool inserted = userMap.insert(newUser).second;
    return inserted;
}

bool Session::deleteUser(std::string &userName) {
    User *toDelete = getUser(userName);
    delete toDelete;
    toDelete = nullptr;
    bool erased = userMap.erase(userName);
    return erased;
}


//actionsLog methods
std::string Session::actionsLogToString() {
    std::string output;
    for (const auto &action: getActionsLog()) {
        output.append(action->toString()).append("\n");
    }
    return output;
}

//-private actionsLog method
void Session::addActionToLog(BaseAction *action) {
    actionsLog.push_back(action);
}

//private
void Session::clear() {
    //clear content vector
    for (auto &watchable_ptr: content) {
        delete watchable_ptr;
        watchable_ptr = nullptr;
    }
    content.clear();

    //clear content vector
    for (auto &action_ptr: actionsLog) {
        delete action_ptr;
        action_ptr = nullptr;
    }
    actionsLog.clear();

    //clear content vector
    activeUser = nullptr;
    for (auto &pair_iter: userMap) {
        delete pair_iter.second;
        pair_iter.second = nullptr;
    }
    userMap.clear();
}

void Session::copy(const Session &other) {
    this->endSession = other.endSession;

    for (auto &watchable : other.content) {
        content.push_back(watchable->clone());
    }
    for (auto &action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }
    for (const auto &user : other.userMap) {
        std::string userName = user.first;
        auto pair = std::make_pair(userName, user.second->clone(userName));
        this->userMap.insert(pair);
    }

    std::string newActiveUser = other.activeUser->getName();
    this->activeUser = getUser(newActiveUser);
}

void Session::move(Session &&other) {
    endSession = other.endSession;
    for (auto &watchable : other.content) {
        content.push_back(watchable);
        watchable = nullptr;
    }
    for (auto &action : other.actionsLog) {
        actionsLog.push_back(action);
        action = nullptr;
    }
    for (auto &user : other.userMap) {
        this->userMap.insert(user);
        user.second = nullptr;
    }

    std::string newActiveUser = other.activeUser->getName();
    this->activeUser = getUser(newActiveUser);
    other.activeUser = nullptr;
}

//getters and setters
std::vector<Watchable *> const &Session::getContent() const {
    return content;
}

std::vector<BaseAction *> const &Session::getActionsLog() const {
    return actionsLog;
}

std::unordered_map<std::string, User *> const &Session::getUserMap() const {
    return userMap;
}

User *Session::getActiveUser() const {
    return activeUser;
}

void Session::setActiveUser(User *newUser) {
    activeUser = newUser;
}

std::string Session::watchableVectorToString(const std::vector<Watchable *> &vec) {
    std::string output;
    std::string id;
    std::string length;
    std::string tags;

    for (const auto &watchable: vec) {
        id = std::to_string(watchable->getId());
        length = std::to_string(watchable->getLength());
        tags = watchable->tagsToString();

        output.append(id).append(". ").append(watchable->toString()).append(" ").append(length)
                .append(" minutes ").append(tags).append("\n");
    }
    return output;
}

bool Session::getEndSession() const {
    return endSession;
}

void Session::setEndSession(bool set) {
    endSession = set;
}

Watchable *Session::getWatchable(long &id) {
    Watchable *watchable;
    try {
        (watchable = content.at(id - 1));
        return watchable;
    }
    catch (std::out_of_range &) {
        return nullptr;
    }
}