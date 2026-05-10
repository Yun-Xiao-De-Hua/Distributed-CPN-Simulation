//
// Generated file, do not edit! Created by opp_msgtool 6.0 from inet/distributed_computing_power_network/message/cpn_message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "cpn_message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace inet {

Register_Class(CprpRequestMsg)

CprpRequestMsg::CprpRequestMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(12 + 40));

}

CprpRequestMsg::CprpRequestMsg(const CprpRequestMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CprpRequestMsg::~CprpRequestMsg()
{
}

CprpRequestMsg& CprpRequestMsg::operator=(const CprpRequestMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CprpRequestMsg::copy(const CprpRequestMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    this->userGatewayAddress = other.userGatewayAddress;
    this->generationTime = other.generationTime;
    this->computingType = other.computingType;
    this->requiredStorage = other.requiredStorage;
    this->computingAmount = other.computingAmount;
    this->transferAmount = other.transferAmount;
    this->totalDelayRequirement = other.totalDelayRequirement;
    this->budget = other.budget;
    this->userMaxBandwidth = other.userMaxBandwidth;
}

void CprpRequestMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->userGatewayAddress);
    doParsimPacking(b,this->generationTime);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->requiredStorage);
    doParsimPacking(b,this->computingAmount);
    doParsimPacking(b,this->transferAmount);
    doParsimPacking(b,this->totalDelayRequirement);
    doParsimPacking(b,this->budget);
    doParsimPacking(b,this->userMaxBandwidth);
}

void CprpRequestMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->userGatewayAddress);
    doParsimUnpacking(b,this->generationTime);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->requiredStorage);
    doParsimUnpacking(b,this->computingAmount);
    doParsimUnpacking(b,this->transferAmount);
    doParsimUnpacking(b,this->totalDelayRequirement);
    doParsimUnpacking(b,this->budget);
    doParsimUnpacking(b,this->userMaxBandwidth);
}

int CprpRequestMsg::getUserId() const
{
    return this->userId;
}

void CprpRequestMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int CprpRequestMsg::getTaskId() const
{
    return this->taskId;
}

void CprpRequestMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * CprpRequestMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CprpRequestMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

const L3Address& CprpRequestMsg::getUserGatewayAddress() const
{
    return this->userGatewayAddress;
}

void CprpRequestMsg::setUserGatewayAddress(const L3Address& userGatewayAddress)
{
    handleChange();
    this->userGatewayAddress = userGatewayAddress;
}

::omnetpp::simtime_t CprpRequestMsg::getGenerationTime() const
{
    return this->generationTime;
}

void CprpRequestMsg::setGenerationTime(::omnetpp::simtime_t generationTime)
{
    handleChange();
    this->generationTime = generationTime;
}

int CprpRequestMsg::getComputingType() const
{
    return this->computingType;
}

void CprpRequestMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

double CprpRequestMsg::getRequiredStorage() const
{
    return this->requiredStorage;
}

void CprpRequestMsg::setRequiredStorage(double requiredStorage)
{
    handleChange();
    this->requiredStorage = requiredStorage;
}

double CprpRequestMsg::getComputingAmount() const
{
    return this->computingAmount;
}

void CprpRequestMsg::setComputingAmount(double computingAmount)
{
    handleChange();
    this->computingAmount = computingAmount;
}

double CprpRequestMsg::getTransferAmount() const
{
    return this->transferAmount;
}

void CprpRequestMsg::setTransferAmount(double transferAmount)
{
    handleChange();
    this->transferAmount = transferAmount;
}

::omnetpp::simtime_t CprpRequestMsg::getTotalDelayRequirement() const
{
    return this->totalDelayRequirement;
}

void CprpRequestMsg::setTotalDelayRequirement(::omnetpp::simtime_t totalDelayRequirement)
{
    handleChange();
    this->totalDelayRequirement = totalDelayRequirement;
}

double CprpRequestMsg::getBudget() const
{
    return this->budget;
}

void CprpRequestMsg::setBudget(double budget)
{
    handleChange();
    this->budget = budget;
}

double CprpRequestMsg::getUserMaxBandwidth() const
{
    return this->userMaxBandwidth;
}

void CprpRequestMsg::setUserMaxBandwidth(double userMaxBandwidth)
{
    handleChange();
    this->userMaxBandwidth = userMaxBandwidth;
}

class CprpRequestMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_userGatewayAddress,
        FIELD_generationTime,
        FIELD_computingType,
        FIELD_requiredStorage,
        FIELD_computingAmount,
        FIELD_transferAmount,
        FIELD_totalDelayRequirement,
        FIELD_budget,
        FIELD_userMaxBandwidth,
    };
  public:
    CprpRequestMsgDescriptor();
    virtual ~CprpRequestMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CprpRequestMsgDescriptor)

CprpRequestMsgDescriptor::CprpRequestMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CprpRequestMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CprpRequestMsgDescriptor::~CprpRequestMsgDescriptor()
{
    delete[] propertyNames;
}

bool CprpRequestMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CprpRequestMsg *>(obj)!=nullptr;
}

const char **CprpRequestMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CprpRequestMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CprpRequestMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 12+base->getFieldCount() : 12;
}

unsigned int CprpRequestMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        0,    // FIELD_userGatewayAddress
        FD_ISEDITABLE,    // FIELD_generationTime
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_requiredStorage
        FD_ISEDITABLE,    // FIELD_computingAmount
        FD_ISEDITABLE,    // FIELD_transferAmount
        FD_ISEDITABLE,    // FIELD_totalDelayRequirement
        FD_ISEDITABLE,    // FIELD_budget
        FD_ISEDITABLE,    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 12) ? fieldTypeFlags[field] : 0;
}

const char *CprpRequestMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "userGatewayAddress",
        "generationTime",
        "computingType",
        "requiredStorage",
        "computingAmount",
        "transferAmount",
        "totalDelayRequirement",
        "budget",
        "userMaxBandwidth",
    };
    return (field >= 0 && field < 12) ? fieldNames[field] : nullptr;
}

int CprpRequestMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "userGatewayAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "generationTime") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "requiredStorage") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "computingAmount") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "transferAmount") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "totalDelayRequirement") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "budget") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "userMaxBandwidth") == 0) return baseIndex + 11;
    return base ? base->findField(fieldName) : -1;
}

const char *CprpRequestMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "inet::L3Address",    // FIELD_userGatewayAddress
        "omnetpp::simtime_t",    // FIELD_generationTime
        "int",    // FIELD_computingType
        "double",    // FIELD_requiredStorage
        "double",    // FIELD_computingAmount
        "double",    // FIELD_transferAmount
        "omnetpp::simtime_t",    // FIELD_totalDelayRequirement
        "double",    // FIELD_budget
        "double",    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 12) ? fieldTypeStrings[field] : nullptr;
}

const char **CprpRequestMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CprpRequestMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CprpRequestMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CprpRequestMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CprpRequestMsg'", field);
    }
}

const char *CprpRequestMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CprpRequestMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_userGatewayAddress: return pp->getUserGatewayAddress().str();
        case FIELD_generationTime: return simtime2string(pp->getGenerationTime());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_requiredStorage: return double2string(pp->getRequiredStorage());
        case FIELD_computingAmount: return double2string(pp->getComputingAmount());
        case FIELD_transferAmount: return double2string(pp->getTransferAmount());
        case FIELD_totalDelayRequirement: return simtime2string(pp->getTotalDelayRequirement());
        case FIELD_budget: return double2string(pp->getBudget());
        case FIELD_userMaxBandwidth: return double2string(pp->getUserMaxBandwidth());
        default: return "";
    }
}

void CprpRequestMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_generationTime: pp->setGenerationTime(string2simtime(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(string2double(value)); break;
        case FIELD_computingAmount: pp->setComputingAmount(string2double(value)); break;
        case FIELD_transferAmount: pp->setTransferAmount(string2double(value)); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(string2simtime(value)); break;
        case FIELD_budget: pp->setBudget(string2double(value)); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpRequestMsg'", field);
    }
}

omnetpp::cValue CprpRequestMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        case FIELD_generationTime: return pp->getGenerationTime().dbl();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_requiredStorage: return pp->getRequiredStorage();
        case FIELD_computingAmount: return pp->getComputingAmount();
        case FIELD_transferAmount: return pp->getTransferAmount();
        case FIELD_totalDelayRequirement: return pp->getTotalDelayRequirement().dbl();
        case FIELD_budget: return pp->getBudget();
        case FIELD_userMaxBandwidth: return pp->getUserMaxBandwidth();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CprpRequestMsg' as cValue -- field index out of range?", field);
    }
}

void CprpRequestMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_generationTime: pp->setGenerationTime(value.doubleValue()); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(value.doubleValue()); break;
        case FIELD_computingAmount: pp->setComputingAmount(value.doubleValue()); break;
        case FIELD_transferAmount: pp->setTransferAmount(value.doubleValue()); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(value.doubleValue()); break;
        case FIELD_budget: pp->setBudget(value.doubleValue()); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpRequestMsg'", field);
    }
}

const char *CprpRequestMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CprpRequestMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CprpRequestMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpRequestMsg *pp = omnetpp::fromAnyPtr<CprpRequestMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpRequestMsg'", field);
    }
}

Register_Class(CprpResponseMsg)

CprpResponseMsg::CprpResponseMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(32 + 72));

}

CprpResponseMsg::CprpResponseMsg(const CprpResponseMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CprpResponseMsg::~CprpResponseMsg()
{
}

CprpResponseMsg& CprpResponseMsg::operator=(const CprpResponseMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CprpResponseMsg::copy(const CprpResponseMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    this->computeNodeAddress = other.computeNodeAddress;
    this->computeNodeId = other.computeNodeId;
    this->computeNodePort = other.computeNodePort;
    this->computingType = other.computingType;
    this->computingCapacity = other.computingCapacity;
    this->availableStorage = other.availableStorage;
    this->maxNetworkBandwidth = other.maxNetworkBandwidth;
    this->sendTime = other.sendTime;
    this->requiredBandwidth = other.requiredBandwidth;
    this->maxDelayTolerance = other.maxDelayTolerance;
    this->computeCost = other.computeCost;
    this->accumulatedDelay = other.accumulatedDelay;
    this->lastHopSendTime = other.lastHopSendTime;
    this->lastHopAddress = other.lastHopAddress;
}

void CprpResponseMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->computeNodeAddress);
    doParsimPacking(b,this->computeNodeId);
    doParsimPacking(b,this->computeNodePort);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->computingCapacity);
    doParsimPacking(b,this->availableStorage);
    doParsimPacking(b,this->maxNetworkBandwidth);
    doParsimPacking(b,this->sendTime);
    doParsimPacking(b,this->requiredBandwidth);
    doParsimPacking(b,this->maxDelayTolerance);
    doParsimPacking(b,this->computeCost);
    doParsimPacking(b,this->accumulatedDelay);
    doParsimPacking(b,this->lastHopSendTime);
    doParsimPacking(b,this->lastHopAddress);
}

void CprpResponseMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->computeNodeAddress);
    doParsimUnpacking(b,this->computeNodeId);
    doParsimUnpacking(b,this->computeNodePort);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->computingCapacity);
    doParsimUnpacking(b,this->availableStorage);
    doParsimUnpacking(b,this->maxNetworkBandwidth);
    doParsimUnpacking(b,this->sendTime);
    doParsimUnpacking(b,this->requiredBandwidth);
    doParsimUnpacking(b,this->maxDelayTolerance);
    doParsimUnpacking(b,this->computeCost);
    doParsimUnpacking(b,this->accumulatedDelay);
    doParsimUnpacking(b,this->lastHopSendTime);
    doParsimUnpacking(b,this->lastHopAddress);
}

int CprpResponseMsg::getUserId() const
{
    return this->userId;
}

void CprpResponseMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int CprpResponseMsg::getTaskId() const
{
    return this->taskId;
}

void CprpResponseMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * CprpResponseMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CprpResponseMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

const L3Address& CprpResponseMsg::getComputeNodeAddress() const
{
    return this->computeNodeAddress;
}

