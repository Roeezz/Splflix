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

    /**
     * Virtual method. When implemented it creates a clone of this BaseAction derived class,
     * then returns a pointer to it.
     */
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

    /**
     * Changes the active user to the user with the name userName if it exists.
     * @param sess the session in which to change the user.
     */
    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();

private:
    std::string userName;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string &userName);

    /**
     * Deletes the user with name userName if it exists.
     * @param sess the session i which to delete the user.
     */
    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();

private:
    std::string userName;
};

class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string &oldUserName, std::string &newUserName);

    /**
     * Finds the user with name oldUserName if it exists and creates a new user with name newUserName and
     * adds it to the session if no other user with the same name exists.
     * @param sess the Session to duplicate the user from.
     */
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

    /**
     * Prints the the content list of given Session.
     * @param sess the Session of which to print the content list.
     */
    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
};

class Watch : public BaseAction {
public:
    Watch(long id);

    virtual void act(Session &sess);

    virtual std::string toString() const;

    bool getKeepWatching() const;

    long getNextId() const;

    void setNextId(long newNextId);

    virtual BaseAction *clone();

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

    virtual BaseAction *clone();
};

class Exit : public BaseAction {
public:
    Exit();

    virtual void act(Session &sess);

    virtual std::string toString() const;

    virtual BaseAction *clone();
};

#endif
