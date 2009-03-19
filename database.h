#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include "newsgroup.h"
#include "article.h"
#include "databaseinterface.h"

namespace client_server {

  /*
   * Class Database.
   */
  class Database : public DatabaseInterface{
	
  public:

    typedef std::map<size_t, Newsgroup>::const_iterator newsgroup_const_iterator;
    typedef std::map<size_t, Newsgroup>::iterator newsgroup_iterator;

    Database();

    // Return the number of newsgroups in the database
    size_t numberOfNewsgroups() const throw(DiskException);
	
    // Return a list of the identification numbers and names of the
    // newsgroups in the database ordered by increasing ID numbers
    std::vector<Newsgroup> listNewsgroups() const throw(DiskException);

    // Create a newsgroup.
    void createNewsgroup(std::string name) throw(NewsgroupExistsException, DiskException);
	
    // Delete a newsgroup.
    void deleteNewsgroup(size_t ID) throw(NoNewsgroupException, DiskException);

    // Return a list of identification numbers and titles of the articles
    // in the newsgroup with the specified ID number if it exists.
    std::vector<Article> listArticles(size_t newsgroupID) throw(NoNewsgroupException, DiskException);

    // Add an article to the specified newsgroup.
    void addArticle(size_t newsgroupID, const std::string& title, const std::string& author, const std::string& text) throw(NoNewsgroupException, DiskException);

    // Delete an article in the specified newsgroup.
    void deleteArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException, DiskException);

    // Returns a specified article in a newsgroup.  
    Article getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException, DiskException);

  private:
    std::map<size_t, Newsgroup> groups;
    size_t newsgroupCount; // Used to create unique ID numbers for Newsgroups
    size_t articleCount;   // Used to create unique ID numbers for Articles

  };

} // End namespace

#endif