void CprpResponseMsg::setComputeNodeAddress(const L3Address& computeNodeAddress)
{
    handleChange();
    this->computeNodeAddress = computeNodeAddress;
}

int CprpResponseMsg::getComputeNodeId() const
{
    return this->computeNodeId;
}

void CprpResponseMsg::setComputeNodeId(int computeNodeId)
{
    handleChange();
    this->computeNodeId = computeNodeId;
}

int CprpResponseMsg::getComputeNodePort() const
{
    return this->computeNodePort;
}

void CprpResponseMsg::setComputeNodePort(int computeNodePort)
{
    handleChange();
    this->computeNodePort = computeNodePort;
}

int CprpResponseMsg::getComputingType() const
{
    return this->computingType;
}

void CprpResponseMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

double CprpResponseMsg::getComputingCapacity() const
{
    return this->computingCapacity;
}

void CprpResponseMsg::setComputingCapacity(double computingCapacity)
{
    handleChange();
    this->computingCapacity = computingCapacity;
}

double CprpResponseMsg::getAvailableStorage() const
{
    return this->availableStorage;
}

void CprpResponseMsg::setAvailableStorage(double availableStorage)
{
    handleChange();
    this->availableStorage = availableStorage;
}

double CprpResponseMsg::getMaxNetworkBandwidth() const
{
    return this->maxNetworkBandwidth;
}

void CprpResponseMsg::setMaxNetworkBandwidth(double maxNetworkBandwidth)
{
    handleChange();
    this->maxNetworkBandwidth = maxNetworkBandwidth;
}

::omnetpp::simtime_t CprpResponseMsg::getSendTime() const
{
    return this->sendTime;
}

void CprpResponseMsg::setSendTime(::omnetpp::simtime_t sendTime)
{
    handleChange();
    this->sendTime = sendTime;
}

double CprpResponseMsg::getRequiredBandwidth() const
{
    return this->requiredBandwidth;
}

void CprpResponseMsg::setRequiredBandwidth(double requiredBandwidth)
{
    handleChange();
    this->requiredBandwidth = requiredBandwidth;
}

::omnetpp::simtime_t CprpResponseMsg::getMaxDelayTolerance() const
{
    return this->maxDelayTolerance;
}

void CprpResponseMsg::setMaxDelayTolerance(::omnetpp::simtime_t maxDelayTolerance)
{
    handleChange();
    this->maxDelayTolerance = maxDelayTolerance;
}

double CprpResponseMsg::getComputeCost() const
{
    return this->computeCost;
}

void CprpResponseMsg::setComputeCost(double computeCost)
{
    handleChange();
    this->computeCost = computeCost;
}

::omnetpp::simtime_t CprpResponseMsg::getAccumulatedDelay() const
{
    return this->accumulatedDelay;
}

void CprpResponseMsg::setAccumulatedDelay(::omnetpp::simtime_t accumulatedDelay)
{
    handleChange();
    this->accumulatedDelay = accumulatedDelay;
}

::omnetpp::simtime_t CprpResponseMsg::getLastHopSendTime() const
{
    return this->lastHopSendTime;
}

void CprpResponseMsg::setLastHopSendTime(::omnetpp::simtime_t lastHopSendTime)
{
    handleChange();
    this->lastHopSendTime = lastHopSendTime;
}

const L3Address& CprpResponseMsg::getLastHopAddress() const
{
    return this->lastHopAddress;
}

void CprpResponseMsg::setLastHopAddress(const L3Address& lastHopAddress)
{
    handleChange();
    this->lastHopAddress = lastHopAddress;
}

class CprpResponseMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_computeNodeAddress,
        FIELD_computeNodeId,
        FIELD_computeNodePort,
        FIELD_computingType,
        FIELD_computingCapacity,
        FIELD_availableStorage,
        FIELD_maxNetworkBandwidth,
        FIELD_sendTime,
        FIELD_requiredBandwidth,
        FIELD_maxDelayTolerance,
        FIELD_computeCost,
        FIELD_accumulatedDelay,
        FIELD_lastHopSendTime,
        FIELD_lastHopAddress,
    };
  public:
    CprpResponseMsgDescriptor();
    virtual ~CprpResponseMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CprpResponseMsgDescriptor)

CprpResponseMsgDescriptor::CprpResponseMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CprpResponseMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CprpResponseMsgDescriptor::~CprpResponseMsgDescriptor()
{
    delete[] propertyNames;
}

bool CprpResponseMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CprpResponseMsg *>(obj)!=nullptr;
}

const char **CprpResponseMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CprpResponseMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CprpResponseMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 17+base->getFieldCount() : 17;
}

unsigned int CprpResponseMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        0,    // FIELD_computeNodeAddress
        FD_ISEDITABLE,    // FIELD_computeNodeId
        FD_ISEDITABLE,    // FIELD_computeNodePort
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_computingCapacity
        FD_ISEDITABLE,    // FIELD_availableStorage
        FD_ISEDITABLE,    // FIELD_maxNetworkBandwidth
        FD_ISEDITABLE,    // FIELD_sendTime
        FD_ISEDITABLE,    // FIELD_requiredBandwidth
        FD_ISEDITABLE,    // FIELD_maxDelayTolerance
        FD_ISEDITABLE,    // FIELD_computeCost
        FD_ISEDITABLE,    // FIELD_accumulatedDelay
        FD_ISEDITABLE,    // FIELD_lastHopSendTime
        0,    // FIELD_lastHopAddress
    };
    return (field >= 0 && field < 17) ? fieldTypeFlags[field] : 0;
}

const char *CprpResponseMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "computeNodeAddress",
        "computeNodeId",
        "computeNodePort",
        "computingType",
        "computingCapacity",
        "availableStorage",
        "maxNetworkBandwidth",
        "sendTime",
        "requiredBandwidth",
        "maxDelayTolerance",
        "computeCost",
        "accumulatedDelay",
        "lastHopSendTime",
        "lastHopAddress",
    };
    return (field >= 0 && field < 17) ? fieldNames[field] : nullptr;
}

int CprpResponseMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "computeNodeAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "computeNodeId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "computeNodePort") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "computingCapacity") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "availableStorage") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "maxNetworkBandwidth") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "sendTime") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "requiredBandwidth") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "maxDelayTolerance") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "computeCost") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "accumulatedDelay") == 0) return baseIndex + 14;
    if (strcmp(fieldName, "lastHopSendTime") == 0) return baseIndex + 15;
    if (strcmp(fieldName, "lastHopAddress") == 0) return baseIndex + 16;
    return base ? base->findField(fieldName) : -1;
}

const char *CprpResponseMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "inet::L3Address",    // FIELD_computeNodeAddress
        "int",    // FIELD_computeNodeId
        "int",    // FIELD_computeNodePort
        "int",    // FIELD_computingType
        "double",    // FIELD_computingCapacity
        "double",    // FIELD_availableStorage
        "double",    // FIELD_maxNetworkBandwidth
        "omnetpp::simtime_t",    // FIELD_sendTime
        "double",    // FIELD_requiredBandwidth
        "omnetpp::simtime_t",    // FIELD_maxDelayTolerance
        "double",    // FIELD_computeCost
        "omnetpp::simtime_t",    // FIELD_accumulatedDelay
        "omnetpp::simtime_t",    // FIELD_lastHopSendTime
        "inet::L3Address",    // FIELD_lastHopAddress
    };
    return (field >= 0 && field < 17) ? fieldTypeStrings[field] : nullptr;
}

const char **CprpResponseMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CprpResponseMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CprpResponseMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CprpResponseMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CprpResponseMsg'", field);
    }
}

const char *CprpResponseMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CprpResponseMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_computeNodeAddress: return pp->getComputeNodeAddress().str();
        case FIELD_computeNodeId: return long2string(pp->getComputeNodeId());
        case FIELD_computeNodePort: return long2string(pp->getComputeNodePort());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_computingCapacity: return double2string(pp->getComputingCapacity());
        case FIELD_availableStorage: return double2string(pp->getAvailableStorage());
        case FIELD_maxNetworkBandwidth: return double2string(pp->getMaxNetworkBandwidth());
        case FIELD_sendTime: return simtime2string(pp->getSendTime());
        case FIELD_requiredBandwidth: return double2string(pp->getRequiredBandwidth());
        case FIELD_maxDelayTolerance: return simtime2string(pp->getMaxDelayTolerance());
        case FIELD_computeCost: return double2string(pp->getComputeCost());
        case FIELD_accumulatedDelay: return simtime2string(pp->getAccumulatedDelay());
        case FIELD_lastHopSendTime: return simtime2string(pp->getLastHopSendTime());
        case FIELD_lastHopAddress: return pp->getLastHopAddress().str();
        default: return "";
    }
}

void CprpResponseMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_computeNodeId: pp->setComputeNodeId(string2long(value)); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(string2long(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_computingCapacity: pp->setComputingCapacity(string2double(value)); break;
        case FIELD_availableStorage: pp->setAvailableStorage(string2double(value)); break;
        case FIELD_maxNetworkBandwidth: pp->setMaxNetworkBandwidth(string2double(value)); break;
        case FIELD_sendTime: pp->setSendTime(string2simtime(value)); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(string2double(value)); break;
        case FIELD_maxDelayTolerance: pp->setMaxDelayTolerance(string2simtime(value)); break;
        case FIELD_computeCost: pp->setComputeCost(string2double(value)); break;
        case FIELD_accumulatedDelay: pp->setAccumulatedDelay(string2simtime(value)); break;
        case FIELD_lastHopSendTime: pp->setLastHopSendTime(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpResponseMsg'", field);
    }
}

omnetpp::cValue CprpResponseMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_computeNodeId: return pp->getComputeNodeId();
        case FIELD_computeNodePort: return pp->getComputeNodePort();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_computingCapacity: return pp->getComputingCapacity();
        case FIELD_availableStorage: return pp->getAvailableStorage();
        case FIELD_maxNetworkBandwidth: return pp->getMaxNetworkBandwidth();
        case FIELD_sendTime: return pp->getSendTime().dbl();
        case FIELD_requiredBandwidth: return pp->getRequiredBandwidth();
        case FIELD_maxDelayTolerance: return pp->getMaxDelayTolerance().dbl();
        case FIELD_computeCost: return pp->getComputeCost();
        case FIELD_accumulatedDelay: return pp->getAccumulatedDelay().dbl();
        case FIELD_lastHopSendTime: return pp->getLastHopSendTime().dbl();
        case FIELD_lastHopAddress: return omnetpp::toAnyPtr(&pp->getLastHopAddress()); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CprpResponseMsg' as cValue -- field index out of range?", field);
    }
}

void CprpResponseMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_computeNodeId: pp->setComputeNodeId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computingCapacity: pp->setComputingCapacity(value.doubleValue()); break;
        case FIELD_availableStorage: pp->setAvailableStorage(value.doubleValue()); break;
        case FIELD_maxNetworkBandwidth: pp->setMaxNetworkBandwidth(value.doubleValue()); break;
        case FIELD_sendTime: pp->setSendTime(value.doubleValue()); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(value.doubleValue()); break;
        case FIELD_maxDelayTolerance: pp->setMaxDelayTolerance(value.doubleValue()); break;
        case FIELD_computeCost: pp->setComputeCost(value.doubleValue()); break;
        case FIELD_accumulatedDelay: pp->setAccumulatedDelay(value.doubleValue()); break;
        case FIELD_lastHopSendTime: pp->setLastHopSendTime(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpResponseMsg'", field);
    }
}

const char *CprpResponseMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CprpResponseMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_lastHopAddress: return omnetpp::toAnyPtr(&pp->getLastHopAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CprpResponseMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpResponseMsg *pp = omnetpp::fromAnyPtr<CprpResponseMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpResponseMsg'", field);
    }
}

Register_Class(CprpConfirmMsg)

CprpConfirmMsg::CprpConfirmMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(28 + 40));

}

