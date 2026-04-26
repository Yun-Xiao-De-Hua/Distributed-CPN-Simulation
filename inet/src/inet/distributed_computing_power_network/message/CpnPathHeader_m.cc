//
// Generated file, do not edit! Created by opp_msgtool 6.0 from inet/distributed_computing_power_network/message/CpnPathHeader.msg.
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
#include "CpnPathHeader_m.h"

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

Register_Enum(inet::CpnPathMode, (inet::CpnPathMode::PATH_RECORD_MODE, inet::CpnPathMode::PATH_USE_MODE));

Register_Class(CpnPathHeader)

CpnPathHeader::CpnPathHeader() : ::inet::FieldsChunk()
{
    this->setChunkLength(B(32));

}

CpnPathHeader::CpnPathHeader(const CpnPathHeader& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

CpnPathHeader::~CpnPathHeader()
{
    delete [] this->hopAddress;
    delete [] this->sidList;
}

CpnPathHeader& CpnPathHeader::operator=(const CpnPathHeader& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void CpnPathHeader::copy(const CpnPathHeader& other)
{
    this->mode = other.mode;
    this->userId = other.userId;
    this->taskId = other.taskId;
    delete [] this->hopAddress;
    this->hopAddress = (other.hopAddress_arraysize==0) ? nullptr : new L3Address[other.hopAddress_arraysize];
    hopAddress_arraysize = other.hopAddress_arraysize;
    for (size_t i = 0; i < hopAddress_arraysize; i++) {
        this->hopAddress[i] = other.hopAddress[i];
    }
    this->userGatewayAddress = other.userGatewayAddress;
    this->requiredBandwidth = other.requiredBandwidth;
    delete [] this->sidList;
    this->sidList = (other.sidList_arraysize==0) ? nullptr : new L3Address[other.sidList_arraysize];
    sidList_arraysize = other.sidList_arraysize;
    for (size_t i = 0; i < sidList_arraysize; i++) {
        this->sidList[i] = other.sidList[i];
    }
    this->currentHopIndex = other.currentHopIndex;
}

void CpnPathHeader::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->mode);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    b->pack(hopAddress_arraysize);
    doParsimArrayPacking(b,this->hopAddress,hopAddress_arraysize);
    doParsimPacking(b,this->userGatewayAddress);
    doParsimPacking(b,this->requiredBandwidth);
    b->pack(sidList_arraysize);
    doParsimArrayPacking(b,this->sidList,sidList_arraysize);
    doParsimPacking(b,this->currentHopIndex);
}

void CpnPathHeader::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->mode);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    delete [] this->hopAddress;
    b->unpack(hopAddress_arraysize);
    if (hopAddress_arraysize == 0) {
        this->hopAddress = nullptr;
    } else {
        this->hopAddress = new L3Address[hopAddress_arraysize];
        doParsimArrayUnpacking(b,this->hopAddress,hopAddress_arraysize);
    }
    doParsimUnpacking(b,this->userGatewayAddress);
    doParsimUnpacking(b,this->requiredBandwidth);
    delete [] this->sidList;
    b->unpack(sidList_arraysize);
    if (sidList_arraysize == 0) {
        this->sidList = nullptr;
    } else {
        this->sidList = new L3Address[sidList_arraysize];
        doParsimArrayUnpacking(b,this->sidList,sidList_arraysize);
    }
    doParsimUnpacking(b,this->currentHopIndex);
}

int CpnPathHeader::getMode() const
{
    return this->mode;
}

void CpnPathHeader::setMode(int mode)
{
    handleChange();
    this->mode = mode;
}

int CpnPathHeader::getUserId() const
{
    return this->userId;
}

void CpnPathHeader::setUserId(int userId)
{
    handleChange();
    this->userId = userId;
}

int CpnPathHeader::getTaskId() const
{
    return this->taskId;
}

void CpnPathHeader::setTaskId(int taskId)
{
    handleChange();
    this->taskId = taskId;
}

size_t CpnPathHeader::getHopAddressArraySize() const
{
    return hopAddress_arraysize;
}

const L3Address& CpnPathHeader::getHopAddress(size_t k) const
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    return this->hopAddress[k];
}

