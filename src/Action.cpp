#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"

//Base Action

//constructor
BaseAction::BaseAction() : errorMsg("The action could not be completed"), status(PENDING) {}

BaseAction::~BaseAction() = default;

//Getters and Setters
ActionStatus BaseAction::getStatus() const {
    return status;
}

std::string BaseAction::getStatusMessage() const {
    std::string output;
    if (getStatus() == COMPLETED) {
        output = "completed";
    } else {
        output = "error: " + getErrorMsg();
    }
    return output;
}

//Protected
std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

void BaseAction::setErrorMsg(std::string &errorMsg) {
    this->errorMsg = errorMsg;
}

//Status updating methods
void BaseAction::complete() {
    this->status = COMPLETED;
}

void BaseAction::error(const std::string &errorMsg) {
    this->status = ERROR;
    std::cout << "Error - " + errorMsg << std::endl;
}

//Create User
CreateUser::CreateUser(const std::string &userName, const std::string &algorithmType) : userName(userName),
                                                                                        algorithmType(algorithmType) {

    std::string errorMsg = "Could not create user '" + userName + "' with watch algorithm '" + algorithmType + "'";
    setErrorMsg(errorMsg);
}

void CreateUser::act(Session &sess) {
    User *newUser;
    if (algorithmType == "len")
        newUser = new LengthRecommenderUser(userName);
    else if (algorithmType == "rer")
        newUser = new RerunRecommenderUser(userName);
    else if (algorithmType == "gen")
        newUser = new GenreRecommenderUser(userName);
    else {
        error(getErrorMsg());
        return;
    }
    if (!sess.addUser(userName, newUser)) {
        error(getErrorMsg());
        delete (newUser);
        newUser = nullptr;
        return;
    }
    complete();
}

std::string CreateUser::toString() const {
    std::string output = "Create user '" + userName + "' with watch algorithm '" + algorithmType + "  "
                         + getStatusMessage();
    return output;
}

BaseAction *CreateUser::clone() {
    return new CreateUser(*this);
}

//Change Active User
ChangeActiveUser::ChangeActiveUser(std::string &userName) : userName(userName) {
    std::string errorMsg = "Could not change active user to " + userName;
    setErrorMsg(errorMsg);
}

void ChangeActiveUser::act(Session &sess) {
    User *newUser = sess.getUser(userName);
    if (newUser != nullptr) {
        sess.setActiveUser(newUser);
        complete();
    } else {
        error(getErrorMsg());
    }
}

std::string ChangeActiveUser::toString() const {
    std::string output = "Change active user to '" + userName + "' " + getStatusMessage();
    return output;
}

BaseAction *ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}

//Duplicate User
DuplicateUser::DuplicateUser(std::string &oldUserName, std::string &newUserName) : oldUserName(oldUserName),
                                                                                   newUserName(newUserName) {
    std::string errorMsg = "Could not duplicate the user '" + oldUserName + "' with new username '" + newUserName + "'";
    setErrorMsg(errorMsg);
}

void DuplicateUser::act(Session &sess) {
    User *oldUser = sess.getUser(oldUserName);
    if (oldUser) {
        User *newUser = oldUser->clone(newUserName);

        if (sess.addUser(newUserName, newUser)) {
            complete();
            return;
        }
        delete (newUser);
        newUser = nullptr;
    }
    error(getErrorMsg());
}

std::string DuplicateUser::toString() const {
    std::string output = "Duplicate user '" + oldUserName + "' with new username '" + newUserName + "' "
                         + getStatusMessage();
    return output;
}

BaseAction *DuplicateUser::clone() {
    return new DuplicateUser(*this);
}

//Delete User
DeleteUser::DeleteUser(std::string &userName) : userName(userName) {
    std::string errorMsg = "Could not delete user: '" + userName + "'";
    setErrorMsg(errorMsg);
}

void DeleteUser::act(Session &sess) {
    bool deleted = sess.deleteUser(userName);
    if (deleted) {
        complete();
    } else {
        error((getErrorMsg()));
    }
}

