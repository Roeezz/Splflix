#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>
#include <algorithm>

class Session;


class Watchable {
public:
    //constructor
    Watchable(long id, int length, const std::vector<std::string> &tags);

    //copy constructor
    Watchable(const Watchable &watchable);

    //assignment operator
    Watchable &operator=(const Watchable &other);

    //isEqual operator
    virtual bool operator==(const Watchable &other) const;

    //Clone
    virtual Watchable *clone() = 0;

    int getLength() const;

    long getId() const;

    //destructor
    virtual ~Watchable();

    virtual std::string toString() const = 0;

    virtual Watchable* getNextWatchable(Session&) const = 0;

    bool checkInTags(const std::string &tag) const;

    /**
     * Creates and returns a string representation of the tags vector int the format: [tag1, tag2,...,tagN].
     */
    std::string tagsToString() const;

    std::vector<std::string> const &getTags() const;

    virtual std::string getName() const = 0;

private:
    const long id;
    int length;
    std::vector<std::string> tags;
};


class Movie : public Watchable {
public:
    Movie(long id, const std::string &name, int length, const std::vector<std::string> &tags);

    Movie(Movie &movie);

    Movie &operator=(const Movie &movie);

    virtual bool operator==(const Watchable &other) const;

    virtual std::string toString() const;

    virtual Watchable *getNextWatchable(Session &) const;

    virtual Watchable *clone();

    virtual ~Movie();

    virtual std::string getName() const;

private:
    std::string name;
};


class Episode : public Watchable {
public:
    Episode(long id, const std::string &seriesName, int length, int season, int episode,
            const std::vector<std::string> &tags);

    Episode(Episode &other);

    Episode &operator=(const Episode &other);

    bool operator==(const Watchable &other) const override;

    virtual std::string toString() const;

    virtual Watchable *getNextWatchable(Session &) const;

    virtual Watchable *clone();

    virtual ~Episode();

    virtual std::string getName() const;
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif
