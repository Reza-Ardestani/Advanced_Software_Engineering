/*
 *CPSC4210 Fall 2022
 *Author: Kevin Masson
 *Copyright 2022 Matthew Ursaki and Kevin Masson
 */

#ifndef INCLUDE_EXCEPTIONS_H_
#define INCLUDE_EXCEPTIONS_H_

#include <stdexcept>

/**
 * Exception for an invalid parameter passed to a constructor or method.
 */
class input_error : public std::runtime_error {
 public:
  /**
   * @param errMessage An error message.
   */
  explicit input_error(const char *errMessage)
      :
      std::runtime_error(errMessage) {
  }
};

#endif  // INCLUDE_EXCEPTIONS_H_
