#include "diskdatabase.h"
#include "article.h"
#include "util.h"
#include "newsgroup.h"
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
using namespace std;

namespace client_server {
  DiskDatabase::DiskDatabase() throw(DiskException) {
    openRootDirectory("newsgroups");
    setHighestId();
  }

  DiskDatabase::DiskDatabase(const char* directory) throw(DiskException) {
    openRootDirectory(directory);
    setHighestId();
  }

  void DiskDatabase::openRootDirectory(const char* path) throw(DiskException) {
    int res = 0;
    root = opendir(path);
    rootPath = string(path);
    if (!root) {
      // Could not open root-directory, try to create it..
      errno = 0;
      res = mkdir(path, 0700);
      if (!res) {
	// Successfully created root-directory, try to open it..
	root = opendir(path);
	if (!root) {
	  string error("Fatal error, could not open working directory ");
	  error += rootPath;
	  error += " after creating it!\n";
	  setErrorMessage(error, errno);
	  throw DiskException(error);
	}
      } else {
	string error("Fatal error, could not create directory ");
	error += rootPath;
	error += "!\n";
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    }
    // Working directory opened, database operational!
  }

  void DiskDatabase::setErrorMessage(std::string& str, int pError) const {
    errno = 0;
    switch (pError) {
    case EACCES: 
      str += "Permission denied.\n";
      break;
    case EBADF: 
      str += "fd is not a valid file descriptor opened for reading.\n";
      break;
    case EMFILE: 
      str += "Too many file descriptors in use by process.\n";
      break;
    case ENFILE: 
      str += "Too many files are currently open in the system.\n";
      break;
    case ENOMEM: 
      str += "Insufficient memory to complete the operation.\n";
      break;
    case EEXIST: 
      str += "pathname already exists (not necessarily as a directory). This includes the case where pathname is a symbolic link, dangling or not.\n";
      break;
    case EFAULT: 
      str += "pathname points outside your accessible address space.\n";
      break;
    case ELOOP:  
      str += "Too many symbolic links were encountered in resolving pathname.\n";
      break;
    case ENAMETOOLONG:
      str += "pathname was too long.\n";
      break;
    case ENOSPC:
      str += "The new directory cannot be created because the user's disk quota is exhausted.\n";
      break;
    case ENOTDIR:
      str += "A component used as a directory in pathname is not, in fact, a directory.\n";
      break;
    case EPERM:
      str += "The file system containing pathname does not support the creation/deletion of directories.\n";
      break;
    case EROFS:  
      str += "pathname refers to a file on a read-only file system.\n";
      break;
    case EBUSY:
      str += "The file pathname cannot be unlinked because it is being used by the system or another process and the implementation considers this an error.\n";
      break;
    case EIO:
      str += "An I/O error occurred.\n";
      break;
    case ENOENT:
      str += "A component in pathname does not exist or is a dangling symbolic link, or pathname is empty.\n";
      break;
    case EINVAL:
      str += "pathname has . as last component.\n";
      break;
    case ENOTEMPTY:
      str += "pathname contains entries other than . and .. ; or, pathname has .. as its final component. POSIX.1-2001 also allows EEXIST for this condition.\n";
      break;
    }
  }

  void DiskDatabase::setArticleId(string newsgroupPath) throw(DiskException) {
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(newsgroupPath.c_str());
    if (!newsgroupRoot) {
      string error("Could not open newsgroup directory for reading!\n");
      setErrorMessage(error, errno);
      throw DiskException(error);
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG && entry->d_name[0] != 'n') {
	  string str(entry->d_name);
	  size_t id = stringtosizet(str);
	  if (id > articleId) {
	    articleId = id;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	string error("Error occured while fetching highest article id!\n");
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    }
  }

  void DiskDatabase::setHighestId() throw(DiskException) {
    struct dirent* entry;
    newsgroupId = 0;
    articleId = 0;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	str.erase(0, ++sharp);
	size_t id = stringtosizet(str);
	if (id > newsgroupId) {
	  newsgroupId = id;
	}
	setArticleId(rootPath + "/" + entry->d_name);
      }
    }
    if (errno) {
      string error("Error occured while fetching highest newsgroup id!\n");
      setErrorMessage(error, errno);
      throw DiskException(error);
    }
    ++articleId;
  }

