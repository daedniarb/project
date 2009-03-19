#ifndef DISKDATABASE_H
#define DISKDATABASE_H

#include <vector>
#include "databaseinterface.h"
#include "newsgroup.h"
#include "article.h"
#include <dirent.h>
#include <string>
#include <sys/types.h>

namespace client_server {


  /*
   * Defines the base class for the Database classes.
   */
  class DiskDatabase : public DatabaseInterface {
	
  public:

    // Creates a diskdatabase with the root of the 
    // newsgroup-directory placed in directory..!
    // directory can be either a relative or absolute address.
    DiskDatabase(const char* directory) throw(DiskException);
    
    // Creates a diskdatabase with the root of the 
    // newsgroup-directory placed in "newsgroups".
    DiskDatabase() throw(DiskException);

    ~DiskDatabase();

    size_t numberOfNewsgroups() const throw(DiskException);

    std::vector<Newsgroup> listNewsgroups() const throw(DiskException);

    void createNewsgroup(std::string name) throw(NewsgroupExistsException, DiskException);

    void deleteNewsgroup(size_t ID) throw(NoNewsgroupException, DiskException);

    std::vector<Article> listArticles(size_t newsgroupID) throw(NoNewsgroupException, DiskException);

    void addArticle(size_t newsgroupID, const std::string& title, const std::string& author, const std::string& text) throw(NoNewsgroupException, DiskException);

    void deleteArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException, DiskException);

    Article getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException, DiskException);

  private:
    void openRootDirectory(const char* path) throw(DiskException);
    void setHighestId() throw(DiskException);
    void setArticleId(std::string newsgroupPath) throw(DiskException);
    void setErrorMessage(std::string& str, int pError) const;
    bool newsgroupExists(std::string name) throw(DiskException);
    std::string rootPath;
    size_t newsgroupId;
    size_t articleId;
    DIR* root;
  };

} // End namespace
#endif