CprpConfirmMsg::CprpConfirmMsg(const CprpConfirmMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CprpConfirmMsg::~CprpConfirmMsg()
{
}

CprpConfirmMsg& CprpConfirmMsg::operator=(const CprpConfirmMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CprpConfirmMsg::copy(const CprpConfirmMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    this->selectedNodeId = other.selectedNodeId;
    this->selectedNodeAddress = other.selectedNodeAddress;
    this->selectedNodePort = other.selectedNodePort;
    this->selectedPathIndex = other.selectedPathIndex;
    this->generationTime = other.generationTime;
    this->computingType = other.computingType;
    this->requiredStorage = other.requiredStorage;
    this->computingAmount = other.computingAmount;
    this->transferAmount = other.transferAmount;
    this->totalDelayRequirement = other.totalDelayRequirement;
    this->budget = other.budget;
    this->userMaxBandwidth = other.userMaxBandwidth;
}

void CprpConfirmMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->selectedNodeId);
    doParsimPacking(b,this->selectedNodeAddress);
    doParsimPacking(b,this->selectedNodePort);
    doParsimPacking(b,this->selectedPathIndex);
    doParsimPacking(b,this->generationTime);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->requiredStorage);
    doParsimPacking(b,this->computingAmount);
    doParsimPacking(b,this->transferAmount);
    doParsimPacking(b,this->totalDelayRequirement);
    doParsimPacking(b,this->budget);
    doParsimPacking(b,this->userMaxBandwidth);
}

void CprpConfirmMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->selectedNodeId);
    doParsimUnpacking(b,this->selectedNodeAddress);
    doParsimUnpacking(b,this->selectedNodePort);
    doParsimUnpacking(b,this->selectedPathIndex);
    doParsimUnpacking(b,this->generationTime);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->requiredStorage);
    doParsimUnpacking(b,this->computingAmount);
    doParsimUnpacking(b,this->transferAmount);
    doParsimUnpacking(b,this->totalDelayRequirement);
    doParsimUnpacking(b,this->budget);
    doParsimUnpacking(b,this->userMaxBandwidth);
}

int CprpConfirmMsg::getUserId() const
{
    return this->userId;
}

void CprpConfirmMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int CprpConfirmMsg::getTaskId() const
{
    return this->taskId;
}

void CprpConfirmMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * CprpConfirmMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CprpConfirmMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

int CprpConfirmMsg::getSelectedNodeId() const
{
    return this->selectedNodeId;
}

void CprpConfirmMsg::setSelectedNodeId(int selectedNodeId)
{
    handleChange();
    this->selectedNodeId = selectedNodeId;
}

const L3Address& CprpConfirmMsg::getSelectedNodeAddress() const
{
    return this->selectedNodeAddress;
}

void CprpConfirmMsg::setSelectedNodeAddress(const L3Address& selectedNodeAddress)
{
    handleChange();
    this->selectedNodeAddress = selectedNodeAddress;
}

int CprpConfirmMsg::getSelectedNodePort() const
{
    return this->selectedNodePort;
}

void CprpConfirmMsg::setSelectedNodePort(int selectedNodePort)
{
    handleChange();
    this->selectedNodePort = selectedNodePort;
}

int CprpConfirmMsg::getSelectedPathIndex() const
{
    return this->selectedPathIndex;
}

void CprpConfirmMsg::setSelectedPathIndex(int selectedPathIndex)
{
    handleChange();
    this->selectedPathIndex = selectedPathIndex;
}

::omnetpp::simtime_t CprpConfirmMsg::getGenerationTime() const
{
    return this->generationTime;
}

void CprpConfirmMsg::setGenerationTime(::omnetpp::simtime_t generationTime)
{
    handleChange();
    this->generationTime = generationTime;
}

int CprpConfirmMsg::getComputingType() const
{
    return this->computingType;
}

void CprpConfirmMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

double CprpConfirmMsg::getRequiredStorage() const
{
    return this->requiredStorage;
}

void CprpConfirmMsg::setRequiredStorage(double requiredStorage)
{
    handleChange();
    this->requiredStorage = requiredStorage;
}

double CprpConfirmMsg::getComputingAmount() const
{
    return this->computingAmount;
}

void CprpConfirmMsg::setComputingAmount(double computingAmount)
{
    handleChange();
    this->computingAmount = computingAmount;
}

double CprpConfirmMsg::getTransferAmount() const
{
    return this->transferAmount;
}

void CprpConfirmMsg::setTransferAmount(double transferAmount)
{
    handleChange();
    this->transferAmount = transferAmount;
}

::omnetpp::simtime_t CprpConfirmMsg::getTotalDelayRequirement() const
{
    return this->totalDelayRequirement;
}

void CprpConfirmMsg::setTotalDelayRequirement(::omnetpp::simtime_t totalDelayRequirement)
{
    handleChange();
    this->totalDelayRequirement = totalDelayRequirement;
}

double CprpConfirmMsg::getBudget() const
{
    return this->budget;
}

void CprpConfirmMsg::setBudget(double budget)
{
    handleChange();
    this->budget = budget;
}

double CprpConfirmMsg::getUserMaxBandwidth() const
{
    return this->userMaxBandwidth;
}

void CprpConfirmMsg::setUserMaxBandwidth(double userMaxBandwidth)
{
    handleChange();
    this->userMaxBandwidth = userMaxBandwidth;
}

class CprpConfirmMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_selectedNodeId,
        FIELD_selectedNodeAddress,
        FIELD_selectedNodePort,
        FIELD_selectedPathIndex,
        FIELD_generationTime,
        FIELD_computingType,
        FIELD_requiredStorage,
        FIELD_computingAmount,
        FIELD_transferAmount,
        FIELD_totalDelayRequirement,
        FIELD_budget,
        FIELD_userMaxBandwidth,
    };
  public:
    CprpConfirmMsgDescriptor();
    virtual ~CprpConfirmMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CprpConfirmMsgDescriptor)

CprpConfirmMsgDescriptor::CprpConfirmMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CprpConfirmMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CprpConfirmMsgDescriptor::~CprpConfirmMsgDescriptor()
{
    delete[] propertyNames;
}

bool CprpConfirmMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CprpConfirmMsg *>(obj)!=nullptr;
}

const char **CprpConfirmMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CprpConfirmMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CprpConfirmMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 15+base->getFieldCount() : 15;
}

unsigned int CprpConfirmMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_selectedNodeId
        0,    // FIELD_selectedNodeAddress
        FD_ISEDITABLE,    // FIELD_selectedNodePort
        FD_ISEDITABLE,    // FIELD_selectedPathIndex
        FD_ISEDITABLE,    // FIELD_generationTime
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_requiredStorage
        FD_ISEDITABLE,    // FIELD_computingAmount
        FD_ISEDITABLE,    // FIELD_transferAmount
        FD_ISEDITABLE,    // FIELD_totalDelayRequirement
        FD_ISEDITABLE,    // FIELD_budget
        FD_ISEDITABLE,    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 15) ? fieldTypeFlags[field] : 0;
}

const char *CprpConfirmMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "selectedNodeId",
        "selectedNodeAddress",
        "selectedNodePort",
        "selectedPathIndex",
        "generationTime",
        "computingType",
        "requiredStorage",
        "computingAmount",
        "transferAmount",
        "totalDelayRequirement",
        "budget",
        "userMaxBandwidth",
    };
    return (field >= 0 && field < 15) ? fieldNames[field] : nullptr;
}

int CprpConfirmMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "selectedNodeId") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "selectedNodeAddress") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "selectedNodePort") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "selectedPathIndex") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "generationTime") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "requiredStorage") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "computingAmount") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "transferAmount") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "totalDelayRequirement") == 0) return baseIndex + 12;
    if (strcmp(fieldName, "budget") == 0) return baseIndex + 13;
    if (strcmp(fieldName, "userMaxBandwidth") == 0) return baseIndex + 14;
    return base ? base->findField(fieldName) : -1;
}

const char *CprpConfirmMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "int",    // FIELD_selectedNodeId
        "inet::L3Address",    // FIELD_selectedNodeAddress
        "int",    // FIELD_selectedNodePort
        "int",    // FIELD_selectedPathIndex
        "omnetpp::simtime_t",    // FIELD_generationTime
        "int",    // FIELD_computingType
        "double",    // FIELD_requiredStorage
        "double",    // FIELD_computingAmount
        "double",    // FIELD_transferAmount
        "omnetpp::simtime_t",    // FIELD_totalDelayRequirement
        "double",    // FIELD_budget
        "double",    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 15) ? fieldTypeStrings[field] : nullptr;
}

const char **CprpConfirmMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CprpConfirmMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CprpConfirmMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CprpConfirmMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CprpConfirmMsg'", field);
    }
}

const char *CprpConfirmMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CprpConfirmMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_selectedNodeId: return long2string(pp->getSelectedNodeId());
        case FIELD_selectedNodeAddress: return pp->getSelectedNodeAddress().str();
        case FIELD_selectedNodePort: return long2string(pp->getSelectedNodePort());
        case FIELD_selectedPathIndex: return long2string(pp->getSelectedPathIndex());
        case FIELD_generationTime: return simtime2string(pp->getGenerationTime());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_requiredStorage: return double2string(pp->getRequiredStorage());
        case FIELD_computingAmount: return double2string(pp->getComputingAmount());
        case FIELD_transferAmount: return double2string(pp->getTransferAmount());
        case FIELD_totalDelayRequirement: return simtime2string(pp->getTotalDelayRequirement());
        case FIELD_budget: return double2string(pp->getBudget());
        case FIELD_userMaxBandwidth: return double2string(pp->getUserMaxBandwidth());
        default: return "";
    }
}

void CprpConfirmMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_selectedNodeId: pp->setSelectedNodeId(string2long(value)); break;
        case FIELD_selectedNodePort: pp->setSelectedNodePort(string2long(value)); break;
        case FIELD_selectedPathIndex: pp->setSelectedPathIndex(string2long(value)); break;
        case FIELD_generationTime: pp->setGenerationTime(string2simtime(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(string2double(value)); break;
        case FIELD_computingAmount: pp->setComputingAmount(string2double(value)); break;
        case FIELD_transferAmount: pp->setTransferAmount(string2double(value)); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(string2simtime(value)); break;
        case FIELD_budget: pp->setBudget(string2double(value)); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpConfirmMsg'", field);
    }
}

omnetpp::cValue CprpConfirmMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_selectedNodeId: return pp->getSelectedNodeId();
        case FIELD_selectedNodeAddress: return omnetpp::toAnyPtr(&pp->getSelectedNodeAddress()); break;
        case FIELD_selectedNodePort: return pp->getSelectedNodePort();
        case FIELD_selectedPathIndex: return pp->getSelectedPathIndex();
        case FIELD_generationTime: return pp->getGenerationTime().dbl();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_requiredStorage: return pp->getRequiredStorage();
        case FIELD_computingAmount: return pp->getComputingAmount();
        case FIELD_transferAmount: return pp->getTransferAmount();
        case FIELD_totalDelayRequirement: return pp->getTotalDelayRequirement().dbl();
        case FIELD_budget: return pp->getBudget();
        case FIELD_userMaxBandwidth: return pp->getUserMaxBandwidth();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CprpConfirmMsg' as cValue -- field index out of range?", field);
    }
}

void CprpConfirmMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_selectedNodeId: pp->setSelectedNodeId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_selectedNodePort: pp->setSelectedNodePort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_selectedPathIndex: pp->setSelectedPathIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_generationTime: pp->setGenerationTime(value.doubleValue()); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(value.doubleValue()); break;
        case FIELD_computingAmount: pp->setComputingAmount(value.doubleValue()); break;
        case FIELD_transferAmount: pp->setTransferAmount(value.doubleValue()); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(value.doubleValue()); break;
        case FIELD_budget: pp->setBudget(value.doubleValue()); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpConfirmMsg'", field);
    }
}

const char *CprpConfirmMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CprpConfirmMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        case FIELD_selectedNodeAddress: return omnetpp::toAnyPtr(&pp->getSelectedNodeAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CprpConfirmMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CprpConfirmMsg *pp = omnetpp::fromAnyPtr<CprpConfirmMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CprpConfirmMsg'", field);
    }
}

