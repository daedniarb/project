#ifndef DISKDATABASE_H
#define DISKDATABASE_H

#include <vector>
#include "databaseinterface.h"
#include "newsgroup.h"
#include "article.h"
#include <dirent.h>
#include <sys/types.h>

namespace client_server {

  /*
   * Defines the base class for the Database classes.
   */
  class DiskDatabase : public DatabaseInterface{
	
  public:

    // Creates a diskdatabase with the root of the 
    // newsgroup-directory placed in directory..!
    // directory can be either a relative or absolute address.
    DiskDatabase(char* directory);
    
    // Creates a diskdatabase with the root of the 
    // newsgroup-directory placed in "newsgroups".
    DiskDatabase();

    ~DiskDatabase();

    size_t numberOfNewsgroups() const;

    std::vector<Newsgroup> listNewsgroups() const;

    void createNewsgroup(std::string name) throw(NewsgroupExistsException);

    void deleteNewsgroup(size_t ID) throw(NoNewsgroupException);

    std::vector<Article> listArticles(size_t newsgroupID) throw(NoNewsgroupException);

    void addArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException);

    void deleteArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException);

    Article getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException);

  private:
    void openRootDirectory(const char* path);
    void setHighestId();
    std::string getNewsgroupPath(size_t id);
    bool newsgroupExists(std::string name);
    size_t getNextArticleId(std::string newsgroupPath);
    std::string rootPath;
    size_t highestId;
    DIR* root;
  };

} // End namespace
#endif
