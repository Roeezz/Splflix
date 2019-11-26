#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <utility>

//USER
User::User(const std::string &name)
        :history(), name(name) {}

User::User(const User &other)
        :history(), name(other.name) {
    copy(other);
}

User &User::operator=(const User &other) {
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}

User::User(User &&other)
        :history(), name(other.name) {
    move(std::move(other));
}

User &User::operator=(User &&other) {
    if (this != &other) {
        clear();
        move(std::move(other));
    }
    return *this;
}

User::~User() {
    clear();
}

std::string User::getName() const {
    return name;
}

bool User::isInHistory(const Watchable *watchable) const {
    for(const auto &content: history){
        if(*content == *watchable)
            return true;
    }
    return false;
}

void User::addToHistory(Watchable *watchable) {
    history.push_back(watchable);
}

std::vector<Watchable *> const &User::getHistory() const {
    return history;
}

//private
void User::copy(const User &other) {
    for (auto watchable_ptr : other.history) {
        history.push_back(watchable_ptr->clone());
    }
}

void User::clear() {
    for (auto &watchable_ptr : history) {
        delete watchable_ptr;
        watchable_ptr = nullptr;
    }
    history.clear();
}

void User::move(User &&other) {
    for (auto &watchable_ptr : other.history) {
        history.push_back(watchable_ptr);
        watchable_ptr = nullptr;
    }
}


//LENGTH_RECOMMENDED_USER
LengthRecommenderUser::LengthRecommenderUser(const std::string &name)
        : User(name) , average (0) {
}

LengthRecommenderUser::LengthRecommenderUser(const LengthRecommenderUser &other)
        :  User(other), average(other.average) {
}

LengthRecommenderUser::LengthRecommenderUser(LengthRecommenderUser &&other)
        : User(std::move(other)), average(other.average){
}


LengthRecommenderUser &LengthRecommenderUser::operator=(const LengthRecommenderUser &other) {
    if (this != &other) {
        average = other.average;
        this->User::operator=(other);
    }
    return *this;
}

LengthRecommenderUser &LengthRecommenderUser::operator=(LengthRecommenderUser &&other) {
    if (this != &other) {
        average = other.average;
        this->User::operator=(std::move(other));
    }
    return *this;
}

LengthRecommenderUser::~LengthRecommenderUser() = default;

/**
 *
 * @param s session
 * @return a pointer the next content to be recommended to the user
 */
Watchable *LengthRecommenderUser::getRecommendation(Session &s) {
    return s.GetRecommendationLength(*this, average);
}

/**
 * Creates a clone of this user with a new name.
 * @param name the name of the new user.
 * @return the created clone.
 */
User *LengthRecommenderUser::clone(std::string &name) {
    auto *clone = new LengthRecommenderUser(name);
    *clone = *this;
    return clone;
}

void LengthRecommenderUser::addToHistory(Watchable *watchable) {
    User::addToHistory(watchable);
    recomputeAverage(watchable->getLength());
}

void LengthRecommenderUser::recomputeAverage(int length) {
    int sum = average + length;
    int amountOfWatchables = history.size();
    average = sum / amountOfWatchables;
}

//RERUN_RECOMMENEDED_USER
RerunRecommenderUser::RerunRecommenderUser(const std::string &name)
        : User(name), currentIndex(-1) {
}

RerunRecommenderUser::RerunRecommenderUser(const RerunRecommenderUser &other)
        : User(other), currentIndex(other.currentIndex) {
}

RerunRecommenderUser::RerunRecommenderUser(RerunRecommenderUser &&other)
        : User(std::move(other)), currentIndex(other.currentIndex) {
}

RerunRecommenderUser &RerunRecommenderUser::operator=(const RerunRecommenderUser &other) {
    if (this != &other) {
        currentIndex = other.currentIndex;
        this->User::operator=(other);
    }
    return *this;
}

RerunRecommenderUser &RerunRecommenderUser::operator=(RerunRecommenderUser &&other) {
    if (this != &other) {
        currentIndex = other.currentIndex;
        this->User::operator=(std::move(other));
    }
    return *this;
}

RerunRecommenderUser::~RerunRecommenderUser() = default;

User *RerunRecommenderUser::clone(std::string &name) {
    auto *clone = new RerunRecommenderUser(name);
    *clone = *this;
    return clone;
}

Watchable *RerunRecommenderUser::getRecommendation(Session &s) {
    return  history.at(currentIndex);
}

void RerunRecommenderUser::addToHistory(Watchable *watchable) {
    User::addToHistory(watchable);
    incrementCurrentIndex();
}

void RerunRecommenderUser::incrementCurrentIndex() { currentIndex++; }

//GENRE_RECOMMENDED_USER
GenreRecommenderUser::GenreRecommenderUser(
        const std::string &name)
        : User(name), mostPopularTags() {}

GenreRecommenderUser::~GenreRecommenderUser() = default;

GenreRecommenderUser::GenreRecommenderUser(const GenreRecommenderUser &other)
        : User(other), mostPopularTags(other.mostPopularTags) {
}

GenreRecommenderUser::GenreRecommenderUser(GenreRecommenderUser &&other)
        : User(std::move(other)), mostPopularTags(other.mostPopularTags) {
}

GenreRecommenderUser &GenreRecommenderUser::operator=(const GenreRecommenderUser &other) {
    if (this != &other) {
        mostPopularTags = other.mostPopularTags;
        this->User::operator=(other);
    }
    return *this;
}

GenreRecommenderUser &GenreRecommenderUser::operator=(GenreRecommenderUser &&other) {
    if (this != &other) {
        mostPopularTags = other.mostPopularTags;
        this->User::operator=(std::move(other));
    }
    return *this;
}

User *GenreRecommenderUser::clone(std::string &name) {
    auto *clone = new GenreRecommenderUser(name);
    *clone = *this;
    return clone;
}

Watchable *GenreRecommenderUser::getRecommendation(Session &s) {
    for (auto const &pair : mostPopularTags) {
        Watchable *recommend = s.GetRecommendationGenre(*this, pair.second);
        if (recommend != nullptr) {
            return recommend;
        }
    }
    return nullptr;
}

void GenreRecommenderUser::addTag(const std::string &tag) {
    bool changed = false;
    for (auto &pair : mostPopularTags) {
        if (pair.second == tag) {
            pair.first++;
            changed = true;
            break;
        }
    }
    if (!changed) {
        mostPopularTags.push_back(std::make_pair(1, tag));
    }
    sortPopularTags();
}

bool GenreRecommenderUser::sortInRevLex(const std::pair<int, std::string> &p1, const std::pair<int, std::string> &p2) {
    if (p1.first == p2.first) {
        return p1.second < p2.second;
    }
    return p1.first > p2.first;
}

void GenreRecommenderUser::sortPopularTags() {
    std::sort(mostPopularTags.begin(), mostPopularTags.end(), sortInRevLex);
}

void GenreRecommenderUser::addToHistory(Watchable *watchable) {
    User::addToHistory(watchable);
    addTags(watchable);
}

void GenreRecommenderUser::addTags(Watchable *watchable) {
    for (auto &tag : watchable->getTags()) {
        addTag(tag);
    }

}