Register_Enum(inet::CancelSenderType, (inet::CancelSenderType::SENDER_USER_GW, inet::CancelSenderType::SENDER_COMPUTE_GW, inet::CancelSenderType::SENDER_COMPUTE_ROUTER));

Register_Class(CancelMsg)

CancelMsg::CancelMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(20));

}

CancelMsg::CancelMsg(const CancelMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CancelMsg::~CancelMsg()
{
}

CancelMsg& CancelMsg::operator=(const CancelMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CancelMsg::copy(const CancelMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->computeNodeAddress = other.computeNodeAddress;
    this->computeNodePort = other.computeNodePort;
    this->senderType = other.senderType;
    this->msgType = other.msgType;
}

void CancelMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->computeNodeAddress);
    doParsimPacking(b,this->computeNodePort);
    doParsimPacking(b,this->senderType);
    doParsimPacking(b,this->msgType);
}

void CancelMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->computeNodeAddress);
    doParsimUnpacking(b,this->computeNodePort);
    doParsimUnpacking(b,this->senderType);
    doParsimUnpacking(b,this->msgType);
}

int CancelMsg::getUserId() const
{
    return this->userId;
}

void CancelMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int CancelMsg::getTaskId() const
{
    return this->taskId;
}

void CancelMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const L3Address& CancelMsg::getComputeNodeAddress() const
{
    return this->computeNodeAddress;
}

void CancelMsg::setComputeNodeAddress(const L3Address& computeNodeAddress)
{
    handleChange();
    this->computeNodeAddress = computeNodeAddress;
}

int CancelMsg::getComputeNodePort() const
{
    return this->computeNodePort;
}

void CancelMsg::setComputeNodePort(int computeNodePort)
{
    handleChange();
    this->computeNodePort = computeNodePort;
}

int CancelMsg::getSenderType() const
{
    return this->senderType;
}

void CancelMsg::setSenderType(int senderType)
{
    handleChange();
    this->senderType = senderType;
}

const char * CancelMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CancelMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

class CancelMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_computeNodeAddress,
        FIELD_computeNodePort,
        FIELD_senderType,
        FIELD_msgType,
    };
  public:
    CancelMsgDescriptor();
    virtual ~CancelMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CancelMsgDescriptor)

CancelMsgDescriptor::CancelMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CancelMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CancelMsgDescriptor::~CancelMsgDescriptor()
{
    delete[] propertyNames;
}

bool CancelMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CancelMsg *>(obj)!=nullptr;
}

const char **CancelMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CancelMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CancelMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 6+base->getFieldCount() : 6;
}

unsigned int CancelMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        0,    // FIELD_computeNodeAddress
        FD_ISEDITABLE,    // FIELD_computeNodePort
        FD_ISEDITABLE,    // FIELD_senderType
        FD_ISEDITABLE,    // FIELD_msgType
    };
    return (field >= 0 && field < 6) ? fieldTypeFlags[field] : 0;
}

const char *CancelMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "computeNodeAddress",
        "computeNodePort",
        "senderType",
        "msgType",
    };
    return (field >= 0 && field < 6) ? fieldNames[field] : nullptr;
}

int CancelMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "computeNodeAddress") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "computeNodePort") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "senderType") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 5;
    return base ? base->findField(fieldName) : -1;
}

const char *CancelMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "inet::L3Address",    // FIELD_computeNodeAddress
        "int",    // FIELD_computeNodePort
        "int",    // FIELD_senderType
        "string",    // FIELD_msgType
    };
    return (field >= 0 && field < 6) ? fieldTypeStrings[field] : nullptr;
}

const char **CancelMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CancelMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CancelMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CancelMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CancelMsg'", field);
    }
}

const char *CancelMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CancelMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_computeNodeAddress: return pp->getComputeNodeAddress().str();
        case FIELD_computeNodePort: return long2string(pp->getComputeNodePort());
        case FIELD_senderType: return long2string(pp->getSenderType());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        default: return "";
    }
}

void CancelMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(string2long(value)); break;
        case FIELD_senderType: pp->setSenderType(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CancelMsg'", field);
    }
}

omnetpp::cValue CancelMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_computeNodePort: return pp->getComputeNodePort();
        case FIELD_senderType: return pp->getSenderType();
        case FIELD_msgType: return pp->getMsgType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CancelMsg' as cValue -- field index out of range?", field);
    }
}

void CancelMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_senderType: pp->setSenderType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CancelMsg'", field);
    }
}

const char *CancelMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CancelMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CancelMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CancelMsg *pp = omnetpp::fromAnyPtr<CancelMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CancelMsg'", field);
    }
}

Register_Class(TaskRequestMsg)

TaskRequestMsg::TaskRequestMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(12 + 40));

}

TaskRequestMsg::TaskRequestMsg(const TaskRequestMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

TaskRequestMsg::~TaskRequestMsg()
{
}

TaskRequestMsg& TaskRequestMsg::operator=(const TaskRequestMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void TaskRequestMsg::copy(const TaskRequestMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    this->generationTime = other.generationTime;
    this->computingType = other.computingType;
    this->requiredStorage = other.requiredStorage;
    this->computingAmount = other.computingAmount;
    this->transferAmount = other.transferAmount;
    this->totalDelayRequirement = other.totalDelayRequirement;
    this->budget = other.budget;
    this->userMaxBandwidth = other.userMaxBandwidth;
}

void TaskRequestMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->generationTime);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->requiredStorage);
    doParsimPacking(b,this->computingAmount);
    doParsimPacking(b,this->transferAmount);
    doParsimPacking(b,this->totalDelayRequirement);
    doParsimPacking(b,this->budget);
    doParsimPacking(b,this->userMaxBandwidth);
}

void TaskRequestMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->generationTime);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->requiredStorage);
    doParsimUnpacking(b,this->computingAmount);
    doParsimUnpacking(b,this->transferAmount);
    doParsimUnpacking(b,this->totalDelayRequirement);
    doParsimUnpacking(b,this->budget);
    doParsimUnpacking(b,this->userMaxBandwidth);
}

int TaskRequestMsg::getUserId() const
{
    return this->userId;
}

void TaskRequestMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int TaskRequestMsg::getTaskId() const
{
    return this->taskId;
}

void TaskRequestMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * TaskRequestMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void TaskRequestMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

::omnetpp::simtime_t TaskRequestMsg::getGenerationTime() const
{
    return this->generationTime;
}

void TaskRequestMsg::setGenerationTime(::omnetpp::simtime_t generationTime)
{
    handleChange();
    this->generationTime = generationTime;
}

int TaskRequestMsg::getComputingType() const
{
    return this->computingType;
}

void TaskRequestMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

double TaskRequestMsg::getRequiredStorage() const
{
    return this->requiredStorage;
}

void TaskRequestMsg::setRequiredStorage(double requiredStorage)
{
    handleChange();
    this->requiredStorage = requiredStorage;
}

double TaskRequestMsg::getComputingAmount() const
{
    return this->computingAmount;
}

void TaskRequestMsg::setComputingAmount(double computingAmount)
{
    handleChange();
    this->computingAmount = computingAmount;
}

double TaskRequestMsg::getTransferAmount() const
{
    return this->transferAmount;
}

void TaskRequestMsg::setTransferAmount(double transferAmount)
{
    handleChange();
    this->transferAmount = transferAmount;
}

::omnetpp::simtime_t TaskRequestMsg::getTotalDelayRequirement() const
{
    return this->totalDelayRequirement;
}

void TaskRequestMsg::setTotalDelayRequirement(::omnetpp::simtime_t totalDelayRequirement)
{
    handleChange();
    this->totalDelayRequirement = totalDelayRequirement;
}

double TaskRequestMsg::getBudget() const
{
    return this->budget;
}

void TaskRequestMsg::setBudget(double budget)
{
    handleChange();
    this->budget = budget;
}

double TaskRequestMsg::getUserMaxBandwidth() const
{
    return this->userMaxBandwidth;
}

void TaskRequestMsg::setUserMaxBandwidth(double userMaxBandwidth)
{
    handleChange();
    this->userMaxBandwidth = userMaxBandwidth;
}

class TaskRequestMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_generationTime,
        FIELD_computingType,
        FIELD_requiredStorage,
        FIELD_computingAmount,
        FIELD_transferAmount,
        FIELD_totalDelayRequirement,
        FIELD_budget,
        FIELD_userMaxBandwidth,
    };
  public:
    TaskRequestMsgDescriptor();
    virtual ~TaskRequestMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TaskRequestMsgDescriptor)

TaskRequestMsgDescriptor::TaskRequestMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::TaskRequestMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

TaskRequestMsgDescriptor::~TaskRequestMsgDescriptor()
{
    delete[] propertyNames;
}

bool TaskRequestMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TaskRequestMsg *>(obj)!=nullptr;
}

const char **TaskRequestMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TaskRequestMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TaskRequestMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 11+base->getFieldCount() : 11;
}

unsigned int TaskRequestMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_generationTime
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_requiredStorage
        FD_ISEDITABLE,    // FIELD_computingAmount
        FD_ISEDITABLE,    // FIELD_transferAmount
        FD_ISEDITABLE,    // FIELD_totalDelayRequirement
        FD_ISEDITABLE,    // FIELD_budget
        FD_ISEDITABLE,    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
}

const char *TaskRequestMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "generationTime",
        "computingType",
        "requiredStorage",
        "computingAmount",
        "transferAmount",
        "totalDelayRequirement",
        "budget",
        "userMaxBandwidth",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int TaskRequestMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "generationTime") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "requiredStorage") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "computingAmount") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "transferAmount") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "totalDelayRequirement") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "budget") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "userMaxBandwidth") == 0) return baseIndex + 10;
    return base ? base->findField(fieldName) : -1;
}

const char *TaskRequestMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "omnetpp::simtime_t",    // FIELD_generationTime
        "int",    // FIELD_computingType
        "double",    // FIELD_requiredStorage
        "double",    // FIELD_computingAmount
        "double",    // FIELD_transferAmount
        "omnetpp::simtime_t",    // FIELD_totalDelayRequirement
        "double",    // FIELD_budget
        "double",    // FIELD_userMaxBandwidth
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
}

const char **TaskRequestMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TaskRequestMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TaskRequestMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TaskRequestMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TaskRequestMsg'", field);
    }
}

const char *TaskRequestMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TaskRequestMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_generationTime: return simtime2string(pp->getGenerationTime());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_requiredStorage: return double2string(pp->getRequiredStorage());
        case FIELD_computingAmount: return double2string(pp->getComputingAmount());
        case FIELD_transferAmount: return double2string(pp->getTransferAmount());
        case FIELD_totalDelayRequirement: return simtime2string(pp->getTotalDelayRequirement());
        case FIELD_budget: return double2string(pp->getBudget());
        case FIELD_userMaxBandwidth: return double2string(pp->getUserMaxBandwidth());
        default: return "";
    }
}

void TaskRequestMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_generationTime: pp->setGenerationTime(string2simtime(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(string2double(value)); break;
        case FIELD_computingAmount: pp->setComputingAmount(string2double(value)); break;
        case FIELD_transferAmount: pp->setTransferAmount(string2double(value)); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(string2simtime(value)); break;
        case FIELD_budget: pp->setBudget(string2double(value)); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskRequestMsg'", field);
    }
}

omnetpp::cValue TaskRequestMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_generationTime: return pp->getGenerationTime().dbl();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_requiredStorage: return pp->getRequiredStorage();
        case FIELD_computingAmount: return pp->getComputingAmount();
        case FIELD_transferAmount: return pp->getTransferAmount();
        case FIELD_totalDelayRequirement: return pp->getTotalDelayRequirement().dbl();
        case FIELD_budget: return pp->getBudget();
        case FIELD_userMaxBandwidth: return pp->getUserMaxBandwidth();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TaskRequestMsg' as cValue -- field index out of range?", field);
    }
}

void TaskRequestMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_generationTime: pp->setGenerationTime(value.doubleValue()); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(value.doubleValue()); break;
        case FIELD_computingAmount: pp->setComputingAmount(value.doubleValue()); break;
        case FIELD_transferAmount: pp->setTransferAmount(value.doubleValue()); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(value.doubleValue()); break;
        case FIELD_budget: pp->setBudget(value.doubleValue()); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskRequestMsg'", field);
    }
}

