/**
 * Autogenerated by Thrift Compiler (0.13.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "simple_pre_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

#include <sstream>
namespace patch6
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

namespace bm_runtime { namespace simple_pre {

int _kMcOperationErrorCodeValues[] = {
  McOperationErrorCode::TABLE_FULL,
  McOperationErrorCode::INVALID_MGID,
  McOperationErrorCode::INVALID_MGRP_HANDLE,
  McOperationErrorCode::INVALID_L1_HANDLE,
  McOperationErrorCode::ERROR
};
const char* _kMcOperationErrorCodeNames[] = {
  "TABLE_FULL",
  "INVALID_MGID",
  "INVALID_MGRP_HANDLE",
  "INVALID_L1_HANDLE",
  "ERROR"
};
const std::map<int, const char*> _McOperationErrorCode_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(5, _kMcOperationErrorCodeValues, _kMcOperationErrorCodeNames), ::apache::thrift::TEnumIterator(-1, NULL, NULL));

std::ostream& operator<<(std::ostream& out, const McOperationErrorCode::type& val) {
  std::map<int, const char*>::const_iterator it = _McOperationErrorCode_VALUES_TO_NAMES.find(val);
  if (it != _McOperationErrorCode_VALUES_TO_NAMES.end()) {
    out << it->second;
  } else {
    out << static_cast<int>(val);
  }
  return out;
}

std::string to_string(const McOperationErrorCode::type& val) {
  std::map<int, const char*>::const_iterator it = _McOperationErrorCode_VALUES_TO_NAMES.find(val);
  if (it != _McOperationErrorCode_VALUES_TO_NAMES.end()) {
    return std::string(it->second);
  } else {
    return patch6::to_string(static_cast<int>(val));
  }
}


InvalidMcOperation::~InvalidMcOperation() noexcept {
}


void InvalidMcOperation::__set_code(const McOperationErrorCode::type val) {
  this->code = val;
}
std::ostream& operator<<(std::ostream& out, const InvalidMcOperation& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t InvalidMcOperation::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          int32_t ecast0;
          xfer += iprot->readI32(ecast0);
          this->code = (McOperationErrorCode::type)ecast0;
          this->__isset.code = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t InvalidMcOperation::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("InvalidMcOperation");

  xfer += oprot->writeFieldBegin("code", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32((int32_t)this->code);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(InvalidMcOperation &a, InvalidMcOperation &b) {
  using ::std::swap;
  swap(a.code, b.code);
  swap(a.__isset, b.__isset);
}

InvalidMcOperation::InvalidMcOperation(const InvalidMcOperation& other1) : TException() {
  code = other1.code;
  __isset = other1.__isset;
}
InvalidMcOperation& InvalidMcOperation::operator=(const InvalidMcOperation& other2) {
  code = other2.code;
  __isset = other2.__isset;
  return *this;
}
void InvalidMcOperation::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "InvalidMcOperation(";
  out << "code=" << to_string(code);
  out << ")";
}

const char* InvalidMcOperation::what() const noexcept {
  try {
    std::stringstream ss;
    ss << "TException - service has thrown: " << *this;
    this->thriftTExceptionMessageHolder_ = ss.str();
    return this->thriftTExceptionMessageHolder_.c_str();
  } catch (const std::exception&) {
    return "TException - service has thrown: InvalidMcOperation";
  }
}

}} // namespace