void CpnPathHeader::setHopAddressArraySize(size_t newSize)
{
    handleChange();
    L3Address *hopAddress2 = (newSize==0) ? nullptr : new L3Address[newSize];
    size_t minSize = hopAddress_arraysize < newSize ? hopAddress_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        hopAddress2[i] = this->hopAddress[i];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathHeader::setHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    handleChange();
    this->hopAddress[k] = hopAddress;
}

void CpnPathHeader::insertHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k > hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = hopAddress_arraysize + 1;
    L3Address *hopAddress2 = new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    hopAddress2[k] = hopAddress;
    for (i = k + 1; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i-1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathHeader::appendHopAddress(const L3Address& hopAddress)
{
    insertHopAddress(hopAddress_arraysize, hopAddress);
}

void CpnPathHeader::eraseHopAddress(size_t k)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = hopAddress_arraysize - 1;
    L3Address *hopAddress2 = (newSize == 0) ? nullptr : new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    for (i = k; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i+1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

const L3Address& CpnPathHeader::getUserGatewayAddress() const
{
    return this->userGatewayAddress;
}

void CpnPathHeader::setUserGatewayAddress(const L3Address& userGatewayAddress)
{
    handleChange();
    this->userGatewayAddress = userGatewayAddress;
}

double CpnPathHeader::getRequiredBandwidth() const
{
    return this->requiredBandwidth;
}

void CpnPathHeader::setRequiredBandwidth(double requiredBandwidth)
{
    handleChange();
    this->requiredBandwidth = requiredBandwidth;
}

size_t CpnPathHeader::getSidListArraySize() const
{
    return sidList_arraysize;
}

const L3Address& CpnPathHeader::getSidList(size_t k) const
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    return this->sidList[k];
}

void CpnPathHeader::setSidListArraySize(size_t newSize)
{
    handleChange();
    L3Address *sidList2 = (newSize==0) ? nullptr : new L3Address[newSize];
    size_t minSize = sidList_arraysize < newSize ? sidList_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        sidList2[i] = this->sidList[i];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

void CpnPathHeader::setSidList(size_t k, const L3Address& sidList)
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    handleChange();
    this->sidList[k] = sidList;
}

void CpnPathHeader::insertSidList(size_t k, const L3Address& sidList)
{
    if (k > sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = sidList_arraysize + 1;
    L3Address *sidList2 = new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        sidList2[i] = this->sidList[i];
    sidList2[k] = sidList;
    for (i = k + 1; i < newSize; i++)
        sidList2[i] = this->sidList[i-1];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

void CpnPathHeader::appendSidList(const L3Address& sidList)
{
    insertSidList(sidList_arraysize, sidList);
}

void CpnPathHeader::eraseSidList(size_t k)
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = sidList_arraysize - 1;
    L3Address *sidList2 = (newSize == 0) ? nullptr : new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        sidList2[i] = this->sidList[i];
    for (i = k; i < newSize; i++)
        sidList2[i] = this->sidList[i+1];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

int CpnPathHeader::getCurrentHopIndex() const
{
    return this->currentHopIndex;
}

void CpnPathHeader::setCurrentHopIndex(int currentHopIndex)
{
    handleChange();
    this->currentHopIndex = currentHopIndex;
}

class CpnPathHeaderDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_mode,
        FIELD_userId,
        FIELD_taskId,
        FIELD_hopAddress,
        FIELD_userGatewayAddress,
        FIELD_requiredBandwidth,
        FIELD_sidList,
        FIELD_currentHopIndex,
    };
  public:
    CpnPathHeaderDescriptor();
    virtual ~CpnPathHeaderDescriptor();

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

Register_ClassDescriptor(CpnPathHeaderDescriptor)

CpnPathHeaderDescriptor::CpnPathHeaderDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CpnPathHeader)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

CpnPathHeaderDescriptor::~CpnPathHeaderDescriptor()
{
    delete[] propertyNames;
}

bool CpnPathHeaderDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CpnPathHeader *>(obj)!=nullptr;
}

