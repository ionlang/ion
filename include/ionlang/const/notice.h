#pragma once

// Misc.
#define IONLANG_NOTICE_MISC_UNEXPECTED_TOKEN "Unexpected token '%s'"
#define IONLANG_NOTICE_MISC_UNDEFINED_REFERENCE "Reference to an undefined entity '%s'"
#define IONLANG_NOTICE_MISC_NO_ENTRY_POINT "Could not find a suitable entry point"

// Variable.
#define IONLANG_NOTICE_VARIABLE_UNUSED "Unused variable '%s'"
#define IONLANG_NOTICE_VARIABLE_REDECLARATION "Redeclaration of variable '%s'"
#define IONLANG_NOTICE_VARIABLE_ALREADY_DEFINED "Variable '%s' is already defined in this scope"
#define IONLANG_NOTICE_VARIABLE_REASSIGN_CONST "Cannot reassign a value to a constant variable '%s'"

// Function.
#define IONLANG_NOTICE_FUNCTION_UNUSED "Unused function '%s'"
#define IONLANG_NOTICE_FUNCTION_UNUSED_ARGUMENT "Unused function argument '%s'"
#define IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED "Function '%s' is already defined in this scope"
#define IONLANG_NOTICE_FUNCTION_REDEFINITION_DIFFERENT_SIG "Function '%s' is being redefined with a different signature"
#define IONLANG_NOTICE_FUNCTION_INVALID_RETURN_TYPE "Expected function '%' to return value of type '%' but got '%s'"
#define IONLANG_NOTICE_FUNCTION_INVALID_CALL_ARGUMENTS "Invalid argument(s) were provided when calling '%s'"
#define IONLANG_NOTICE_FUNCTION_MISSING_RETURN "Missing return value for function '%s' returning type '%s'"
