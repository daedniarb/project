#include "newsgroup.h"
#include "article.h"

namespace client_server {

    typedef Newsgroup::article_const_iterator article_const_iterator;
    typedef Newsgroup::article_iterator article_iterator;

    size_t Newsgroup::ID_count = 0;  // Initialize the identification number counter.

    // Create a new, empty Newsgroup. All Newsgroups must have a name.
    Newsgroup::Newsgroup(std::string name)
	: ID(ID_count++), name(name)
    {}

    // Default constructor 
    Newsgroup::Newsgroup()
	: ID(ID_count++), name("")
    {}

    std::string Newsgroup::getName() const
    {
	return name;
    }


    const size_t Newsgroup::getID() const
    {
	return ID;
    }

    const size_t Newsgroup::numberOfArticles() const
    {
	return articles.size();
    }

    // Return an iterator to the first article in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    article_const_iterator Newsgroup::firstArticle() const
    {
	return articles.begin();
    }

    // Return an iterator to one past the last article in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    article_const_iterator Newsgroup::endArticles() const
    {
	return articles.end();
    }


    // Returns a reference to the article with ID-number ID if it exists in the newsgroup, else 0.
    const Article& Newsgroup::getArticle(size_t ID) const throw(NoArticleException)
    {
	article_const_iterator res = articles.find(ID);

	if(res == articles.end())
	    throw NoArticleException();
	else
	    return res->second;
    }

    // Add an article to the newsgroup.
    void Newsgroup::addArticle(const Article& article)
    {
	articles.insert(std::make_pair(article.getID(), article));
    }
    
    // Delete an article from the newsgroup.
    void Newsgroup::deleteArticle(size_t ID) throw(NoArticleException)
    {
	size_t res = articles.erase(ID);
	
	if(res == 0)
	    throw NoArticleException();
    }


} // End namespace