const char *TaskRequestMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TaskRequestMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TaskRequestMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskRequestMsg *pp = omnetpp::fromAnyPtr<TaskRequestMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskRequestMsg'", field);
    }
}

Register_Class(TaskDataMsg)

TaskDataMsg::TaskDataMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(16 + 40));

}

TaskDataMsg::TaskDataMsg(const TaskDataMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

TaskDataMsg::~TaskDataMsg()
{
}

TaskDataMsg& TaskDataMsg::operator=(const TaskDataMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void TaskDataMsg::copy(const TaskDataMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    this->userNodeAddress = other.userNodeAddress;
    this->generationTime = other.generationTime;
    this->computingType = other.computingType;
    this->requiredStorage = other.requiredStorage;
    this->computingAmount = other.computingAmount;
    this->transferAmount = other.transferAmount;
    this->totalDelayRequirement = other.totalDelayRequirement;
    this->budget = other.budget;
    this->userMaxBandwidth = other.userMaxBandwidth;
    this->priority = other.priority;
}

void TaskDataMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->userNodeAddress);
    doParsimPacking(b,this->generationTime);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->requiredStorage);
    doParsimPacking(b,this->computingAmount);
    doParsimPacking(b,this->transferAmount);
    doParsimPacking(b,this->totalDelayRequirement);
    doParsimPacking(b,this->budget);
    doParsimPacking(b,this->userMaxBandwidth);
    doParsimPacking(b,this->priority);
}

void TaskDataMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->userNodeAddress);
    doParsimUnpacking(b,this->generationTime);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->requiredStorage);
    doParsimUnpacking(b,this->computingAmount);
    doParsimUnpacking(b,this->transferAmount);
    doParsimUnpacking(b,this->totalDelayRequirement);
    doParsimUnpacking(b,this->budget);
    doParsimUnpacking(b,this->userMaxBandwidth);
    doParsimUnpacking(b,this->priority);
}

int TaskDataMsg::getUserId() const
{
    return this->userId;
}

void TaskDataMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int TaskDataMsg::getTaskId() const
{
    return this->taskId;
}

void TaskDataMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * TaskDataMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void TaskDataMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

const L3Address& TaskDataMsg::getUserNodeAddress() const
{
    return this->userNodeAddress;
}

void TaskDataMsg::setUserNodeAddress(const L3Address& userNodeAddress)
{
    handleChange();
    this->userNodeAddress = userNodeAddress;
}

::omnetpp::simtime_t TaskDataMsg::getGenerationTime() const
{
    return this->generationTime;
}

void TaskDataMsg::setGenerationTime(::omnetpp::simtime_t generationTime)
{
    handleChange();
    this->generationTime = generationTime;
}

int TaskDataMsg::getComputingType() const
{
    return this->computingType;
}

void TaskDataMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

double TaskDataMsg::getRequiredStorage() const
{
    return this->requiredStorage;
}

void TaskDataMsg::setRequiredStorage(double requiredStorage)
{
    handleChange();
    this->requiredStorage = requiredStorage;
}

double TaskDataMsg::getComputingAmount() const
{
    return this->computingAmount;
}

void TaskDataMsg::setComputingAmount(double computingAmount)
{
    handleChange();
    this->computingAmount = computingAmount;
}

double TaskDataMsg::getTransferAmount() const
{
    return this->transferAmount;
}

void TaskDataMsg::setTransferAmount(double transferAmount)
{
    handleChange();
    this->transferAmount = transferAmount;
}

::omnetpp::simtime_t TaskDataMsg::getTotalDelayRequirement() const
{
    return this->totalDelayRequirement;
}

void TaskDataMsg::setTotalDelayRequirement(::omnetpp::simtime_t totalDelayRequirement)
{
    handleChange();
    this->totalDelayRequirement = totalDelayRequirement;
}

double TaskDataMsg::getBudget() const
{
    return this->budget;
}

void TaskDataMsg::setBudget(double budget)
{
    handleChange();
    this->budget = budget;
}

double TaskDataMsg::getUserMaxBandwidth() const
{
    return this->userMaxBandwidth;
}

void TaskDataMsg::setUserMaxBandwidth(double userMaxBandwidth)
{
    handleChange();
    this->userMaxBandwidth = userMaxBandwidth;
}

int TaskDataMsg::getPriority() const
{
    return this->priority;
}

void TaskDataMsg::setPriority(int priority)
{
    handleChange();
    this->priority = priority;
}

class TaskDataMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_userNodeAddress,
        FIELD_generationTime,
        FIELD_computingType,
        FIELD_requiredStorage,
        FIELD_computingAmount,
        FIELD_transferAmount,
        FIELD_totalDelayRequirement,
        FIELD_budget,
        FIELD_userMaxBandwidth,
        FIELD_priority,
    };
  public:
    TaskDataMsgDescriptor();
    virtual ~TaskDataMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TaskDataMsgDescriptor)

TaskDataMsgDescriptor::TaskDataMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::TaskDataMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

TaskDataMsgDescriptor::~TaskDataMsgDescriptor()
{
    delete[] propertyNames;
}

bool TaskDataMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TaskDataMsg *>(obj)!=nullptr;
}

const char **TaskDataMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TaskDataMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TaskDataMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 13+base->getFieldCount() : 13;
}

unsigned int TaskDataMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        0,    // FIELD_userNodeAddress
        FD_ISEDITABLE,    // FIELD_generationTime
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_requiredStorage
        FD_ISEDITABLE,    // FIELD_computingAmount
        FD_ISEDITABLE,    // FIELD_transferAmount
        FD_ISEDITABLE,    // FIELD_totalDelayRequirement
        FD_ISEDITABLE,    // FIELD_budget
        FD_ISEDITABLE,    // FIELD_userMaxBandwidth
        FD_ISEDITABLE,    // FIELD_priority
    };
    return (field >= 0 && field < 13) ? fieldTypeFlags[field] : 0;
}

const char *TaskDataMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "userNodeAddress",
        "generationTime",
        "computingType",
        "requiredStorage",
        "computingAmount",
        "transferAmount",
        "totalDelayRequirement",
        "budget",
        "userMaxBandwidth",
        "priority",
    };
    return (field >= 0 && field < 13) ? fieldNames[field] : nullptr;
}

int TaskDataMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "userNodeAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "generationTime") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "requiredStorage") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "computingAmount") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "transferAmount") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "totalDelayRequirement") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "budget") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "userMaxBandwidth") == 0) return baseIndex + 11;
    if (strcmp(fieldName, "priority") == 0) return baseIndex + 12;
    return base ? base->findField(fieldName) : -1;
}

const char *TaskDataMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "inet::L3Address",    // FIELD_userNodeAddress
        "omnetpp::simtime_t",    // FIELD_generationTime
        "int",    // FIELD_computingType
        "double",    // FIELD_requiredStorage
        "double",    // FIELD_computingAmount
        "double",    // FIELD_transferAmount
        "omnetpp::simtime_t",    // FIELD_totalDelayRequirement
        "double",    // FIELD_budget
        "double",    // FIELD_userMaxBandwidth
        "int",    // FIELD_priority
    };
    return (field >= 0 && field < 13) ? fieldTypeStrings[field] : nullptr;
}

const char **TaskDataMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TaskDataMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TaskDataMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TaskDataMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TaskDataMsg'", field);
    }
}

const char *TaskDataMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TaskDataMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_userNodeAddress: return pp->getUserNodeAddress().str();
        case FIELD_generationTime: return simtime2string(pp->getGenerationTime());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_requiredStorage: return double2string(pp->getRequiredStorage());
        case FIELD_computingAmount: return double2string(pp->getComputingAmount());
        case FIELD_transferAmount: return double2string(pp->getTransferAmount());
        case FIELD_totalDelayRequirement: return simtime2string(pp->getTotalDelayRequirement());
        case FIELD_budget: return double2string(pp->getBudget());
        case FIELD_userMaxBandwidth: return double2string(pp->getUserMaxBandwidth());
        case FIELD_priority: return long2string(pp->getPriority());
        default: return "";
    }
}

void TaskDataMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_generationTime: pp->setGenerationTime(string2simtime(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(string2double(value)); break;
        case FIELD_computingAmount: pp->setComputingAmount(string2double(value)); break;
        case FIELD_transferAmount: pp->setTransferAmount(string2double(value)); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(string2simtime(value)); break;
        case FIELD_budget: pp->setBudget(string2double(value)); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(string2double(value)); break;
        case FIELD_priority: pp->setPriority(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskDataMsg'", field);
    }
}

omnetpp::cValue TaskDataMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_userNodeAddress: return omnetpp::toAnyPtr(&pp->getUserNodeAddress()); break;
        case FIELD_generationTime: return pp->getGenerationTime().dbl();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_requiredStorage: return pp->getRequiredStorage();
        case FIELD_computingAmount: return pp->getComputingAmount();
        case FIELD_transferAmount: return pp->getTransferAmount();
        case FIELD_totalDelayRequirement: return pp->getTotalDelayRequirement().dbl();
        case FIELD_budget: return pp->getBudget();
        case FIELD_userMaxBandwidth: return pp->getUserMaxBandwidth();
        case FIELD_priority: return pp->getPriority();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TaskDataMsg' as cValue -- field index out of range?", field);
    }
}

void TaskDataMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_generationTime: pp->setGenerationTime(value.doubleValue()); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredStorage: pp->setRequiredStorage(value.doubleValue()); break;
        case FIELD_computingAmount: pp->setComputingAmount(value.doubleValue()); break;
        case FIELD_transferAmount: pp->setTransferAmount(value.doubleValue()); break;
        case FIELD_totalDelayRequirement: pp->setTotalDelayRequirement(value.doubleValue()); break;
        case FIELD_budget: pp->setBudget(value.doubleValue()); break;
        case FIELD_userMaxBandwidth: pp->setUserMaxBandwidth(value.doubleValue()); break;
        case FIELD_priority: pp->setPriority(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskDataMsg'", field);
    }
}

const char *TaskDataMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TaskDataMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userNodeAddress: return omnetpp::toAnyPtr(&pp->getUserNodeAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TaskDataMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskDataMsg *pp = omnetpp::fromAnyPtr<TaskDataMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskDataMsg'", field);
    }
}

Register_Class(RespTimeoutSelfMsg)

RespTimeoutSelfMsg::RespTimeoutSelfMsg(const char *name) : ::omnetpp::cMessage(name)
{
}

RespTimeoutSelfMsg::RespTimeoutSelfMsg(const RespTimeoutSelfMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

RespTimeoutSelfMsg::~RespTimeoutSelfMsg()
{
}

RespTimeoutSelfMsg& RespTimeoutSelfMsg::operator=(const RespTimeoutSelfMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void RespTimeoutSelfMsg::copy(const RespTimeoutSelfMsg& other)
{
    this->msgType = other.msgType;
    this->userId = other.userId;
    this->taskId = other.taskId;
}

void RespTimeoutSelfMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
}

void RespTimeoutSelfMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
}

const char * RespTimeoutSelfMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void RespTimeoutSelfMsg::setMsgType(const char * msgType)
{
    this->msgType = msgType;
}

int RespTimeoutSelfMsg::getUserId() const
{
    return this->userId;
}

void RespTimeoutSelfMsg::setUserId(int userId)
{
    this->userId = userId;
}

int RespTimeoutSelfMsg::getTaskId() const
{
    return this->taskId;
}

void RespTimeoutSelfMsg::setTaskId(int taskId)
{
    this->taskId = taskId;
}

class RespTimeoutSelfMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgType,
        FIELD_userId,
        FIELD_taskId,
    };
  public:
    RespTimeoutSelfMsgDescriptor();
    virtual ~RespTimeoutSelfMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(RespTimeoutSelfMsgDescriptor)

RespTimeoutSelfMsgDescriptor::RespTimeoutSelfMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::RespTimeoutSelfMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