const char **CpnPathHeaderDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CpnPathHeaderDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CpnPathHeaderDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int CpnPathHeaderDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_mode
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_hopAddress
        0,    // FIELD_userGatewayAddress
        FD_ISEDITABLE,    // FIELD_requiredBandwidth
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_sidList
        FD_ISEDITABLE,    // FIELD_currentHopIndex
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *CpnPathHeaderDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "mode",
        "userId",
        "taskId",
        "hopAddress",
        "userGatewayAddress",
        "requiredBandwidth",
        "sidList",
        "currentHopIndex",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int CpnPathHeaderDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "mode") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "hopAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "userGatewayAddress") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "requiredBandwidth") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "sidList") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "currentHopIndex") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *CpnPathHeaderDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_mode
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "inet::L3Address",    // FIELD_hopAddress
        "inet::L3Address",    // FIELD_userGatewayAddress
        "double",    // FIELD_requiredBandwidth
        "inet::L3Address",    // FIELD_sidList
        "int",    // FIELD_currentHopIndex
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **CpnPathHeaderDescriptor::getFieldPropertyNames(int field) const
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

const char *CpnPathHeaderDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CpnPathHeaderDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return pp->getHopAddressArraySize();
        case FIELD_sidList: return pp->getSidListArraySize();
        default: return 0;
    }
}

void CpnPathHeaderDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: pp->setHopAddressArraySize(size); break;
        case FIELD_sidList: pp->setSidListArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CpnPathHeader'", field);
    }
}

const char *CpnPathHeaderDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CpnPathHeaderDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_mode: return long2string(pp->getMode());
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_hopAddress: return pp->getHopAddress(i).str();
        case FIELD_userGatewayAddress: return pp->getUserGatewayAddress().str();
        case FIELD_requiredBandwidth: return double2string(pp->getRequiredBandwidth());
        case FIELD_sidList: return pp->getSidList(i).str();
        case FIELD_currentHopIndex: return long2string(pp->getCurrentHopIndex());
        default: return "";
    }
}

void CpnPathHeaderDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_mode: pp->setMode(string2long(value)); break;
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(string2double(value)); break;
        case FIELD_currentHopIndex: pp->setCurrentHopIndex(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathHeader'", field);
    }
}

omnetpp::cValue CpnPathHeaderDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_mode: return pp->getMode();
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        case FIELD_requiredBandwidth: return pp->getRequiredBandwidth();
        case FIELD_sidList: return omnetpp::toAnyPtr(&pp->getSidList(i)); break;
        case FIELD_currentHopIndex: return pp->getCurrentHopIndex();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CpnPathHeader' as cValue -- field index out of range?", field);
    }
}

void CpnPathHeaderDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_mode: pp->setMode(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(value.doubleValue()); break;
        case FIELD_currentHopIndex: pp->setCurrentHopIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathHeader'", field);
    }
}

const char *CpnPathHeaderDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CpnPathHeaderDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        case FIELD_sidList: return omnetpp::toAnyPtr(&pp->getSidList(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CpnPathHeaderDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathHeader *pp = omnetpp::fromAnyPtr<CpnPathHeader>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathHeader'", field);
    }
}

Register_Class(CpnPathReq)

CpnPathReq::CpnPathReq() : ::inet::TagBase()
{
}

CpnPathReq::CpnPathReq(const CpnPathReq& other) : ::inet::TagBase(other)
{
    copy(other);
}

CpnPathReq::~CpnPathReq()
{
    delete [] this->hopAddress;
    delete [] this->sidList;
}

CpnPathReq& CpnPathReq::operator=(const CpnPathReq& other)
{
    if (this == &other) return *this;
    ::inet::TagBase::operator=(other);
    copy(other);
    return *this;
}

void CpnPathReq::copy(const CpnPathReq& other)
{
    this->mode = other.mode;
    this->userId = other.userId;
    this->taskId = other.taskId;
    delete [] this->hopAddress;
    this->hopAddress = (other.hopAddress_arraysize==0) ? nullptr : new L3Address[other.hopAddress_arraysize];
    hopAddress_arraysize = other.hopAddress_arraysize;
    for (size_t i = 0; i < hopAddress_arraysize; i++) {
        this->hopAddress[i] = other.hopAddress[i];
    }
    this->userGatewayAddress = other.userGatewayAddress;
    this->requiredBandwidth = other.requiredBandwidth;
    delete [] this->sidList;
    this->sidList = (other.sidList_arraysize==0) ? nullptr : new L3Address[other.sidList_arraysize];
    sidList_arraysize = other.sidList_arraysize;
    for (size_t i = 0; i < sidList_arraysize; i++) {
        this->sidList[i] = other.sidList[i];
    }
    this->currentHopIndex = other.currentHopIndex;
}

