#include "stdlib.h"
#include "stdio.h"
#include "otf2.h"

const char*
OTF2_Error_GetName( OTF2_ErrorCode errorCode ) {
  char* str = malloc(128);
  switch(errorCode) {
  case OTF2_DEPRECATED:
    snprintf(str, 128, "OTF2_DEPRECATED");
    break;
  case OTF2_ABORT:
    snprintf(str, 128, "OTF2_ABORT");
    break;
  case OTF2_WARNING:
    snprintf(str, 128, "OTF2_WARNING");
    break;
  case OTF2_SUCCESS:
    snprintf(str, 128, "OTF2_SUCCESS");
    break;
  case OTF2_ERROR_INVALID:
    snprintf(str, 128, "OTF2_ERROR_INVALID");
    break;
  case OTF2_ERROR_E2BIG:
    snprintf(str, 128, "OTF2_ERROR_E2BIG");
    break;
  case OTF2_ERROR_EACCES:
    snprintf(str, 128, "OTF2_ERROR_EACCES");
    break;
  case OTF2_ERROR_EADDRNOTAVAIL:
    snprintf(str, 128, "OTF2_ERROR_EADDRNOTAVAIL");
    break;
  case OTF2_ERROR_EAFNOSUPPORT:
    snprintf(str, 128, "OTF2_ERROR_EAFNOSUPPORT");
    break;
  case OTF2_ERROR_EAGAIN:
    snprintf(str, 128, "OTF2_ERROR_EAGAIN");
    break;
  case OTF2_ERROR_EALREADY:
    snprintf(str, 128, "OTF2_ERROR_EALREADY");
    break;
  case OTF2_ERROR_EBADF:
    snprintf(str, 128, "OTF2_ERROR_EBADF");
    break;
  case OTF2_ERROR_EBADMSG:
    snprintf(str, 128, "OTF2_ERROR_EBADMSG");
    break;
  case OTF2_ERROR_EBUSY:
    snprintf(str, 128, "OTF2_ERROR_EBUSY");
    break;
  case OTF2_ERROR_ECANCELED:
    snprintf(str, 128, "OTF2_ERROR_ECANCELED");
    break;
  case OTF2_ERROR_ECHILD:
    snprintf(str, 128, "OTF2_ERROR_ECHILD");
    break;
  case OTF2_ERROR_ECONNREFUSED:
    snprintf(str, 128, "OTF2_ERROR_ECONNREFUSED");
    break;
  case OTF2_ERROR_ECONNRESET:
    snprintf(str, 128, "OTF2_ERROR_ECONNRESET");
    break;
  case OTF2_ERROR_EDEADLK:
    snprintf(str, 128, "OTF2_ERROR_EDEADLK");
    break;
  case OTF2_ERROR_EDESTADDRREQ:
    snprintf(str, 128, "OTF2_ERROR_EDESTADDRREQ");
    break;
  case OTF2_ERROR_EDOM:
    snprintf(str, 128, "OTF2_ERROR_EDOM");
    break;
  case OTF2_ERROR_EDQUOT:
    snprintf(str, 128, "OTF2_ERROR_EDQUOT");
    break;
  case OTF2_ERROR_EEXIST:
    snprintf(str, 128, "OTF2_ERROR_EEXIST");
    break;
  case OTF2_ERROR_EFAULT:
    snprintf(str, 128, "OTF2_ERROR_EFAULT");
    break;
  case OTF2_ERROR_EFBIG:
    snprintf(str, 128, "OTF2_ERROR_EFBIG");
    break;
  case OTF2_ERROR_EINPROGRESS:
    snprintf(str, 128, "OTF2_ERROR_EINPROGRESS");
    break;
  case OTF2_ERROR_EINTR:
    snprintf(str, 128, "OTF2_ERROR_EINTR");
    break;
  case OTF2_ERROR_EINVAL:
    snprintf(str, 128, "OTF2_ERROR_EINVAL");
    break;
  case OTF2_ERROR_EIO:
    snprintf(str, 128, "OTF2_ERROR_EIO");
    break;
  case OTF2_ERROR_EISCONN:
    snprintf(str, 128, "OTF2_ERROR_EISCONN");
    break;
  case OTF2_ERROR_EISDIR:
    snprintf(str, 128, "OTF2_ERROR_EISDIR");
    break;
  case OTF2_ERROR_ELOOP:
    snprintf(str, 128, "OTF2_ERROR_ELOOP");
    break;
  case OTF2_ERROR_EMFILE:
    snprintf(str, 128, "OTF2_ERROR_EMFILE");
    break;
  case OTF2_ERROR_EMLINK:
    snprintf(str, 128, "OTF2_ERROR_EMLINK");
    break;
  case OTF2_ERROR_EMSGSIZE:
    snprintf(str, 128, "OTF2_ERROR_EMSGSIZE");
    break;
  case OTF2_ERROR_EMULTIHOP:
    snprintf(str, 128, "OTF2_ERROR_EMULTIHOP");
    break;
  case OTF2_ERROR_ENAMETOOLONG:
    snprintf(str, 128, "OTF2_ERROR_ENAMETOOLONG");
    break;
  case OTF2_ERROR_ENETDOWN:
    snprintf(str, 128, "OTF2_ERROR_ENETDOWN");
    break;
  case OTF2_ERROR_ENETRESET:
    snprintf(str, 128, "OTF2_ERROR_ENETRESET");
    break;
  case OTF2_ERROR_ENETUNREACH:
    snprintf(str, 128, "OTF2_ERROR_ENETUNREACH");
    break;
  case OTF2_ERROR_ENFILE:
    snprintf(str, 128, "OTF2_ERROR_ENFILE");
    break;
  case OTF2_ERROR_ENOBUFS:
    snprintf(str, 128, "OTF2_ERROR_ENOBUFS");
    break;
  case OTF2_ERROR_ENODATA:
    snprintf(str, 128, "OTF2_ERROR_ENODATA");
    break;
  case OTF2_ERROR_ENODEV:
    snprintf(str, 128, "OTF2_ERROR_ENODEV");
    break;
  case OTF2_ERROR_ENOENT:
    snprintf(str, 128, "OTF2_ERROR_ENOENT");
    break;
  case OTF2_ERROR_ENOEXEC:
    snprintf(str, 128, "OTF2_ERROR_ENOEXEC");
    break;
  case OTF2_ERROR_ENOLCK:
    snprintf(str, 128, "OTF2_ERROR_ENOLCK");
    break;
  case OTF2_ERROR_ENOLINK:
    snprintf(str, 128, "OTF2_ERROR_ENOLINK");
    break;
  case OTF2_ERROR_ENOMEM:
    snprintf(str, 128, "OTF2_ERROR_ENOMEM");
    break;
  case OTF2_ERROR_ENOMSG:
    snprintf(str, 128, "OTF2_ERROR_ENOMSG");
    break;
  case OTF2_ERROR_ENOPROTOOPT:
    snprintf(str, 128, "OTF2_ERROR_ENOPROTOOPT");
    break;
  case OTF2_ERROR_ENOSPC:
    snprintf(str, 128, "OTF2_ERROR_ENOSPC");
    break;
  case OTF2_ERROR_ENOSR:
    snprintf(str, 128, "OTF2_ERROR_ENOSR");
    break;
  case OTF2_ERROR_ENOSTR:
    snprintf(str, 128, "OTF2_ERROR_ENOSTR");
    break;
  case OTF2_ERROR_ENOSYS:
    snprintf(str, 128, "OTF2_ERROR_ENOSYS");
    break;
  case OTF2_ERROR_ENOTCONN:
    snprintf(str, 128, "OTF2_ERROR_ENOTCONN");
    break;
  case OTF2_ERROR_ENOTDIR:
    snprintf(str, 128, "OTF2_ERROR_ENOTDIR");
    break;
  case OTF2_ERROR_ENOTEMPTY:
    snprintf(str, 128, "OTF2_ERROR_ENOTEMPTY");
    break;
  case OTF2_ERROR_ENOTSOCK:
    snprintf(str, 128, "OTF2_ERROR_ENOTSOCK");
    break;
  case OTF2_ERROR_ENOTSUP:
    snprintf(str, 128, "OTF2_ERROR_ENOTSUP");
    break;
  case OTF2_ERROR_ENOTTY:
    snprintf(str, 128, "OTF2_ERROR_ENOTTY");
    break;
  case OTF2_ERROR_ENXIO:
    snprintf(str, 128, "OTF2_ERROR_ENXIO");
    break;
  case OTF2_ERROR_EOPNOTSUPP:
    snprintf(str, 128, "OTF2_ERROR_EOPNOTSUPP");
    break;
  case OTF2_ERROR_EOVERFLOW:
    snprintf(str, 128, "OTF2_ERROR_EOVERFLOW");
    break;
  case OTF2_ERROR_EPERM:
    snprintf(str, 128, "OTF2_ERROR_EPERM");
    break;
  case OTF2_ERROR_EPIPE:
    snprintf(str, 128, "OTF2_ERROR_EPIPE");
    break;
  case OTF2_ERROR_EPROTO:
    snprintf(str, 128, "OTF2_ERROR_EPROTO");
    break;
  case OTF2_ERROR_EPROTONOSUPPORT:
    snprintf(str, 128, "OTF2_ERROR_EPROTONOSUPPORT");
    break;
  case OTF2_ERROR_EPROTOTYPE:
    snprintf(str, 128, "OTF2_ERROR_EPROTOTYPE");
    break;
  case OTF2_ERROR_ERANGE:
    snprintf(str, 128, "OTF2_ERROR_ERANGE");
    break;
  case OTF2_ERROR_EROFS:
    snprintf(str, 128, "OTF2_ERROR_EROFS");
    break;
  case OTF2_ERROR_ESPIPE:
    snprintf(str, 128, "OTF2_ERROR_ESPIPE");
    break;
  case OTF2_ERROR_ESRCH:
    snprintf(str, 128, "OTF2_ERROR_ESRCH");
    break;
  case OTF2_ERROR_ESTALE:
    snprintf(str, 128, "OTF2_ERROR_ESTALE");
    break;
  case OTF2_ERROR_ETIME:
    snprintf(str, 128, "OTF2_ERROR_ETIME");
    break;
  case OTF2_ERROR_ETIMEDOUT:
    snprintf(str, 128, "OTF2_ERROR_ETIMEDOUT");
    break;
  case OTF2_ERROR_ETXTBSY:
    snprintf(str, 128, "OTF2_ERROR_ETXTBSY");
    break;
  case OTF2_ERROR_EWOULDBLOCK:
    snprintf(str, 128, "OTF2_ERROR_EWOULDBLOCK");
    break;
  case OTF2_ERROR_EXDEV:
    snprintf(str, 128, "OTF2_ERROR_EXDEV");
    break;
  case OTF2_ERROR_END_OF_FUNCTION:
    snprintf(str, 128, "OTF2_ERROR_END_OF_FUNCTION");
    break;
  case OTF2_ERROR_INVALID_CALL:
    snprintf(str, 128, "OTF2_ERROR_INVALID_CALL");
    break;
  case OTF2_ERROR_INVALID_ARGUMENT:
    snprintf(str, 128, "OTF2_ERROR_INVALID_ARGUMENT");
    break;
  case OTF2_ERROR_INVALID_RECORD:
    snprintf(str, 128, "OTF2_ERROR_INVALID_RECORD");
    break;
  case OTF2_ERROR_INVALID_DATA:
    snprintf(str, 128, "OTF2_ERROR_INVALID_DATA");
    break;
  case OTF2_ERROR_INVALID_SIZE_GIVEN:
    snprintf(str, 128, "OTF2_ERROR_INVALID_SIZE_GIVEN");
    break;
  case OTF2_ERROR_UNKNOWN_TYPE:
    snprintf(str, 128, "OTF2_ERROR_UNKNOWN_TYPE");
    break;
  case OTF2_ERROR_INTEGRITY_FAULT:
    snprintf(str, 128, "OTF2_ERROR_INTEGRITY_FAULT");
    break;
  case OTF2_ERROR_MEM_FAULT:
    snprintf(str, 128, "OTF2_ERROR_MEM_FAULT");
    break;
  case OTF2_ERROR_MEM_ALLOC_FAILED:
    snprintf(str, 128, "OTF2_ERROR_MEM_ALLOC_FAILED");
    break;
  case OTF2_ERROR_PROCESSED_WITH_FAULTS:
    snprintf(str, 128, "OTF2_ERROR_PROCESSED_WITH_FAULTS");
    break;
  case OTF2_ERROR_INDEX_OUT_OF_BOUNDS:
    snprintf(str, 128, "OTF2_ERROR_INDEX_OUT_OF_BOUNDS");
    break;
  case OTF2_ERROR_INVALID_LINENO:
    snprintf(str, 128, "OTF2_ERROR_INVALID_LINENO");
    break;
  case OTF2_ERROR_END_OF_BUFFER:
    snprintf(str, 128, "OTF2_ERROR_END_OF_BUFFER");
    break;
  case OTF2_ERROR_FILE_INTERACTION:
    snprintf(str, 128, "OTF2_ERROR_FILE_INTERACTION");
    break;
  case OTF2_ERROR_FILE_CAN_NOT_OPEN:
    snprintf(str, 128, "OTF2_ERROR_FILE_CAN_NOT_OPEN");
    break;
  case OTF2_ERROR_INTERRUPTED_BY_CALLBACK:
    snprintf(str, 128, "OTF2_ERROR_INTERRUPTED_BY_CALLBACK");
    break;
  case OTF2_ERROR_PROPERTY_NAME_INVALID:
    snprintf(str, 128, "OTF2_ERROR_PROPERTY_NAME_INVALID");
    break;
  case OTF2_ERROR_PROPERTY_EXISTS:
    snprintf(str, 128, "OTF2_ERROR_PROPERTY_EXISTS");
    break;
  case OTF2_ERROR_PROPERTY_NOT_FOUND:
    snprintf(str, 128, "OTF2_ERROR_PROPERTY_NOT_FOUND");
    break;
  case OTF2_ERROR_PROPERTY_VALUE_INVALID:
    snprintf(str, 128, "OTF2_ERROR_PROPERTY_VALUE_INVALID");
    break;
  case OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED:
    snprintf(str, 128, "OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED");
    break;
  case OTF2_ERROR_DUPLICATE_MAPPING_TABLE:
    snprintf(str, 128, "OTF2_ERROR_DUPLICATE_MAPPING_TABLE");
    break;
  case OTF2_ERROR_INVALID_FILE_MODE_TRANSITION:
    snprintf(str, 128, "OTF2_ERROR_INVALID_FILE_MODE_TRANSITION");
    break;
  case OTF2_ERROR_COLLECTIVE_CALLBACK:
    snprintf(str, 128, "OTF2_ERROR_COLLECTIVE_CALLBACK");
    break;
  case OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED:
    snprintf(str, 128, "OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED");
    break;
  case OTF2_ERROR_INVALID_ATTRIBUTE_TYPE:
    snprintf(str, 128, "OTF2_ERROR_INVALID_ATTRIBUTE_TYPE");
    break;
  case OTF2_ERROR_LOCKING_CALLBACK:
    snprintf(str, 128, "OTF2_ERROR_LOCKING_CALLBACK");
    break;
  case OTF2_ERROR_HINT_INVALID:
    snprintf(str, 128, "OTF2_ERROR_HINT_INVALID");
    break;
  case OTF2_ERROR_HINT_LOCKED:
    snprintf(str, 128, "OTF2_ERROR_HINT_LOCKED");
    break;
  case OTF2_ERROR_HINT_INVALID_VALUE:
    snprintf(str, 128, "OTF2_ERROR_HINT_INVALID_VALUE");
    break;
  default:    
    snprintf(str, 128, "Unknown");
  }
  return str;
}