std::string DeleteUser::toString() const {
    std::string output = "Delete user:" + userName + " " + getStatusMessage();
    return output;
}

BaseAction *DeleteUser::clone() {
    return new DeleteUser(*this);
}

//Print Content List
PrintContentList::PrintContentList() {

    std::string error = "Could not print content list";
    setErrorMsg(error);
}

void PrintContentList::act(Session &sess) {
    //todo: check for memory leaks
    std::string output = Session::watchableVectorToString(sess.getContent());
    std::cout << output << std::endl;
    complete();
}

std::string PrintContentList::toString() const {
    std::string output = "Print content list " + getStatusMessage();
    return output;
}

BaseAction *PrintContentList::clone() {
    return new PrintContentList(*this);
}

//Print Watch History
PrintWatchHistory::PrintWatchHistory() {
    std::string errorMsg = "Could not print watch history";
    setErrorMsg(errorMsg);
}

void PrintWatchHistory::act(Session &sess) {
    std::vector<Watchable *> history = sess.getActiveUser()->getHistory();
    std::string output = Session::watchableVectorToString(history);
    std::cout << sess.getActiveUser()->getName() << std::endl;
    std::cout << output << std::endl;
    complete();
}

std::string PrintWatchHistory::toString() const {
    std::string output = "Print Watch history " + getStatusMessage();
    return output;
}

BaseAction *PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}

//Print Actions Log
PrintActionsLog::PrintActionsLog() {
    std::string errorMsg = "Could not print actions log";
    setErrorMsg(errorMsg);
}

void PrintActionsLog::act(Session &sess) {
    std::string output = sess.actionsLogToString();
    std::cout << output << std::endl;
    complete();
}

std::string PrintActionsLog::toString() const {
    std::string output = "Print actions log " + getStatusMessage();
    return output;
}

BaseAction *PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

//Watch
Watch::Watch(long id) : id(id), nextId(-1), keepWatching(false) {
    std::string errorMsg = "Could not stream content with id '" + std::to_string(id) + "'";
    setErrorMsg(errorMsg);
}

void Watch::act(Session &sess) {
    //try to get a watchable with given id
    Watchable *watchable = sess.getWatchable(id);
    if (!watchable) {
        error(getErrorMsg());
        return;
    }
    Watchable *toWatch = watchable->clone();

    //print to screen and add to history
    std::cout << "Watching " + toWatch->toString() << std::endl;
    User *activeUser = sess.getActiveUser();
    activeUser->addToHistory(toWatch);

    //try to get recommendation and change status to complete
    newRecommendation(sess, toWatch);
    if (getStatus() != ERROR) {
        complete();
    }
}

void Watch::newRecommendation(Session &sess, Watchable *const watched) {
    Watchable *recommendation = watched->getNextWatchable(sess);
    if (recommendation) {
        std::cout << "We recommend watching " + recommendation->toString() + ", continue watching?[Y/N]";
        std::string answer;
        std::cin >> answer;

        setNextId(recommendation->getId());
        if (answer == "Y" || answer == "y") {
            keepWatching = true;

        } else if (answer != "N" && answer != "n") {
            std::string newErrorMsg = "Invalid input";
            error(newErrorMsg);
        }
    } else {
        error(getErrorMsg());
    }
    recommendation = nullptr;
}

std::string Watch::toString() const {
    return "Watch " + getStatusMessage();
}

//Getters and setters
long Watch::getNextId() const {
    return nextId;
}

bool Watch::getKeepWatching() const {
    return keepWatching;
}

void Watch::setNextId(long newNextId) {
    this->nextId = newNextId;
}

BaseAction *Watch::clone() {
    return new Watch(*this);
}

//Exit
Exit::Exit() {
    std::string errorMsg = "Could not exit the session";
    setErrorMsg(errorMsg);
}

void Exit::act(Session &sess) {
    sess.setEndSession(true);
    complete();
}

std::string Exit::toString() const {
    std::string output = "Exit session " + getStatusMessage();
    return output;
}

BaseAction *Exit::clone() {
    return new Exit(*this);
}