void CpnPathReq::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::TagBase::parsimPack(b);
    doParsimPacking(b,this->mode);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    b->pack(hopAddress_arraysize);
    doParsimArrayPacking(b,this->hopAddress,hopAddress_arraysize);
    doParsimPacking(b,this->userGatewayAddress);
    doParsimPacking(b,this->requiredBandwidth);
    b->pack(sidList_arraysize);
    doParsimArrayPacking(b,this->sidList,sidList_arraysize);
    doParsimPacking(b,this->currentHopIndex);
}

void CpnPathReq::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::TagBase::parsimUnpack(b);
    doParsimUnpacking(b,this->mode);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    delete [] this->hopAddress;
    b->unpack(hopAddress_arraysize);
    if (hopAddress_arraysize == 0) {
        this->hopAddress = nullptr;
    } else {
        this->hopAddress = new L3Address[hopAddress_arraysize];
        doParsimArrayUnpacking(b,this->hopAddress,hopAddress_arraysize);
    }
    doParsimUnpacking(b,this->userGatewayAddress);
    doParsimUnpacking(b,this->requiredBandwidth);
    delete [] this->sidList;
    b->unpack(sidList_arraysize);
    if (sidList_arraysize == 0) {
        this->sidList = nullptr;
    } else {
        this->sidList = new L3Address[sidList_arraysize];
        doParsimArrayUnpacking(b,this->sidList,sidList_arraysize);
    }
    doParsimUnpacking(b,this->currentHopIndex);
}

int CpnPathReq::getMode() const
{
    return this->mode;
}

void CpnPathReq::setMode(int mode)
{
    this->mode = mode;
}

int CpnPathReq::getUserId() const
{
    return this->userId;
}

void CpnPathReq::setUserId(int userId)
{
    this->userId = userId;
}

int CpnPathReq::getTaskId() const
{
    return this->taskId;
}

void CpnPathReq::setTaskId(int taskId)
{
    this->taskId = taskId;
}

size_t CpnPathReq::getHopAddressArraySize() const
{
    return hopAddress_arraysize;
}

const L3Address& CpnPathReq::getHopAddress(size_t k) const
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    return this->hopAddress[k];
}

void CpnPathReq::setHopAddressArraySize(size_t newSize)
{
    L3Address *hopAddress2 = (newSize==0) ? nullptr : new L3Address[newSize];
    size_t minSize = hopAddress_arraysize < newSize ? hopAddress_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        hopAddress2[i] = this->hopAddress[i];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathReq::setHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    this->hopAddress[k] = hopAddress;
}