RespTimeoutSelfMsgDescriptor::~RespTimeoutSelfMsgDescriptor()
{
    delete[] propertyNames;
}

bool RespTimeoutSelfMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RespTimeoutSelfMsg *>(obj)!=nullptr;
}

const char **RespTimeoutSelfMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RespTimeoutSelfMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RespTimeoutSelfMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int RespTimeoutSelfMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *RespTimeoutSelfMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "userId",
        "taskId",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int RespTimeoutSelfMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *RespTimeoutSelfMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_msgType
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **RespTimeoutSelfMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RespTimeoutSelfMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RespTimeoutSelfMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void RespTimeoutSelfMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'RespTimeoutSelfMsg'", field);
    }
}

const char *RespTimeoutSelfMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RespTimeoutSelfMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        default: return "";
    }
}

void RespTimeoutSelfMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespTimeoutSelfMsg'", field);
    }
}

omnetpp::cValue RespTimeoutSelfMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'RespTimeoutSelfMsg' as cValue -- field index out of range?", field);
    }
}

void RespTimeoutSelfMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespTimeoutSelfMsg'", field);
    }
}

const char *RespTimeoutSelfMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr RespTimeoutSelfMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RespTimeoutSelfMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    RespTimeoutSelfMsg *pp = omnetpp::fromAnyPtr<RespTimeoutSelfMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespTimeoutSelfMsg'", field);
    }
}

computeNodeInfo::computeNodeInfo()
{
}

void __doPacking(omnetpp::cCommBuffer *b, const computeNodeInfo& a)
{
    doParsimPacking(b,a.computeNodeAddress);
    doParsimPacking(b,a.computeNodeId);
    doParsimPacking(b,a.computingType);
    doParsimPacking(b,a.computingCapacity);
    doParsimPacking(b,a.availableStorage);
    doParsimPacking(b,a.maxNetworkBandwidth);
    doParsimPacking(b,a.computeCost);
    doParsimPacking(b,a.sendTime);
}

void __doUnpacking(omnetpp::cCommBuffer *b, computeNodeInfo& a)
{
    doParsimUnpacking(b,a.computeNodeAddress);
    doParsimUnpacking(b,a.computeNodeId);
    doParsimUnpacking(b,a.computingType);
    doParsimUnpacking(b,a.computingCapacity);
    doParsimUnpacking(b,a.availableStorage);
    doParsimUnpacking(b,a.maxNetworkBandwidth);
    doParsimUnpacking(b,a.computeCost);
    doParsimUnpacking(b,a.sendTime);
}

class computeNodeInfoDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_computeNodeAddress,
        FIELD_computeNodeId,
        FIELD_computingType,
        FIELD_computingCapacity,
        FIELD_availableStorage,
        FIELD_maxNetworkBandwidth,
        FIELD_computeCost,
        FIELD_sendTime,
    };
  public:
    computeNodeInfoDescriptor();
    virtual ~computeNodeInfoDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(computeNodeInfoDescriptor)

computeNodeInfoDescriptor::computeNodeInfoDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::computeNodeInfo)), "")
{
    propertyNames = nullptr;
}

computeNodeInfoDescriptor::~computeNodeInfoDescriptor()
{
    delete[] propertyNames;
}

bool computeNodeInfoDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<computeNodeInfo *>(obj)!=nullptr;
}

const char **computeNodeInfoDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *computeNodeInfoDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int computeNodeInfoDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int computeNodeInfoDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_computeNodeAddress
        FD_ISEDITABLE,    // FIELD_computeNodeId
        FD_ISEDITABLE,    // FIELD_computingType
        FD_ISEDITABLE,    // FIELD_computingCapacity
        FD_ISEDITABLE,    // FIELD_availableStorage
        FD_ISEDITABLE,    // FIELD_maxNetworkBandwidth
        FD_ISEDITABLE,    // FIELD_computeCost
        FD_ISEDITABLE,    // FIELD_sendTime
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *computeNodeInfoDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "computeNodeAddress",
        "computeNodeId",
        "computingType",
        "computingCapacity",
        "availableStorage",
        "maxNetworkBandwidth",
        "computeCost",
        "sendTime",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int computeNodeInfoDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "computeNodeAddress") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "computeNodeId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "computingCapacity") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "availableStorage") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "maxNetworkBandwidth") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "computeCost") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "sendTime") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *computeNodeInfoDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "inet::L3Address",    // FIELD_computeNodeAddress
        "int",    // FIELD_computeNodeId
        "int",    // FIELD_computingType
        "double",    // FIELD_computingCapacity
        "double",    // FIELD_availableStorage
        "double",    // FIELD_maxNetworkBandwidth
        "double",    // FIELD_computeCost
        "omnetpp::simtime_t",    // FIELD_sendTime
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **computeNodeInfoDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *computeNodeInfoDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int computeNodeInfoDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void computeNodeInfoDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'computeNodeInfo'", field);
    }
}

const char *computeNodeInfoDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string computeNodeInfoDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return pp->computeNodeAddress.str();
        case FIELD_computeNodeId: return long2string(pp->computeNodeId);
        case FIELD_computingType: return long2string(pp->computingType);
        case FIELD_computingCapacity: return double2string(pp->computingCapacity);
        case FIELD_availableStorage: return double2string(pp->availableStorage);
        case FIELD_maxNetworkBandwidth: return double2string(pp->maxNetworkBandwidth);
        case FIELD_computeCost: return double2string(pp->computeCost);
        case FIELD_sendTime: return simtime2string(pp->sendTime);
        default: return "";
    }
}

void computeNodeInfoDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeId: pp->computeNodeId = string2long(value); break;
        case FIELD_computingType: pp->computingType = string2long(value); break;
        case FIELD_computingCapacity: pp->computingCapacity = string2double(value); break;
        case FIELD_availableStorage: pp->availableStorage = string2double(value); break;
        case FIELD_maxNetworkBandwidth: pp->maxNetworkBandwidth = string2double(value); break;
        case FIELD_computeCost: pp->computeCost = string2double(value); break;
        case FIELD_sendTime: pp->sendTime = string2simtime(value); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'computeNodeInfo'", field);
    }
}

omnetpp::cValue computeNodeInfoDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->computeNodeAddress); break;
        case FIELD_computeNodeId: return pp->computeNodeId;
        case FIELD_computingType: return pp->computingType;
        case FIELD_computingCapacity: return pp->computingCapacity;
        case FIELD_availableStorage: return pp->availableStorage;
        case FIELD_maxNetworkBandwidth: return pp->maxNetworkBandwidth;
        case FIELD_computeCost: return pp->computeCost;
        case FIELD_sendTime: return pp->sendTime.dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'computeNodeInfo' as cValue -- field index out of range?", field);
    }
}

void computeNodeInfoDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeId: pp->computeNodeId = omnetpp::checked_int_cast<int>(value.intValue()); break;
        case FIELD_computingType: pp->computingType = omnetpp::checked_int_cast<int>(value.intValue()); break;
        case FIELD_computingCapacity: pp->computingCapacity = value.doubleValue(); break;
        case FIELD_availableStorage: pp->availableStorage = value.doubleValue(); break;
        case FIELD_maxNetworkBandwidth: pp->maxNetworkBandwidth = value.doubleValue(); break;
        case FIELD_computeCost: pp->computeCost = value.doubleValue(); break;
        case FIELD_sendTime: pp->sendTime = value.doubleValue(); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'computeNodeInfo'", field);
    }
}

const char *computeNodeInfoDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr computeNodeInfoDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->computeNodeAddress); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void computeNodeInfoDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    computeNodeInfo *pp = omnetpp::fromAnyPtr<computeNodeInfo>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'computeNodeInfo'", field);
    }
}

Register_Class(RespSummaryMsg)

RespSummaryMsg::RespSummaryMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(12 + 32));

}

RespSummaryMsg::RespSummaryMsg(const RespSummaryMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

RespSummaryMsg::~RespSummaryMsg()
{
    delete [] this->nodeInfo;
}

RespSummaryMsg& RespSummaryMsg::operator=(const RespSummaryMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void RespSummaryMsg::copy(const RespSummaryMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->msgType = other.msgType;
    delete [] this->nodeInfo;
    this->nodeInfo = (other.nodeInfo_arraysize==0) ? nullptr : new computeNodeInfo[other.nodeInfo_arraysize];
    nodeInfo_arraysize = other.nodeInfo_arraysize;
    for (size_t i = 0; i < nodeInfo_arraysize; i++) {
        this->nodeInfo[i] = other.nodeInfo[i];
    }
}

void RespSummaryMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->msgType);
    b->pack(nodeInfo_arraysize);
    doParsimArrayPacking(b,this->nodeInfo,nodeInfo_arraysize);
}

void RespSummaryMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->msgType);
    delete [] this->nodeInfo;
    b->unpack(nodeInfo_arraysize);
    if (nodeInfo_arraysize == 0) {
        this->nodeInfo = nullptr;
    } else {
        this->nodeInfo = new computeNodeInfo[nodeInfo_arraysize];
        doParsimArrayUnpacking(b,this->nodeInfo,nodeInfo_arraysize);
    }
}

int RespSummaryMsg::getUserId() const
{
    return this->userId;
}

void RespSummaryMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int RespSummaryMsg::getTaskId() const
{
    return this->taskId;
}

void RespSummaryMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const char * RespSummaryMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void RespSummaryMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

size_t RespSummaryMsg::getNodeInfoArraySize() const
{
    return nodeInfo_arraysize;
}

const computeNodeInfo& RespSummaryMsg::getNodeInfo(size_t k) const
{
    if (k >= nodeInfo_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)nodeInfo_arraysize, (unsigned long)k);
    return this->nodeInfo[k];
}

void RespSummaryMsg::setNodeInfoArraySize(size_t newSize)
{
    handleChange();
    computeNodeInfo *nodeInfo2 = (newSize==0) ? nullptr : new computeNodeInfo[newSize];
    size_t minSize = nodeInfo_arraysize < newSize ? nodeInfo_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        nodeInfo2[i] = this->nodeInfo[i];
    delete [] this->nodeInfo;
    this->nodeInfo = nodeInfo2;
    nodeInfo_arraysize = newSize;
}

void RespSummaryMsg::setNodeInfo(size_t k, const computeNodeInfo& nodeInfo)
{
    if (k >= nodeInfo_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)nodeInfo_arraysize, (unsigned long)k);
    handleChange();
    this->nodeInfo[k] = nodeInfo;
}

void RespSummaryMsg::insertNodeInfo(size_t k, const computeNodeInfo& nodeInfo)
{
    if (k > nodeInfo_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)nodeInfo_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = nodeInfo_arraysize + 1;
    computeNodeInfo *nodeInfo2 = new computeNodeInfo[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        nodeInfo2[i] = this->nodeInfo[i];
    nodeInfo2[k] = nodeInfo;
    for (i = k + 1; i < newSize; i++)
        nodeInfo2[i] = this->nodeInfo[i-1];
    delete [] this->nodeInfo;
    this->nodeInfo = nodeInfo2;
    nodeInfo_arraysize = newSize;
}

void RespSummaryMsg::appendNodeInfo(const computeNodeInfo& nodeInfo)
{
    insertNodeInfo(nodeInfo_arraysize, nodeInfo);
}

void RespSummaryMsg::eraseNodeInfo(size_t k)
{
    if (k >= nodeInfo_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)nodeInfo_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = nodeInfo_arraysize - 1;
    computeNodeInfo *nodeInfo2 = (newSize == 0) ? nullptr : new computeNodeInfo[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        nodeInfo2[i] = this->nodeInfo[i];
    for (i = k; i < newSize; i++)
        nodeInfo2[i] = this->nodeInfo[i+1];
    delete [] this->nodeInfo;
    this->nodeInfo = nodeInfo2;
    nodeInfo_arraysize = newSize;
}

class RespSummaryMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_msgType,
        FIELD_nodeInfo,
    };
  public:
    RespSummaryMsgDescriptor();
    virtual ~RespSummaryMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(RespSummaryMsgDescriptor)

RespSummaryMsgDescriptor::RespSummaryMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::RespSummaryMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

RespSummaryMsgDescriptor::~RespSummaryMsgDescriptor()
{
    delete[] propertyNames;
}

bool RespSummaryMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RespSummaryMsg *>(obj)!=nullptr;
}

