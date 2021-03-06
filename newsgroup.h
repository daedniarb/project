#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include <map>
#include "article.h"

namespace client_server {
    

  class NoArticleException{};

  /*
   * Describes a newsgroup. A Newsgroup contains a number of Articles related to the same topic.
   */
  class Newsgroup{

  public:
    // Create a new, empty Newsgroup. All Newsgroups must have a name.
    Newsgroup(size_t id, std::string name);

    // Default constructor 
    Newsgroup();

    Newsgroup(const Newsgroup& rhs);

    Newsgroup& operator=(const Newsgroup& rhs);

    const std::string getName() const;

    const size_t& getID() const;

    size_t numberOfArticles() const;

    // Return an iterator to the first <articleID, article> pair in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    std::map<size_t, Article>::const_iterator firstPair() const;

    // Return an iterator to one past the last <articleID, article> pair in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    std::map<size_t, Article>::const_iterator endPair() const;

    // Returns a reference to the article with ID-number ID if it exists in the newsgroup.
    const Article& getArticle(size_t ID) const throw(NoArticleException);

    // Add an article to the newsgroup.
    void addArticle(const Article& article);
    
    // Delete an article from the newsgroup.
    void deleteArticle(size_t ID) throw(NoArticleException);


  private:
//    static size_t ID_count;

    // Unique identification number.
    size_t ID;
    
    // Name - uniqueness to be determined by the database.
    std::string name;
    
    // A map containing all the Articles.
    std::map<size_t, Article> articles;
  };


} // End namespace
#endif