void CpnPathReq::insertHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k > hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    size_t newSize = hopAddress_arraysize + 1;
    L3Address *hopAddress2 = new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    hopAddress2[k] = hopAddress;
    for (i = k + 1; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i-1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathReq::appendHopAddress(const L3Address& hopAddress)
{
    insertHopAddress(hopAddress_arraysize, hopAddress);
}

void CpnPathReq::eraseHopAddress(size_t k)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    size_t newSize = hopAddress_arraysize - 1;
    L3Address *hopAddress2 = (newSize == 0) ? nullptr : new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    for (i = k; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i+1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

const L3Address& CpnPathReq::getUserGatewayAddress() const
{
    return this->userGatewayAddress;
}

void CpnPathReq::setUserGatewayAddress(const L3Address& userGatewayAddress)
{
    this->userGatewayAddress = userGatewayAddress;
}

double CpnPathReq::getRequiredBandwidth() const
{
    return this->requiredBandwidth;
}

void CpnPathReq::setRequiredBandwidth(double requiredBandwidth)
{
    this->requiredBandwidth = requiredBandwidth;
}

size_t CpnPathReq::getSidListArraySize() const
{
    return sidList_arraysize;
}

const L3Address& CpnPathReq::getSidList(size_t k) const
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    return this->sidList[k];
}

void CpnPathReq::setSidListArraySize(size_t newSize)
{
    L3Address *sidList2 = (newSize==0) ? nullptr : new L3Address[newSize];
    size_t minSize = sidList_arraysize < newSize ? sidList_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        sidList2[i] = this->sidList[i];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

void CpnPathReq::setSidList(size_t k, const L3Address& sidList)
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    this->sidList[k] = sidList;
}

void CpnPathReq::insertSidList(size_t k, const L3Address& sidList)
{
    if (k > sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    size_t newSize = sidList_arraysize + 1;
    L3Address *sidList2 = new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        sidList2[i] = this->sidList[i];
    sidList2[k] = sidList;
    for (i = k + 1; i < newSize; i++)
        sidList2[i] = this->sidList[i-1];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

void CpnPathReq::appendSidList(const L3Address& sidList)
{
    insertSidList(sidList_arraysize, sidList);
}

void CpnPathReq::eraseSidList(size_t k)
{
    if (k >= sidList_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)sidList_arraysize, (unsigned long)k);
    size_t newSize = sidList_arraysize - 1;
    L3Address *sidList2 = (newSize == 0) ? nullptr : new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        sidList2[i] = this->sidList[i];
    for (i = k; i < newSize; i++)
        sidList2[i] = this->sidList[i+1];
    delete [] this->sidList;
    this->sidList = sidList2;
    sidList_arraysize = newSize;
}

int CpnPathReq::getCurrentHopIndex() const
{
    return this->currentHopIndex;
}

void CpnPathReq::setCurrentHopIndex(int currentHopIndex)
{
    this->currentHopIndex = currentHopIndex;
}

class CpnPathReqDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_mode,
        FIELD_userId,
        FIELD_taskId,
        FIELD_hopAddress,
        FIELD_userGatewayAddress,
        FIELD_requiredBandwidth,
        FIELD_sidList,
        FIELD_currentHopIndex,
    };
  public:
    CpnPathReqDescriptor();
    virtual ~CpnPathReqDescriptor();

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

Register_ClassDescriptor(CpnPathReqDescriptor)

CpnPathReqDescriptor::CpnPathReqDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CpnPathReq)), "inet::TagBase")
{
    propertyNames = nullptr;
}

CpnPathReqDescriptor::~CpnPathReqDescriptor()
{
    delete[] propertyNames;
}

bool CpnPathReqDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CpnPathReq *>(obj)!=nullptr;
}

const char **CpnPathReqDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CpnPathReqDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CpnPathReqDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int CpnPathReqDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_mode
        FD_ISEDITABLE,    // FIELD_userId
        FD_ISEDITABLE,    // FIELD_taskId
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_hopAddress
        0,    // FIELD_userGatewayAddress
        FD_ISEDITABLE,    // FIELD_requiredBandwidth
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_sidList
        FD_ISEDITABLE,    // FIELD_currentHopIndex
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *CpnPathReqDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "mode",
        "userId",
        "taskId",
        "hopAddress",
        "userGatewayAddress",
        "requiredBandwidth",
        "sidList",
        "currentHopIndex",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int CpnPathReqDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "mode") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "hopAddress") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "userGatewayAddress") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "requiredBandwidth") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "sidList") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "currentHopIndex") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *CpnPathReqDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_mode
        "int",    // FIELD_userId
        "int",    // FIELD_taskId
        "inet::L3Address",    // FIELD_hopAddress
        "inet::L3Address",    // FIELD_userGatewayAddress
        "double",    // FIELD_requiredBandwidth
        "inet::L3Address",    // FIELD_sidList
        "int",    // FIELD_currentHopIndex
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **CpnPathReqDescriptor::getFieldPropertyNames(int field) const
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

const char *CpnPathReqDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CpnPathReqDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return pp->getHopAddressArraySize();
        case FIELD_sidList: return pp->getSidListArraySize();
        default: return 0;
    }
}

void CpnPathReqDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: pp->setHopAddressArraySize(size); break;
        case FIELD_sidList: pp->setSidListArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CpnPathReq'", field);
    }
}

const char *CpnPathReqDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CpnPathReqDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_mode: return long2string(pp->getMode());
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_hopAddress: return pp->getHopAddress(i).str();
        case FIELD_userGatewayAddress: return pp->getUserGatewayAddress().str();
        case FIELD_requiredBandwidth: return double2string(pp->getRequiredBandwidth());
        case FIELD_sidList: return pp->getSidList(i).str();
        case FIELD_currentHopIndex: return long2string(pp->getCurrentHopIndex());
        default: return "";
    }
}

void CpnPathReqDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_mode: pp->setMode(string2long(value)); break;
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(string2double(value)); break;
        case FIELD_currentHopIndex: pp->setCurrentHopIndex(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathReq'", field);
    }
}

omnetpp::cValue CpnPathReqDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_mode: return pp->getMode();
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        case FIELD_requiredBandwidth: return pp->getRequiredBandwidth();
        case FIELD_sidList: return omnetpp::toAnyPtr(&pp->getSidList(i)); break;
        case FIELD_currentHopIndex: return pp->getCurrentHopIndex();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CpnPathReq' as cValue -- field index out of range?", field);
    }
}

void CpnPathReqDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_mode: pp->setMode(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_requiredBandwidth: pp->setRequiredBandwidth(value.doubleValue()); break;
        case FIELD_currentHopIndex: pp->setCurrentHopIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathReq'", field);
    }
}

const char *CpnPathReqDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CpnPathReqDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        case FIELD_userGatewayAddress: return omnetpp::toAnyPtr(&pp->getUserGatewayAddress()); break;
        case FIELD_sidList: return omnetpp::toAnyPtr(&pp->getSidList(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CpnPathReqDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathReq *pp = omnetpp::fromAnyPtr<CpnPathReq>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathReq'", field);
    }
}

Register_Class(CpnPathInd)

CpnPathInd::CpnPathInd() : ::inet::TagBase()
{
}

CpnPathInd::CpnPathInd(const CpnPathInd& other) : ::inet::TagBase(other)
{
    copy(other);
}

CpnPathInd::~CpnPathInd()
{
    delete [] this->hopAddress;
}

CpnPathInd& CpnPathInd::operator=(const CpnPathInd& other)
{
    if (this == &other) return *this;
    ::inet::TagBase::operator=(other);
    copy(other);
    return *this;
}

void CpnPathInd::copy(const CpnPathInd& other)
{
    this->userId = other.userId;
    this->taskId = other.taskId;
    delete [] this->hopAddress;
    this->hopAddress = (other.hopAddress_arraysize==0) ? nullptr : new L3Address[other.hopAddress_arraysize];
    hopAddress_arraysize = other.hopAddress_arraysize;
    for (size_t i = 0; i < hopAddress_arraysize; i++) {
        this->hopAddress[i] = other.hopAddress[i];
    }
    this->hopCount = other.hopCount;
    this->reservedBandwidth = other.reservedBandwidth;
}

void CpnPathInd::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::TagBase::parsimPack(b);
    doParsimPacking(b,this->userId);
    doParsimPacking(b,this->taskId);
    b->pack(hopAddress_arraysize);
    doParsimArrayPacking(b,this->hopAddress,hopAddress_arraysize);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->reservedBandwidth);
}

void CpnPathInd::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::TagBase::parsimUnpack(b);
    doParsimUnpacking(b,this->userId);
    doParsimUnpacking(b,this->taskId);
    delete [] this->hopAddress;
    b->unpack(hopAddress_arraysize);
    if (hopAddress_arraysize == 0) {
        this->hopAddress = nullptr;
    } else {
        this->hopAddress = new L3Address[hopAddress_arraysize];
        doParsimArrayUnpacking(b,this->hopAddress,hopAddress_arraysize);
    }
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->reservedBandwidth);
}

int CpnPathInd::getUserId() const
{
    return this->userId;
}

void CpnPathInd::setUserId(int userId)
{
    this->userId = userId;
}

int CpnPathInd::getTaskId() const
{
    return this->taskId;
}

void CpnPathInd::setTaskId(int taskId)
{
    this->taskId = taskId;
}

size_t CpnPathInd::getHopAddressArraySize() const
{
    return hopAddress_arraysize;
}

const L3Address& CpnPathInd::getHopAddress(size_t k) const
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    return this->hopAddress[k];
}

void CpnPathInd::setHopAddressArraySize(size_t newSize)
{
    L3Address *hopAddress2 = (newSize==0) ? nullptr : new L3Address[newSize];
    size_t minSize = hopAddress_arraysize < newSize ? hopAddress_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        hopAddress2[i] = this->hopAddress[i];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathInd::setHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    this->hopAddress[k] = hopAddress;
}

