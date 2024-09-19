// Minimal source file for a placebo class that does nothing but allows the
// pipeline to pass.

#ifndef CODE_COMPLETION_H_INCLUDED
#define CODE_COMPLETION_CLASS_H_INCLUDED

#include <string>
#include <vector>

class CodeCompletion {
 public:
  CodeCompletion() {
  }
  virtual ~CodeCompletion() {
  }

  /**
   * Read files in a given directory (and sub-directories)
   * to populate the code completion database.
   * @param directory Relative path to the directory.
   */
  virtual void readData(std::string directory) = 0;

  /**
   * Return a
   * @param query A string with which to query the code completion system.
   * @return A pointer to an ordered list of suggestions for the query in descending order of
   * relevance. The maximum number of suggestions is 7.
   */
  virtual std::vector<std::string>* getSuggestions(const std::string query) = 0;
};
#endif //CODE_COMPLETION_H_INCLUDED