const char*
OTF2_Error_GetDescription( OTF2_ErrorCode errorCode ) {
  int len=1024;
  char* str = malloc(len);

  switch(errorCode) {
  case OTF2_DEPRECATED:
    snprintf(str, len, "Special marker for error messages which indicates a deprecation");
    break;

  case OTF2_ABORT:
    snprintf(str, len, "Special marker when the application will be aborted.");
    break;

  case OTF2_WARNING:
    snprintf(str, len, "Special marker for error messages which are only warnings.");
    break;

  case OTF2_SUCCESS:
    snprintf(str, len, "Operation successful");
    break;
  case OTF2_ERROR_INVALID:
    snprintf(str, len, "Invalid error code");
    break;

  case OTF2_ERROR_E2BIG:
    snprintf(str, len, "The list of arguments is too long");
    break;
  case OTF2_ERROR_EACCES:
    snprintf(str, len, "Not enough rights");
    break;
  case OTF2_ERROR_EADDRNOTAVAIL:
    snprintf(str, len, "Address is not available");
    break;
  case OTF2_ERROR_EAFNOSUPPORT:
    snprintf(str, len, "Address family is not supported");
    break;
  case OTF2_ERROR_EAGAIN:
    snprintf(str, len, "Resource temporarily not available");
    break;
  case OTF2_ERROR_EALREADY:
    snprintf(str, len, "Connection is already processed");
    break;
  case OTF2_ERROR_EBADF:
    snprintf(str, len, "Invalid file pointer");
    break;
  case OTF2_ERROR_EBADMSG:
    snprintf(str, len, "Invalid message");
    break;
  case OTF2_ERROR_EBUSY:
    snprintf(str, len, "Resource or device is busy");
    break;
  case OTF2_ERROR_ECANCELED:
    snprintf(str, len, "Operation was aborted");
    break;
  case OTF2_ERROR_ECHILD:
    snprintf(str, len, "No child process available");
    break;
  case OTF2_ERROR_ECONNREFUSED:
    snprintf(str, len, "Connection was refused");
    break;
  case OTF2_ERROR_ECONNRESET:
    snprintf(str, len, "Connection was reset");
    break;
  case OTF2_ERROR_EDEADLK:
    snprintf(str, len, "Resolved deadlock");
    break;
  case OTF2_ERROR_EDESTADDRREQ:
    snprintf(str, len, "Destination address was expected");
    break;
  case OTF2_ERROR_EDOM:
    snprintf(str, len, "Domain error");
    break;
  case OTF2_ERROR_EDQUOT:
    snprintf(str, len, "Reserved");
    break;
  case OTF2_ERROR_EEXIST:
    snprintf(str, len, "File does already exist");
    break;
  case OTF2_ERROR_EFAULT:
    snprintf(str, len, "Invalid address");
    break;
  case OTF2_ERROR_EFBIG:
    snprintf(str, len, "File is too large");
    break;
  case OTF2_ERROR_EINPROGRESS:
    snprintf(str, len, "Operation is work in progress");
    break;
  case OTF2_ERROR_EINTR:
    snprintf(str, len, "Interruption of an operating system call");
    break;
  case OTF2_ERROR_EINVAL:
    snprintf(str, len, "Invalid argument");
    break;
  case OTF2_ERROR_EIO:
    snprintf(str, len, "Generic I/O error");
    break;
  case OTF2_ERROR_EISCONN:
    snprintf(str, len, "Socket is already connected");
    break;
  case OTF2_ERROR_EISDIR:
    snprintf(str, len, "Target is a directory");
    break;
  case OTF2_ERROR_ELOOP:
    snprintf(str, len, "Too many layers of symbolic links");
    break;
  case OTF2_ERROR_EMFILE:
    snprintf(str, len, "Too many opened files");
    break;
  case OTF2_ERROR_EMLINK:
    snprintf(str, len, "Too many links");
    break;
  case OTF2_ERROR_EMSGSIZE:
    snprintf(str, len, "Message buffer is too small");
    break;
  case OTF2_ERROR_EMULTIHOP:
    snprintf(str, len, "Reserved");
    break;
  case OTF2_ERROR_ENAMETOOLONG:
    snprintf(str, len, "Filename is too long");
    break;
  case OTF2_ERROR_ENETDOWN:
    snprintf(str, len, "Network is down");
    break;
  case OTF2_ERROR_ENETRESET:
    snprintf(str, len, "Connection was reset from the network");
    break;
  case OTF2_ERROR_ENETUNREACH:
    snprintf(str, len, "Network is not reachable");
    break;
  case OTF2_ERROR_ENFILE:
    snprintf(str, len, "Too many opened files");
    break;
  case OTF2_ERROR_ENOBUFS:
    snprintf(str, len, "No buffer space available");
    break;
  case OTF2_ERROR_ENODATA:
    snprintf(str, len, "No more data left in the queue");
    break;
  case OTF2_ERROR_ENODEV:
    snprintf(str, len, "This device does not support this function");
    break;
  case OTF2_ERROR_ENOENT:
    snprintf(str, len, "File or directory does not exist");
    break;
  case OTF2_ERROR_ENOEXEC:
    snprintf(str, len, "Cannot execute binary");
    break;
  case OTF2_ERROR_ENOLCK:
    snprintf(str, len, "Locking failed");
    break;
  case OTF2_ERROR_ENOLINK:
    snprintf(str, len, "Reserved");
    break;
  case OTF2_ERROR_ENOMEM:
    snprintf(str, len, "Not enough main memory available");
    break;
  case OTF2_ERROR_ENOMSG:
    snprintf(str, len, "Message has not the expected type");
    break;
  case OTF2_ERROR_ENOPROTOOPT:
    snprintf(str, len, "This protocol is not available");
    break;
  case OTF2_ERROR_ENOSPC:
    snprintf(str, len, "No space left on device");
    break;
  case OTF2_ERROR_ENOSR:
    snprintf(str, len, "No stream available");
    break;
  case OTF2_ERROR_ENOSTR:
    snprintf(str, len, "This is not a stream");
    break;
  case OTF2_ERROR_ENOSYS:
    snprintf(str, len, "Requested function is not implemented");
    break;
  case OTF2_ERROR_ENOTCONN:
    snprintf(str, len, "Socket is not connected");
    break;
  case OTF2_ERROR_ENOTDIR:
    snprintf(str, len, "This is not a directory");
    break;
  case OTF2_ERROR_ENOTEMPTY:
    snprintf(str, len, "This directory is not empty");
    break;
  case OTF2_ERROR_ENOTSOCK:
    snprintf(str, len, "No socket");
    break;
  case OTF2_ERROR_ENOTSUP:
    snprintf(str, len, "This operation is not supported");
    break;
  case OTF2_ERROR_ENOTTY:
    snprintf(str, len, "This IOCTL is not supported by the device");
    break;
  case OTF2_ERROR_ENXIO:
    snprintf(str, len, "Device is not yet configured");
    break;
  case OTF2_ERROR_EOPNOTSUPP:
    snprintf(str, len, "Operation is not supported by this socket");
    break;
  case OTF2_ERROR_EOVERFLOW:
    snprintf(str, len, "Value is to long for the datatype");
    break;
  case OTF2_ERROR_EPERM:
    snprintf(str, len, "Operation is not permitted");
    break;
  case OTF2_ERROR_EPIPE:
    snprintf(str, len, "Broken pipe");
    break;
  case OTF2_ERROR_EPROTO:
    snprintf(str, len, "Protocol error");
    break;
  case OTF2_ERROR_EPROTONOSUPPORT:
    snprintf(str, len, "Protocol is not supported");
    break;
  case OTF2_ERROR_EPROTOTYPE:
    snprintf(str, len, "Wrong protocol type for this socket");
    break;
  case OTF2_ERROR_ERANGE:
    snprintf(str, len, "Value is out of range");
    break;
  case OTF2_ERROR_EROFS:
    snprintf(str, len, "Filesystem is read only");
    break;
  case OTF2_ERROR_ESPIPE:
    snprintf(str, len, "This seek is not allowed");
    break;
  case OTF2_ERROR_ESRCH:
    snprintf(str, len, "No matching process found");
    break;
  case OTF2_ERROR_ESTALE:
    snprintf(str, len, "Reserved");
    break;
  case OTF2_ERROR_ETIME:
    snprintf(str, len, "Timeout in file stream or IOCTL");
    break;
  case OTF2_ERROR_ETIMEDOUT:
    snprintf(str, len, "Connection timed out");
    break;
  case OTF2_ERROR_ETXTBSY:
    snprintf(str, len, "File could not be executed while it is opened");
    break;
  case OTF2_ERROR_EWOULDBLOCK:
    snprintf(str, len, "Operation would be blocking");
    break;
  case OTF2_ERROR_EXDEV:
    snprintf(str, len, "Invalid link between devices");
    break;

  case OTF2_ERROR_END_OF_FUNCTION:
    snprintf(str, len, "Unintentional reached end of function");
    break;
  case OTF2_ERROR_INVALID_CALL:
    snprintf(str, len, "Function call not allowed in current state");
    break;
  case OTF2_ERROR_INVALID_ARGUMENT:
    snprintf(str, len, "Parameter value out of range");
    break;
  case OTF2_ERROR_INVALID_RECORD:
    snprintf(str, len, "Invalid definition or event record");
    break;
  case OTF2_ERROR_INVALID_DATA:
    snprintf(str, len, "Invalid or inconsistent record data");
    break;
  case OTF2_ERROR_INVALID_SIZE_GIVEN:
    snprintf(str, len, "The given size cannot be used");
    break;
  case OTF2_ERROR_UNKNOWN_TYPE:
    snprintf(str, len, "The given type is not known");
    break;
  case OTF2_ERROR_INTEGRITY_FAULT:
    snprintf(str, len, "The structural integrity is not given");
    break;
  case OTF2_ERROR_MEM_FAULT:
    snprintf(str, len, "This could not be done with the given memory");
    break;
  case OTF2_ERROR_MEM_ALLOC_FAILED:
    snprintf(str, len, "Memory allocation failed");
    break;
  case OTF2_ERROR_PROCESSED_WITH_FAULTS:
    snprintf(str, len, "An error appeared when data was processed");
    break;
  case OTF2_ERROR_INDEX_OUT_OF_BOUNDS:
    snprintf(str, len, "Index out of bounds");
    break;
  case OTF2_ERROR_INVALID_LINENO:
    snprintf(str, len, "Invalid source code line number");
    break;
  case OTF2_ERROR_END_OF_BUFFER:
    snprintf(str, len, "End of buffer/file reached");
    break;
  case OTF2_ERROR_FILE_INTERACTION:
    snprintf(str, len, "Invalid file operation");
    break;
  case OTF2_ERROR_FILE_CAN_NOT_OPEN:
    snprintf(str, len, "Unable to open file");
    break;
  case OTF2_ERROR_INTERRUPTED_BY_CALLBACK:
    snprintf(str, len, "Record reading interrupted by reader callback");
    break;
  case OTF2_ERROR_PROPERTY_NAME_INVALID:
    snprintf(str, len, "Property name does not conform to the naming scheme");
    break;
  case OTF2_ERROR_PROPERTY_EXISTS:
    snprintf(str, len, "Property already exists");
    break;
  case OTF2_ERROR_PROPERTY_NOT_FOUND:
    snprintf(str, len, "Property not found found in this archive");
    break;
  case OTF2_ERROR_PROPERTY_VALUE_INVALID:
    snprintf(str, len, "Property value does not have the expected value");
    break;
  case OTF2_ERROR_FILE_COMPRESSION_NOT_SUPPORTED:
    snprintf(str, len, "Missing library support for requested compression mode");
    break;
  case OTF2_ERROR_DUPLICATE_MAPPING_TABLE:
    snprintf(str, len, "Multiple definitions for the same mapping type");
    break;
  case OTF2_ERROR_INVALID_FILE_MODE_TRANSITION:
    snprintf(str, len, "File mode transition not permitted");
    break;
  case OTF2_ERROR_COLLECTIVE_CALLBACK:
    snprintf(str, len, "Collective callback failed");
    break;
  case OTF2_ERROR_FILE_SUBSTRATE_NOT_SUPPORTED:
    snprintf(str, len, "Missing library support for requested file substrate");
    break;
  case OTF2_ERROR_INVALID_ATTRIBUTE_TYPE:
    snprintf(str, len, "The type of the attribute does not match the expected one.");
    break;
  case OTF2_ERROR_LOCKING_CALLBACK:
    snprintf(str, len, "Locking callback failed");
    break;
  case OTF2_ERROR_HINT_INVALID:
    snprintf(str, len, "The hint is not valid for the current operation mode of OTF2");
    break;
  case OTF2_ERROR_HINT_LOCKED:
    snprintf(str, len, "The hint was either already set by the user or at least once queried from OTF2");
    break; 
  case OTF2_ERROR_HINT_INVALID_VALUE:
    snprintf(str, len, "Invalid value for hint.");
    break;
  }

  return str;
}
