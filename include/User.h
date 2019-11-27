#ifndef USER_H_
#define USER_H_

#include <algorithm>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

class Watchable;

class Session;

class User {
public:
    //ctor
    User(const std::string &name);

    //Copy ctor
    User(const User &other);

    //Copy assignment
    User &operator=(const User &other);

    //Move constructor
    User(User &&other);

    //Move assignment
    User &operator=(User &&other);

    //Destructor
    virtual ~User();

    virtual User *clone(std::string &name) = 0;

    virtual Watchable *getRecommendation(Session &s) = 0;

    std::string getName() const;

    std::vector<Watchable *> const &getHistory() const;

    bool isInHistory(const Watchable *) const;

    virtual void addToHistory(Watchable *watchable);

protected:

    std::vector<Watchable *> history;
private:
    void clear();

    void copy(const User &other);

    void move(User &&other);

    const std::string name;
};


class LengthRecommenderUser : public User {
public:
    //constructor
    LengthRecommenderUser(const std::string &name);

    //copy constructor
    LengthRecommenderUser(const LengthRecommenderUser &other);

    //move copy constructor
    LengthRecommenderUser(LengthRecommenderUser &&other);

    //assignment operator
    LengthRecommenderUser &operator=(const LengthRecommenderUser &other);

    //move assignment operator
    LengthRecommenderUser &operator=(LengthRecommenderUser &&other);

    //destructor
    virtual ~LengthRecommenderUser();

    virtual User *clone(std::string &name);

    virtual Watchable *getRecommendation(Session &s);

    virtual void addToHistory(Watchable *watchable);

private:

    /**
     * Computes the average length of content that the user watched
     */
    void recomputeAverage(int length);

    int average;
};

class RerunRecommenderUser : public User {
public:
    //constructor
    RerunRecommenderUser(const std::string &name);

    //copy constructor
    RerunRecommenderUser(const RerunRecommenderUser &other);

    //move copy constructor
    RerunRecommenderUser(RerunRecommenderUser &&other);

    //assignment operator
    RerunRecommenderUser &operator=(const RerunRecommenderUser &other);

    //move assignmernt operator
    RerunRecommenderUser &operator=(RerunRecommenderUser &&other);

    //destructor
    virtual ~RerunRecommenderUser();

    virtual User *clone(std::string &name);

    virtual Watchable *getRecommendation(Session &s);

    virtual void addToHistory(Watchable *watchable);

private:

    int currentIndex;

    void incrementCurrentIndex();
};

class GenreRecommenderUser : public User {
public:
    //constructor
    GenreRecommenderUser(const std::string &name);

    //copy constructor
    GenreRecommenderUser(const GenreRecommenderUser &other);

    //move copy constructor
    GenreRecommenderUser(GenreRecommenderUser &&other);

    //assignment operator
    GenreRecommenderUser &operator=(const GenreRecommenderUser &other);

    //move assignment operator
    GenreRecommenderUser &operator=(GenreRecommenderUser &&other);

    //destructor
    virtual ~GenreRecommenderUser();

    virtual User *clone(std::string &name);

    virtual Watchable *getRecommendation(Session &s);

    virtual void addToHistory(Watchable *watchable);

private:
    //keeps for each user its most popular tags
    std::vector<std::pair<int, std::string>> mostPopularTags;

    // Driver function to sort the vector elements by
    // first element of pair in descending order and if two are equal - by lexicographic order of the second
    static bool sortInRevLex(const std::pair<int, std::string> &p1, const std::pair<int, std::string> &p2);

    /**
     * Sorts the mostPopularTags vector in descending order according to the first type,
     * and then according to the second by lexicographic order.
     */
    void sortPopularTags();

    /**
    * if the tag exists in the vector we sum the tag counter of the tag in mostPopularTags
    * if it doesn't exist - create a new pair.
    * @param tag of a tag to add to the mostPopularVector
     */
    void addTag(const std::string &tag);

    /**
     * adds all his tags to the mostPopularTags using addTag
     * @param watchable
     */
    void addTags(Watchable *watchable);
};

#endif