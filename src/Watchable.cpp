#include "../include/Watchable.h"
#include "../include/Session.h"

//WATCHABLE
//constructors and operators
Watchable::Watchable(long id, int length, const std::vector<std::string> &tags) : id(id), length(length),
                                                                                  tags(tags) {}
Watchable::Watchable(const Watchable &watchable) = default;

Watchable &Watchable::operator=(const Watchable &other) {
    if (&other != this) {
        length = other.length;
        tags = other.tags;
    }
    return *this;
}

bool Watchable::operator==(const Watchable &other) const {
    return id == other.id;
}

//getters
int Watchable::getLength() const {
    return length;
}

long Watchable::getId() const {
    return id;
}

std::string Watchable::tagsToString() const {
    std::string tagsString = "[";
    for (const auto &tag: tags) {
        if (tag != tags.back()) {
            tagsString.append(tag).append(", ");
        } else {
            tagsString.append(tag).append("]");
        }
    }
    return tagsString;
}

std::vector<std::string> const &Watchable::getTags() const {
    return tags;
}

bool Watchable::checkInTags(const std::string &tag) const {

    return std::find(tags.begin(), tags.end(), tag) != tags.end();

}

Watchable::~Watchable() = default;


//MOVIE
Movie::Movie(long id, const std::string &name, int length, const std::vector<std::string> &tags) : Watchable(id, length,
                                                                                                             tags), name(name){}

Movie::Movie(Movie &movie) = default;

Movie &Movie::operator=(const Movie &other) {
    if (&other != this) {
        name = other.name;
        this->Watchable::operator=(other);
    }
    return *this;
}

bool Movie::operator==(const Watchable &other) const {
    if(Watchable::operator==(other)) {
        return getName() == other.getName();
    }
    return false;
}

Watchable *Movie::clone() {
    return new Movie(*this);
}

Movie::~Movie() = default;

std::string Movie::toString() const {

    return name;
}

Watchable *Movie::getNextWatchable(Session &sess) const {
    User *activeUser = sess.getActiveUser();
    Watchable *recommendation = activeUser->getRecommendation(sess);
    return recommendation;
}

std::string Movie::getName() const {
    return name;
}


//EPISODE
Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode,
                 const std::vector<std::string> &tags) : Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode),
                                                         nextEpisodeId(id + 1) {
}

Episode::Episode(Episode &other) = default;

Watchable *Episode::getNextWatchable(Session &sess) const {
    long nextId = getId() + 1;
    Watchable *next = sess.getWatchable(nextId);
    if(next && getName() == next->getName()){
        return next;
    }
    return sess.getActiveUser()->getRecommendation(sess);
}

Watchable *Episode::clone() {
    return new Episode(*this);
}

Episode &Episode::operator=(const Episode &other) {
    if (&other != this) {
        seriesName = other.seriesName;
        episode = other.episode;
        season = other.season;
        nextEpisodeId = other.nextEpisodeId;
        this->Watchable::operator=(other);
    }
    return *this;
}

bool Episode::operator==(const Watchable &other) const {
    if(Watchable::operator==(other)){
        return (getName() == other.getName());
    }
    return false;
}

Episode::~Episode() = default;

std::string Episode::getName() const {
    return seriesName;
}

std::string Episode::toString() const {
    std::string output = seriesName + " S" + std::to_string(season) + "E" + std::to_string(episode);
    return output;
}