  DiskDatabase::~DiskDatabase() {
    closedir(root);
  }

  size_t DiskDatabase::numberOfNewsgroups() const throw(DiskException) {
    rewinddir(root);
    struct dirent* entry;
    size_t groups=0;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	++groups;
      }
    }
    if (errno) {
      string error("Error occured while fetching number of newsgroups!\n");
      setErrorMessage(error, errno);
      throw DiskException(error);
    }
    
    return groups;
  }

  vector<Newsgroup> DiskDatabase::listNewsgroups() const throw(DiskException) {
    vector<Newsgroup> v;
    rewinddir(root);
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	size_t id = stringtosizet(string(entry->d_name));
	string title;
	ifstream ifs((rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	if (ifs.good()) {
	  string line;
	  while(getline(ifs,line)) {
	    title += line;
	  }
	  v.push_back(Newsgroup(id, title));
	} else {
	  string error("Error, could not read newsgroupdata:");
	  error += rootPath + "/" + entry->d_name + "/newsgroup";
	  error += " \n";
	  setErrorMessage(error, errno);
	  throw DiskException(error);
	}
	ifs.close();
      }
    }
    if (errno) {
     string error("Error occured while listing newsgroups!\n");
     setErrorMessage(error, errno);
     throw DiskException(error);
    }
    
    return v;
  }

  void DiskDatabase::createNewsgroup(string name) throw(NewsgroupExistsException, DiskException) {
    if (newsgroupExists(name)) {
      throw NewsgroupExistsException();
    }
    rewinddir(root);
    string dirName = rootPath;
    dirName += "/";
    dirName += sizettostring(newsgroupId);
    DIR* prev = opendir(dirName.c_str());
    if(!prev && errno == ENOENT) {
      int res = mkdir(dirName.c_str(), 0700);
      if (res) {
	string error("An error occured while making newsgroup directory: ");
	error += name;
	error += " \n";
	setErrorMessage(error, errno);
	throw DiskException(error);
      } else {
	ofstream ofs((dirName + "/newsgroup").c_str());
	if (ofs.good()) {
	  ofs << name;
	  ++newsgroupId;
	} else {
	  string error("Could not open/create filestream to newsgroupdata: ");
	  error += dirName + "/newsgroup";
	  error += " \n";
	  setErrorMessage(error, errno);
	  throw DiskException(error);
	}
	ofs.close();	
      }
      closedir(prev);
      errno=0;
    } else {
      closedir(prev);
      errno=0;
      throw NewsgroupExistsException();
    }
  }

  bool DiskDatabase::newsgroupExists(string name) throw(DiskException) {
    rewinddir(root);
    struct dirent* entry;
    bool rtr = false;
    while((entry = readdir(root))) {
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	string title;
	ifstream ifs((rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	if (ifs.good()) {
	  string line;
	  while(getline(ifs,line)) {
	    title += line;
	  }
	  if (title.compare(name) == 0) {
	    rtr = true;
	  }
	} else {
	  string error("Error, could not read newsgroupdata: ");
	  error += rootPath + "/" + entry->d_name + "/newsgroup";
	  error += " \n";
	  setErrorMessage(error, errno);
	  throw DiskException(error);
	}
	ifs.close();
      }
    }
    if (errno) {
      string error("Error, could not verify if newsgroup: ");
      error += name;
      error += " exists!\n";
      setErrorMessage(error, errno);
      throw DiskException(error);
    }
    
    return rtr;
  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException, DiskException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(ID);
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	throw NoNewsgroupException();
      } else {
	string error("Could not open newsgroup directory for reading!\n");
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    } else {
      int res = 0;
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	  string error("Error, newsgroup ");
	  error += entry->d_name;
	  error += " contains directories!)";
	  throw DiskException(error);
	} else if (entry->d_type == DT_REG) {
	  string filePath(path);
	  filePath += "/";
	  filePath += entry->d_name;
	  res = remove(filePath.c_str()); 
	  if (errno || res) {	    
	    string error("Error, could not remove content of newsgroup: ");
	    error += filePath;
	    error += " due to:\n";
	    setErrorMessage(error, errno);
	    throw DiskException(error);
	  }
	}
      }
      
      closedir(newsgroupRoot);
      res = rmdir(path.c_str());
      if (errno || res) {
	string error("Error, could not remove newsgroup directory with id: ");
	error += sizettostring(ID);
	error += "!\n";
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    }
  }

  vector<Article> DiskDatabase::listArticles(size_t newsgroupID) throw(NoNewsgroupException, DiskException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    
    rewinddir(root);
    struct dirent* entry;
    vector<Article> v;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
     if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG && entry->d_name[0] != 'n') {
	  string str(entry->d_name);
	  size_t id = stringtosizet(str);
	  ifstream ifs((path + entry->d_name).c_str());
	  if(ifs.good()) {
	    string title;
	    getline(ifs, title);
	    string author;
	    getline(ifs, author);
	    string text;
	    string line;
	    while(getline(ifs, line)) {
	      text += line;
	    }
	    v.push_back(Article(id, title, author, text));
	  } else {
	    printf("Could not read %s, ignoring\n", (path +entry->d_name).c_str());
	  }
	  ifs.close();
	}
      }
      
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while listing articles: %d\n", pError);
      }
    }
    
    return v;
  }

  void DiskDatabase::addArticle(size_t newsgroupID,  const std::string& title, const std::string& author, const std::string& text) throw(NoNewsgroupException, DiskException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    DIR* newsgroupRoot = opendir(path.c_str());
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	string error("Could not open newsgroup directory for reading!\n");
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    }
    closedir(newsgroupRoot);
    errno = 0;
    path += "/";
    path += sizettostring(articleId);
    ofstream ofs(path.c_str());
    if (ofs.good()) {
      ofs << title << "\n";
      ofs << author << "\n";
      ofs << text << "\n";
    } else {
      string error("Could not open filestream to: ");
      error += path;
      error += " \n";
      setErrorMessage(error, errno);
      throw DiskException(error);
    }
    ++articleId;
    ofs.close();
  }

  void DiskDatabase::deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException, DiskException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	string error("Could not open newsgroup directory for reading!\n");
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG && entry->d_name[0] != 'n') {
	  string str(entry->d_name);
	  size_t id = stringtosizet(str);
	  if (id == articleID) {
	    remove((path + entry->d_name).c_str());
	    if (errno) {
	      string error("Could not delete article with id: ");
	      error += sizettostring(articleID);
	      error += " \n";
	      setErrorMessage(error, errno);
	      throw DiskException(error);
	    }
	    closedir(newsgroupRoot);
	    return;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	string error("Error occured while trying to delete article with id: ");
	error += sizettostring(articleID);
	error += " \n";
	setErrorMessage(error, errno);
	throw DiskException(error);
      } else {
	throw NoArticleException();
      }
    }
  }

  Article DiskDatabase::getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException, DiskException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    Article* article = 0;
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	string error("Could not open newsgroup directory for reading!\n");
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG && entry->d_name[0] != 'n') {
	  string str(entry->d_name);
	  size_t id = stringtosizet(str);
	  if (id == articleID) {
	    ifstream ifs((path + entry->d_name).c_str());
	    if(ifs.good()) {
	      string title;
	      getline(ifs, title);
	      string author;
	      getline(ifs, author);
	      string text;
	      string line;
	      while(getline(ifs, line)) {
		text += line;
	      }
	      article = new Article(id, title, author, text);
	    } else {
	      string error("Could not open filestream to: ");
	      error += path +entry->d_name;
	      error += " \n";
	      setErrorMessage(error, errno);
	      throw DiskException(error);
	    }
	    ifs.close();
	    break;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	string error("Error occured while trying to read article with id: ");
	error += sizettostring(articleID);
	error += " \n";
	setErrorMessage(error, errno);
	throw DiskException(error);
      }
      if(article == 0) {
	throw NoArticleException();
      }
    }
    Article rtr = *article;
    delete article;
    return rtr;
  }

}
