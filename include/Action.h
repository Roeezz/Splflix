#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include "User.h"

class Session;

enum ActionStatus {
    PENDING, COMPLETED, ERROR
};

class BaseAction {
public:
    BaseAction();

    virtual ~BaseAction();

    virtual void act(Session &sess) = 0;

    ActionStatus getStatus() const;

    std::string getStatusMessage() const;

    virtual std::string toString() const = 0;

    virtual BaseAction *clone() = 0;

protected:
    /**
     * Sets the status of an action to COMPLETED.
     */
    void complete();

    /**
     * Sets the status of an action to ERROR and prints to the screen the error message.
     * @param errorMsg the error message to print.
     */
    void error(const std::string &errorMsg);

    std::string getErrorMsg() const;

    void setErrorMsg(std::string &errorMsg);

private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser : public BaseAction {
public:
    CreateUser(const std::string &userName, const std::string &algorithmType);

    /**
     * Creates a new User named userName of the type specified in algorithmType and
     * calls the session to add it to the user database.
     * @param sess the session to add the user to.
     */
    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
private:
    std::string userName;
    std::string algorithmType;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(std::string &userName);

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
private:
    std::string userName;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string &userName);

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
private:
    std::string userName;
};

class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string &oldUserName, std::string &newUserName);

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
private:
    std::string oldUserName;
    std::string newUserName;
};

class PrintContentList : public BaseAction {
public:
    PrintContentList();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction* clone();
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction* clone();
};

class Watch : public BaseAction {
public:
    Watch(long id);

    virtual void act(Session &sess);

    virtual std::string toString() const;

    bool getKeepWatching() const;

    long getNextId() const;

    void setNextId(long newNextId);

    virtual BaseAction* clone();

private:
    long id;
    long nextId;
    bool keepWatching;

    void newRecommendation(Session &sess, Watchable *const watched);
};

class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction* clone();
};

class Exit : public BaseAction {
public:
    Exit();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction* clone();
};

#endif