void CpnPathInd::insertHopAddress(size_t k, const L3Address& hopAddress)
{
    if (k > hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    size_t newSize = hopAddress_arraysize + 1;
    L3Address *hopAddress2 = new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    hopAddress2[k] = hopAddress;
    for (i = k + 1; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i-1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

void CpnPathInd::appendHopAddress(const L3Address& hopAddress)
{
    insertHopAddress(hopAddress_arraysize, hopAddress);
}

void CpnPathInd::eraseHopAddress(size_t k)
{
    if (k >= hopAddress_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)hopAddress_arraysize, (unsigned long)k);
    size_t newSize = hopAddress_arraysize - 1;
    L3Address *hopAddress2 = (newSize == 0) ? nullptr : new L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        hopAddress2[i] = this->hopAddress[i];
    for (i = k; i < newSize; i++)
        hopAddress2[i] = this->hopAddress[i+1];
    delete [] this->hopAddress;
    this->hopAddress = hopAddress2;
    hopAddress_arraysize = newSize;
}

int CpnPathInd::getHopCount() const
{
    return this->hopCount;
}

void CpnPathInd::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

double CpnPathInd::getReservedBandwidth() const
{
    return this->reservedBandwidth;
}

void CpnPathInd::setReservedBandwidth(double reservedBandwidth)
{
    this->reservedBandwidth = reservedBandwidth;
}

class CpnPathIndDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_userId,
        FIELD_taskId,
        FIELD_hopAddress,
        FIELD_hopCount,
        FIELD_reservedBandwidth,
    };
  public:
    CpnPathIndDescriptor();
    virtual ~CpnPathIndDescriptor();

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

Register_ClassDescriptor(CpnPathIndDescriptor)

CpnPathIndDescriptor::CpnPathIndDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(inet::CpnPathInd)), "inet::TagBase")
{
    propertyNames = nullptr;
}

CpnPathIndDescriptor::~CpnPathIndDescriptor()
{
    delete[] propertyNames;
}

bool CpnPathIndDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CpnPathInd *>(obj)!=nullptr;
}

const char **CpnPathIndDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CpnPathIndDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CpnPathIndDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int CpnPathIndDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_hopAddress
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_reservedBandwidth
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *CpnPathIndDescriptor::getFieldName(int field) const
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
        "hopAddress",
        "hopCount",
        "reservedBandwidth",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int CpnPathIndDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "userId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "taskId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "hopAddress") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "reservedBandwidth") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *CpnPathIndDescriptor::getFieldTypeString(int field) const
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
        "inet::L3Address",    // FIELD_hopAddress
        "int",    // FIELD_hopCount
        "double",    // FIELD_reservedBandwidth
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **CpnPathIndDescriptor::getFieldPropertyNames(int field) const
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

const char *CpnPathIndDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int CpnPathIndDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return pp->getHopAddressArraySize();
        default: return 0;
    }
}

void CpnPathIndDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: pp->setHopAddressArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CpnPathInd'", field);
    }
}

const char *CpnPathIndDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CpnPathIndDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return long2string(pp->getUserId());
        case FIELD_taskId: return long2string(pp->getTaskId());
        case FIELD_hopAddress: return pp->getHopAddress(i).str();
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_reservedBandwidth: return double2string(pp->getReservedBandwidth());
        default: return "";
    }
}

void CpnPathIndDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(string2long(value)); break;
        case FIELD_taskId: pp->setTaskId(string2long(value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        case FIELD_reservedBandwidth: pp->setReservedBandwidth(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathInd'", field);
    }
}

omnetpp::cValue CpnPathIndDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_userId: return pp->getUserId();
        case FIELD_taskId: return pp->getTaskId();
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        case FIELD_hopCount: return pp->getHopCount();
        case FIELD_reservedBandwidth: return pp->getReservedBandwidth();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CpnPathInd' as cValue -- field index out of range?", field);
    }
}

void CpnPathIndDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_userId: pp->setUserId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_taskId: pp->setTaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_reservedBandwidth: pp->setReservedBandwidth(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathInd'", field);
    }
}

const char *CpnPathIndDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr CpnPathIndDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        case FIELD_hopAddress: return omnetpp::toAnyPtr(&pp->getHopAddress(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CpnPathIndDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CpnPathInd *pp = omnetpp::fromAnyPtr<CpnPathInd>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CpnPathInd'", field);
    }
}

}  // namespace inet

namespace omnetpp {

}  // namespace omnetpp

