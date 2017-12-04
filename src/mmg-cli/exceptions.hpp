#ifndef _CLI_EXCEPTIONS_
#define _CLI_EXCEPTIONS_

#include <exception>

//thrown when an error message has already been printed, so the exception should not be displayed further
class already_handled_error : public std::exception {};

#endif