const char **RespSummaryMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RespSummaryMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RespSummaryMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int RespSummaryMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISRESIZABLE,    // FIELD_nodeInfo
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *RespSummaryMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "msgType",
        "nodeInfo",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int RespSummaryMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "nodeInfo") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *RespSummaryMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "string",    // FIELD_msgType
        "inet::computeNodeInfo",    // FIELD_nodeInfo
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **RespSummaryMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RespSummaryMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RespSummaryMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_nodeInfo: return pp->getNodeInfoArraySize();
        default: return 0;
    }
}

void RespSummaryMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_nodeInfo: pp->setNodeInfoArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'RespSummaryMsg'", field);
    }
}

const char *RespSummaryMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RespSummaryMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_nodeInfo: return "";
        default: return "";
    }
}

void RespSummaryMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespSummaryMsg'", field);
    }
}

omnetpp::cValue RespSummaryMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_nodeInfo: return omnetpp::toAnyPtr(&pp->getNodeInfo(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'RespSummaryMsg' as cValue -- field index out of range?", field);
    }
}

void RespSummaryMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespSummaryMsg'", field);
    }
}

const char *RespSummaryMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_nodeInfo: return omnetpp::opp_typename(typeid(computeNodeInfo));
        default: return nullptr;
    };
}

omnetpp::any_ptr RespSummaryMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_nodeInfo: return omnetpp::toAnyPtr(&pp->getNodeInfo(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RespSummaryMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    RespSummaryMsg *pp = omnetpp::fromAnyPtr<RespSummaryMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RespSummaryMsg'", field);
    }
}

Register_Class(CgmpQueryMsg)

CgmpQueryMsg::CgmpQueryMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(1));

}

CgmpQueryMsg::CgmpQueryMsg(const CgmpQueryMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CgmpQueryMsg::~CgmpQueryMsg()
{
}

CgmpQueryMsg& CgmpQueryMsg::operator=(const CgmpQueryMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CgmpQueryMsg::copy(const CgmpQueryMsg& other)
{
    this->msgType = other.msgType;
}

void CgmpQueryMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->msgType);
}

void CgmpQueryMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
}

const char * CgmpQueryMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CgmpQueryMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

class CgmpQueryMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgType,
    };
  public:
    CgmpQueryMsgDescriptor();
    virtual ~CgmpQueryMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CgmpQueryMsgDescriptor)

CgmpQueryMsgDescriptor::CgmpQueryMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CgmpQueryMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CgmpQueryMsgDescriptor::~CgmpQueryMsgDescriptor()
{
    delete[] propertyNames;
}

bool CgmpQueryMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CgmpQueryMsg *>(obj)!=nullptr;
}

const char **CgmpQueryMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CgmpQueryMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CgmpQueryMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 1+base->getFieldCount() : 1;
}

unsigned int CgmpQueryMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgType
    };
    return (field >= 0 && field < 1) ? fieldTypeFlags[field] : 0;
}

const char *CgmpQueryMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
    };
    return (field >= 0 && field < 1) ? fieldNames[field] : nullptr;
}

int CgmpQueryMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 0;
    return base ? base->findField(fieldName) : -1;
}

const char *CgmpQueryMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_msgType
    };
    return (field >= 0 && field < 1) ? fieldTypeStrings[field] : nullptr;
}

const char **CgmpQueryMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CgmpQueryMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CgmpQueryMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CgmpQueryMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CgmpQueryMsg'", field);
    }
}

const char *CgmpQueryMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CgmpQueryMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        default: return "";
    }
}

void CgmpQueryMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpQueryMsg'", field);
    }
}

omnetpp::cValue CgmpQueryMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return pp->getMsgType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CgmpQueryMsg' as cValue -- field index out of range?", field);
    }
}

void CgmpQueryMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpQueryMsg'", field);
    }
}

const char *CgmpQueryMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CgmpQueryMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CgmpQueryMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpQueryMsg *pp = omnetpp::fromAnyPtr<CgmpQueryMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpQueryMsg'", field);
    }
}

Register_Class(CgmpReportMsg)

CgmpReportMsg::CgmpReportMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(8 + 32));

}

CgmpReportMsg::CgmpReportMsg(const CgmpReportMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CgmpReportMsg::~CgmpReportMsg()
{
}

CgmpReportMsg& CgmpReportMsg::operator=(const CgmpReportMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CgmpReportMsg::copy(const CgmpReportMsg& other)
{
    this->msgType = other.msgType;
    this->computeNodeId = other.computeNodeId;
    this->computingType = other.computingType;
    this->computeNodeAddress = other.computeNodeAddress;
    this->computingCapacity = other.computingCapacity;
    this->availableStorage = other.availableStorage;
    this->maxNetworkBandwidth = other.maxNetworkBandwidth;
    this->computeCost = other.computeCost;
    this->sendTime = other.sendTime;
}

void CgmpReportMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->computeNodeId);
    doParsimPacking(b,this->computingType);
    doParsimPacking(b,this->computeNodeAddress);
    doParsimPacking(b,this->computingCapacity);
    doParsimPacking(b,this->availableStorage);
    doParsimPacking(b,this->maxNetworkBandwidth);
    doParsimPacking(b,this->computeCost);
    doParsimPacking(b,this->sendTime);
}

void CgmpReportMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->computeNodeId);
    doParsimUnpacking(b,this->computingType);
    doParsimUnpacking(b,this->computeNodeAddress);
    doParsimUnpacking(b,this->computingCapacity);
    doParsimUnpacking(b,this->availableStorage);
    doParsimUnpacking(b,this->maxNetworkBandwidth);
    doParsimUnpacking(b,this->computeCost);
    doParsimUnpacking(b,this->sendTime);
}

const char * CgmpReportMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void CgmpReportMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

int CgmpReportMsg::getComputeNodeId() const
{
    return this->computeNodeId;
}

void CgmpReportMsg::setComputeNodeId(int computeNodeId)
{
    handleChange();
    this->computeNodeId = computeNodeId;
}

int CgmpReportMsg::getComputingType() const
{
    return this->computingType;
}

void CgmpReportMsg::setComputingType(int computingType)
{
    handleChange();
    this->computingType = computingType;
}

const L3Address& CgmpReportMsg::getComputeNodeAddress() const
{
    return this->computeNodeAddress;
}

void CgmpReportMsg::setComputeNodeAddress(const L3Address& computeNodeAddress)
{
    handleChange();
    this->computeNodeAddress = computeNodeAddress;
}

double CgmpReportMsg::getComputingCapacity() const
{
    return this->computingCapacity;
}

void CgmpReportMsg::setComputingCapacity(double computingCapacity)
{
    handleChange();
    this->computingCapacity = computingCapacity;
}

double CgmpReportMsg::getAvailableStorage() const
{
    return this->availableStorage;
}

void CgmpReportMsg::setAvailableStorage(double availableStorage)
{
    handleChange();
    this->availableStorage = availableStorage;
}

double CgmpReportMsg::getMaxNetworkBandwidth() const
{
    return this->maxNetworkBandwidth;
}

void CgmpReportMsg::setMaxNetworkBandwidth(double maxNetworkBandwidth)
{
    handleChange();
    this->maxNetworkBandwidth = maxNetworkBandwidth;
}

double CgmpReportMsg::getComputeCost() const
{
    return this->computeCost;
}

void CgmpReportMsg::setComputeCost(double computeCost)
{
    handleChange();
    this->computeCost = computeCost;
}

::omnetpp::simtime_t CgmpReportMsg::getSendTime() const
{
    return this->sendTime;
}

void CgmpReportMsg::setSendTime(::omnetpp::simtime_t sendTime)
{
    handleChange();
    this->sendTime = sendTime;
}

class CgmpReportMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgType,
        FIELD_computeNodeId,
        FIELD_computingType,
        FIELD_computeNodeAddress,
        FIELD_computingCapacity,
        FIELD_availableStorage,
        FIELD_maxNetworkBandwidth,
        FIELD_computeCost,
        FIELD_sendTime,
    };
  public:
    CgmpReportMsgDescriptor();
    virtual ~CgmpReportMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CgmpReportMsgDescriptor)

CgmpReportMsgDescriptor::CgmpReportMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CgmpReportMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CgmpReportMsgDescriptor::~CgmpReportMsgDescriptor()
{
    delete[] propertyNames;
}

bool CgmpReportMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CgmpReportMsg *>(obj)!=nullptr;
}

const char **CgmpReportMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CgmpReportMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CgmpReportMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 9+base->getFieldCount() : 9;
}

unsigned int CgmpReportMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_computeNodeId
        FD_ISEDITABLE,    // FIELD_computingType
        0,    // FIELD_computeNodeAddress
        FD_ISEDITABLE,    // FIELD_computingCapacity
        FD_ISEDITABLE,    // FIELD_availableStorage
        FD_ISEDITABLE,    // FIELD_maxNetworkBandwidth
        FD_ISEDITABLE,    // FIELD_computeCost
        FD_ISEDITABLE,    // FIELD_sendTime
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *CgmpReportMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "computeNodeId",
        "computingType",
        "computeNodeAddress",
        "computingCapacity",
        "availableStorage",
        "maxNetworkBandwidth",
        "computeCost",
        "sendTime",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int CgmpReportMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "computeNodeId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "computingType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "computeNodeAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "computingCapacity") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "availableStorage") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "maxNetworkBandwidth") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "computeCost") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "sendTime") == 0) return baseIndex + 8;
    return base ? base->findField(fieldName) : -1;
}

const char *CgmpReportMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_msgType
        "int",    // FIELD_computeNodeId
        "int",    // FIELD_computingType
        "inet::L3Address",    // FIELD_computeNodeAddress
        "double",    // FIELD_computingCapacity
        "double",    // FIELD_availableStorage
        "double",    // FIELD_maxNetworkBandwidth
        "double",    // FIELD_computeCost
        "omnetpp::simtime_t",    // FIELD_sendTime
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **CgmpReportMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CgmpReportMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CgmpReportMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CgmpReportMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CgmpReportMsg'", field);
    }
}

const char *CgmpReportMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CgmpReportMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        case FIELD_computeNodeId: return long2string(pp->getComputeNodeId());
        case FIELD_computingType: return long2string(pp->getComputingType());
        case FIELD_computeNodeAddress: return pp->getComputeNodeAddress().str();
        case FIELD_computingCapacity: return double2string(pp->getComputingCapacity());
        case FIELD_availableStorage: return double2string(pp->getAvailableStorage());
        case FIELD_maxNetworkBandwidth: return double2string(pp->getMaxNetworkBandwidth());
        case FIELD_computeCost: return double2string(pp->getComputeCost());
        case FIELD_sendTime: return simtime2string(pp->getSendTime());
        default: return "";
    }
}

void CgmpReportMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType((value)); break;
        case FIELD_computeNodeId: pp->setComputeNodeId(string2long(value)); break;
        case FIELD_computingType: pp->setComputingType(string2long(value)); break;
        case FIELD_computingCapacity: pp->setComputingCapacity(string2double(value)); break;
        case FIELD_availableStorage: pp->setAvailableStorage(string2double(value)); break;
        case FIELD_maxNetworkBandwidth: pp->setMaxNetworkBandwidth(string2double(value)); break;
        case FIELD_computeCost: pp->setComputeCost(string2double(value)); break;
        case FIELD_sendTime: pp->setSendTime(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpReportMsg'", field);
    }
}

omnetpp::cValue CgmpReportMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_computeNodeId: return pp->getComputeNodeId();
        case FIELD_computingType: return pp->getComputingType();
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_computingCapacity: return pp->getComputingCapacity();
        case FIELD_availableStorage: return pp->getAvailableStorage();
        case FIELD_maxNetworkBandwidth: return pp->getMaxNetworkBandwidth();
        case FIELD_computeCost: return pp->getComputeCost();
        case FIELD_sendTime: return pp->getSendTime().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CgmpReportMsg' as cValue -- field index out of range?", field);
    }
}

void CgmpReportMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        case FIELD_computeNodeId: pp->setComputeNodeId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computingType: pp->setComputingType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computingCapacity: pp->setComputingCapacity(value.doubleValue()); break;
        case FIELD_availableStorage: pp->setAvailableStorage(value.doubleValue()); break;
        case FIELD_maxNetworkBandwidth: pp->setMaxNetworkBandwidth(value.doubleValue()); break;
        case FIELD_computeCost: pp->setComputeCost(value.doubleValue()); break;
        case FIELD_sendTime: pp->setSendTime(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpReportMsg'", field);
    }
}

const char *CgmpReportMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CgmpReportMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CgmpReportMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CgmpReportMsg *pp = omnetpp::fromAnyPtr<CgmpReportMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CgmpReportMsg'", field);
    }
}

Register_Enum(inet::TaskFailureCode, (inet::TaskFailureCode::TASK_OK, inet::TaskFailureCode::TASK_INVALID_COMPUTING_TYPE, inet::TaskFailureCode::TASK_INSUFFICIENT_STORAGE, inet::TaskFailureCode::TASK_INSUFFICIENT_BANDWIDTH, inet::TaskFailureCode::TASK_INVALID_RESOURCE_REQUEST));

Register_Class(TaskCompletionMsg)

TaskCompletionMsg::TaskCompletionMsg() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(64));

}

TaskCompletionMsg::TaskCompletionMsg(const TaskCompletionMsg& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

TaskCompletionMsg::~TaskCompletionMsg()
{
}

TaskCompletionMsg& TaskCompletionMsg::operator=(const TaskCompletionMsg& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void TaskCompletionMsg::copy(const TaskCompletionMsg& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    this->computeNodeAddress = other.computeNodeAddress;
    this->computeNodePort = other.computeNodePort;
    this->userNodeAddress = other.userNodeAddress;
    this->completionTime = other.completionTime;
    this->success = other.success;
    this->failureCode = other.failureCode;
    this->failureReason = other.failureReason;
    this->taskResult = other.taskResult;
    this->executionTime = other.executionTime;
    this->msgType = other.msgType;
}

void TaskCompletionMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    doParsimPacking(b,this->computeNodeAddress);
    doParsimPacking(b,this->computeNodePort);
    doParsimPacking(b,this->userNodeAddress);
    doParsimPacking(b,this->completionTime);
    doParsimPacking(b,this->success);
    doParsimPacking(b,this->failureCode);
    doParsimPacking(b,this->failureReason);
    doParsimPacking(b,this->taskResult);
    doParsimPacking(b,this->executionTime);
    doParsimPacking(b,this->msgType);
}

void TaskCompletionMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    doParsimUnpacking(b,this->computeNodeAddress);
    doParsimUnpacking(b,this->computeNodePort);
    doParsimUnpacking(b,this->userNodeAddress);
    doParsimUnpacking(b,this->completionTime);
    doParsimUnpacking(b,this->success);
    doParsimUnpacking(b,this->failureCode);
    doParsimUnpacking(b,this->failureReason);
    doParsimUnpacking(b,this->taskResult);
    doParsimUnpacking(b,this->executionTime);
    doParsimUnpacking(b,this->msgType);
}

int TaskCompletionMsg::getUserId() const
{
    return this->userId;
}

void TaskCompletionMsg::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int TaskCompletionMsg::getTaskId() const
{
    return this->taskId;
}

void TaskCompletionMsg::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

const L3Address& TaskCompletionMsg::getComputeNodeAddress() const
{
    return this->computeNodeAddress;
}

void TaskCompletionMsg::setComputeNodeAddress(const L3Address& computeNodeAddress)
{
    handleChange();
    this->computeNodeAddress = computeNodeAddress;
}

int TaskCompletionMsg::getComputeNodePort() const
{
    return this->computeNodePort;
}

void TaskCompletionMsg::setComputeNodePort(int computeNodePort)
{
    handleChange();
    this->computeNodePort = computeNodePort;
}

const L3Address& TaskCompletionMsg::getUserNodeAddress() const
{
    return this->userNodeAddress;
}

void TaskCompletionMsg::setUserNodeAddress(const L3Address& userNodeAddress)
{
    handleChange();
    this->userNodeAddress = userNodeAddress;
}

::omnetpp::simtime_t TaskCompletionMsg::getCompletionTime() const
{
    return this->completionTime;
}

void TaskCompletionMsg::setCompletionTime(::omnetpp::simtime_t completionTime)
{
    handleChange();
    this->completionTime = completionTime;
}

bool TaskCompletionMsg::getSuccess() const
{
    return this->success;
}

void TaskCompletionMsg::setSuccess(bool success)
{
    handleChange();
    this->success = success;
}

int TaskCompletionMsg::getFailureCode() const
{
    return this->failureCode;
}

void TaskCompletionMsg::setFailureCode(int failureCode)
{
    handleChange();
    this->failureCode = failureCode;
}

const char * TaskCompletionMsg::getFailureReason() const
{
    return this->failureReason.c_str();
}

void TaskCompletionMsg::setFailureReason(const char * failureReason)
{
    handleChange();
    this->failureReason = failureReason;
}

double TaskCompletionMsg::getTaskResult() const
{
    return this->taskResult;
}

void TaskCompletionMsg::setTaskResult(double taskResult)
{
    handleChange();
    this->taskResult = taskResult;
}

double TaskCompletionMsg::getExecutionTime() const
{
    return this->executionTime;
}

void TaskCompletionMsg::setExecutionTime(double executionTime)
{
    handleChange();
    this->executionTime = executionTime;
}

const char * TaskCompletionMsg::getMsgType() const
{
    return this->msgType.c_str();
}

void TaskCompletionMsg::setMsgType(const char * msgType)
{
    handleChange();
    this->msgType = msgType;
}

class TaskCompletionMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_computeNodeAddress,
        FIELD_computeNodePort,
        FIELD_userNodeAddress,
        FIELD_completionTime,
        FIELD_success,
        FIELD_failureCode,
        FIELD_failureReason,
        FIELD_taskResult,
        FIELD_executionTime,
        FIELD_msgType,
    };
  public:
    TaskCompletionMsgDescriptor();
    virtual ~TaskCompletionMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TaskCompletionMsgDescriptor)

TaskCompletionMsgDescriptor::TaskCompletionMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::TaskCompletionMsg)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

TaskCompletionMsgDescriptor::~TaskCompletionMsgDescriptor()
{
    delete[] propertyNames;
}

bool TaskCompletionMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TaskCompletionMsg *>(obj)!=nullptr;
}

const char **TaskCompletionMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TaskCompletionMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TaskCompletionMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 12+base->getFieldCount() : 12;
}

unsigned int TaskCompletionMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        0,    // FIELD_computeNodeAddress
        FD_ISEDITABLE,    // FIELD_computeNodePort
        0,    // FIELD_userNodeAddress
        FD_ISEDITABLE,    // FIELD_completionTime
        FD_ISEDITABLE,    // FIELD_success
        FD_ISEDITABLE,    // FIELD_failureCode
        FD_ISEDITABLE,    // FIELD_failureReason
        FD_ISEDITABLE,    // FIELD_taskResult
        FD_ISEDITABLE,    // FIELD_executionTime
        FD_ISEDITABLE,    // FIELD_msgType
    };
    return (field >= 0 && field < 12) ? fieldTypeFlags[field] : 0;
}

const char *TaskCompletionMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "userId",
        "taskId",
        "computeNodeAddress",
        "computeNodePort",
        "userNodeAddress",
        "completionTime",
        "success",
        "failureCode",
        "failureReason",
        "taskResult",
        "executionTime",
        "msgType",
    };
    return (field >= 0 && field < 12) ? fieldNames[field] : nullptr;
}

int TaskCompletionMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "computeNodeAddress") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "computeNodePort") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "userNodeAddress") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "completionTime") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "success") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "failureCode") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "failureReason") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "taskResult") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "executionTime") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 11;
    return base ? base->findField(fieldName) : -1;
}

const char *TaskCompletionMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "inet::L3Address",    // FIELD_computeNodeAddress
        "int",    // FIELD_computeNodePort
        "inet::L3Address",    // FIELD_userNodeAddress
        "omnetpp::simtime_t",    // FIELD_completionTime
        "bool",    // FIELD_success
        "int",    // FIELD_failureCode
        "string",    // FIELD_failureReason
        "double",    // FIELD_taskResult
        "double",    // FIELD_executionTime
        "string",    // FIELD_msgType
    };
    return (field >= 0 && field < 12) ? fieldTypeStrings[field] : nullptr;
}

const char **TaskCompletionMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TaskCompletionMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TaskCompletionMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TaskCompletionMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TaskCompletionMsg'", field);
    }
}

const char *TaskCompletionMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TaskCompletionMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_computeNodeAddress: return pp->getComputeNodeAddress().str();
        case FIELD_computeNodePort: return long2string(pp->getComputeNodePort());
        case FIELD_userNodeAddress: return pp->getUserNodeAddress().str();
        case FIELD_completionTime: return simtime2string(pp->getCompletionTime());
        case FIELD_success: return bool2string(pp->getSuccess());
        case FIELD_failureCode: return long2string(pp->getFailureCode());
        case FIELD_failureReason: return oppstring2string(pp->getFailureReason());
        case FIELD_taskResult: return double2string(pp->getTaskResult());
        case FIELD_executionTime: return double2string(pp->getExecutionTime());
        case FIELD_msgType: return oppstring2string(pp->getMsgType());
        default: return "";
    }
}

void TaskCompletionMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(string2long(value)); break;
        case FIELD_completionTime: pp->setCompletionTime(string2simtime(value)); break;
        case FIELD_success: pp->setSuccess(string2bool(value)); break;
        case FIELD_failureCode: pp->setFailureCode(string2long(value)); break;
        case FIELD_failureReason: pp->setFailureReason((value)); break;
        case FIELD_taskResult: pp->setTaskResult(string2double(value)); break;
        case FIELD_executionTime: pp->setExecutionTime(string2double(value)); break;
        case FIELD_msgType: pp->setMsgType((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskCompletionMsg'", field);
    }
}

omnetpp::cValue TaskCompletionMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_computeNodePort: return pp->getComputeNodePort();
        case FIELD_userNodeAddress: return omnetpp::toAnyPtr(&pp->getUserNodeAddress()); break;
        case FIELD_completionTime: return pp->getCompletionTime().dbl();
        case FIELD_success: return pp->getSuccess();
        case FIELD_failureCode: return pp->getFailureCode();
        case FIELD_failureReason: return pp->getFailureReason();
        case FIELD_taskResult: return pp->getTaskResult();
        case FIELD_executionTime: return pp->getExecutionTime();
        case FIELD_msgType: return pp->getMsgType();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TaskCompletionMsg' as cValue -- field index out of range?", field);
    }
}

void TaskCompletionMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_computeNodePort: pp->setComputeNodePort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_completionTime: pp->setCompletionTime(value.doubleValue()); break;
        case FIELD_success: pp->setSuccess(value.boolValue()); break;
        case FIELD_failureCode: pp->setFailureCode(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_failureReason: pp->setFailureReason(value.stringValue()); break;
        case FIELD_taskResult: pp->setTaskResult(value.doubleValue()); break;
        case FIELD_executionTime: pp->setExecutionTime(value.doubleValue()); break;
        case FIELD_msgType: pp->setMsgType(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskCompletionMsg'", field);
    }
}

const char *TaskCompletionMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TaskCompletionMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        case FIELD_computeNodeAddress: return omnetpp::toAnyPtr(&pp->getComputeNodeAddress()); break;
        case FIELD_userNodeAddress: return omnetpp::toAnyPtr(&pp->getUserNodeAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TaskCompletionMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TaskCompletionMsg *pp = omnetpp::fromAnyPtr<TaskCompletionMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TaskCompletionMsg'", field);
    }
}

}  // namespace inet

namespace omnetpp {

}  // namespace omnetpp

